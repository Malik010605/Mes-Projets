const CONFIG = {
    simulationMode: false,

    // En local, on appelle l'ESP32 par son IP. Servi par l'ESP32, on reste relatif.
    apiBase: window.location.protocol === "file:" ? "http://192.168.4.1" : "",

    statusEndpoint: "/api/status",
    moveEndpoint: "/api/move",
    powerEndpoint: "/api/power",
    trimEndpoint: "/api/trim",
    stairSequenceEndpoint: "/api/sequence/stair",
    circleSequenceEndpoint: "/api/sequence/circle",
    northArrowSequenceEndpoint: "/api/sequence/north-arrow",
    turnInPlaceEndpoint: "/api/turn-in-place",

    refreshDelayMs: 250,
    maxRpm: 120,

    defaultPowerPwm: 90,
    defaultTrim: -2
};

let currentSpeed = Math.round((CONFIG.defaultPowerPwm / 255) * 100);
let currentPowerPwm = CONFIG.defaultPowerPwm;
let currentTrim = CONFIG.defaultTrim;
let currentCommand = "stop";



let pose = {
    x: 0,
    y: 0,
    theta: 0
};

let trajectoryPoints = [];

const elements = {
    connectionStatus: document.getElementById("connectionStatus"),
    imuStatus: document.getElementById("imuStatus"),
    magStatus: document.getElementById("magStatus"),
    encoderStatus: document.getElementById("encoderStatus"),
    batteryValue: document.getElementById("batteryValue"),

    currentCommand: document.getElementById("currentCommand"),
    remoteHint: document.getElementById("remoteHint"),
    speedRange: document.getElementById("speedRange"),
    speedValue: document.getElementById("speedValue"),

    poseEncX: document.getElementById("poseEncX"),
    poseEncY: document.getElementById("poseEncY"),
    poseEncTheta: document.getElementById("poseEncTheta"),
    orientationText: document.getElementById("orientationText"),
    compassNeedle: document.getElementById("compassNeedle"),
    penPositionValue: document.getElementById("penPositionValue"),
    distanceValue: document.getElementById("distanceValue"),
    poseStateValue: document.getElementById("poseStateValue"),

    axValue: document.getElementById("axValue"),
    ayValue: document.getElementById("ayValue"),
    azValue: document.getElementById("azValue"),

    gxValue: document.getElementById("gxValue"),
    gyValue: document.getElementById("gyValue"),
    gzValue: document.getElementById("gzValue"),

    mxValue: document.getElementById("mxValue"),
    myValue: document.getElementById("myValue"),
    mzValue: document.getElementById("mzValue"),
    headingValue: document.getElementById("headingValue"),

    leftRpmValue: document.getElementById("leftRpmValue"),
    rightRpmValue: document.getElementById("rightRpmValue"),
    leftRpmBar: document.getElementById("leftRpmBar"),
    rightRpmBar: document.getElementById("rightRpmBar"),
    leftTicksValue: document.getElementById("leftTicksValue"),
    rightTicksValue: document.getElementById("rightTicksValue"),
    wheelDeltaValue: document.getElementById("wheelDeltaValue"),

    alertText: document.getElementById("alertText"),
    logList: document.getElementById("logList"),

    canvas: document.getElementById("trajectoryCanvas"),
    resetTrajectoryBtn: document.getElementById("resetTrajectoryBtn"),
    calibrateBtn: document.getElementById("calibrateBtn"),
    stairSequenceBtn: document.getElementById("stairSequenceBtn"),
    circleSequenceBtn: document.getElementById("circleSequenceBtn"),
    turnInPlaceBtn: document.getElementById("turnInPlaceBtn"),
    circleRadius: document.getElementById("circleRadius"),
    circleDirection: document.getElementById("circleDirection"),
    northArrowSequenceBtn: document.getElementById("northArrowSequenceBtn"),
    sequenceStatus: document.getElementById("sequenceStatus"),
    sequenceStep: document.getElementById("sequenceStep")
};

const ctx = elements.canvas.getContext("2d");

function init() {
    createTrimControl();
    createMagCalibrationButton();
    setupControls();
    setupActions();

    elements.speedRange.value = currentSpeed;
    elements.speedValue.textContent = `${currentSpeed} %`;
    updateSpeedBackground(currentSpeed);

    updateActiveCommandButton("stop");
    updateRemoteHint("stop");
    resetUnavailableTelemetry();

    resizeCanvasForDisplay();

    window.addEventListener("resize", resizeCanvasForDisplay);

    addLog("Interface initialisée");
    addLog("Connexion à l'ESP32");

    drawMap();
    fetchRobotData();
    startLoop();
}

function createTrimControl() {
    const speedConsole = document.querySelector(".speed-console");

    if (!speedConsole) {
        return;
    }

    const trimConsole = document.createElement("div");
    trimConsole.className = "speed-console";
    trimConsole.style.marginTop = "14px";

    trimConsole.innerHTML = `
        <div class="speed-console-top">
            <div>
                <span>Correction direction</span>
                <strong id="trimValue">${currentTrim}</strong>
            </div>
            <div class="speed-chip">TRIM</div>
        </div>

        <input type="range" id="trimRange" min="-30" max="30" value="${currentTrim}">

        <div class="speed-scale">
            <span>Gauche</span>
            <span>0</span>
            <span>Droite</span>
        </div>
    `;

    speedConsole.insertAdjacentElement("afterend", trimConsole);

    elements.trimRange = document.getElementById("trimRange");
    elements.trimValue = document.getElementById("trimValue");

    updateTrimBackground(currentTrim);
}

function createMagCalibrationButton() {
    const remoteActions = document.querySelector(".remote-actions");

    if (!remoteActions) {
        return;
    }

    const magButton = document.createElement("button");
    magButton.id = "calibrateMagBtn";
    magButton.innerHTML = `
        <span>◇</span>
        Calibration MAG
    `;

    remoteActions.appendChild(magButton);

    elements.calibrateMagBtn = magButton;
}

function setupControls() {
    const commandButtons = document.querySelectorAll(".remote-btn");

    commandButtons.forEach((button) => {
        const command = button.dataset.command;

        button.addEventListener("pointerdown", (event) => {
            event.preventDefault();
            sendCommand(command);
        });

        if (command !== "stop") {
            button.addEventListener("pointerup", () => {
                sendCommand("stop");
            });

            button.addEventListener("pointerleave", () => {
                if (currentCommand === command) {
                    sendCommand("stop");
                }
            });

            button.addEventListener("pointercancel", () => {
                sendCommand("stop");
            });
        }
    });

    elements.speedRange.addEventListener("input", () => {
        currentSpeed = Number(elements.speedRange.value);
        currentPowerPwm = percentToPwm(currentSpeed);

        elements.speedValue.textContent = `${currentSpeed} %`;
        updateSpeedBackground(currentSpeed);
    });

    elements.speedRange.addEventListener("change", () => {
        setPower(currentPowerPwm);
    });

    if (elements.trimRange) {
        elements.trimRange.addEventListener("input", () => {
            currentTrim = Number(elements.trimRange.value);

            elements.trimValue.textContent = currentTrim;
            updateTrimBackground(currentTrim);
        });

        elements.trimRange.addEventListener("change", () => {
            setTrim(currentTrim);
        });
    }
}

function setupActions() {
    elements.resetTrajectoryBtn.addEventListener("click", async () => {
        pose = {
            x: 0,
            y: 0,
            theta: 0
        };

        trajectoryPoints = [];
        drawMap();
        addLog("Trajectoire réinitialisée");

        try {
            await apiGet("/api/reset-pose");
            addLog("Odométrie ESP32 réinitialisée");
        } catch (error) {
            addLog("Erreur reset odométrie");
        }
    });

    elements.calibrateBtn.addEventListener("click", async () => {
    addLog("Calibration gyroscope demandée");
    setAlert("Ne pas bouger le robot");

    try {
        await apiGet("/api/calibrate-gyro");
        addLog("Calibration gyroscope lancée");
    } catch (error) {
        addLog("Erreur lancement calibration");
        setAlert("Erreur calibration gyroscope");
    }
});

    if (elements.calibrateMagBtn) {
    elements.calibrateMagBtn.addEventListener("click", async () => {
        addLog("Calibration magnétomètre demandée");
        setAlert("Tourner doucement le robot pendant 10 s");

        try {
            await apiGet("/api/calibrate-mag");
            addLog("Calibration magnétomètre lancée");
        } catch (error) {
            addLog("Erreur lancement calibration MAG");
            setAlert("Erreur calibration magnétomètre");
        }
    });
}

    if (elements.stairSequenceBtn) {
        elements.stairSequenceBtn.addEventListener("click", startStairSequence);
    }

    if (elements.circleSequenceBtn) {
        elements.circleSequenceBtn.addEventListener("click", startCircleSequence);
    }

    if (elements.turnInPlaceBtn) {
        elements.turnInPlaceBtn.hidden = true;
    }

    if (elements.northArrowSequenceBtn) {
        elements.northArrowSequenceBtn.addEventListener("click", startNorthArrowSequence);
    }

    if (false) {
        const disabledLegacySequenceHandler = async () => {
            const radius = 0;
            const direction = 'left';

            addLog(`Lancement cercle R=${radius} cm dir=${direction}`);

            try {
                const data = null;
                addLog('Cercle démarré');
                // status will update via polling
            } catch (err) {
                addLog('Erreur lancement cercle');
                setAlert('Erreur communication cercle');
            }
        };
    }
}

async function startStairSequence() {
    addLog("Sequence escalier demandee");

    try {
        await apiGet(CONFIG.stairSequenceEndpoint);
        addLog("Sequence escalier lancee");
    } catch (error) {
        addLog("Erreur lancement escalier");
        setAlert("Erreur sequence escalier");
    }
}

async function startCircleSequence() {
    const radius = clamp(Number(elements.circleRadius ? elements.circleRadius.value : 10), 2, 20);
    const direction = elements.circleDirection ? elements.circleDirection.value : "left";

    if (elements.circleRadius) {
        elements.circleRadius.value = radius;
    }

    addLog(`Sequence cercle demandee R=${radius} cm`);

    try {
        const data = await apiGet(`${CONFIG.circleSequenceEndpoint}?radius=${encodeURIComponent(radius)}&direction=${encodeURIComponent(direction)}`);
        const effectiveRadius = data.circle_radius_cm || radius;

        if (elements.circleRadius) {
            elements.circleRadius.value = effectiveRadius;
        }

        addLog(`Sequence cercle lancee R=${effectiveRadius} cm`);
    } catch (error) {
        addLog("Erreur lancement cercle");
        setAlert("Erreur sequence cercle");
    }
}

async function startTurnInPlace() {
    addLog("Pivot sur place demande");

    try {
        await apiGet(CONFIG.turnInPlaceEndpoint);
        addLog("Pivot sur place lance");
    } catch (error) {
        addLog("Erreur lancement pivot");
        setAlert("Erreur pivot sur place");
    }
}

async function startNorthArrowSequence() {
    addLog("Sequence fleche Nord demandee");

    try {
        await apiGet(CONFIG.northArrowSequenceEndpoint);
        addLog("Sequence fleche Nord lancee");
    } catch (error) {
        addLog("Erreur lancement fleche Nord");
        setAlert("Erreur sequence fleche Nord");
    }
}

async function sendCommand(command) {
    currentCommand = command;

    elements.currentCommand.textContent = formatCommand(command);
    updateRemoteHint(command);
    updateActiveCommandButton(command);

    try {
        const data = await apiGet(`${CONFIG.moveEndpoint}?cmd=${encodeURIComponent(command)}`);

        setStatus(elements.connectionStatus, "ok", "Connecté");
        setAlert("Aucune");
        applyMotorStatus(data);

        addLog(`Commande : ${formatCommand(command)}`);
    } catch (error) {
        setStatus(elements.connectionStatus, "error", "Déconnecté");
        setAlert("Erreur communication commande");
        addLog("Erreur commande");
    }
}

async function setPower(powerPwm) {
    try {
        const data = await apiGet(`${CONFIG.powerEndpoint}?value=${powerPwm}`);

        setStatus(elements.connectionStatus, "ok", "Connecté");
        applyMotorStatus(data);

        addLog(`Puissance : ${currentSpeed} %`);
    } catch (error) {
        setStatus(elements.connectionStatus, "error", "Déconnecté");
        setAlert("Erreur réglage puissance");
    }
}

async function setTrim(trimValue) {
    try {
        const data = await apiGet(`${CONFIG.trimEndpoint}?value=${trimValue}`);

        setStatus(elements.connectionStatus, "ok", "Connecté");
        applyMotorStatus(data);

        addLog(`Trim : ${trimValue}`);
    } catch (error) {
        setStatus(elements.connectionStatus, "error", "Déconnecté");
        setAlert("Erreur réglage trim");
    }
}

async function apiGet(path) {
    const response = await fetch(`${CONFIG.apiBase}${path}`);

    if (!response.ok) {
        throw new Error("Réponse API invalide");
    }

    const data = await response.json();

    if (data && data.ok === false) {
        throw new Error(data.error || "Erreur API");
    }

    return data;
}

function applyMotorStatus(data) {
    if (!data) {
        return;
    }

    if (typeof data.command === "string") {
        currentCommand = data.command;
        elements.currentCommand.textContent = formatCommand(currentCommand);
        updateRemoteHint(currentCommand);
        updateActiveCommandButton(currentCommand);
    }

    if (typeof data.power === "number") {
        currentPowerPwm = data.power;
        currentSpeed = pwmToPercent(currentPowerPwm);

        elements.speedRange.value = currentSpeed;
        elements.speedValue.textContent = `${currentSpeed} %`;
        updateSpeedBackground(currentSpeed);
    }

    if (typeof data.trim === "number") {
        currentTrim = data.trim;

        if (elements.trimRange) {
            elements.trimRange.value = currentTrim;
        }

        if (elements.trimValue) {
            elements.trimValue.textContent = currentTrim;
        }

        updateTrimBackground(currentTrim);
    }

    updateMotorPreview(data);
}

function updateMotorPreview(data) {
    const leftPwm = typeof data.leftPwm === "number" ? data.leftPwm : 0;
    const rightPwm = typeof data.rightPwm === "number" ? data.rightPwm : 0;

    elements.leftRpmValue.textContent = `${leftPwm} PWM`;
    elements.rightRpmValue.textContent = `${rightPwm} PWM`;

    elements.leftRpmBar.style.width = `${clamp((leftPwm / 255) * 100, 0, 100)}%`;
    elements.rightRpmBar.style.width = `${clamp((rightPwm / 255) * 100, 0, 100)}%`;
}

function updateActiveCommandButton(command) {
    const buttons = document.querySelectorAll(".remote-btn");

    buttons.forEach((button) => {
        button.classList.remove("is-active");

        if (button.dataset.command === command) {
            button.classList.add("is-active");
        }
    });
}

function updateRemoteHint(command) {
    if (!elements.remoteHint) {
        return;
    }

    const hints = {
        forward: "Déplacement vers l'avant",
        backward: "Déplacement vers l'arrière",
        left: "Rotation vers la gauche",
        right: "Rotation vers la droite",
        stop: "Robot en attente"
    };

    elements.remoteHint.textContent = hints[command] || "Commande envoyée";
}

function updateSpeedBackground(speed) {
    elements.speedRange.style.background =
        `linear-gradient(90deg, var(--blue) 0%, var(--blue) ${speed}%, #d4e2f2 ${speed}%, #d4e2f2 100%)`;
}

function updateTrimBackground(trim) {
    if (!elements.trimRange) {
        return;
    }

    const min = Number(elements.trimRange.min);
    const max = Number(elements.trimRange.max);
    const percent = ((trim - min) / (max - min)) * 100;

    elements.trimRange.style.background =
        `linear-gradient(90deg, #d4e2f2 0%, #d4e2f2 ${percent}%, var(--blue) ${percent}%, var(--blue) 100%)`;
}

function startLoop() {
    setInterval(async () => {
        const data = await fetchRobotData();

        if (data) {
            updateDashboard(data);
        }
    }, CONFIG.refreshDelayMs);
}

async function fetchRobotData() {
    try {
        const data = await apiGet(CONFIG.statusEndpoint);

        setStatus(elements.connectionStatus, "ok", "Connecté");
        setAlert("Aucune");

        return normalizeRobotData(data);
    } catch (error) {
        setStatus(elements.connectionStatus, "error", "Déconnecté");
        setAlert("Connexion perdue");
        return null;
    }
}

function normalizeRobotData(raw) {
    applyMotorStatus(raw);

    const hasEncoders = raw && raw.encoders;

    return {
        battery: "--",

        status_imu: raw.status_imu || "waiting",
        status_mag: raw.status_mag || "waiting",
        status_encoders: raw.status_encoders || (hasEncoders ? "ok" : "waiting"),

        imu: raw.imu || null,
        mag: raw.mag || null,

        encoders: hasEncoders
            ? {
                left_ticks: raw.encoders.left_ticks,
                right_ticks: raw.encoders.right_ticks,
                left_rpm: raw.encoders.left_rpm || 0,
                right_rpm: raw.encoders.right_rpm || 0,
                delta_ticks: raw.encoders.delta_ticks,
                left_pwm: raw.leftPwm || 0,
                right_pwm: raw.rightPwm || 0
            }
            : {
                left_ticks: "--",
                right_ticks: "--",
                left_rpm: 0,
                right_rpm: 0,
                left_pwm: raw.leftPwm || 0,
                right_pwm: raw.rightPwm || 0
            },

        pose_encoders: raw.pose_encoders
            ? {
                x: raw.pose_encoders.x,
                y: raw.pose_encoders.y,
                theta: raw.pose_encoders.theta
            }
            : {
                x: pose.x,
                y: pose.y,
                theta: pose.theta
            },
        sequence: raw.sequence
            ? {
                active: raw.sequence.active || "none",
                status: raw.sequence.status || "idle",
                step: raw.sequence.step || "none",
                error: raw.sequence.error || "none",
                circle_radius_cm: raw.sequence.circle_radius_cm,
                circle_direction: raw.sequence.circle_direction,
                last_block: raw.sequence.last_block || "none",
                last_left_ticks: raw.sequence.last_left_ticks,
                last_right_ticks: raw.sequence.last_right_ticks,
                last_target_left_ticks: raw.sequence.last_target_left_ticks,
                last_target_right_ticks: raw.sequence.last_target_right_ticks
            }
            : {
                active: "none",
                status: "idle",
                step: "none",
                error: "none",
                circle_radius_cm: 10,
                circle_direction: "left",
                last_block: "none",
                last_left_ticks: 0,
                last_right_ticks: 0,
                last_target_left_ticks: 0,
                last_target_right_ticks: 0
            },
        alerts: ["none"]
    };
}

function updateDashboard(data) {
    updateStatus(data);
    updateImu(data.imu);
    updateMagnetometer(data.mag);
    updateEncoders(data.encoders);
    updatePose(data.pose_encoders);
    updateAlerts(data.alerts);

    drawMap();

    if (data.sequence) {
        updateSequenceDisplay(data.sequence);
    }
}

function updateSequenceDisplay(sequence) {
    if (elements.sequenceStatus) {
        elements.sequenceStatus.textContent = sequence.status || "idle";
    }

    if (elements.sequenceStep) {
        const activeLabel = sequence.active && sequence.active !== "none"
            ? sequence.active
            : "Aucune sequence";

        const circleLabel = sequence.active === "circle"
            ? ` | R=${formatNumber(sequence.circle_radius_cm, 1)} cm ${sequence.circle_direction || "left"}`
            : "";

        const errorLabel = sequence.error && sequence.error !== "none"
            ? ` | erreur: ${sequence.error}`
            : "";

        const ticksLabel = sequence.last_block && sequence.last_block !== "none"
            ? ` | dernier: ${sequence.last_block} G=${sequence.last_left_ticks}/${sequence.last_target_left_ticks} D=${sequence.last_right_ticks}/${sequence.last_target_right_ticks}`
            : "";

        elements.sequenceStep.textContent = `${activeLabel}${circleLabel} | etape: ${sequence.step || "none"}${ticksLabel}${errorLabel}`;
    }
}

function updateStatus(data) {
    elements.batteryValue.textContent =
        typeof data.battery === "number" ? `${data.battery} %` : "-- %";

    setStatus(
        elements.imuStatus,
        data.status_imu === "ok" ? "ok" : "warning",
        data.status_imu === "ok" ? "IMU OK" : "IMU attente"
    );

    setStatus(
        elements.magStatus,
        data.status_mag === "ok" ? "ok" : "warning",
        data.status_mag === "ok" ? "MAG OK" : "MAG attente"
    );

    setStatus(
        elements.encoderStatus,
        data.status_encoders === "ok" ? "ok" : "warning",
        data.status_encoders === "ok" ? "Encodeurs OK" : "Encodeurs attente"
    );
}

function updateImu(imu) {
    if (!imu) {
        elements.axValue.textContent = "--";
        elements.ayValue.textContent = "--";
        elements.azValue.textContent = "--";

        elements.gxValue.textContent = "--";
        elements.gyValue.textContent = "--";
        elements.gzValue.textContent = "--";
        return;
    }

    elements.axValue.textContent = formatNumber(imu.ax, 2);
    elements.ayValue.textContent = formatNumber(imu.ay, 2);
    elements.azValue.textContent = formatNumber(imu.az, 2);

    elements.gxValue.textContent = formatNumber(imu.gx, 2);
    elements.gyValue.textContent = formatNumber(imu.gy, 2);
    elements.gzValue.textContent = formatNumber(imu.gz, 2);

    if (imu.gyro_calibrating) {
    setAlert(`Calibration gyro ${imu.gyro_calibration_count || 0} / 500`);
    }

    if (imu.gyro_calibrated && !imu.gyro_calibrating) {
        setAlert("Gyroscope calibré");
    }
}

function updateMagnetometer(mag) {
    if (!mag) {
        elements.mxValue.textContent = "--";
        elements.myValue.textContent = "--";

        if (elements.mzValue) {
            elements.mzValue.textContent = "--";
        }

        if (elements.headingValue) {
            elements.headingValue.textContent = "--";
        }
        return;
    }

    elements.mxValue.textContent = formatNumber(mag.mx, 1);
    elements.myValue.textContent = formatNumber(mag.my, 1);

    if (elements.mzValue) {
        elements.mzValue.textContent = formatNumber(mag.mz, 1);
    }

    if (elements.headingValue) {
        elements.headingValue.textContent = formatNumber(mag.heading, 1);
    }

    if (mag.mag_calibrating) {
    setAlert("Calibration MAG en cours : tourner le robot");
    }

    if (mag.mag_calibrated && !mag.mag_calibrating) {
        setAlert("Magnétomètre calibré");
    }
}

function updateEncoders(encoders) {
    if (!encoders) {
        return;
    }

    const leftRpm = typeof encoders.left_rpm === "number" ? encoders.left_rpm : 0;
    const rightRpm = typeof encoders.right_rpm === "number" ? encoders.right_rpm : 0;

    const leftTicks = encoders.left_ticks !== undefined ? encoders.left_ticks : "--";
    const rightTicks = encoders.right_ticks !== undefined ? encoders.right_ticks : "--";

    const deltaTicks =
        encoders.delta_ticks !== undefined
            ? encoders.delta_ticks
            : "--";

    elements.leftRpmValue.textContent = `${formatNumber(leftRpm, 1)} tr/min`;
    elements.rightRpmValue.textContent = `${formatNumber(rightRpm, 1)} tr/min`;

    elements.leftRpmBar.style.width =
        `${clamp((Math.abs(leftRpm) / CONFIG.maxRpm) * 100, 0, 100)}%`;

    elements.rightRpmBar.style.width =
        `${clamp((Math.abs(rightRpm) / CONFIG.maxRpm) * 100, 0, 100)}%`;

    elements.leftTicksValue.textContent = leftTicks;
    elements.rightTicksValue.textContent = rightTicks;

    if (elements.wheelDeltaValue) {
        elements.wheelDeltaValue.textContent = deltaTicks;
    }
}

function updatePose(poseEncoders) {
    if (!poseEncoders) {
        return;
    }

    elements.poseEncX.textContent = `${formatNumber(poseEncoders.x, 2)} cm`;
    elements.poseEncY.textContent = `${formatNumber(poseEncoders.y, 2)} cm`;
    elements.poseEncTheta.textContent = `${formatNumber(poseEncoders.theta, 2)} °`;

    elements.orientationText.textContent = `${formatNumber(poseEncoders.theta, 2)} °`;

    elements.compassNeedle.style.transform =
        `translateX(-50%) rotate(${poseEncoders.theta}deg)`;

    elements.penPositionValue.textContent =
        `X ${formatNumber(poseEncoders.x, 1)} / Y ${formatNumber(poseEncoders.y, 1)}`;

    const distance = Math.hypot(poseEncoders.x, poseEncoders.y);
    elements.distanceValue.textContent = `${formatNumber(distance, 1)} cm`;

    elements.poseStateValue.textContent =
        currentCommand === "stop" ? "Stable" : "En mouvement";

    const lastPoint = trajectoryPoints[trajectoryPoints.length - 1];

    const shouldAddPoint =
        !lastPoint ||
        Math.hypot(poseEncoders.x - lastPoint.x, poseEncoders.y - lastPoint.y) > 0.2 ||
        Math.abs(poseEncoders.theta - lastPoint.theta) > 2;

    if (shouldAddPoint) {
        trajectoryPoints.push({
            x: poseEncoders.x,
            y: poseEncoders.y,
            theta: poseEncoders.theta
        });
    }

    if (trajectoryPoints.length > 900) {
        trajectoryPoints.shift();
    }
}

function updateAlerts(alerts) {
    if (!alerts || alerts.length === 0 || alerts[0] === "none") {
        setAlert("Aucune");
        return;
    }

    setAlert(alerts.join(" | "));
}

function resetUnavailableTelemetry() {
    elements.batteryValue.textContent = "-- %";

    elements.axValue.textContent = "--";
    elements.ayValue.textContent = "--";
    elements.azValue.textContent = "--";

    elements.gxValue.textContent = "--";
    elements.gyValue.textContent = "--";
    elements.gzValue.textContent = "--";

    elements.mxValue.textContent = "--";
    elements.myValue.textContent = "--";
    if (elements.headingValue) {
        elements.headingValue.textContent = "--";
    }

    elements.leftTicksValue.textContent = "--";
    elements.rightTicksValue.textContent = "--";
    elements.wheelDeltaValue.textContent = "--";

    elements.leftRpmValue.textContent = "0 PWM";
    elements.rightRpmValue.textContent = "0 PWM";
    elements.leftRpmBar.style.width = "0%";
    elements.rightRpmBar.style.width = "0%";
}

function setAlert(text) {
    elements.alertText.textContent = text;
}

function setStatus(element, type, label) {
    if (!element) {
        return;
    }

    element.classList.remove("ok", "warning", "error");
    element.classList.add(type);

    const statusDot = element.querySelector("span") || document.createElement("span");

    element.innerHTML = "";
    element.appendChild(statusDot);
    element.append(` ${label}`);
}

function resizeCanvasForDisplay() {
    const rect = elements.canvas.getBoundingClientRect();
    const ratio = window.devicePixelRatio || 1;

    elements.canvas.width = rect.width * ratio;
    elements.canvas.height = rect.height * ratio;

    ctx.setTransform(ratio, 0, 0, ratio, 0, 0);
    drawMap();
}

function drawMap() {
    const width = elements.canvas.clientWidth;
    const height = elements.canvas.clientHeight;

    ctx.clearRect(0, 0, width, height);

    drawMapBackground(width, height);
    drawCenterAxes(width, height);
    drawTrajectory(width, height);
}

function drawMapBackground(width, height) {
    ctx.fillStyle = "rgba(248, 251, 255, 0.65)";
    ctx.fillRect(0, 0, width, height);

    const centerX = width / 2;
    const centerY = height / 2;

    const gradient = ctx.createRadialGradient(
        centerX,
        centerY,
        10,
        centerX,
        centerY,
        Math.max(width, height) / 1.6
    );

    gradient.addColorStop(0, "rgba(0, 111, 189, 0.10)");
    gradient.addColorStop(1, "rgba(0, 111, 189, 0)");

    ctx.fillStyle = gradient;
    ctx.fillRect(0, 0, width, height);
}

function drawCenterAxes(width, height) {
    const cx = width / 2;
    const cy = height / 2;

    ctx.save();

    ctx.strokeStyle = "rgba(0, 46, 99, 0.18)";
    ctx.lineWidth = 1;

    ctx.beginPath();
    ctx.moveTo(cx, 0);
    ctx.lineTo(cx, height);
    ctx.moveTo(0, cy);
    ctx.lineTo(width, cy);
    ctx.stroke();

    ctx.fillStyle = "rgba(0, 46, 99, 0.35)";
    ctx.font = "12px Arial";
    ctx.fillText("Départ", cx + 8, cy - 8);

    ctx.beginPath();
    ctx.arc(cx, cy, 5, 0, Math.PI * 2);
    ctx.fillStyle = "#f5b400";
    ctx.fill();

    ctx.restore();
}

function drawTrajectory(width, height) {
    if (trajectoryPoints.length === 0) {
        drawWaitingText(width, height);
        return;
    }

    const cx = width / 2;
    const cy = height / 2;
    const scale = 4;

    ctx.save();

    ctx.beginPath();

    trajectoryPoints.forEach((point, index) => {
        const px = cx + point.x * scale;
        const py = cy - point.y * scale;

        if (index === 0) {
            ctx.moveTo(px, py);
        } else {
            ctx.lineTo(px, py);
        }
    });

    ctx.strokeStyle = "#006fbd";
    ctx.lineWidth = 4;
    ctx.lineCap = "round";
    ctx.lineJoin = "round";
    ctx.stroke();

    ctx.beginPath();

    trajectoryPoints.forEach((point, index) => {
        if (index % 16 !== 0) {
            return;
        }

        const px = cx + point.x * scale;
        const py = cy - point.y * scale;

        ctx.moveTo(px, py);
        ctx.arc(px, py, 2.4, 0, Math.PI * 2);
    });

    ctx.fillStyle = "rgba(0, 111, 189, 0.32)";
    ctx.fill();

    const last = trajectoryPoints[trajectoryPoints.length - 1];
    drawRobot(cx + last.x * scale, cy - last.y * scale, last.theta);

    ctx.restore();
}

function drawRobot(x, y, thetaDeg) {
    const theta = degreesToRadians(thetaDeg);

    ctx.save();
    ctx.translate(x, y);
    ctx.rotate(-theta);

    ctx.beginPath();
    ctx.ellipse(0, 13, 34, 13, 0, 0, Math.PI * 2);
    ctx.fillStyle = "rgba(0, 46, 99, 0.15)";
    ctx.fill();

    roundedRect(ctx, -26, -18, 52, 36, 12);
    ctx.fillStyle = "#002e63";
    ctx.fill();

    ctx.beginPath();
    ctx.moveTo(34, 0);
    ctx.lineTo(12, -15);
    ctx.lineTo(12, 15);
    ctx.closePath();
    ctx.fillStyle = "#006fbd";
    ctx.fill();

    ctx.fillStyle = "#101828";

    roundedRect(ctx, -18, -27, 28, 7, 3);
    ctx.fill();

    roundedRect(ctx, -18, 20, 28, 7, 3);
    ctx.fill();

    ctx.beginPath();
    ctx.arc(0, 0, 7, 0, Math.PI * 2);
    ctx.fillStyle = "#f5b400";
    ctx.fill();

    ctx.beginPath();
    ctx.arc(0, 0, 3, 0, Math.PI * 2);
    ctx.fillStyle = "#d93025";
    ctx.fill();

    ctx.beginPath();
    ctx.moveTo(0, 0);
    ctx.lineTo(31, 0);
    ctx.strokeStyle = "white";
    ctx.lineWidth = 4;
    ctx.lineCap = "round";
    ctx.stroke();

    ctx.restore();
}

function drawWaitingText(width, height) {
    ctx.save();
    ctx.fillStyle = "rgba(0, 46, 99, 0.48)";
    ctx.font = "16px Arial";
    ctx.textAlign = "center";
    ctx.fillText("Trajectoire en attente de données", width / 2, height / 2);
    ctx.restore();
}

function roundedRect(context, x, y, width, height, radius) {
    context.beginPath();
    context.moveTo(x + radius, y);
    context.lineTo(x + width - radius, y);
    context.quadraticCurveTo(x + width, y, x + width, y + radius);
    context.lineTo(x + width, y + height - radius);
    context.quadraticCurveTo(x + width, y + height, x + width - radius, y + height);
    context.lineTo(x + radius, y + height);
    context.quadraticCurveTo(x, y + height, x, y + height - radius);
    context.lineTo(x, y + radius);
    context.quadraticCurveTo(x, y, x + radius, y);
    context.closePath();
}

function addLog(message) {
    const now = new Date();

    const time = now.toLocaleTimeString("fr-FR", {
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit"
    });

    const item = document.createElement("div");
    item.className = "log-item";

    const timeSpan = document.createElement("span");
    timeSpan.textContent = time;

    const textSpan = document.createElement("span");
    textSpan.textContent = message;

    item.appendChild(timeSpan);
    item.appendChild(textSpan);

    elements.logList.prepend(item);

    while (elements.logList.children.length > 8) {
        elements.logList.removeChild(elements.logList.lastChild);
    }
}

function formatCommand(command) {
    const labels = {
        forward: "AVANCER",
        backward: "RECULER",
        left: "GAUCHE",
        right: "DROITE",
        stop: "STOP",
        sequence_stair: "SEQUENCE 1",
        sequence_circle: "SEQUENCE 2",
        sequence_north_arrow: "SEQUENCE 3"
    };

    return labels[command] || command.toUpperCase();
}

function formatNumber(value, decimals) {
    if (typeof value !== "number" || Number.isNaN(value)) {
        return "--";
    }

    return value.toFixed(decimals);
}

function percentToPwm(percent) {
    return Math.round(clamp(percent, 0, 100) * 255 / 100);
}

function pwmToPercent(pwm) {
    return Math.round(clamp(pwm, 0, 255) * 100 / 255);
}

function degreesToRadians(degrees) {
    return degrees * Math.PI / 180;
}

function clamp(value, min, max) {
    return Math.max(min, Math.min(max, value));
}

document.addEventListener("DOMContentLoaded", init);
