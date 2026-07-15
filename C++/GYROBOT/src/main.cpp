#include <Arduino.h>
#include <PID_v1.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Wire.h>

// ============================================================
// Brochage du robot
// ============================================================

#define ROBOT_STATUS_LED_PIN 25
#define ROBOT_READY_LED_PIN 26

#define RIGHT_DRIVE_ENABLE_PIN 23
#define LEFT_DRIVE_ENABLE_PIN 4

#define RIGHT_DRIVE_INPUT_A_PIN 19
#define RIGHT_DRIVE_INPUT_B_PIN 18
#define LEFT_DRIVE_INPUT_A_PIN 17
#define LEFT_DRIVE_INPUT_B_PIN 16

#define LEFT_WHEEL_ENCODER_A_PIN 32
#define LEFT_WHEEL_ENCODER_B_PIN 33
#define RIGHT_WHEEL_ENCODER_A_PIN 27
#define RIGHT_WHEEL_ENCODER_B_PIN 14

// ============================================================
// WiFi AP
// ============================================================

static const char* WIFI_ACCESS_POINT_NAME = "Gyrobot-Control";
static const char* WIFI_ACCESS_POINT_PASSWORD = "12345678";
WebServer server(80);

// ============================================================
// Constants
// ============================================================

#define SERIAL_MONITOR_BAUD_RATE 9600

// Roues du robot
#define DRIVE_WHEEL_DIAMETER_MM 90.0
#define DRIVE_WHEEL_BASE_MM 68.0

// Encodeurs
// Valeur à ajuster si tes tests montrent un autre nombre de ticks par tour.
static const int    ENCODER_COUNTS_PER_WHEEL_REV = 4396;
static const double MILLIMETERS_PER_ENCODER_COUNT   = (PI * DRIVE_WHEEL_DIAMETER_MM) / ENCODER_COUNTS_PER_WHEEL_REV;
static const double ENCODER_COUNTS_PER_MILLIMETER  = 1.0 / MILLIMETERS_PER_ENCODER_COUNT;

static const int MOTOR_PWM_FREQUENCY_HZ       = 5000;
static const int MOTOR_PWM_RESOLUTION_BITS = 8;
static const int MOTOR_PWM_MINIMUM_VALUE        = 60;

static const int RIGHT_DRIVE_INPUT_A_PWM_CHANNEL = 0;
static const int RIGHT_DRIVE_INPUT_B_PWM_CHANNEL = 1;
static const int LEFT_DRIVE_INPUT_A_PWM_CHANNEL = 2;
static const int LEFT_DRIVE_INPUT_B_PWM_CHANNEL = 3;

static const int STRAIGHT_MOVE_REFERENCE_PWM = 150;
static const int TURN_MOVE_REFERENCE_PWM     = 120;
static const unsigned long ENCODER_MOVE_TIMEOUT_MS  = 10000;
static const unsigned long ENCODER_STALL_TIMEOUT_MS = 800;

static const int LEFT_DRIVE_MINIMUM_PWM  = 200;
static const int RIGHT_DRIVE_MINIMUM_PWM = 120;

static const int LEFT_STRAIGHT_REFERENCE_PWM  = 224;
static const int RIGHT_STRAIGHT_REFERENCE_PWM = 156;

static const int LEFT_TURN_REFERENCE_PWM  = 230;
static const int RIGHT_TURN_REFERENCE_PWM = 150;




// ============================================================
// SEQUENCE 2 - CERCLE PARAMETRABLE A DEUX ROUES
// ============================================================

// Stylo placé à 13 cm devant l’axe des roues
static const double FRONT_PEN_OFFSET_MM = 130.0;

// Rayon reçu depuis la GUI
float circleRadiusCm = 15.0f;

// Avec le stylo à 13 cm, on interdit les rayons trop proches du cas pivot.
// En dessous, le robot tourne quasiment sur lui-même.
static const double CIRCLE_RADIUS_MINIMUM_CM = 14.0;
static const double CIRCLE_RADIUS_MAXIMUM_CM = 20.0;

// Calibration du rayon
static const double CIRCLE_RADIUS_CALIBRATION_FACTOR = 1.00;

// Compensation de fermeture
static const double CIRCLE_CLOSURE_CALIBRATION_FACTOR = 1.16;

// PWM séparés pour forcer les deux roues à tourner
static const int CIRCLE_LEFT_REFERENCE_PWM = 225;
static const int CIRCLE_RIGHT_MINIMUM_PWM = 155;
static const int CIRCLE_RIGHT_REFERENCE_MAXIMUM_PWM = 210;

// Correction de synchronisation entre progressions gauche/droite
static const float CIRCLE_PROGRESS_CORRECTION_GAIN = 65.0f;

// Timeout sécurité
static const unsigned long CIRCLE_MOVE_TIMEOUT_MS = 45000;

// Stop GUI
volatile bool stopRequested = false;

// Compatibilité anciennes fonctions cercle si elles existent encore
static const double CIRCLE_SPEED_DEADBAND_MM_PER_S = 4.0;
static const double CIRCLE_WHEEL_SPEED_AT_MAX_PWM_MM_PER_S = 90.0;

static const int CIRCLE_LEFT_LOADED_MINIMUM_PWM  = 225;
static const int CIRCLE_RIGHT_LOADED_MINIMUM_PWM = 145;

static const int CIRCLE_LEFT_MAXIMUM_PWM  = 255;
static const int CIRCLE_RIGHT_OUTPUT_MAXIMUM_PWM = 210;





// ============================================================
// SEQUENCE 3 - Orientation Nord adaptee au robot
// ============================================================

// Le Nord est considere devant le robot quand Mx est proche de 0.
static const float NORTH_MX_TOLERANCE_UT = 2.0f;       // tolerance en uT autour de Mx = 0
static const int NORTH_STABLE_SAMPLE_COUNT = 5;      // nb de mesures stables consecutives

// A regler apres test.
// Si le robot s'oriente vers le Sud au lieu du Nord, mettre -1.
static const int NORTH_EXPECTED_MY_SIGN = -1;

// A regler apres test.
// Si pendant l'orientation Mx s'eloigne de 0 au lieu de s'en rapprocher, mettre -1.
static const int NORTH_MX_TURN_DIRECTION_SIGN = 1;

// Impulsions de rotation douce pour chercher Mx = 0
static const int NORTH_LEFT_PIVOT_PWM  = 210;
static const int NORTH_RIGHT_PIVOT_PWM = 150;
static const int NORTH_MINIMUM_PULSE_MS = 45;
static const int NORTH_MAXIMUM_PULSE_MS = 180;

static const float NORTH_RAW_HEADING_TARGET_DEG = -137.44f;
static const float NORTH_HEADING_TOLERANCE_DEG = 12.0f;

// Si le robot tourne dans le mauvais sens, passe cette valeur à -1.
static const int MAGNETIC_TURN_DIRECTION_SIGN = 1;




// ============================================================
// Enregistrement & Lecture (Record & Playback)
// ============================================================

#define RECORDING_MAX_SAMPLE_COUNT 300   // 300 x 100ms = 30 secondes max

struct RecordStep {
  long L;   // encodeur gauche cumulatif depuis debut enreg.
  long R;   // encodeur droit cumulatif depuis debut enreg.
};

// ============================================================
// Encoder - manual quadrature (compatible WiFi ESP32)
// ============================================================

volatile long encLeftCount  = 0;
volatile long encRightCount = 0;

void IRAM_ATTR isrEncLeftA() {
  if (digitalRead(LEFT_WHEEL_ENCODER_A_PIN) == digitalRead(LEFT_WHEEL_ENCODER_B_PIN))
    encLeftCount--;
  else
    encLeftCount++;
}

void IRAM_ATTR isrEncLeftB() {
  if (digitalRead(LEFT_WHEEL_ENCODER_A_PIN) == digitalRead(LEFT_WHEEL_ENCODER_B_PIN))
    encLeftCount++;
  else
    encLeftCount--;
}

void IRAM_ATTR isrEncRightA() {
  if (digitalRead(RIGHT_WHEEL_ENCODER_A_PIN) == digitalRead(RIGHT_WHEEL_ENCODER_B_PIN))
    encRightCount--;
  else
    encRightCount++;
}

void IRAM_ATTR isrEncRightB() {
  if (digitalRead(RIGHT_WHEEL_ENCODER_A_PIN) == digitalRead(RIGHT_WHEEL_ENCODER_B_PIN))
    encRightCount++;
  else
    encRightCount--;
}

long readLeftWheelEncoderTicks() {
  noInterrupts();
  long val = encLeftCount;
  interrupts();
  return val;
}

long readRightWheelEncoderTicks() {
  noInterrupts();
  long val = -encRightCount;  // moteur droit monté en miroir: inverser le signe
  interrupts();
  return val;
}

void clearWheelEncoderCounters() {
  noInterrupts();
  encLeftCount = 0;
  encRightCount = 0;
  interrupts();
}

// ============================================================
// IMU: LSM6DS3 (accel+gyro) + LIS3MDL (magnetometre)
// ============================================================

uint8_t imuI2cAddress = 0x6A; // auto-detected (0x6A ou 0x6B selon SA0)
#define LIS3MDL_MAGNETOMETER_ADDRESS 0x1E

float accelX=0, accelY=0, accelZ=0; // m/s²
float gyroX=0,  gyroY=0,  gyroZ=0;  // deg/s
float magX=0,   magY=0,   magZ=0;   // µT
bool  imuOk=false, magOk=false;

// Heading intégré depuis le gyroscope (axe Z)
float imuHeading=0; // degrés
unsigned long lastImuUs=0;

// Odometrie (encodeurs)
double odoX=0, odoY=0, odoTheta=0; // mm, mm, rad
long   odoEncL=0, odoEncR=0;

// Calibration actionneurs (tests unitaires)
struct CalibPoint { int pwm; float speedL; float speedR; };
CalibPoint calibPoints[4] = {{80,0,0},{120,0,0},{160,0,0},{200,0,0}};
bool calibDone = false;

void writeI2cRegisterByte(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

bool initializeInertialSensor() {
  // Essaie 0x6A (SA0=GND) puis 0x6B (SA0=VCC)
  for (uint8_t candidate : {(uint8_t)0x6A, (uint8_t)0x6B}) {
    Wire.beginTransmission(candidate);
    Wire.write(0x0F); // WHO_AM_I
    if (Wire.endTransmission(false) != 0) continue;
    Wire.requestFrom(candidate, (uint8_t)1);
    if (!Wire.available()) continue;
    uint8_t who = Wire.read();
    Serial.print("Adresse capteur inertiel 0x"); Serial.print(candidate, HEX);
    Serial.print(" | registre identite 0x"); Serial.println(who, HEX);
    if (who != 0x69) continue; // attendu: 0x69
    imuI2cAddress = candidate;
    writeI2cRegisterByte(imuI2cAddress, 0x10, 0x40); // CTRL1_XL: 104 Hz, ±2g
    writeI2cRegisterByte(imuI2cAddress, 0x11, 0x40); // CTRL2_G:  104 Hz, ±245 dps
    return true;
  }
  return false;
}

void readInertialSensorSample() {
  Wire.beginTransmission(imuI2cAddress);
  Wire.write(0x22); // OUTX_L_G (gyro then accel, 12 bytes)
  Wire.endTransmission(false);
  Wire.requestFrom(imuI2cAddress, (uint8_t)12);
  if (Wire.available() < 12) return;
  int16_t gx = Wire.read() | (Wire.read() << 8);
  int16_t gy = Wire.read() | (Wire.read() << 8);
  int16_t gz = Wire.read() | (Wire.read() << 8);
  int16_t ax = Wire.read() | (Wire.read() << 8);
  int16_t ay = Wire.read() | (Wire.read() << 8);
  int16_t az = Wire.read() | (Wire.read() << 8);
  gyroX  = gx * 0.00875f;  // ±245 dps: 8.75 mdps/LSB
  gyroY  = gy * 0.00875f;
  gyroZ  = gz * 0.00875f;
  accelX = ax * 0.000598f; // ±2g: 0.061 mg/LSB → 0.0005984 m/s²/LSB
  accelY = ay * 0.000598f;
  accelZ = az * 0.000598f;
}

bool initializeMagneticSensor() {
  Wire.beginTransmission(LIS3MDL_MAGNETOMETER_ADDRESS);
  Wire.write(0x0F); // WHO_AM_I
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom(LIS3MDL_MAGNETOMETER_ADDRESS, (uint8_t)1);
  if (!Wire.available() || Wire.read() != 0x3D) return false;
  writeI2cRegisterByte(LIS3MDL_MAGNETOMETER_ADDRESS, 0x20, 0x70); // CTRL1: OM=ultra-high, 10 Hz
  writeI2cRegisterByte(LIS3MDL_MAGNETOMETER_ADDRESS, 0x21, 0x00); // CTRL2: FS=±4 gauss
  writeI2cRegisterByte(LIS3MDL_MAGNETOMETER_ADDRESS, 0x22, 0x00); // CTRL3: continuous mode
  writeI2cRegisterByte(LIS3MDL_MAGNETOMETER_ADDRESS, 0x23, 0x0C); // CTRL4: Z ultra-high perf
  return true;
}

void readMagneticSensorSample() {
  Wire.beginTransmission(LIS3MDL_MAGNETOMETER_ADDRESS);
  Wire.write(0x28); // OUT_X_L
  Wire.endTransmission(false);
  Wire.requestFrom(LIS3MDL_MAGNETOMETER_ADDRESS, (uint8_t)6);
  if (Wire.available() < 6) return;
  int16_t mx = Wire.read() | (Wire.read() << 8);
  int16_t my = Wire.read() | (Wire.read() << 8);
  int16_t mz = Wire.read() | (Wire.read() << 8);
  magX = mx * 0.01462f; // FS=±4G: (100 µT/gauss) / 6842 LSB/gauss
  magY = my * 0.01462f;
  magZ = mz * 0.01462f;
}

void refreshWheelOdometry() {
  long curL = readLeftWheelEncoderTicks();
  long curR = readRightWheelEncoderTicks();
  double distL = (curL - odoEncL) * MILLIMETERS_PER_ENCODER_COUNT;
  double distR = (curR - odoEncR) * MILLIMETERS_PER_ENCODER_COUNT;
  odoEncL = curL;
  odoEncR = curR;
  double d      = (distL + distR) / 2.0;
  double dTheta = (distR - distL) / DRIVE_WHEEL_BASE_MM;
  odoX     += d * cos(odoTheta + dTheta / 2.0);
  odoY     += d * sin(odoTheta + dTheta / 2.0);
  odoTheta += dTheta;
}

void refreshRobotSensors() {
  unsigned long now = micros();
  float dtSec = (lastImuUs > 0) ? (now - lastImuUs) / 1e6f : 0;
  lastImuUs = now;
  if (imuOk) {
    readInertialSensorSample();
    if (dtSec > 0 && dtSec < 0.5f) {
      imuHeading += gyroZ * dtSec;
      while (imuHeading >  180) imuHeading -= 360;
      while (imuHeading < -180) imuHeading += 360;
    }
  }
  if (magOk) readMagneticSensorSample();
  refreshWheelOdometry();
}

// ============================================================
// PID
// ============================================================

double headingInput = 0, headingOutput = 0, headingSetpoint = 0;
double headingKp = 2.0, headingKi = 0.5, headingKd = 0.1;
PID headingPID(&headingInput, &headingOutput, &headingSetpoint,
               headingKp, headingKi, headingKd, DIRECT);

double turnInput = 0, turnOutput = 0, turnSetpoint = 0;
double turnKp = 3.0, turnKi = 0.3, turnKd = 0.2;
PID turnPID(&turnInput, &turnOutput, &turnSetpoint,
            turnKp, turnKi, turnKd, DIRECT);

// ============================================================
// State Machine
// ============================================================

enum RobotState {
  ROBOT_STATE_IDLE,
  ROBOT_STATE_SEQUENCE_1,
  ROBOT_STATE_SEQUENCE_2,
  ROBOT_STATE_SEQUENCE_3,
  ROBOT_STATE_DIAGNOSTIC,
  ROBOT_STATE_DISTANCE_CALIBRATION,
  ROBOT_STATE_ACTUATOR_CALIBRATION,
  ROBOT_STATE_RECORDING,
  ROBOT_STATE_PLAYBACK,
  ROBOT_STATE_DONE
};
volatile RobotState robotState = ROBOT_STATE_IDLE;
String statusMessage = "Pret";
String lastLog = "";
String fullReport = "Aucun rapport disponible";

// Enregistrement
RecordStep recordBuffer[RECORDING_MAX_SAMPLE_COUNT];
int   recordCount    = 0;
bool  isRecording    = false;
unsigned long lastRecordMs = 0;
long  recStartL      = 0;
long  recStartR      = 0;

// ============================================================
// Motor Control
// ============================================================

int lastLeftMotorCommand = 0;
int lastRightMotorCommand = 0;

void commandLeftWheelMotor(int speed) {
  speed = constrain(speed, -255, 255);
  lastLeftMotorCommand = speed;

  if (speed > 0) {
    ledcWrite(LEFT_DRIVE_INPUT_A_PWM_CHANNEL, speed);
    ledcWrite(LEFT_DRIVE_INPUT_B_PWM_CHANNEL, 0);
  } else if (speed < 0) {
    ledcWrite(LEFT_DRIVE_INPUT_A_PWM_CHANNEL, 0);
    ledcWrite(LEFT_DRIVE_INPUT_B_PWM_CHANNEL, -speed);
  } else {
    ledcWrite(LEFT_DRIVE_INPUT_A_PWM_CHANNEL, 0);
    ledcWrite(LEFT_DRIVE_INPUT_B_PWM_CHANNEL, 0);
  }
}

void commandRightWheelMotor(int speed) {
  // Inversé: le moteur droit est monté en miroir
  speed = constrain(speed, -255, 255);
  lastRightMotorCommand = speed;

  if (speed > 0) {
    ledcWrite(RIGHT_DRIVE_INPUT_A_PWM_CHANNEL, 0);
    ledcWrite(RIGHT_DRIVE_INPUT_B_PWM_CHANNEL, speed);
  } else if (speed < 0) {
    ledcWrite(RIGHT_DRIVE_INPUT_A_PWM_CHANNEL, -speed);
    ledcWrite(RIGHT_DRIVE_INPUT_B_PWM_CHANNEL, 0);
  } else {
    ledcWrite(RIGHT_DRIVE_INPUT_A_PWM_CHANNEL, 0);
    ledcWrite(RIGHT_DRIVE_INPUT_B_PWM_CHANNEL, 0);
  }
}

void stopWheelMotors() {
  commandLeftWheelMotor(0);
  commandRightWheelMotor(0);
}

// Frein doux : on coupe simplement les moteurs.
// Plus sûr pour éviter les redémarrages ESP32 ou les chutes de tension.
void releaseLeftWheelMotor() {
  commandLeftWheelMotor(0);
}

void releaseRightWheelMotor() {
  commandRightWheelMotor(0);
}

void stopWheelMotorsBriefly() {
  stopWheelMotors();
  delay(120);
}

// ============================================================
// Diagnostic: test moteurs + encodeurs
// ============================================================

void executeDriveEncoderInspection() {
  Serial.println("\n====== Controle de la chaine motrice ======");
  statusMessage = "Diagnostic en cours...";
  fullReport = "=== DIAGNOSTIC ===\n";

  // Test 1: encodeurs au repos
  clearWheelEncoderCounters();
  delay(500);
  long rL = readLeftWheelEncoderTicks(), rR = readRightWheelEncoderTicks();
  fullReport += "\n[Repos 500ms] L=" + String(rL) + " | compteur droit=" + String(rR) + "\n";

  // Test 2: moteur gauche seul
  statusMessage = "Diag: moteur gauche...";
  fullReport += "\n--- Moteur GAUCHE (PWM=150, 1s) ---\n";
  clearWheelEncoderCounters();
  commandLeftWheelMotor(150);
  for (int i = 0; i < 10; i++) {
    delay(100);
    fullReport += "t=" + String((i+1)*100) + "ms L=" + String(readLeftWheelEncoderTicks()) + " | compteur droit=" + String(readRightWheelEncoderTicks()) + "\n";
  }
  stopWheelMotors();
  long gL = readLeftWheelEncoderTicks(), gR = readRightWheelEncoderTicks();
  fullReport += "Final: L=" + String(gL) + "(" + String(gL * MILLIMETERS_PER_ENCODER_COUNT, 1) + "mm) R=" + String(gR) + "\n";
  delay(200);

  // Test 3: moteur droit seul
  statusMessage = "Diag: moteur droit...";
  fullReport += "\n--- Moteur DROIT (PWM=150, 1s) ---\n";
  clearWheelEncoderCounters();
  commandRightWheelMotor(150);
  for (int i = 0; i < 10; i++) {
    delay(100);
    fullReport += "t=" + String((i+1)*100) + "ms L=" + String(readLeftWheelEncoderTicks()) + " | compteur droit=" + String(readRightWheelEncoderTicks()) + "\n";
  }
  stopWheelMotors();
  long dL = readLeftWheelEncoderTicks(), dR = readRightWheelEncoderTicks();
  fullReport += "Final: L=" + String(dL) + " | compteur droit=" + String(dR) + "(" + String(dR * MILLIMETERS_PER_ENCODER_COUNT, 1) + "mm)\n";
  delay(200);

  // Test 4: les deux moteurs
  statusMessage = "Diag: deux moteurs...";
  fullReport += "\n--- DEUX MOTEURS (PWM=150, 1s) ---\n";
  clearWheelEncoderCounters();
  commandLeftWheelMotor(150);
  commandRightWheelMotor(150);
  for (int i = 0; i < 10; i++) {
    delay(100);
    fullReport += "t=" + String((i+1)*100) + "ms L=" + String(readLeftWheelEncoderTicks()) + " | compteur droit=" + String(readRightWheelEncoderTicks()) + "\n";
  }
  stopWheelMotors();

  long finalL = readLeftWheelEncoderTicks();
  long finalR = readRightWheelEncoderTicks();
  fullReport += "\n=== RESULTAT FINAL ===\n";
  fullReport += "encL=" + String(finalL) + " (" + String(finalL * MILLIMETERS_PER_ENCODER_COUNT, 1) + "mm)\n";
  fullReport += "encR=" + String(finalR) + " (" + String(finalR * MILLIMETERS_PER_ENCODER_COUNT, 1) + "mm)\n";

  if (finalL < 0) fullReport += "!! encL NEGATIF -> inverser encodeur gauche\n";
  if (finalR < 0) fullReport += "!! encR NEGATIF -> inverser encodeur droit\n";
  if (finalL == 0) fullReport += "!! encL=0 -> encodeur gauche ne repond pas!\n";
  if (finalR == 0) fullReport += "!! encR=0 -> encodeur droit ne repond pas!\n";
  if (finalL > 0 && finalR > 0) fullReport += "OK: les deux encodeurs comptent positif\n";

  fullReport += "\nConfig: " + String(ENCODER_COUNTS_PER_WHEEL_REV) + " counts/tour, " + String(MILLIMETERS_PER_ENCODER_COUNT, 3) + " mm/count\n";

  Serial.println(fullReport);
  lastLog = "gauche=" + String(finalL) + "(" + String(finalL * MILLIMETERS_PER_ENCODER_COUNT, 1) + "mm) R=" + String(finalR) + "(" + String(finalR * MILLIMETERS_PER_ENCODER_COUNT, 1) + "mm)";
  statusMessage = "Diagnostic termine";
}

// ============================================================
// Calibration: avance tout droit 3 secondes, mesurer la distance reelle
// ============================================================

void executeStraightDistanceMeasurement() {
  Serial.println("\n====== Essai chronometre de ligne droite ======");
  statusMessage = "Calibration: avance 3s...";
  fullReport = "=== CALIBRATION ===\n";
  fullReport += "Le robot va avancer tout droit pendant 3 secondes.\n";
  fullReport += "Mesurez la distance parcourue avec un metre.\n\n";

  clearWheelEncoderCounters();
  commandLeftWheelMotor(LEFT_STRAIGHT_REFERENCE_PWM);
  commandRightWheelMotor(RIGHT_STRAIGHT_REFERENCE_PWM);

  for (int i = 0; i < 30; i++) {
    delay(100);
    if (i % 10 == 9) {
      fullReport += "t=" + String((i+1)/10) + "s L=" + String(readLeftWheelEncoderTicks()) + " | compteur droit=" + String(readRightWheelEncoderTicks()) + "\n";
    }
  }

  stopWheelMotors();
  delay(300);

  long finalL = readLeftWheelEncoderTicks();
  long finalR = readRightWheelEncoderTicks();
  long avgCounts = (finalL + finalR) / 2;
  double distCalc = avgCounts * MILLIMETERS_PER_ENCODER_COUNT;

  fullReport += "\n=== RESULTAT ===\n";
  fullReport += "Counts: L=" + String(finalL) + " | compteur droit=" + String(finalR) + " Moy=" + String(avgCounts) + "\n";
  fullReport += "Distance calculee (config actuelle): " + String(distCalc, 1) + " mm\n";
  fullReport += "mm/count actuel: " + String(MILLIMETERS_PER_ENCODER_COUNT, 4) + "\n";
  fullReport += "counts/tour actuel: " + String(ENCODER_COUNTS_PER_WHEEL_REV) + "\n\n";
  fullReport += ">>> MESUREZ la distance reelle parcourue avec un metre <<<\n";
  fullReport += ">>> Puis calculez: nouveau mm/count = distance_reelle_mm / " + String(avgCounts) + " <<<\n";
  fullReport += ">>> Et: nouveau GEAR_RATIO = PI*90 / (nouveau_mm_count * 80) <<<\n";

  Serial.println(fullReport);
  lastLog = "Calib: " + String(avgCounts) + " counts, calc=" + String(distCalc, 1) + "mm";
  statusMessage = "Calibration terminee";
}

// ============================================================
// Tests unitaires — calibration actionneurs (4 points PWM)
// ============================================================

void executeMotorResponseMeasurement() {
  statusMessage = "Tests unitaires: calibration...";
  const int actuatorCalibrationPwmValues[4] = {80, 120, 160, 200};
  calibDone = false;
  for (int i = 0; i < 4; i++) {
    calibPoints[i].pwm = actuatorCalibrationPwmValues[i];
    clearWheelEncoderCounters();
    statusMessage = "Calib PWM=" + String(actuatorCalibrationPwmValues[i]);
    commandLeftWheelMotor(actuatorCalibrationPwmValues[i]);
    commandRightWheelMotor(actuatorCalibrationPwmValues[i]);
    delay(1200);
    stopWheelMotors();
    delay(100);
    long L = readLeftWheelEncoderTicks(), R = readRightWheelEncoderTicks();
    calibPoints[i].speedL = fabs((float)L) * (float)MILLIMETERS_PER_ENCODER_COUNT / 1.2f;
    calibPoints[i].speedR = fabs((float)R) * (float)MILLIMETERS_PER_ENCODER_COUNT / 1.2f;
    delay(400);
  }
  calibDone = true;
  statusMessage = "Calibration terminee";
  Serial.println("=== Releve PWM et vitesse roues ===");
  for (int i = 0; i < 4; i++) {
    Serial.print("PWM applique "); Serial.print(calibPoints[i].pwm);
    Serial.print(" | gauche "); Serial.print(calibPoints[i].speedL,0);
    Serial.print(" mm/s | droite "); Serial.print(calibPoints[i].speedR,0); Serial.println(" mm/s releves");
  }
}

// ============================================================
// Movement Functions
// ============================================================

// Pilote chaque roue independamment vers son compteur cible (absolu depuis reset)
// pwmL_cmd et pwmR_cmd sont les PWM appliques quand la roue n'a pas atteint sa cible.
// La roue la plus courte (ou moins rapide) s'arrete en premier; l'autre continue.
void executeRawWheelTickMove(long targL, long targR, int pwmL_cmd, int pwmR_cmd) {
  clearWheelEncoderCounters();
  int  dirL = (targL >= 0) ? 1 : -1;
  int  dirR = (targR >= 0) ? 1 : -1;
  long absL = labs(targL);
  long absR = labs(targR);

  unsigned long t = millis();
  while (millis() - t < 5000) {   // 5s max
    long curL = readLeftWheelEncoderTicks()  * dirL;
    long curR = readRightWheelEncoderTicks() * dirR;
    bool doneL = (absL == 0 || curL >= absL);
    bool doneR = (absR == 0 || curR >= absR);
    if (doneL && doneR) break;
    // Frein actif sur la roue qui a fini (pas de roue libre = pas de dérive)
    if (doneL) releaseLeftWheelMotor();  else commandLeftWheelMotor (dirL * pwmL_cmd);
    if (doneR) releaseRightWheelMotor(); else commandRightWheelMotor(dirR * pwmR_cmd);
    delay(10);
  }
  stopWheelMotorsBriefly();
}

double executeLoggedStraightMove(double distance_mm, int pwmUnused);

// Simple encoder-based move — no PID, both motors at same PWM
void executeSimpleStraightMove(double distance_mm, int pwmUnused) {
  executeLoggedStraightMove(distance_mm, pwmUnused);
}

// Simple encoder-based pivot — positive = left turn, negative = right turn
// Returns measured angle in degrees
double executeGyroEstimatedPivot(double angle_deg, int pwm) {
  clearWheelEncoderCounters();
  double arcMM = (DRIVE_WHEEL_BASE_MM / 2.0) * fabs(angle_deg) * PI / 180.0;
  double targetCounts = arcMM * ENCODER_COUNTS_PER_MILLIMETER * 0.963;
  double rampStart = targetCounts * 0.65;
  int signL = (angle_deg > 0) ? -1 : 1;
  int signR = (angle_deg > 0) ? 1 : -1;
  unsigned long startMs = millis();

  while (millis() - startMs < 3000) {  // 3s timeout
    double avgCounts = (fabs((double)readLeftWheelEncoderTicks()) + fabs((double)readRightWheelEncoderTicks())) / 2.0;
    if (avgCounts >= targetCounts) break;

    // deceleration ramp: slow down in last 35% to reduce overshoot
    int effectivePWM = pwm;
    if (avgCounts > rampStart) {
      double remaining = targetCounts - avgCounts;
      double rampLength = targetCounts - rampStart;
      effectivePWM = (int)(MOTOR_PWM_MINIMUM_VALUE + (pwm - MOTOR_PWM_MINIMUM_VALUE) * (remaining / rampLength));
    }
    effectivePWM = constrain(effectivePWM, MOTOR_PWM_MINIMUM_VALUE, pwm);

    commandLeftWheelMotor(signL * effectivePWM);
    commandRightWheelMotor(signR * effectivePWM);
    delay(10);
  }
  stopWheelMotorsBriefly();

  // compute measured angle from final encoder counts
  long fL = readLeftWheelEncoderTicks(); long fR = readRightWheelEncoderTicks();
  double measuredAngle = ((fabs((double)fL) + fabs((double)fR)) / 2.0) * MILLIMETERS_PER_ENCODER_COUNT / (DRIVE_WHEEL_BASE_MM / 2.0) * 180.0 / PI;
  Serial.print("  Angle calcule durant la rotation : "); Serial.print(measuredAngle, 1); Serial.println(" degres");
  return measuredAngle;
}

// Simple encoder-based line — returns measured distance in mm
double executeLoggedStraightMove(double distance_mm, int pwmUnused) {
  clearWheelEncoderCounters();

  double targetCounts = fabs(distance_mm) * ENCODER_COUNTS_PER_MILLIMETER;
  double rampStart = targetCounts * 0.75;
  int dir = (distance_mm >= 0) ? 1 : -1;
  unsigned long startMs = millis();

  while (millis() - startMs < 7000) {
    long encL = readLeftWheelEncoderTicks();
    long encR = readRightWheelEncoderTicks();

    double absL = fabs((double)encL);
    double absR = fabs((double)encR);
    double avgCounts = (absL + absR) / 2.0;

    if (avgCounts >= targetCounts) break;

    double progress = avgCounts / targetCounts;
    progress = constrain(progress, 0.0, 1.0);

    int pwmBaseL = LEFT_STRAIGHT_REFERENCE_PWM;
    int pwmBaseR = RIGHT_STRAIGHT_REFERENCE_PWM;

    if (avgCounts > rampStart) {
      double remaining = targetCounts - avgCounts;
      double rampLength = targetCounts - rampStart;
      double k = remaining / rampLength;
      k = constrain(k, 0.0, 1.0);

      pwmBaseL = (int)(LEFT_DRIVE_MINIMUM_PWM  + (LEFT_STRAIGHT_REFERENCE_PWM  - LEFT_DRIVE_MINIMUM_PWM)  * k);
      pwmBaseR = (int)(RIGHT_DRIVE_MINIMUM_PWM + (RIGHT_STRAIGHT_REFERENCE_PWM - RIGHT_DRIVE_MINIMUM_PWM) * k);
    }

    pwmBaseL = constrain(pwmBaseL, LEFT_DRIVE_MINIMUM_PWM, 255);
    pwmBaseR = constrain(pwmBaseR, RIGHT_DRIVE_MINIMUM_PWM, 255);

    long e = encL - encR;

    int correction = constrain((int)(e * 0.09), -35, 35);

    int pwmL = pwmBaseL - correction;
    int pwmR = pwmBaseR + correction;

    pwmL = constrain(pwmL, LEFT_DRIVE_MINIMUM_PWM, 255);
    pwmR = constrain(pwmR, RIGHT_DRIVE_MINIMUM_PWM, 255);

    commandLeftWheelMotor(dir * pwmL); 
    commandRightWheelMotor(dir * pwmR);

    server.handleClient();
    delay(10);
  }

  stopWheelMotorsBriefly();

  long fL = readLeftWheelEncoderTicks();
  long fR = readRightWheelEncoderTicks();

  double measuredDistL = fabs((double)fL) * MILLIMETERS_PER_ENCODER_COUNT;
  double measuredDistR = fabs((double)fR) * MILLIMETERS_PER_ENCODER_COUNT;
  double measuredDist = (measuredDistL + measuredDistR) / 2.0;

  Serial.print("  Distance estimee sur le segment : ");
  Serial.print(measuredDist, 1);
  Serial.print(" mm | gauche=");
  Serial.print(measuredDistL, 1);
  Serial.print(" mm droite=");
  Serial.print(measuredDistR, 1);
  Serial.println(" mm effectues");

  fullReport += "Ligne: cible=" + String(fabs(distance_mm), 1)
             + "mm reel=" + String(measuredDist, 1)
             + "mm L=" + String(measuredDistL, 1)
             + "mm R=" + String(measuredDistR, 1)
             + "mm ticksL=" + String(fL)
             + " ticksR=" + String(fR) + "\n";

  return measuredDist;
}

// ============================================================

bool executeClosedLoopStraightMove(double distance_mm, int basePWM) {
  Serial.print("  Ordre de ligne droite par encodeurs : ");
  Serial.print(distance_mm);
  Serial.println(" mm effectues");

  clearWheelEncoderCounters();

  // On utilise les valeurs signees: en avant = compteur positif attendu
  double targetCounts = fabs(distance_mm) * ENCODER_COUNTS_PER_MILLIMETER;
  int direction = (distance_mm >= 0) ? 1 : -1;
  double rampStartCounts = targetCounts * 0.7;

  headingInput = 0;
  headingOutput = 0;
  headingPID.SetMode(AUTOMATIC);
  headingPID.SetOutputLimits(-50, 50);
  headingPID.SetSampleTime(10);

  unsigned long startTime = millis();
  long lastEncL = 0, lastEncR = 0;
  unsigned long lastChangeTime = millis();
  int logCounter = 0;

  while (true) {
    long rawL = readLeftWheelEncoderTicks();
    long rawR = readRightWheelEncoderTicks();

    // Counts dans le sens du mouvement (multiplier par direction)
    long encL = rawL * direction;
    long encR = rawR * direction;

    // Log toutes les 500ms
    logCounter++;
    if (logCounter % 50 == 0) {
      Serial.print("     ticks gauche="); Serial.print(rawL);
      Serial.print(" ticks droite="); Serial.print(rawR);
      Serial.print(" distance="); Serial.print(((fabs((double)rawL) + fabs((double)rawR)) / 2.0) * MILLIMETERS_PER_ENCODER_COUNT, 1);
      Serial.println(" mm effectues");
    }

    // Stall detection
    if (rawL != lastEncL || rawR != lastEncR) {
      lastChangeTime = millis();
      lastEncL = rawL;
      lastEncR = rawR;
    }
    if (millis() - lastChangeTime > ENCODER_STALL_TIMEOUT_MS && millis() - startTime > 300) {
      Serial.println("  !! Arret: encodeurs immobiles");
      stopWheelMotorsBriefly();
      headingPID.SetMode(MANUAL);
      return false;
    }

    if (millis() - startTime > ENCODER_MOVE_TIMEOUT_MS) {
      Serial.println("  !! Arret: temps maximal depasse");
      stopWheelMotorsBriefly();
      headingPID.SetMode(MANUAL);
      return false;
    }

    // Distance = moyenne des valeurs absolues des deux encodeurs
    double avgCounts = (fabs((double)rawL) + fabs((double)rawR)) / 2.0;

    if (avgCounts >= targetCounts) break;

    // Heading correction PID
    headingInput = (double)(rawL - rawR);
    headingPID.Compute();

    // Deceleration ramp
    int effectivePWM = basePWM;
    if (avgCounts > rampStartCounts) {
      double remaining = targetCounts - avgCounts;
      double rampLength = targetCounts - rampStartCounts;
      effectivePWM = (int)(MOTOR_PWM_MINIMUM_VALUE + (basePWM - MOTOR_PWM_MINIMUM_VALUE) * (remaining / rampLength));
    }
    effectivePWM = constrain(effectivePWM, MOTOR_PWM_MINIMUM_VALUE, 255);

    int pwmL = direction * (effectivePWM - (int)headingOutput);
    int pwmR = direction * (effectivePWM + (int)headingOutput);

    commandLeftWheelMotor(pwmL);
    commandRightWheelMotor(pwmR);
    delay(10);
  }

  stopWheelMotorsBriefly();
  headingPID.SetMode(MANUAL);

  long finalL = readLeftWheelEncoderTicks();
  long finalR = readRightWheelEncoderTicks();
  double finalDistL = fabs((double)finalL) * MILLIMETERS_PER_ENCODER_COUNT;
  double finalDistR = fabs((double)finalR) * MILLIMETERS_PER_ENCODER_COUNT;
  double finalAvg = (finalDistL + finalDistR) / 2.0;

  Serial.print("     Valeur visee: "); Serial.print(fabs(distance_mm), 1);
  Serial.print(" mm | mesure gauche: "); Serial.print(finalDistL, 1);
  Serial.print(" mesure droite: "); Serial.print(finalDistR, 1);
  Serial.print(" moyenne obtenue: "); Serial.print(finalAvg, 1);
  Serial.print(" | difference: "); Serial.print(finalAvg - fabs(distance_mm), 1);
  Serial.print(" mm | compteur gauche="); Serial.print(finalL);
  Serial.print(" | compteur droit="); Serial.println(finalR);

  lastLog = "Dist cible=" + String(fabs(distance_mm), 0) + "mm reel=" + String(finalAvg, 1) + "mm";
  fullReport += "Ligne: cible=" + String(fabs(distance_mm), 1) + "mm reel=" + String(finalAvg, 1) + "mm err=" + String(finalAvg - fabs(distance_mm), 1) + "mm (L=" + String(finalL) + " | compteur droit=" + String(finalR) + ")\n";

  delay(200);
  return true;
}

bool executeEncoderCorrectedPivot(double angle_deg, int basePWM) {
  Serial.print("  Ordre de rotation par encodeurs : ");
  Serial.print(angle_deg);
  Serial.println(" degres");

  clearWheelEncoderCounters();

  double absAngleRad = fabs(angle_deg) * PI / 180.0;
  double arcMM = (DRIVE_WHEEL_BASE_MM / 2.0) * absAngleRad;
  double targetCounts = arcMM * ENCODER_COUNTS_PER_MILLIMETER * 0.951; // stop early to compensate motor inertia overshoot

  // Positive angle = left turn: left backward, right forward
  int signL = (angle_deg > 0) ? -1 : 1;
  int signR = (angle_deg > 0) ? 1 : -1;

  double rampStartCounts = targetCounts * 0.7;

  turnInput = 0;
  turnOutput = 0;
  turnPID.SetMode(AUTOMATIC);
  turnPID.SetOutputLimits(-40, 40);
  turnPID.SetSampleTime(10);

  unsigned long startTime = millis();
  long lastEncL = 0, lastEncR = 0;
  unsigned long lastChangeTime = millis();
  int logCounter = 0;

  while (true) {
    long rawL = readLeftWheelEncoderTicks();
    long rawR = readRightWheelEncoderTicks();

    logCounter++;
    if (logCounter % 50 == 0) {
      double currentAngle = ((fabs((double)rawL) + fabs((double)rawR)) / 2.0) * MILLIMETERS_PER_ENCODER_COUNT / (DRIVE_WHEEL_BASE_MM / 2.0) * 180.0 / PI;
      Serial.print("     ticks gauche="); Serial.print(rawL);
      Serial.print(" ticks droite="); Serial.print(rawR);
      Serial.print(" angle="); Serial.print(currentAngle, 1);
      Serial.println(" degres");
    }

    if (rawL != lastEncL || rawR != lastEncR) {
      lastChangeTime = millis();
      lastEncL = rawL;
      lastEncR = rawR;
    }
    if (millis() - lastChangeTime > ENCODER_STALL_TIMEOUT_MS && millis() - startTime > 300) {
      Serial.println("  !! Arret: encodeurs immobiles");
      stopWheelMotorsBriefly();
      turnPID.SetMode(MANUAL);
      return false;
    }

    if (millis() - startTime > ENCODER_MOVE_TIMEOUT_MS) {
      Serial.println("  !! Arret: temps maximal depasse");
      stopWheelMotorsBriefly();
      turnPID.SetMode(MANUAL);
      return false;
    }

    double avgCounts = (fabs((double)rawL) + fabs((double)rawR)) / 2.0;

    if (avgCounts >= targetCounts) break;

    turnInput = fabs((double)rawL) - fabs((double)rawR);
    turnPID.Compute();

    int effectivePWM = basePWM;
    if (avgCounts > rampStartCounts) {
      double remaining = targetCounts - avgCounts;
      double rampLength = targetCounts - rampStartCounts;
      effectivePWM = (int)(MOTOR_PWM_MINIMUM_VALUE + (basePWM - MOTOR_PWM_MINIMUM_VALUE) * (remaining / rampLength));
    }
    effectivePWM = constrain(effectivePWM, MOTOR_PWM_MINIMUM_VALUE, 255);

    int pwmL = signL * (effectivePWM - (int)turnOutput);
    int pwmR = signR * (effectivePWM + (int)turnOutput);

    commandLeftWheelMotor(pwmL);
    commandRightWheelMotor(pwmR);
    delay(10);
  }

  stopWheelMotorsBriefly();
  turnPID.SetMode(MANUAL);

  long finalL = readLeftWheelEncoderTicks();
  long finalR = readRightWheelEncoderTicks();
  double finalArcL = fabs((double)finalL) * MILLIMETERS_PER_ENCODER_COUNT;
  double finalArcR = fabs((double)finalR) * MILLIMETERS_PER_ENCODER_COUNT;
  double finalAngle = ((finalArcL + finalArcR) / 2.0) / (DRIVE_WHEEL_BASE_MM / 2.0) * 180.0 / PI;

  Serial.print("     Valeur visee: "); Serial.print(fabs(angle_deg), 1);
  Serial.print(" degres | angle obtenu: "); Serial.print(finalAngle, 1);
  Serial.print(" degres | difference: "); Serial.print(finalAngle - fabs(angle_deg), 1);
  Serial.print(" degres | compteur gauche="); Serial.print(finalL);
  Serial.print(" | compteur droit="); Serial.println(finalR);

  lastLog = "Angle cible=" + String(fabs(angle_deg), 0) + " reel=" + String(finalAngle, 1) + "deg";
  fullReport += "Pivot: cible=" + String(fabs(angle_deg), 1) + "deg reel=" + String(finalAngle, 1) + "deg err=" + String(finalAngle - fabs(angle_deg), 1) + "deg (L=" + String(finalL) + " | compteur droit=" + String(finalR) + ")\n";

  delay(300);
  return true;
}

// ============================================================
// Corner A→B  (hardcode depuis enregistrement manuel)
// ============================================================
//
// Enregistrement ref: 154pts, stylo de A (fin ligne 1) vers B.
// Deux phases extraites:
//   Phase 1 — Pivot: roue D avance 604cts, roue G en roue libre (-17cts naturel)
//   Phase 2 — Arc  : roue D +406cts, roue G +158cts (ratio D/G = 2.57)
//
// Pour ajuster: modifier ENC_R_PIVOT, ENC_R_ARC, ENC_L_ARC
#define CORNER_AB_RIGHT_PIVOT_TICKS  604   // counts roue D, phase pivot
#define CORNER_AB_RIGHT_ARC_TICKS    406   // counts roue D, phase arc
#define CORNER_AB_LEFT_ARC_TICKS    158   // counts roue G, phase arc
#define CORNER_AB_PIVOT_PWM    150   // PWM roue D pendant pivot (G en roue libre)
#define CORNER_AB_RIGHT_ARC_PWM    150   // PWM roue D pendant arc
#define CORNER_AB_LEFT_ARC_PWM     60   // PWM roue G pendant arc (~MOTOR_PWM_MINIMUM_VALUE)

void executeTwoStageCornerTrace() {
  // ── Phase 1: Pivot roue droite uniquement ─────────────────────────
  //
  // La roue gauche est trop difficile a faire bouger en micro-mouvement
  // (friction statique trop forte meme a 220 PWM).
  // Solution: on la laisse en roue libre (PWM=0), la friction la maintient
  // naturellement a ~0. Seule la roue droite pivote.
  // Pas de bruit, pas de kicks, comportement propre.

  Serial.println("  [Virage A vers B] Phase initiale avec roue droite");
  clearWheelEncoderCounters();

  unsigned long t1 = millis();

  while (millis() - t1 < 5000) {
    long encR = readRightWheelEncoderTicks();

    if (encR >= CORNER_AB_RIGHT_PIVOT_TICKS) break;

    commandLeftWheelMotor(0);
    commandRightWheelMotor(CORNER_AB_PIVOT_PWM);
    server.handleClient();
    delay(10);
  }
  stopWheelMotorsBriefly();

  long p1L = readLeftWheelEncoderTicks(), p1R = readRightWheelEncoderTicks();
  Serial.print("    Sortie phase 1 : gauche="); Serial.print(p1L);
  Serial.print(" | droite="); Serial.print(p1R);
  Serial.print("  (but droite="); Serial.print(CORNER_AB_RIGHT_PIVOT_TICKS); Serial.println(" fin)");
  fullReport += "  executeTwoStageCornerTrace P1: L=" + String(p1L)
              + " | compteur droit=" + String(p1R)
              + "  (but droite=" + String(CORNER_AB_RIGHT_PIVOT_TICKS) + ")\n";
  delay(150);

  // ── Phase 2: Arc — les deux roues ────────────────────────────────
  // Ratio D/G = 406/158 = 2.57  →  D a PWM_STRAIGHT, G a MOTOR_PWM_MINIMUM_VALUE
  Serial.println("  [Virage A vers B] Phase finale avec deux roues");
  executeRawWheelTickMove(CORNER_AB_LEFT_ARC_TICKS, CORNER_AB_RIGHT_ARC_TICKS,
              CORNER_AB_LEFT_ARC_PWM, CORNER_AB_RIGHT_ARC_PWM);

  long p2L = readLeftWheelEncoderTicks(), p2R = readRightWheelEncoderTicks();
  Serial.print("    Sortie phase 2 : gauche="); Serial.print(p2L);
  Serial.print(" (but "); Serial.print(CORNER_AB_LEFT_ARC_TICKS); Serial.print(" fin)");
  Serial.print(" | droite="); Serial.print(p2R);
  Serial.print(" (but "); Serial.print(CORNER_AB_RIGHT_ARC_TICKS); Serial.println(" fin)");
  fullReport += "  executeTwoStageCornerTrace P2: L=" + String(p2L) + " | compteur droit=" + String(p2R)
              + "  (cibles L=" + String(CORNER_AB_LEFT_ARC_TICKS)
              + "  R=" + String(CORNER_AB_RIGHT_ARC_TICKS) + ")\n";
}

// ============================================================
// Parametres pivot — modifiables via /config sans re-upload
// ============================================================
// Valeurs par defaut (remplacer par les valeurs trouvees en test)
// Parametres pivot — 2 blocs, reglables via /config
// Differentiel auto : roue courte ralentie proportionnellement,
// clamped a shortWheelMinPwm si en dessous du seuil physique.
// Quand la roue courte a fini, elle est freinee (pas de derive).

int   shortWheelMinPwm =  125;  // PWM minimum roue courte (reglable via /config)

int stairBlock1LeftTicks   = -270;
int stairBlock1RightTicks   =  410;
int stairBlock1Pwm  =  220;

int stairBlock2LeftTicks   =  270;
int stairBlock2RightTicks   =  1180;
int stairBlock2Pwm  =  205;

// Bloc 3: alignement vers la derniere ligne.
// Base de depart volontairement douce: la roue gauche avance un peu plus
// vite/loin que la droite pour engager une courbe vers la droite.
int   stairBlock3LeftTicks   =  943;  // Bloc 3 roue G
int   stairBlock3RightTicks   =  574;  // Bloc 3 roue D
int   stairBlock3Pwm  =  222;  // Bloc 3 PWM max (roue longue)
bool  stairBlock3Active = true;

int   stairBlock4LeftTicks   =  512;  // Bloc 4 roue G
int   stairBlock4RightTicks   =  399;  // Bloc 4 roue D
int   stairBlock4Pwm  =  214;  // Bloc 4 PWM max
bool  stairBlock4Active = true;

int   stairBlock5LeftTicks   =  500;  // Bloc 5 roue G
int   stairBlock5RightTicks   =  400;  // Bloc 5 roue D
int   stairBlock5Pwm  =  200;  // Bloc 5 PWM max
bool  stairBlock5Active = true;

int   stairBlock6LeftTicks   =  400;  // Bloc 6 roue G
int   stairBlock6RightTicks   =  350;  // Bloc 6 roue D
int   stairBlock6Pwm  =  200;  // Bloc 6 PWM max
bool  stairBlock6Active = true;

int   stairBlock7LeftTicks   =  420;  // Bloc 7 roue G
int   stairBlock7RightTicks   =  374;  // Bloc 7 roue D
int   stairBlock7Pwm  =  200;  // Bloc 7 PWM max
bool  stairBlock7Active = true;

int   stairBlock8LeftTicks   =  400;  // Bloc 8 roue G
int   stairBlock8RightTicks   =  379;  // Bloc 8 roue D
int   stairBlock8Pwm  =  200;  // Bloc 8 PWM max
bool  stairBlock8Active = true;

int   stairBlock9LeftTicks   =  400;  // Bloc 9 roue G
int   stairBlock9RightTicks   =  382;  // Bloc 9 roue D
int   stairBlock9Pwm  =  200;  // Bloc 9 PWM max
bool  stairBlock9Active = true;

int   stairBlock10LeftTicks  =  400;  // Bloc 10 roue G
int   stairBlock10RightTicks  =  379;  // Bloc 10 roue D
int   stairBlock10Pwm =  200;  // Bloc 10 PWM max
bool  stairBlock10Active = true;

float stairFinalLineMm        = 50;  // mm
bool  stairFinalLineActive = true;   // activer via /config

// ============================================================
// SEQUENCE 3 - FLECHE NORD
// ============================================================

float northArrowNorthOffsetDeg = 0.0f;
float northArrowShaftMm         = 95.0f;
int   northArrowArc1LeftTicks          = 290;
int   northArrowArc1RightTicks          = 580;
int   northArrowArc1Pwm         = 220;
int   northArrowArc2LeftTicks          = 580;
int   northArrowArc2RightTicks          = 290;
int   northArrowArc2Pwm         = 220;
int   northArrowArc3LeftTicks          = -290;
int   northArrowArc3RightTicks          = -580;
int   northArrowArc3Pwm         = 220;
int   northArrowArc4LeftTicks          = -560;
int   northArrowArc4RightTicks          = -280;
int   northArrowArc4Pwm         = 220;

int   northArrowTurnBasePwm    = 175;
int   northArrowTurnSmallPwm   = 205;
int   northArrowTurnKickPwm    = 235;
int   northArrowTurnKickMs     = 110;
float northArrowMinTurnDeg     = 14.0f;

double limitFloatingPointValue(double v, double lo, double hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

float wrapDegreesToMinusPlus180(float angleDeg) {
  while (angleDeg > 180.0f) angleDeg -= 360.0f;
  while (angleDeg < -180.0f) angleDeg += 360.0f;
  return angleDeg;
}



float calculateMagneticHeadingDegrees() {
  if (!magOk) return 0.0f;
  if (fabs(magX) < 0.001f && fabs(magY) < 0.001f) return 0.0f;
  return wrapDegreesToMinusPlus180(atan2f(magY, magX) * 180.0f / PI);
}

double wrapRadiansToMinusPlusPi(double a) {
  while (a > PI) a -= 2.0 * PI;
  while (a < -PI) a += 2.0 * PI;
  return a;
}

float calculateAveragedMagneticHeading(int samples = 28, int delayMs = 25) {
  if (!magOk) return 0.0f;

  float sumX = 0.0f;
  float sumY = 0.0f;
  int valid = 0;

  for (int i = 0; i < samples; i++) {
    refreshRobotSensors();
    if (fabs(magX) < 0.001f && fabs(magY) < 0.001f) {
      delay(delayMs);
      continue;
    }
    float a = atan2f(magY, magX);
    sumX += cosf(a);
    sumY += sinf(a);
    valid++;
    delay(delayMs);
  }

  if (valid == 0) return 0.0f;
  return wrapDegreesToMinusPlus180(atan2f(sumY, sumX) * 180.0f / PI);
}

float measureSettledMagneticHeading(float* spreadDeg = nullptr, int samples = 24, int delayMs = 18) {
  if (!magOk) return 0.0f;

  stopWheelMotors();
  delay(140);

  float values[32];
  int valid = 0;
  float refDeg = 0.0f;
  bool refSet = false;

  for (int i = 0; i < samples && valid < 32; i++) {
    refreshRobotSensors();
    float h = calculateMagneticHeadingDegrees();
    if (!refSet) {
      refDeg = h;
      refSet = true;
    }
    values[valid++] = refDeg + wrapDegreesToMinusPlus180(h - refDeg);
    delay(delayMs);
  }

  if (valid == 0) {
    if (spreadDeg) *spreadDeg = 999.0f;
    return 0.0f;
  }

  for (int i = 0; i < valid - 1; i++) {
    for (int j = i + 1; j < valid; j++) {
      if (values[j] < values[i]) {
        float tmp = values[i];
        values[i] = values[j];
        values[j] = tmp;
      }
    }
  }

  int trim = max(1, valid / 5);
  int start = trim;
  int end = valid - trim;
  if (end <= start) {
    start = 0;
    end = valid;
  }

  float sum = 0.0f;
  for (int i = start; i < end; i++) sum += values[i];
  float avgDeg = sum / (float)(end - start);
  float spanDeg = values[end - 1] - values[start];

  if (spreadDeg) *spreadDeg = spanDeg;
  return wrapDegreesToMinusPlus180(avgDeg);
}

void applyPivotKickPulse(float angleDeg, int pwm, int kickMs) {
  int signL = (angleDeg > 0.0f) ? -1 : 1;
  int signR = (angleDeg > 0.0f) ? 1 : -1;
  commandLeftWheelMotor(signL * pwm);
  commandRightWheelMotor(signR * pwm);
  delay(kickMs);
  stopWheelMotors();
  delay(35);
}

bool alignRobotWithMagneticHeading(float targetDeg, float* finalHeadingDeg = nullptr, float* finalErrorDeg = nullptr) {
  if (!magOk) return false;

  float spreadDeg = 0.0f;
  float headingDeg = measureSettledMagneticHeading(&spreadDeg);
  float errorDeg = wrapDegreesToMinusPlus180(targetDeg - headingDeg);
  unsigned long startMs = millis();
  float stepGain = 0.72f;
  float bestHeadingDeg = headingDeg;
  float bestErrorDeg = errorDeg;

  fullReport += "OrientationNord STOP&MEASURE: depart=" + String(headingDeg, 1)
             + "deg cible=" + String(targetDeg, 1)
             + "deg err=" + String(errorDeg, 1)
             + "deg spread=" + String(spreadDeg, 1) + "deg\n";

  for (int attempt = 0; attempt < 10 && millis() - startMs < 9000; attempt++) {
    float absErr = fabs(errorDeg);

    if (absErr <= 7.0f && spreadDeg <= 8.0f) {
      if (finalHeadingDeg) *finalHeadingDeg = headingDeg;
      if (finalErrorDeg) *finalErrorDeg = errorDeg;
      fullReport += "OrientationNord STOP&MEASURE: OK heading=" + String(headingDeg, 1)
                 + "deg err=" + String(errorDeg, 1)
                 + "deg spread=" + String(spreadDeg, 1) + "deg\n";
      return true;
    }

    if (fabs(errorDeg) < fabs(bestErrorDeg)) {
      bestErrorDeg = errorDeg;
      bestHeadingDeg = headingDeg;
    }

    float maxStepDeg = (absErr > 120.0f) ? 60.0f : (absErr > 60.0f ? 45.0f : 28.0f);
    float cmdDeg = constrain(-errorDeg * stepGain, -maxStepDeg, maxStepDeg);
    float stepDeg = fabs(cmdDeg);
    if (fabs(cmdDeg) < northArrowMinTurnDeg) {
      cmdDeg = (cmdDeg >= 0.0f ? 1.0f : -1.0f) * northArrowMinTurnDeg;
    }
    int pivotPwm = (absErr < 25.0f) ? northArrowTurnSmallPwm : northArrowTurnBasePwm;

    fullReport += "  tentative " + String(attempt + 1)
               + ": heading=" + String(headingDeg, 1)
               + " err=" + String(errorDeg, 1)
               + " spread=" + String(spreadDeg, 1)
               + " cmd=" + String(cmdDeg, 1)
               + "deg pwm=" + String(pivotPwm) + "\n";

    if (stepDeg <= 18.0f) {
      fullReport += "    kick arret pwm=" + String(northArrowTurnKickPwm)
                 + " ms=" + String(northArrowTurnKickMs) + "\n";
      applyPivotKickPulse(cmdDeg, northArrowTurnKickPwm, northArrowTurnKickMs);
    }

    double measuredPivotDeg = executeGyroEstimatedPivot(cmdDeg, pivotPwm);
    delay(180);

    float newSpreadDeg = 0.0f;
    float newHeadingDeg = measureSettledMagneticHeading(&newSpreadDeg);
    float newErrorDeg = wrapDegreesToMinusPlus180(targetDeg - newHeadingDeg);

    fullReport += "    apres arret: heading=" + String(newHeadingDeg, 1)
               + " err=" + String(newErrorDeg, 1)
               + " spread=" + String(newSpreadDeg, 1)
               + " pivotMesure=" + String(measuredPivotDeg, 1) + "deg\n";

    if (fabs(newErrorDeg) > fabs(errorDeg) + 4.0f) {
      stepGain = max(0.45f, stepGain - 0.10f);
      fullReport += "    correction moins efficace -> reduction stepGain=" + String(stepGain, 2) + "\n";
    } else if (fabs(newErrorDeg) < fabs(errorDeg) - 2.0f) {
      stepGain = min(0.80f, stepGain + 0.04f);
    }

    headingDeg = newHeadingDeg;
    errorDeg = newErrorDeg;
    spreadDeg = newSpreadDeg;
    server.handleClient();
  }

  stopWheelMotors();
  if (fabs(bestErrorDeg) < fabs(errorDeg)) {
    headingDeg = bestHeadingDeg;
    errorDeg = bestErrorDeg;
  }
  if (finalHeadingDeg) *finalHeadingDeg = headingDeg;
  if (finalErrorDeg) *finalErrorDeg = errorDeg;
  fullReport += "OrientationNord STOP&MEASURE: TIMEOUT heading=" + String(headingDeg, 1)
             + "deg err=" + String(errorDeg, 1)
             + "deg spread=" + String(spreadDeg, 1) + "deg\n";
  return fabs(errorDeg) <= 10.0f;
}

// ============================================================
// Blocs primitifs — réglage manuel roues
// ============================================================
// Utiliser ces blocs dans executeStairDrawingSequence() pour construire
// le pivot pas à pas. Modifier les constantes dans la séquence.

// Bloc A: roue droite uniquement (gauche en roue libre)
void executeRightWheelOnlyTurn(long counts, int pwm) {
  String label = "  [Tour roue droite] objectif ticks " + String(counts) + " avec PWM " + String(pwm);
  Serial.println(label);
  clearWheelEncoderCounters();
  int  dir = (counts >= 0) ? 1 : -1;
  long abs_c = labs(counts);
  unsigned long t = millis();
  while (millis() - t < 8000) {
    if ((readRightWheelEncoderTicks() * dir) >= (long)abs_c) break;
    commandLeftWheelMotor(0);
    commandRightWheelMotor(dir * pwm);
    server.handleClient();
    delay(10);
  }
  stopWheelMotorsBriefly();
  long rD = readRightWheelEncoderTicks(), rL = readLeftWheelEncoderTicks();
  Serial.print("    resultat droite "); Serial.print(rD);
  Serial.print(" | gauche "); Serial.println(rL);
  fullReport += label + " -> D=" + String(rD) + " | gauche " + String(rL) + "\n";
}

// Bloc B: roue gauche uniquement (droite en roue libre)
void executeLeftWheelOnlyTurn(long counts, int pwm) {
  String label = "  [Tour roue gauche] objectif ticks " + String(counts) + " avec PWM " + String(pwm);
  Serial.println(label);
  clearWheelEncoderCounters();
  int  dir = (counts >= 0) ? 1 : -1;
  long abs_c = labs(counts);
  unsigned long t = millis();
  while (millis() - t < 8000) {
    if ((readLeftWheelEncoderTicks() * dir) >= (long)abs_c) break;
    commandLeftWheelMotor(dir * pwm);
    commandRightWheelMotor(0);
    server.handleClient();
    delay(10);
  }
  stopWheelMotorsBriefly();
  long rL = readLeftWheelEncoderTicks(), rD = readRightWheelEncoderTicks();
  Serial.print("    resultat gauche "); Serial.print(rL);
  Serial.print(" | droite "); Serial.println(rD);
  fullReport += label + " -> G=" + String(rL) + " D=" + String(rD) + "\n";
}

// Bloc C: deux roues — différentiel automatique.
// Si le ratio demande un PWM trop faible pour la roue courte, on ne la force pas
// a tourner en continu. A la place, on la pilote par impulsions a shortWheelMinPwm
// pour suivre sa progression-cible sans partir quasi a la meme vitesse que l'autre.
void executeDifferentialWheelTickBlock(long countsL, long countsR, int pwm, long* outL = nullptr, long* outR = nullptr) {
  long absL = labs(countsL);
  long absR = labs(countsR);
  long maxC = max(absL, absR);
  if (maxC == 0) return;

  int dirL = (countsL >= 0) ? 1 : -1;
  int dirR = (countsR >= 0) ? 1 : -1;

  // Formule nominale : roue courte = PWM × (distance courte / distance longue)
  int pwmL_nom = (absL == 0) ? 0 : (int)((long)pwm * absL / maxC);
  int pwmR_nom = (absR == 0) ? 0 : (int)((long)pwm * absR / maxC);

  bool pulseL = (absL > 0 && pwmL_nom > 0 && pwmL_nom < shortWheelMinPwm);
  bool pulseR = (absR > 0 && pwmR_nom > 0 && pwmR_nom < shortWheelMinPwm);
  int pwmL = pulseL ? shortWheelMinPwm : pwmL_nom;
  int pwmR = pulseR ? shortWheelMinPwm : pwmR_nom;

  String label = "  [Bloc deux roues] gauche " + String(countsL) + " droite " + String(countsR)
               + " pwm limite " + String(pwm)
               + " pwm gauche " + String(pwmL)
               + (pulseL ? " pulse" : "")
               + " pwm droite " + String(pwmR)
               + (pulseR ? " pulse" : "")
                ;
  Serial.println(label);

  // Chaque roue s'arrete quand elle a atteint sa cible.
  // En mode "pulse", la roue courte ne recoit que des impulsions quand elle
  // est en retard par rapport a sa progression theorique.
  clearWheelEncoderCounters();
  unsigned long t0 = millis();
  while (millis() - t0 < 8000) {
    long curL = readLeftWheelEncoderTicks()  * dirL;
    long curR = readRightWheelEncoderTicks() * dirR;
    const long wheelBlockCompletionToleranceTicks = 35;

    bool doneL = (absL == 0 || curL >= absL - wheelBlockCompletionToleranceTicks);
    bool doneR = (absR == 0 || curR >= absR - wheelBlockCompletionToleranceTicks);

if (doneL && doneR) break;
    if (doneL && doneR) break;

    int cmdL = 0;
    int cmdR = 0;

    if (!doneL && pwmL > 0) {
      if (!pulseL) {
        cmdL = dirL * pwmL;
      } else {
        float progressL = (absL > 0) ? (float)curL / (float)absL : 1.0f;
        float progressR = (absR > 0) ? (float)curR / (float)absR : 1.0f;
        if (progressL <= progressR + 0.02f) cmdL = dirL * pwmL;
      }
    }

    if (!doneR && pwmR > 0) {
      if (!pulseR) {
        cmdR = dirR * pwmR;
      } else {
        float progressR = (absR > 0) ? (float)curR / (float)absR : 1.0f;
        float progressL = (absL > 0) ? (float)curL / (float)absL : 1.0f;
        if (progressR <= progressL + 0.02f) cmdR = dirR * pwmR;
      }
    }

    if (doneL) {
      releaseLeftWheelMotor();
    } else {
      commandLeftWheelMotor(cmdL);
    }

    if (doneR) {
      releaseRightWheelMotor();
    } else {
      commandRightWheelMotor(cmdR);
    }

    server.handleClient();
    delay(10);
  }
  stopWheelMotorsBriefly();

  long rL = readLeftWheelEncoderTicks(), rD = readRightWheelEncoderTicks();
  if (outL) *outL = rL;
  if (outR) *outR = rD;
  Serial.print("    resultat gauche "); Serial.print(rL);
  Serial.print(" | droite "); Serial.println(rD);
  fullReport += label + " -> G=" + String(rL) + " D=" + String(rD) + "\n";
}

// ============================================================
// Sequence 1
// ============================================================
// Structure:
//   [1] Ligne droite 20 cm
//   [2] PIVOT — blocs à régler manuellement (voir constantes ci-dessous)
//   [3] Ligne finale — décommenter quand le pivot est calé
//
// Pour régler le pivot:
//   1. Uploader, lancer Start
//   2. Regarder /results pour les counts réels de chaque bloc
//   3. Ajuster les constantes PIVOT_* ci-dessous, re-uploader, retester
//   4. Quand satisfait, décommenter le bloc [3]

void executeStairDrawingSequence() {
  Serial.println("----------------------------------------");
  Serial.println("  Trace numero un - escalier");
  Serial.println("----------------------------------------");

  statusMessage = "Seq1 en cours...";
  fullReport = "=== SEQUENCE 1 - GAUCHE PUIS LIGNE 40CM ===\n";

  digitalWrite(ROBOT_STATUS_LED_PIN, HIGH);
  digitalWrite(ROBOT_READY_LED_PIN, LOW);

  // [1] Ligne droite 20 cm
  Serial.println("[1] Segment de depart sur 20 cm");
  statusMessage = "[1] Ligne 20cm";

  double d1 = executeLoggedStraightMove(200.0, STRAIGHT_MOVE_REFERENCE_PWM);
  fullReport += "Ligne20cm: reel=" + String(d1, 1) + "mm\n";
  delay(300);

  // [2] Virage gauche 90 deg
  Serial.println("[2] Mise en forme du virage gauche");
  statusMessage = "[2] Virage gauche 90";

  fullReport += "--- VIRAGE GAUCHE 90 ---\n";
  executeRightWheelOnlyTurn(410, 220);
  fullReport += "--- VIRAGE GAUCHE 90 fin ---\n";
  delay(300);

  // [3] Ligne droite 40 cm directement apres le virage gauche
  Serial.println("[3] Grande ligne de sortie sur 40 cm");
  statusMessage = "[3] Ligne 40cm";

  double d2 = executeLoggedStraightMove(400.0, STRAIGHT_MOVE_REFERENCE_PWM);
  fullReport += "Ligne40cm: reel=" + String(d2, 1) + "mm\n";
  delay(300);

  stopWheelMotorsBriefly();
  delay(300);

  digitalWrite(ROBOT_STATUS_LED_PIN, LOW);
  digitalWrite(ROBOT_READY_LED_PIN, HIGH);

  statusMessage = "Seq1 terminee";
  fullReport += "=== FIN SEQUENCE 1 ===\n";

  Serial.println("----------------------------------------");
  Serial.println("  Escalier termine");
  Serial.println("----------------------------------------");
}

int calculateLeftCircleWheelPwm(double speedMmS) {
  double s = fabs(speedMmS);

  if (s < CIRCLE_SPEED_DEADBAND_MM_PER_S) return 0;

  double ratio = s / CIRCLE_WHEEL_SPEED_AT_MAX_PWM_MM_PER_S;
  ratio = limitFloatingPointValue(ratio, 0.0, 1.0);

static const int CIRCLE_LEFT_SPEED_MODEL_MINIMUM_PWM = 225;

int pwm = (int)(CIRCLE_LEFT_SPEED_MODEL_MINIMUM_PWM + ratio * (CIRCLE_LEFT_MAXIMUM_PWM - CIRCLE_LEFT_SPEED_MODEL_MINIMUM_PWM));
return constrain(pwm, CIRCLE_LEFT_SPEED_MODEL_MINIMUM_PWM, CIRCLE_LEFT_MAXIMUM_PWM);
}

int calculateRightCircleWheelPwm(double speedMmS) {
  double s = fabs(speedMmS);

  if (s < CIRCLE_SPEED_DEADBAND_MM_PER_S) return 0;

  double ratio = s / CIRCLE_WHEEL_SPEED_AT_MAX_PWM_MM_PER_S;
  ratio = limitFloatingPointValue(ratio, 0.0, 1.0);

static const int CIRCLE_RIGHT_SPEED_MODEL_MINIMUM_PWM = 145;

int pwm = (int)(CIRCLE_RIGHT_SPEED_MODEL_MINIMUM_PWM + ratio * (CIRCLE_RIGHT_OUTPUT_MAXIMUM_PWM - CIRCLE_RIGHT_SPEED_MODEL_MINIMUM_PWM));
return constrain(pwm, CIRCLE_RIGHT_SPEED_MODEL_MINIMUM_PWM, CIRCLE_RIGHT_OUTPUT_MAXIMUM_PWM);
}

void commandCircleWheelPair(double vLeftMmS, double vRightMmS) {
  int pwmL = calculateLeftCircleWheelPwm(vLeftMmS);
  int pwmR = calculateRightCircleWheelPwm(vRightMmS);

  if (pwmL == 0) {
    commandLeftWheelMotor(0);
  } else {
    commandLeftWheelMotor((vLeftMmS >= 0.0) ? pwmL : -pwmL);
  }

  if (pwmR == 0) {
    commandRightWheelMotor(0);
  } else {
    commandRightWheelMotor((vRightMmS >= 0.0) ? pwmR : -pwmR);
  }
}


void executeTwoWheelCircleTrace(long targetLeftTicks, long targetRightTicks,
                          int basePwmLeft, int basePwmRight,
                          long* outL = nullptr, long* outR = nullptr) {
  clearWheelEncoderCounters();

  long absL = labs(targetLeftTicks);
  long absR = labs(targetRightTicks);

  int dirL = (targetLeftTicks >= 0) ? 1 : -1;
  int dirR = (targetRightTicks >= 0) ? 1 : -1;

  bool rightIsOuter = (absR >= absL);

  long outerTarget = rightIsOuter ? absR : absL;
  long innerTarget = rightIsOuter ? absL : absR;

  float ratioInnerOuter = 1.0f;
  if (outerTarget > 0) {
    ratioInnerOuter = (float)innerTarget / (float)outerTarget;
  }

  unsigned long t0 = millis();

  fullReport += "--- DRIVE CIRCLE TWO WHEELS MASTER ---\n";
  fullReport += "TargetL=" + String(targetLeftTicks)
             + " TargetR=" + String(targetRightTicks) + "\n";
  fullReport += "Outer=" + String(rightIsOuter ? "RIGHT" : "LEFT") + "\n";
  fullReport += "Ratio inner/outer=" + String(ratioInnerOuter, 3) + "\n";
  fullReport += "BasePWM L=" + String(basePwmLeft)
             + " | compteur droit=" + String(basePwmRight) + "\n";

  while (millis() - t0 < CIRCLE_MOVE_TIMEOUT_MS) {
    server.handleClient();

    if (stopRequested) {
      fullReport += "ARRET: stop demande\n";
      break;
    }

    long curL = labs(readLeftWheelEncoderTicks());
    long curR = labs(readRightWheelEncoderTicks());

    long curOuter = rightIsOuter ? curR : curL;
    long curInner = rightIsOuter ? curL : curR;

    if (curOuter >= outerTarget) {
      // Le cercle est théoriquement terminé.
      // On laisse une très courte marge déjà intégrée par CIRCLE_CLOSURE_CALIBRATION_FACTOR.
      break;
    }

    // Position théorique de la roue intérieure en fonction de l’avancement extérieur
    long expectedInner = (long)(ratioInnerOuter * (float)curOuter);

    // Erreur positive : roue intérieure en retard
    // Erreur négative : roue intérieure en avance
    long innerError = expectedInner - curInner;

    int pwmOuter = rightIsOuter ? basePwmRight : basePwmLeft;
    int pwmInner = rightIsOuter ? basePwmLeft : basePwmRight;

    // Correction douce de la roue intérieure
    int innerCorr = constrain((int)(innerError * 0.08), -25, 35);

    int cmdInner = pwmInner + innerCorr;
    int cmdOuter = pwmOuter;

    // Si la roue intérieure est trop en avance, on ralentit l’extérieur aussi
    // au lieu de laisser le robot pivoter brutalement.
    if (innerError < -80) {
      cmdOuter -= 20;
    }

    if (innerError < -160) {
      cmdOuter -= 35;
    }

    // Si la roue intérieure est très en avance, elle peut être coupée brièvement,
    // mais pas définitivement : dès que l’extérieur rattrape, elle repart.
    bool innerTooFarAhead = (innerError < -120);

    if (rightIsOuter) {
      cmdOuter = constrain(cmdOuter, CIRCLE_RIGHT_MINIMUM_PWM, CIRCLE_RIGHT_REFERENCE_MAXIMUM_PWM);
      cmdInner = constrain(cmdInner, LEFT_DRIVE_MINIMUM_PWM, 255);

      // roue droite extérieure
      commandRightWheelMotor(dirR * cmdOuter);

      // roue gauche intérieure
      if (innerTooFarAhead) {
        commandLeftWheelMotor(0);
      } else {
        commandLeftWheelMotor(dirL * cmdInner);
      }
    } else {
      cmdOuter = constrain(cmdOuter, LEFT_DRIVE_MINIMUM_PWM, 255);
      cmdInner = constrain(cmdInner, CIRCLE_RIGHT_MINIMUM_PWM, CIRCLE_RIGHT_REFERENCE_MAXIMUM_PWM);

      // roue gauche extérieure
      commandLeftWheelMotor(dirL * cmdOuter);

      // roue droite intérieure
      if (innerTooFarAhead) {
        commandRightWheelMotor(0);
      } else {
        commandRightWheelMotor(dirR * cmdInner);
      }
    }

    delay(10);
  }

  stopWheelMotors();
  delay(250);

  long finalL = readLeftWheelEncoderTicks();
  long finalR = readRightWheelEncoderTicks();

  if (outL) *outL = finalL;
  if (outR) *outR = finalR;

  fullReport += "Fin circle master: L=" + String(finalL)
             + " | compteur droit=" + String(finalR) + "\n";
}


void executeCircleDrawingSequence() {
  Serial.println("----------------------------------------");
  Serial.println("  Trace numero deux - cercle reglable");
  Serial.println("----------------------------------------");

  statusMessage = "Seq2 cercle en cours...";
  fullReport = "=== SEQUENCE 2 - CERCLE DEUX ROUES STRICT ===\n";

  digitalWrite(ROBOT_STATUS_LED_PIN, HIGH);
  digitalWrite(ROBOT_READY_LED_PIN, LOW);

  stopRequested = false;

  double radiusCm = circleRadiusCm;
  radiusCm = limitFloatingPointValue(radiusCm, CIRCLE_RADIUS_MINIMUM_CM, CIRCLE_RADIUS_MAXIMUM_CM);

  double radiusPenMmAsked = radiusCm * 10.0;
  double radiusPenMm = radiusPenMmAsked * CIRCLE_RADIUS_CALIBRATION_FACTOR;

  fullReport += "Rayon demande GUI=" + String(circleRadiusCm, 2) + " cm\n";
  fullReport += "Rayon utilise=" + String(radiusPenMm / 10.0, 2) + " cm\n";
  fullReport += "Rayon stylo mm=" + String(radiusPenMm, 1) + "\n";
  fullReport += "Pen offset mm=" + String(FRONT_PEN_OFFSET_MM, 1) + "\n";

  double minPhysicalRadiusMm = sqrt(
    FRONT_PEN_OFFSET_MM * FRONT_PEN_OFFSET_MM +
    (DRIVE_WHEEL_BASE_MM / 2.0) * (DRIVE_WHEEL_BASE_MM / 2.0)
  );

  fullReport += "Rayon minimum physique deux roues="
             + String(minPhysicalRadiusMm / 10.0, 2) + " cm\n";

  if (radiusPenMm <= minPhysicalRadiusMm) {
    fullReport += "ERREUR: rayon trop petit pour faire tourner les deux roues proprement.\n";
    fullReport += "Solution: demander au moins "
               + String(minPhysicalRadiusMm / 10.0, 2)
               + " cm, idealement 14 cm ou plus.\n";

    stopWheelMotorsBriefly();
    digitalWrite(ROBOT_STATUS_LED_PIN, LOW);
    digitalWrite(ROBOT_READY_LED_PIN, HIGH);
    statusMessage = "Seq2 impossible: rayon trop petit";
    return;
  }

  // Géométrie :
  // R_stylo^2 = R_centre^2 + offset_stylo^2
  double radiusCenterMm = sqrt(
    radiusPenMm * radiusPenMm - FRONT_PEN_OFFSET_MM * FRONT_PEN_OFFSET_MM
  );

  double radiusLeftMm  = radiusCenterMm - (DRIVE_WHEEL_BASE_MM / 2.0);
  double radiusRightMm = radiusCenterMm + (DRIVE_WHEEL_BASE_MM / 2.0);

  if (radiusLeftMm <= 0.0) {
    fullReport += "ERREUR: roue interieure devrait reculer ou rester immobile.\n";
    fullReport += "Rayon roue gauche=" + String(radiusLeftMm, 1) + " mm\n";

    stopWheelMotorsBriefly();
    digitalWrite(ROBOT_STATUS_LED_PIN, LOW);
    digitalWrite(ROBOT_READY_LED_PIN, HIGH);
    statusMessage = "Seq2 impossible: rayon trop petit";
    return;
  }

  double distLeftMm  = 2.0 * PI * radiusLeftMm  * CIRCLE_CLOSURE_CALIBRATION_FACTOR;
  double distRightMm = 2.0 * PI * radiusRightMm * CIRCLE_CLOSURE_CALIBRATION_FACTOR;

  long targetLeftTicks  = (long)(distLeftMm  * ENCODER_COUNTS_PER_MILLIMETER);
  long targetRightTicks = (long)(distRightMm * ENCODER_COUNTS_PER_MILLIMETER);

  float ratioLR = (targetRightTicks > 0)
                    ? (float)targetLeftTicks / (float)targetRightTicks
                    : 1.0f;

  ratioLR = constrain(ratioLR, 0.20f, 1.00f);

  // PWM de départ adaptés au rapport demandé.
  // La roue gauche est faible, mais si elle reçoit trop de PWM,
  // elle finit trop tôt et le robot pivote sur lui-même.
  // On la met donc juste au-dessus de son seuil utile.
  int baseLeft = LEFT_DRIVE_MINIMUM_PWM + (int)(ratioLR * 35.0f);
  baseLeft = constrain(baseLeft, LEFT_DRIVE_MINIMUM_PWM, 235);

  // La roue droite est plus forte, mais c’est souvent la roue extérieure.
  // Elle doit avancer plus vite, sans exploser le rapport.
  int baseRight = CIRCLE_RIGHT_MINIMUM_PWM
                + (int)((1.0f - ratioLR) * 45.0f);

baseRight = constrain(baseRight, CIRCLE_RIGHT_MINIMUM_PWM, CIRCLE_RIGHT_REFERENCE_MAXIMUM_PWM);

  fullReport += "Rayon centre roues=" + String(radiusCenterMm, 1) + " mm\n";
  fullReport += "Rayon roue gauche=" + String(radiusLeftMm, 1) + " mm\n";
  fullReport += "Rayon roue droite=" + String(radiusRightMm, 1) + " mm\n";
  fullReport += "Distance gauche=" + String(distLeftMm, 1) + " mm\n";
  fullReport += "Distance droite=" + String(distRightMm, 1) + " mm\n";
  fullReport += "Target ticks gauche=" + String(targetLeftTicks) + "\n";
  fullReport += "Target ticks droite=" + String(targetRightTicks) + "\n";
  fullReport += "Ratio ticks G/D=" + String(ratioLR, 3) + "\n";
  fullReport += "PWM cercle gauche=" + String(baseLeft) + "\n";
  fullReport += "PWM cercle droite=" + String(baseRight) + "\n";

  // Kick de départ pour vaincre la friction statique
  commandLeftWheelMotor(245);
  commandRightWheelMotor(175);
  delay(160);
  stopWheelMotors();
  delay(100);

  long realL = 0;
  long realR = 0;

  executeTwoWheelCircleTrace(targetLeftTicks, targetRightTicks,
                       baseLeft, baseRight,
                       &realL, &realR);

  fullReport += "Reel ticks gauche=" + String(realL) + "\n";
  fullReport += "Reel ticks droite=" + String(realR) + "\n";

  stopWheelMotorsBriefly();
  delay(300);

  digitalWrite(ROBOT_STATUS_LED_PIN, LOW);
  digitalWrite(ROBOT_READY_LED_PIN, HIGH);

  statusMessage = "Seq2 cercle terminee";
  fullReport += "=== FIN SEQUENCE 2 ===\n";

  Serial.println("----------------------------------------");
  Serial.println("  Cercle termine");
  Serial.println("----------------------------------------");
}

void applyNorthSearchPivotPulse(int dir, int pulseMs) {
  // dir > 0 : rotation gauche
  // dir < 0 : rotation droite
  if (dir > 0) {
    commandLeftWheelMotor(-NORTH_LEFT_PIVOT_PWM);
    commandRightWheelMotor(NORTH_RIGHT_PIVOT_PWM);
  } else {
    commandLeftWheelMotor(NORTH_LEFT_PIVOT_PWM);
    commandRightWheelMotor(-NORTH_RIGHT_PIVOT_PWM);
  }

  delay(pulseMs);
  stopWheelMotors();
  delay(180);
}

void measureAverageMagneticXY(float &avgMx, float &avgMy, int samples = 20, int delayMs = 20) {
  avgMx = 0.0f;
  avgMy = 0.0f;

  int valid = 0;

  for (int i = 0; i < samples; i++) {
    refreshRobotSensors();

    avgMx += magX;
    avgMy += magY;
    valid++;

    delay(delayMs);
  }

  if (valid > 0) {
    avgMx /= valid;
    avgMy /= valid;
  }
}

bool alignRobotToNorthUsingMx() {
  if (!magOk) {
    fullReport += "OrientationNordMx: ERREUR magnetometre indisponible\n";
    return false;
  }

  fullReport += "OrientationNordMx: recherche de Mx = 0\n";

  int stableCount = 0;
  unsigned long startMs = millis();

  for (int attempt = 0; attempt < 40 && millis() - startMs < 12000; attempt++) {
    float mx = 0.0f;
    float my = 0.0f;

    measureAverageMagneticXY(mx, my);

    bool mxOk = fabs(mx) <= NORTH_MX_TOLERANCE_UT;
    bool myOk = (my * NORTH_EXPECTED_MY_SIGN) > 0.0f;

    fullReport += "  tentative " + String(attempt + 1)
               + " Mx=" + String(mx, 2)
               + " My=" + String(my, 2)
               + " mxOk=" + String(mxOk ? "1" : "0")
               + " myOk=" + String(myOk ? "1" : "0")
               + "\n";

    if (mxOk && myOk) {
      stableCount++;

      if (stableCount >= NORTH_STABLE_SAMPLE_COUNT) {
        fullReport += "OrientationNordMx: OK Mx=" + String(mx, 2)
                   + " My=" + String(my, 2) + "\n";
        stopWheelMotors();
        return true;
      }

      delay(120);
      continue;
    }

    stableCount = 0;

    int dir = 1;

    if (!mxOk) {
      // Si Mx est positif, on tourne dans un sens.
      // Si Mx est negatif, on tourne dans l'autre.
      // NORTH_MX_TURN_DIRECTION_SIGN permet d'inverser si le sens est faux au test.
      dir = (mx > 0.0f) ? NORTH_MX_TURN_DIRECTION_SIGN : -NORTH_MX_TURN_DIRECTION_SIGN;
    } else {
      // Mx est proche de 0 mais My a le mauvais signe :
      // on est probablement sur l'axe oppose, donc on continue a tourner.
      dir = NORTH_MX_TURN_DIRECTION_SIGN;
    }

    int pulseMs = map((int)constrain(fabs(mx), 0.0f, 35.0f),
                      0, 35,
                      NORTH_MINIMUM_PULSE_MS,
                      NORTH_MAXIMUM_PULSE_MS);

    applyNorthSearchPivotPulse(dir, pulseMs);
    server.handleClient();
  }

  stopWheelMotors();
  fullReport += "OrientationNordMx: ECHEC timeout\n";
  return false;
}

bool alignRobotToNorthUsingRawHeading() {
  if (!magOk) {
    fullReport += "OrientationNordRaw: ERREUR magnetometre indisponible\n";
    return false;
  }

  fullReport += "OrientationNordRaw: cible brute=" 
             + String(NORTH_RAW_HEADING_TARGET_DEG, 2) + " deg\n";

  unsigned long startMs = millis();

  for (int attempt = 0; attempt < 35 && millis() - startMs < 18000; attempt++) {
    float currentHeading = calculateAveragedMagneticHeading(25, 20);
    float error = wrapDegreesToMinusPlus180(NORTH_RAW_HEADING_TARGET_DEG - currentHeading);
    float absErr = fabs(error);

    fullReport += "Tentative " + String(attempt + 1)
               + " heading=" + String(currentHeading, 2)
               + " err=" + String(error, 2)
               + "\n";

    if (absErr <= NORTH_HEADING_TOLERANCE_DEG) {
      stopWheelMotors();
      fullReport += "OrientationNordRaw: OK\n";
      return true;
    }

    int counts = map((int)constrain(absErr, 0.0f, 90.0f), 0, 90, 40, 220);
    counts = constrain(counts, 40, 220);

    int dir = (error > 0) ? MAGNETIC_TURN_DIRECTION_SIGN : -MAGNETIC_TURN_DIRECTION_SIGN;

    if (dir > 0) {
      // Rotation gauche : roue droite seule
      executeRightWheelOnlyTurn(counts, 220);
    } else {
      // Rotation droite : roue gauche seule
      executeLeftWheelOnlyTurn(counts, 220);
    }

    delay(250);
    server.handleClient();
  }

  stopWheelMotors();
  fullReport += "OrientationNordRaw: ECHEC timeout\n";
  return false;
}

void executeNorthArrowHeadTrace() {
  long realL = 0;
  long realR = 0;

  fullReport += "--- TETE DE FLECHE ---\n";

  statusMessage = "[3] Tete fleche AB";
  executeDifferentialWheelTickBlock(northArrowArc1LeftTicks, northArrowArc1RightTicks, northArrowArc1Pwm, &realL, &realR);
  fullReport += "AB: cibleG=" + String(northArrowArc1LeftTicks)
             + " cibleD=" + String(northArrowArc1RightTicks)
             + " reelG=" + String(realL)
             + " reelD=" + String(realR) + "\n";
  delay(150);

  statusMessage = "[4] Tete fleche BC";
  executeDifferentialWheelTickBlock(northArrowArc2LeftTicks, northArrowArc2RightTicks, northArrowArc2Pwm, &realL, &realR);
  fullReport += "BC: cibleG=" + String(northArrowArc2LeftTicks)
             + " cibleD=" + String(northArrowArc2RightTicks)
             + " reelG=" + String(realL)
             + " reelD=" + String(realR) + "\n";
  delay(150);

  statusMessage = "[5] Tete fleche CD";
  executeDifferentialWheelTickBlock(northArrowArc3LeftTicks, northArrowArc3RightTicks, northArrowArc3Pwm, &realL, &realR);
  fullReport += "CD: cibleG=" + String(northArrowArc3LeftTicks)
             + " cibleD=" + String(northArrowArc3RightTicks)
             + " reelG=" + String(realL)
             + " reelD=" + String(realR) + "\n";
  delay(150);

  statusMessage = "[6] Tete fleche DA";
  executeDifferentialWheelTickBlock(northArrowArc4LeftTicks, northArrowArc4RightTicks, northArrowArc4Pwm, &realL, &realR);
  fullReport += "DA: cibleG=" + String(northArrowArc4LeftTicks)
             + " cibleD=" + String(northArrowArc4RightTicks)
             + " reelG=" + String(realL)
             + " reelD=" + String(realR) + "\n";
  delay(150);

  fullReport += "--- TETE DE FLECHE fin ---\n";
}

void executeNorthArrowDrawingSequence() {
  Serial.println("----------------------------------------");
  Serial.println("  Trace numero trois - fleche vers le nord");
  Serial.println("----------------------------------------");

  statusMessage = "Seq3 fleche nord...";
  fullReport = "=== SEQUENCE 3 - FLECHE NORD COMPLETE ===\n";

  digitalWrite(ROBOT_STATUS_LED_PIN, HIGH);
  digitalWrite(ROBOT_READY_LED_PIN, LOW);

  // [1] Orientation vers le Nord calibré
  statusMessage = "[1] Orientation Nord";
  fullReport += "--- ORIENTATION NORD ---\n";

  bool ok = alignRobotToNorthUsingRawHeading();

  float finalHeading = calculateAveragedMagneticHeading(30, 20);
  float finalError = wrapDegreesToMinusPlus180(NORTH_RAW_HEADING_TARGET_DEG - finalHeading);

  fullReport += "Orientation=" + String(ok ? "OK" : "ECHEC") + "\n";
  fullReport += "Cap final brut=" + String(finalHeading, 2) + " deg\n";
  fullReport += "Cible brute=" + String(NORTH_RAW_HEADING_TARGET_DEG, 2) + " deg\n";
  fullReport += "Erreur finale=" + String(finalError, 2) + " deg\n";

  delay(300);

  // [2] Tige de la flèche
  statusMessage = "[2] Tige fleche";
  fullReport += "--- TIGE FLECHE ---\n";

  double shaftRealMm = executeLoggedStraightMove(northArrowShaftMm, STRAIGHT_MOVE_REFERENCE_PWM);

  fullReport += "Tige: commande=" + String(northArrowShaftMm, 1)
             + "mm reel=" + String(shaftRealMm, 1) + "mm\n";

  delay(250);

  // [3] Tête de flèche
  executeNorthArrowHeadTrace();

  stopWheelMotorsBriefly();

  digitalWrite(ROBOT_STATUS_LED_PIN, LOW);
  digitalWrite(ROBOT_READY_LED_PIN, HIGH);

  statusMessage = "Seq3 fleche nord terminee";
  fullReport += "=== FIN SEQUENCE 3 ===\n";

  Serial.println("----------------------------------------");
  Serial.println("  Fleche nord terminee");
  Serial.println("----------------------------------------");
}

// ============================================================
// Config WiFi — /config et /setparams
// ============================================================

void servePlainConfigurationNotice() {
  String body = "Configuration visuelle retiree du firmware. ";
  body += "Utiliser /setparams avec les memes parametres pour regler le robot.";
  server.send(200, "text/plain", body);
}

void serveParameterUpdate() {
  if (server.hasArg("pb"))  shortWheelMinPwm = server.arg("pb").toInt();
  if (server.hasArg("b1g")) stairBlock1LeftTicks    = server.arg("b1g").toInt();
  if (server.hasArg("b1d")) stairBlock1RightTicks    = server.arg("b1d").toInt();
  if (server.hasArg("b1p")) stairBlock1Pwm   = server.arg("b1p").toInt();
  if (server.hasArg("b2g")) stairBlock2LeftTicks    = server.arg("b2g").toInt();
  if (server.hasArg("b2d")) stairBlock2RightTicks    = server.arg("b2d").toInt();
  if (server.hasArg("b2p")) stairBlock2Pwm   = server.arg("b2p").toInt();
  if (server.hasArg("b3g")) stairBlock3LeftTicks    = server.arg("b3g").toInt();
  if (server.hasArg("b3d")) stairBlock3RightTicks    = server.arg("b3d").toInt();
  if (server.hasArg("b3p")) stairBlock3Pwm   = server.arg("b3p").toInt();
  if (server.hasArg("b3a")) stairBlock3Active = (server.arg("b3a") == "1");
  if (server.hasArg("b4g")) stairBlock4LeftTicks    = server.arg("b4g").toInt();
  if (server.hasArg("b4d")) stairBlock4RightTicks    = server.arg("b4d").toInt();
  if (server.hasArg("b4p")) stairBlock4Pwm   = server.arg("b4p").toInt();
  if (server.hasArg("b4a")) stairBlock4Active = (server.arg("b4a") == "1");
  if (server.hasArg("b5g")) stairBlock5LeftTicks    = server.arg("b5g").toInt();
  if (server.hasArg("b5d")) stairBlock5RightTicks    = server.arg("b5d").toInt();
  if (server.hasArg("b5p")) stairBlock5Pwm   = server.arg("b5p").toInt();
  if (server.hasArg("b5a")) stairBlock5Active = (server.arg("b5a") == "1");
  if (server.hasArg("b6g")) stairBlock6LeftTicks    = server.arg("b6g").toInt();
  if (server.hasArg("b6d")) stairBlock6RightTicks    = server.arg("b6d").toInt();
  if (server.hasArg("b6p")) stairBlock6Pwm   = server.arg("b6p").toInt();
  if (server.hasArg("b6a")) stairBlock6Active = (server.arg("b6a") == "1");
  if (server.hasArg("b7g")) stairBlock7LeftTicks    = server.arg("b7g").toInt();
  if (server.hasArg("b7d")) stairBlock7RightTicks    = server.arg("b7d").toInt();
  if (server.hasArg("b7p")) stairBlock7Pwm   = server.arg("b7p").toInt();
  if (server.hasArg("b7a")) stairBlock7Active = (server.arg("b7a") == "1");
  if (server.hasArg("b8g")) stairBlock8LeftTicks    = server.arg("b8g").toInt();
  if (server.hasArg("b8d")) stairBlock8RightTicks    = server.arg("b8d").toInt();
  if (server.hasArg("b8p")) stairBlock8Pwm   = server.arg("b8p").toInt();
  if (server.hasArg("b8a")) stairBlock8Active = (server.arg("b8a") == "1");
  if (server.hasArg("b9g")) stairBlock9LeftTicks    = server.arg("b9g").toInt();
  if (server.hasArg("b9d")) stairBlock9RightTicks    = server.arg("b9d").toInt();
  if (server.hasArg("b9p")) stairBlock9Pwm   = server.arg("b9p").toInt();
  if (server.hasArg("b9a")) stairBlock9Active = (server.arg("b9a") == "1");
  if (server.hasArg("b10g")) stairBlock10LeftTicks    = server.arg("b10g").toInt();
  if (server.hasArg("b10d")) stairBlock10RightTicks    = server.arg("b10d").toInt();
  if (server.hasArg("b10p")) stairBlock10Pwm   = server.arg("b10p").toInt();
  if (server.hasArg("b10a")) stairBlock10Active = (server.arg("b10a") == "1");
  if (server.hasArg("lf"))  stairFinalLineMm = server.arg("lf").toFloat();
  if (server.hasArg("lfa")) stairFinalLineActive = (server.arg("lfa") == "1");
  if (server.hasArg("n3o"))  northArrowNorthOffsetDeg = server.arg("n3o").toFloat();
  if (server.hasArg("n3s"))  northArrowShaftMm         = server.arg("n3s").toFloat();
  if (server.hasArg("n3a1g")) northArrowArc1LeftTicks         = server.arg("n3a1g").toInt();
  if (server.hasArg("n3a1d")) northArrowArc1RightTicks         = server.arg("n3a1d").toInt();
  if (server.hasArg("n3a1p")) northArrowArc1Pwm        = server.arg("n3a1p").toInt();
  if (server.hasArg("n3a2g")) northArrowArc2LeftTicks         = server.arg("n3a2g").toInt();
  if (server.hasArg("n3a2d")) northArrowArc2RightTicks         = server.arg("n3a2d").toInt();
  if (server.hasArg("n3a2p")) northArrowArc2Pwm        = server.arg("n3a2p").toInt();
  if (server.hasArg("n3a3g")) northArrowArc3LeftTicks         = server.arg("n3a3g").toInt();
  if (server.hasArg("n3a3d")) northArrowArc3RightTicks         = server.arg("n3a3d").toInt();
  if (server.hasArg("n3a3p")) northArrowArc3Pwm        = server.arg("n3a3p").toInt();
  if (server.hasArg("n3a4g")) northArrowArc4LeftTicks         = server.arg("n3a4g").toInt();
  if (server.hasArg("n3a4d")) northArrowArc4RightTicks         = server.arg("n3a4d").toInt();
  if (server.hasArg("n3a4p")) northArrowArc4Pwm        = server.arg("n3a4p").toInt();
  server.send(200, "text/plain", "OK");
}

// ============================================================
// Web Page
// ============================================================











// ============================================================
// Web Handlers
// ============================================================

bool staticFilesReady = false;
String apiCurrentCommand = "stop";
int apiMotorPower = STRAIGHT_MOVE_REFERENCE_PWM;
int apiTrim = 0;
String apiLastSequence = "none";
String apiCircleDirection = "left";
String apiSequenceError = "none";
String apiLastBlockName = "none";
long apiLastLeftTicks = 0;
long apiLastRightTicks = 0;
long apiLastTargetLeftTicks = 0;
long apiLastTargetRightTicks = 0;

String escapeJsonValueText(const String& value) {
  String out = value;
  out.replace("\\", "\\\\");
  out.replace("\"", "\\\"");
  out.replace("\n", "\\n");
  out.replace("\r", "");
  return out;
}

void sendApiJsonResponse(int httpCode, const String& json) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(httpCode, "application/json", json);
}

void sendApiErrorJson(int httpCode, const String& errorCode) {
  String json = "{";
  json += "\"ok\":false,";
  json += "\"error\":\"" + escapeJsonValueText(errorCode) + "\"";
  json += "}";
  sendApiJsonResponse(httpCode, json);
}

String normalizeApiMoveCommand(String cmd) {
  cmd.toLowerCase();

  if (cmd == "fwd" || cmd == "up" || cmd == "haut") return "forward";
  if (cmd == "bwd" || cmd == "down" || cmd == "bas") return "backward";
  if (cmd == "lft" || cmd == "gauche") return "left";
  if (cmd == "rgt" || cmd == "droite") return "right";
  if (cmd == "stop" || cmd == "halt") return "stop";

  return cmd;
}

void serveLittleFsFile(const char* path, const char* contentType) {
  if (!staticFilesReady) {
    server.send(500, "text/plain", "LittleFS non initialise");
    return;
  }

  if (!LittleFS.exists(path)) {
    server.send(404, "text/plain", "Fichier introuvable");
    return;
  }

  File file = LittleFS.open(path, "r");
  server.streamFile(file, contentType);
  file.close();
}

void redirectToMainInterface() {
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

const char* resolveApiActiveSequenceName() {
  if (robotState == ROBOT_STATE_SEQUENCE_1 || apiLastSequence == "stair") return "stair";
  if (robotState == ROBOT_STATE_SEQUENCE_2 || apiLastSequence == "circle") return "circle";
  if (robotState == ROBOT_STATE_SEQUENCE_3 || apiLastSequence == "north_arrow") return "north_arrow";
  return "none";
}

const char* resolveApiSequenceStatusName() {
  if (robotState == ROBOT_STATE_SEQUENCE_1 || robotState == ROBOT_STATE_SEQUENCE_2 || robotState == ROBOT_STATE_SEQUENCE_3) return "running";
  if (apiSequenceError != "none" || statusMessage.indexOf("impossible") >= 0) return "error";
  if (robotState == ROBOT_STATE_DONE && apiLastSequence != "none") return "done";
  if (robotState == ROBOT_STATE_IDLE && statusMessage == "STOP") return "cancelled";
  return "idle";
}

String composeApiStatusJson() {
  static long previousLeftTicks = 0;
  static long previousRightTicks = 0;
  static unsigned long previousTimeMs = 0;

  unsigned long now = millis();
  long leftTicks = readLeftWheelEncoderTicks();
  long rightTicks = readRightWheelEncoderTicks();
  float leftRpm = 0.0f;
  float rightRpm = 0.0f;

  float dt = (previousTimeMs > 0) ? (now - previousTimeMs) / 1000.0f : 0.0f;
  if (dt > 0.0f) {
    float leftSpeedMmS = (leftTicks - previousLeftTicks) / dt * (float)MILLIMETERS_PER_ENCODER_COUNT;
    float rightSpeedMmS = (rightTicks - previousRightTicks) / dt * (float)MILLIMETERS_PER_ENCODER_COUNT;
    leftRpm = leftSpeedMmS / (PI * DRIVE_WHEEL_DIAMETER_MM) * 60.0f;
    rightRpm = rightSpeedMmS / (PI * DRIVE_WHEEL_DIAMETER_MM) * 60.0f;
  }

  previousLeftTicks = leftTicks;
  previousRightTicks = rightTicks;
  previousTimeMs = now;

  float magHeading = atan2(magY, magX) * 180.0f / PI;
  if (magHeading < 0.0f) magHeading += 360.0f;

  String json = "{";
  json += "\"ok\":true,";
  json += "\"ssid\":\"" + String(WIFI_ACCESS_POINT_NAME) + "\",";
  json += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"clients\":" + String(WiFi.softAPgetStationNum()) + ",";
  json += "\"command\":\"" + escapeJsonValueText(apiCurrentCommand) + "\",";
  json += "\"power\":" + String(apiMotorPower) + ",";
  json += "\"trim\":" + String(apiTrim) + ",";
  json += "\"leftPwm\":" + String(abs(lastLeftMotorCommand)) + ",";
  json += "\"rightPwm\":" + String(abs(lastRightMotorCommand)) + ",";
  json += "\"signedLeftPwm\":" + String(lastLeftMotorCommand) + ",";
  json += "\"signedRightPwm\":" + String(lastRightMotorCommand) + ",";
  json += "\"status_imu\":\"" + String(imuOk ? "ok" : "missing") + "\",";
  json += "\"status_mag\":\"" + String(magOk ? "ok" : "missing") + "\",";
  json += "\"status_encoders\":\"ok\",";
  json += "\"imu\":{";
  json += "\"ax\":" + String(accelX, 4) + ",";
  json += "\"ay\":" + String(accelY, 4) + ",";
  json += "\"az\":" + String(accelZ, 4) + ",";
  json += "\"gx\":" + String(gyroX, 3) + ",";
  json += "\"gy\":" + String(gyroY, 3) + ",";
  json += "\"gz\":" + String(gyroZ, 3);
  json += "},";
  json += "\"mag\":{";
  json += "\"mx\":" + String(magX, 2) + ",";
  json += "\"my\":" + String(magY, 2) + ",";
  json += "\"mz\":" + String(magZ, 2) + ",";
  json += "\"heading\":" + String(magHeading, 2);
  json += "},";
  json += "\"encoders\":{";
  json += "\"left_ticks\":" + String(leftTicks) + ",";
  json += "\"right_ticks\":" + String(rightTicks) + ",";
  json += "\"delta_ticks\":" + String(leftTicks - rightTicks) + ",";
  json += "\"left_rpm\":" + String(leftRpm, 2) + ",";
  json += "\"right_rpm\":" + String(rightRpm, 2);
  json += "},";
  json += "\"pose_encoders\":{";
  json += "\"x\":" + String(odoX / 10.0, 2) + ",";
  json += "\"y\":" + String(odoY / 10.0, 2) + ",";
  json += "\"theta\":" + String(odoTheta * 180.0 / PI, 2);
  json += "},";
  json += "\"sequence\":{";
  json += "\"active\":\"" + String(resolveApiActiveSequenceName()) + "\",";
  json += "\"status\":\"" + String(resolveApiSequenceStatusName()) + "\",";
  json += "\"step\":\"" + escapeJsonValueText(statusMessage) + "\",";
  json += "\"circle_radius_cm\":" + String(circleRadiusCm, 2) + ",";
  json += "\"circle_direction\":\"" + escapeJsonValueText(apiCircleDirection) + "\",";
  json += "\"last_block\":\"" + escapeJsonValueText(apiLastBlockName) + "\",";
  json += "\"last_left_ticks\":" + String(apiLastLeftTicks) + ",";
  json += "\"last_right_ticks\":" + String(apiLastRightTicks) + ",";
  json += "\"last_target_left_ticks\":" + String(apiLastTargetLeftTicks) + ",";
  json += "\"last_target_right_ticks\":" + String(apiLastTargetRightTicks) + ",";
  String visibleSequenceError = apiSequenceError;
  if (visibleSequenceError == "none" && statusMessage.indexOf("impossible") >= 0) {
    visibleSequenceError = statusMessage;
  }
  json += "\"error\":\"" + escapeJsonValueText(visibleSequenceError) + "\"";
  json += "}";
  json += "}";
  return json;
}

void serveApiStatusJson() {
  sendApiJsonResponse(200, composeApiStatusJson());
}

void startSequenceRequestedByApi(RobotState requestedState, const String& sequenceName) {
  if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
    stopRequested = false;
    robotState = requestedState;
    apiLastSequence = sequenceName;
    apiSequenceError = "none";
    apiLastBlockName = "none";
    apiLastLeftTicks = 0;
    apiLastRightTicks = 0;
    apiLastTargetLeftTicks = 0;
    apiLastTargetRightTicks = 0;
    statusMessage = "Sequence demandee: " + sequenceName;

    String json = "{";
    json += "\"ok\":true,";
    json += "\"sequence\":\"" + sequenceName + "\",";
    json += "\"status\":\"running\",";
    json += "\"circle_radius_cm\":" + String(circleRadiusCm, 2) + ",";
    json += "\"circle_direction\":\"" + escapeJsonValueText(apiCircleDirection) + "\"";
    json += "}";
    sendApiJsonResponse(200, json);
  } else {
    sendApiErrorJson(409, "busy");
  }
}

void serveMainInterfacePage() { serveLittleFsFile("/index.html", "text/html"); }
void serveInterfaceStylesheet() { serveLittleFsFile("/style.css", "text/css"); }
void serveInterfaceJavascript() { serveLittleFsFile("/main.js", "application/javascript"); }
void serveInterfaceLogoImage() { serveLittleFsFile("/assets/logo_gyrobot.png", "image/png"); }

void serveApiStatusRequest() { serveApiStatusJson(); }

void serveApiMoveRequest() {
  if (!server.hasArg("cmd")) {
    sendApiErrorJson(400, "missing_cmd");
    return;
  }

  String cmd = normalizeApiMoveCommand(server.arg("cmd"));
  apiCurrentCommand = cmd;

  if (cmd == "stop") {
    stopRequested = true;
    stopWheelMotorsBriefly();
    if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
      apiLastSequence = "none";
    }
    robotState = ROBOT_STATE_IDLE;
    statusMessage = "STOP";
    serveApiStatusJson();
    return;
  }

  if (robotState != ROBOT_STATE_IDLE && robotState != ROBOT_STATE_DONE) {
    sendApiErrorJson(409, "busy");
    return;
  }

  int spd = constrain(apiMotorPower, 60, 220);
  int leftCmd = map(spd, 60, 220, LEFT_DRIVE_MINIMUM_PWM, LEFT_STRAIGHT_REFERENCE_PWM);
  int rightCmd = map(spd, 60, 220, RIGHT_DRIVE_MINIMUM_PWM, RIGHT_STRAIGHT_REFERENCE_PWM);

  leftCmd = constrain(leftCmd + apiTrim, LEFT_DRIVE_MINIMUM_PWM, 255);
  rightCmd = constrain(rightCmd - apiTrim, RIGHT_DRIVE_MINIMUM_PWM, 255);

  if (cmd == "forward") {
    commandLeftWheelMotor(leftCmd);
    commandRightWheelMotor(rightCmd);
  } else if (cmd == "backward") {
    commandLeftWheelMotor(-leftCmd);
    commandRightWheelMotor(-rightCmd);
  } else if (cmd == "left") {
    commandLeftWheelMotor(-LEFT_TURN_REFERENCE_PWM);
    commandRightWheelMotor(RIGHT_TURN_REFERENCE_PWM);
  } else if (cmd == "right") {
    commandLeftWheelMotor(LEFT_TURN_REFERENCE_PWM);
    commandRightWheelMotor(-RIGHT_TURN_REFERENCE_PWM);
  } else {
    apiCurrentCommand = "stop";
    stopWheelMotors();
    sendApiErrorJson(400, "unknown_cmd");
    return;
  }

  serveApiStatusJson();
}

void serveApiPowerRequest() {
  if (server.hasArg("value")) {
    apiMotorPower = constrain(server.arg("value").toInt(), 0, 255);
  }
  serveApiStatusJson();
}

void serveApiTrimRequest() {
  if (server.hasArg("value")) {
    apiTrim = constrain(server.arg("value").toInt(), -30, 30);
  }
  serveApiStatusJson();
}

void serveApiPoseResetRequest() {
  odoEncL = readLeftWheelEncoderTicks();
  odoEncR = readRightWheelEncoderTicks();
  odoX = 0;
  odoY = 0;
  odoTheta = 0;
  imuHeading = 0;
  serveApiStatusJson();
}

void serveApiGyroCalibrationRequest() {
  statusMessage = "Calibration gyro non separee dans ce firmware";
  serveApiStatusJson();
}

void serveApiMagCalibrationRequest() {
  statusMessage = "Calibration magnetometre non separee dans ce firmware";
  serveApiStatusJson();
}

void serveApiStairSequenceRequest() {
  apiCurrentCommand = "sequence_stair";
  startSequenceRequestedByApi(ROBOT_STATE_SEQUENCE_1, "stair");
}

void serveApiCircleSequenceRequest() {
  if (server.hasArg("radius")) {
    circleRadiusCm = server.arg("radius").toFloat();
  } else if (server.hasArg("r")) {
    circleRadiusCm = server.arg("r").toFloat();
  }

  if (server.hasArg("direction")) {
    apiCircleDirection = server.arg("direction");
  }

  apiCircleDirection = (apiCircleDirection == "right") ? "right" : "left";

  circleRadiusCm = limitFloatingPointValue(circleRadiusCm,
                              CIRCLE_RADIUS_MINIMUM_CM,
                              CIRCLE_RADIUS_MAXIMUM_CM);
  apiCurrentCommand = "sequence_circle";
  startSequenceRequestedByApi(ROBOT_STATE_SEQUENCE_2, "circle");
}

void serveApiNorthArrowSequenceRequest() {
  apiCurrentCommand = "sequence_north_arrow";
  startSequenceRequestedByApi(ROBOT_STATE_SEQUENCE_3, "north_arrow");
}

void serveApiTurnInPlaceRequest() {
  sendApiErrorJson(501, "turn_in_place_not_available");
}

void serveNotFoundRequest() {
  String requestedUri = server.uri();

  if (requestedUri.startsWith("/api/")) {
    sendApiErrorJson(404, "route_not_found");
    return;
  }

  redirectToMainInterface();
}

void serveLegacyStatusRequest()  { server.send(200, "text/plain", statusMessage); }
void serveLegacyLogRequest()     { server.send(200, "text/plain", lastLog); }
void serveResultsReportRequest() {
  server.send(200, "text/plain", fullReport);
}

void serveLegacyStairSequenceRequest() {
  if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
    stopRequested = false;
    apiCurrentCommand = "sequence_stair";
    apiLastSequence = "stair";
    apiSequenceError = "none";
    robotState = ROBOT_STATE_SEQUENCE_1;
    server.send(200, "text/plain", "Demarrage seq1...");
  } else {
    server.send(200, "text/plain", "Deja en cours");
  }
}

void serveLegacyCircleSequenceRequest() {
  if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
    if (server.hasArg("r")) {
      circleRadiusCm = server.arg("r").toFloat();
    } else if (server.hasArg("radius")) {
      circleRadiusCm = server.arg("radius").toFloat();
    }

    circleRadiusCm = limitFloatingPointValue(circleRadiusCm,
                                   CIRCLE_RADIUS_MINIMUM_CM,
                                   CIRCLE_RADIUS_MAXIMUM_CM);

    stopRequested = false;
    apiCurrentCommand = "sequence_circle";
    apiLastSequence = "circle";
    apiSequenceError = "none";
    robotState = ROBOT_STATE_SEQUENCE_2;

    server.send(200, "text/plain",
      "Demarrage seq2 cercle rayon=" + String(circleRadiusCm, 2) + " cm");
  } else {
    server.send(200, "text/plain", "Deja en cours");
  }
}

void serveLegacyNorthArrowSequenceRequest() {
  if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
    stopRequested = false;
    apiCurrentCommand = "sequence_north_arrow";
    apiLastSequence = "north_arrow";
    apiSequenceError = "none";
    robotState = ROBOT_STATE_SEQUENCE_3;
    server.send(200, "text/plain", "Demarrage seq3...");
  } else {
    server.send(200, "text/plain", "Deja en cours");
  }
}

void serveDiagnosticStartRequest() {
  if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
    robotState = ROBOT_STATE_DIAGNOSTIC;
    server.send(200, "text/plain", "Demarrage diagnostic...");
  } else {
    server.send(200, "text/plain", "Deja en cours");
  }
}

void serveDistanceCalibrationStartRequest() {
  if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
    robotState = ROBOT_STATE_DISTANCE_CALIBRATION;
    server.send(200, "text/plain", "Demarrage calibration...");
  } else {
    server.send(200, "text/plain", "Deja en cours");
  }
}

void serveStopRequest() {
  stopRequested = true;
  stopWheelMotorsBriefly();
  apiCurrentCommand = "stop";
  if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
    apiLastSequence = "none";
  }
  robotState = ROBOT_STATE_IDLE;
  statusMessage = "STOP";
  server.send(200, "text/plain", "Robot arrete");
}

void serveLegacySensorPageRequest() { redirectToMainInterface(); }
void serveLegacyTestPageRequest()    { redirectToMainInterface(); }

void serveTestDataRequest() {
  String j = "{";
  j += "\"ssid\":\"Drawbot\",";
  j += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
  j += "\"clients\":" + String(WiFi.softAPgetStationNum()) + ",";
  j += "\"imuOk\":" + String(imuOk?"true":"false") + ",";
  j += "\"magOk\":" + String(magOk?"true":"false") + ",";
  j += "\"ax\":" + String(accelX,2) + ",\"ay\":" + String(accelY,2) + ",\"az\":" + String(accelZ,2) + ",";
  j += "\"gx\":" + String(gyroX,2) + ",\"gy\":" + String(gyroY,2) + ",\"gz\":" + String(gyroZ,2) + ",";
  j += "\"mx\":" + String(magX,1) + ",\"my\":" + String(magY,1) + ",\"mz\":" + String(magZ,1) + ",";
  j += "\"encL\":" + String(readLeftWheelEncoderTicks()) + ",\"encR\":" + String(readRightWheelEncoderTicks()) + ",";
  j += "\"calibDone\":" + String(calibDone?"true":"false") + ",";
  j += "\"calibRunning\":" + String(robotState==ROBOT_STATE_ACTUATOR_CALIBRATION?"true":"false") + ",";
  j += "\"calib\":[";
  for (int i = 0; i < 4; i++) {
    j += "{\"pwm\":" + String(calibPoints[i].pwm) + ",\"speedL\":" + String(calibPoints[i].speedL,0) + ",\"speedR\":" + String(calibPoints[i].speedR,0) + "}";
    if (i < 3) j += ",";
  }
  j += "]}";
  server.send(200, "application/json", j);
}

void serveActuatorCalibrationStartRequest() {
  if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
    robotState = ROBOT_STATE_ACTUATOR_CALIBRATION;
    server.send(200, "text/plain", "OK");
  } else {
    server.send(200, "text/plain", "Deja en cours");
  }
}

// ---- Enregistrement handlers ----

// ============================================================
// Lecture de l'enregistrement
// ============================================================

void executeRecordedTrajectoryPlayback() {
  if (recordCount < 2) {
    statusMessage = "Rien a rejouer — enregistre d'abord";
    fullReport += "\n=== PLAYBACK: aucun enregistrement ===\n";
    return;
  }

  statusMessage = "Lecture en cours...";
  // On AJOUTE le log playback apres le rapport d'enregistrement deja genere
  fullReport += "\n=== PLAYBACK: " + String(recordCount) + " pts ===\n";
  fullReport += "Pt\tTargL\tTargR\tFinalL\tFinalR\tErrL\tErrR\tt_ms\n";
  Serial.println("=== Rejeu de la trajectoire stockee : " + String(recordCount) + " pts ===");

  clearWheelEncoderCounters();
  digitalWrite(ROBOT_STATUS_LED_PIN, HIGH);

  for (int i = 1; i < recordCount; i++) {
    long targetL = recordBuffer[i].L;
    long targetR = recordBuffer[i].R;

    unsigned long tStep = millis();

    while (millis() - tStep < 250) {
      server.handleClient();

      long curL = readLeftWheelEncoderTicks();
      long curR = readRightWheelEncoderTicks();
      long errL = targetL - curL;
      long errR = targetR - curR;

      // Zone morte 8 counts (~0.6mm) pour eviter oscillations autour de MOTOR_PWM_MINIMUM_VALUE
      int pwmL = 0, pwmR = 0;
      if (abs(errL) > 8) {
        pwmL = constrain((int)(errL * 4), -180, 180);
        if (pwmL > 0 && pwmL < MOTOR_PWM_MINIMUM_VALUE)  pwmL =  MOTOR_PWM_MINIMUM_VALUE;
        if (pwmL < 0 && pwmL > -MOTOR_PWM_MINIMUM_VALUE) pwmL = -MOTOR_PWM_MINIMUM_VALUE;
      }
      if (abs(errR) > 8) {
        pwmR = constrain((int)(errR * 4), -180, 180);
        if (pwmR > 0 && pwmR < MOTOR_PWM_MINIMUM_VALUE)  pwmR =  MOTOR_PWM_MINIMUM_VALUE;
        if (pwmR < 0 && pwmR > -MOTOR_PWM_MINIMUM_VALUE) pwmR = -MOTOR_PWM_MINIMUM_VALUE;
      }

      commandLeftWheelMotor(pwmL);
      commandRightWheelMotor(pwmR);

      if (abs(errL) <= 8 && abs(errR) <= 8) break;

      delay(5);
    }

    // Log de ce point dans le rapport
    long fL = readLeftWheelEncoderTicks();
    long fR = readRightWheelEncoderTicks();
    unsigned long dt = millis() - tStep;
    fullReport += String(i)            + "\t";
    fullReport += String(targetL)      + "\t";
    fullReport += String(targetR)      + "\t";
    fullReport += String(fL)           + "\t";
    fullReport += String(fR)           + "\t";
    fullReport += String(targetL - fL) + "\t";
    fullReport += String(targetR - fR) + "\t";
    fullReport += String(dt)           + "\n";
    // Pas de stopWheelMotorsBriefly() ici: transition directe vers le point suivant
  }

  stopWheelMotorsBriefly();
  digitalWrite(ROBOT_STATUS_LED_PIN, LOW);
  digitalWrite(ROBOT_READY_LED_PIN, HIGH);

  long fL = readLeftWheelEncoderTicks(), fR = readRightWheelEncoderTicks();
  fullReport += "\nPosition finale: L=" + String(fL) + "  R=" + String(fR) + "\n";
  fullReport += "Distance: L=" + String(fL * MILLIMETERS_PER_ENCODER_COUNT, 1) + "mm  R=" + String(fR * MILLIMETERS_PER_ENCODER_COUNT, 1) + "mm\n";

  statusMessage = "Lecture terminee — voir /results";
  Serial.println("=== Rejeu fini, details consultables sur /results ===");
}


// Genere un rapport tabulaire copie-collable stocke dans fullReport
void composeRecordedTrajectoryReport() {
  fullReport  = "=== ENREGISTREMENT: " + String(recordCount) + " pts";
  fullReport += " (" + String((recordCount > 1 ? recordCount-1 : 0) * 100) + " ms) ===\n";
  fullReport += "Config: ENCODER_COUNTS_PER_MILLIMETER=" + String(ENCODER_COUNTS_PER_MILLIMETER, 2);
  fullReport += "  MILLIMETERS_PER_ENCODER_COUNT=" + String(MILLIMETERS_PER_ENCODER_COUNT, 4) + "\n";
  fullReport += "WHEEL_BASE=" + String(DRIVE_WHEEL_BASE_MM) + "mm\n\n";

  // En-tete tableau (tab-separated pour copier dans Excel/Sheets)
  fullReport += "Pt\tt_ms\tencL\tencR\tdL\tdR\tmmL\tmmR\n";

  for (int i = 0; i < recordCount; i++) {
    long dL = (i > 0) ? recordBuffer[i].L - recordBuffer[i-1].L : 0;
    long dR = (i > 0) ? recordBuffer[i].R - recordBuffer[i-1].R : 0;
    fullReport += String(i)               + "\t";
    fullReport += String(i * 100)         + "\t";
    fullReport += String(recordBuffer[i].L) + "\t";
    fullReport += String(recordBuffer[i].R) + "\t";
    fullReport += String(dL)              + "\t";
    fullReport += String(dR)              + "\t";
    fullReport += String(dL * MILLIMETERS_PER_ENCODER_COUNT, 1) + "\t";
    fullReport += String(dR * MILLIMETERS_PER_ENCODER_COUNT, 1) + "\n";
  }

  // Resume
  if (recordCount > 1) {
    long totL = recordBuffer[recordCount-1].L;
    long totR = recordBuffer[recordCount-1].R;
    double distL = totL * MILLIMETERS_PER_ENCODER_COUNT;
    double distR = totR * MILLIMETERS_PER_ENCODER_COUNT;
    double avg   = (distL + distR) / 2.0;
    double angle = (distR - distL) / DRIVE_WHEEL_BASE_MM * 180.0 / PI;
    fullReport += "\n=== RESUME ===\n";
    fullReport += "Duree     : " + String((recordCount-1)*100) + " ms\n";
    fullReport += "Total L   : " + String(totL) + " counts = " + String(distL, 1) + " mm\n";
    fullReport += "Total R   : " + String(totR) + " counts = " + String(distR, 1) + " mm\n";
    fullReport += "Dist moy  : " + String(avg, 1) + " mm\n";
    fullReport += "Angle est.: " + String(angle, 1) + " deg  (+ = droite, - = gauche)\n";
  }
}

void serveLegacyRecordPageRequest() { redirectToMainInterface(); }

void serveRecordingStartRequest() {
  if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
    isRecording = false;           // force re-init dans le runGyrobotFirmwareCycle
    robotState  = ROBOT_STATE_RECORDING;
    server.send(200, "text/plain", "OK");
  } else {
    server.send(200, "text/plain", "Deja en cours");
  }
}

void serveRecordingStopRequest() {
  isRecording = false;
  if (robotState == ROBOT_STATE_RECORDING) {
    robotState = ROBOT_STATE_DONE;
  }
  composeRecordedTrajectoryReport();   // rapport disponible immediatement a /results
  statusMessage = "Enreg. termine: " + String(recordCount) + " pts — voir /results";
  server.send(200, "text/plain", String(recordCount));
}

void serveRecordingPlaybackRequest() {
  if (robotState == ROBOT_STATE_IDLE || robotState == ROBOT_STATE_DONE) {
    robotState = ROBOT_STATE_PLAYBACK;
    server.send(200, "text/plain", "OK");
  } else {
    server.send(200, "text/plain", "Deja en cours");
  }
}

void serveRecordingClearRequest() {
  isRecording = false;
  recordCount = 0;
  if (robotState == ROBOT_STATE_RECORDING) robotState = ROBOT_STATE_IDLE;
  statusMessage = "Enregistrement efface";
  server.send(200, "text/plain", "OK");
}

void serveRecordingDataRequest() {
  String j = "{";
  j += "\"count\":"     + String(recordCount) + ",";
  j += "\"recording\":" + String(isRecording ? "true" : "false") + ",";
  j += "\"playing\":"   + String(robotState == ROBOT_STATE_PLAYBACK ? "true" : "false") + ",";
  j += "\"status\":\""  + statusMessage + "\",";
  j += "\"encL\":"      + String(readLeftWheelEncoderTicks() - recStartL) + ",";
  j += "\"encR\":"      + String(readRightWheelEncoderTicks() - recStartR);
  j += "}";
  server.send(200, "application/json", j);
}

void serveLegacySensorJsonRequest() {
  static long prevJL=0, prevJR=0;
  static unsigned long prevJT=0;
  unsigned long now = millis();
  long curL = readLeftWheelEncoderTicks(), curR = readRightWheelEncoderTicks();
  float dt = (prevJT > 0) ? (now - prevJT) / 1000.0f : 0;
  float rpmL=0, rpmR=0;
  if (dt > 0) {
    float spdL = (curL - prevJL) / dt * (float)MILLIMETERS_PER_ENCODER_COUNT;
    float spdR = (curR - prevJR) / dt * (float)MILLIMETERS_PER_ENCODER_COUNT;
    rpmL = spdL / (PI * DRIVE_WHEEL_DIAMETER_MM) * 60.0f;
    rpmR = spdR / (PI * DRIVE_WHEEL_DIAMETER_MM) * 60.0f;
  }
  prevJL=curL; prevJR=curR; prevJT=now;

  String j = "{";
  j += "\"wl\":"  + String(rpmL,1)   + ",\"wr\":"  + String(rpmR,1)   + ",";
  j += "\"ax\":"  + String(accelX,3) + ",\"ay\":"  + String(accelY,3) + ",\"az\":" + String(accelZ,3) + ",";
  j += "\"gx\":"  + String(gyroX,3)  + ",\"gy\":"  + String(gyroY,3)  + ",\"gz\":" + String(gyroZ,3)  + ",";
  j += "\"mx\":"  + String(magX,1)   + ",\"my\":"  + String(magY,1)   + ",\"mz\":" + String(magZ,1)   + ",";
  j += "\"px\":"  + String(odoX,1)   + ",\"py\":"  + String(odoY,1)   + ",";
  j += "\"pe\":"  + String(odoTheta * 180.0 / PI, 1) + ",";
  j += "\"pi\":"  + String(imuHeading, 1) + ",";
  j += "\"imuOk\":" + String(imuOk ? "true" : "false") + ",";
  j += "\"magOk\":" + String(magOk ? "true" : "false");
  j += "}";
  server.send(200, "application/json", j);
}

void serveLegacyPoseResetRequest() {
  odoEncL = readLeftWheelEncoderTicks();
  odoEncR = readRightWheelEncoderTicks();
  odoX = 0; odoY = 0; odoTheta = 0;
  imuHeading = 0;
  server.send(200, "text/plain", "OK");
}

void serveLegacyRemotePageRequest() { redirectToMainInterface(); }

void serveLegacyRemoteCommandRequest() {
  if (robotState != ROBOT_STATE_IDLE && robotState != ROBOT_STATE_DONE) {
    server.send(200, "text/plain", "Sequence en cours");
    return;
  }
  String c = server.arg("c");
  int spd = server.arg("s").toInt();
  spd = constrain(spd, 60, 220);

int leftCmd  = map(spd, 60, 220, LEFT_DRIVE_MINIMUM_PWM,  LEFT_STRAIGHT_REFERENCE_PWM);
int rightCmd = map(spd, 60, 220, RIGHT_DRIVE_MINIMUM_PWM, RIGHT_STRAIGHT_REFERENCE_PWM);

leftCmd  = constrain(leftCmd,  LEFT_DRIVE_MINIMUM_PWM,  255);
rightCmd = constrain(rightCmd, RIGHT_DRIVE_MINIMUM_PWM, 255);

if (c == "fwd") {
  commandLeftWheelMotor(leftCmd);
  commandRightWheelMotor(rightCmd);
  server.send(200, "text/plain", "Avant");
}
else if (c == "bwd") {
  commandLeftWheelMotor(-leftCmd);
  commandRightWheelMotor(-rightCmd);
  server.send(200, "text/plain", "Arriere");
}
else if (c == "lft") {
  commandLeftWheelMotor(-LEFT_TURN_REFERENCE_PWM);
  commandRightWheelMotor(RIGHT_TURN_REFERENCE_PWM);
  server.send(200, "text/plain", "Gauche");
}
else if (c == "rgt") {
  commandLeftWheelMotor(LEFT_TURN_REFERENCE_PWM);
  commandRightWheelMotor(-RIGHT_TURN_REFERENCE_PWM);
  server.send(200, "text/plain", "Droite");
}
else {
  stopWheelMotors();
  server.send(200, "text/plain", "Stop");
}
}

void serveEncoderSpeedRequest() {
  static long prevL = 0, prevR = 0;
  static unsigned long prevT = 0;
  unsigned long now = millis();
  long curL = readLeftWheelEncoderTicks(), curR = readRightWheelEncoderTicks();
  float dt = (now - prevT) / 1000.0f;
  String msg = "L: ";
  if (dt > 0 && prevT > 0) {
    float speedL = (curL - prevL) / dt * MILLIMETERS_PER_ENCODER_COUNT;
    float speedR = (curR - prevR) / dt * MILLIMETERS_PER_ENCODER_COUNT;
    msg += String(speedL, 0) + " mm/s   R: " + String(speedR, 0) + " mm/s";
    msg += "  |  L=" + String(curL) + "  R=" + String(curR);
  } else {
    msg += String(curL) + " counts   R: " + String(curR) + " counts";
  }
  prevL = curL; prevR = curR; prevT = now;
  server.send(200, "text/plain", msg);
}

// ============================================================
// Setup & Loop
// ============================================================

void initializeGyrobotFirmware() {
  Serial.begin(SERIAL_MONITOR_BAUD_RATE);
  delay(500);
  Serial.println("=== Demarrage du programme Gyrobot ===");

  pinMode(ROBOT_STATUS_LED_PIN, OUTPUT); pinMode(ROBOT_READY_LED_PIN, OUTPUT);
  digitalWrite(ROBOT_STATUS_LED_PIN, LOW); digitalWrite(ROBOT_READY_LED_PIN, LOW);

  pinMode(RIGHT_DRIVE_ENABLE_PIN, OUTPUT); pinMode(LEFT_DRIVE_ENABLE_PIN, OUTPUT);
  digitalWrite(RIGHT_DRIVE_ENABLE_PIN, HIGH); digitalWrite(LEFT_DRIVE_ENABLE_PIN, HIGH);

  ledcSetup(0, MOTOR_PWM_FREQUENCY_HZ, MOTOR_PWM_RESOLUTION_BITS); ledcAttachPin(RIGHT_DRIVE_INPUT_A_PIN, 0);
  ledcSetup(1, MOTOR_PWM_FREQUENCY_HZ, MOTOR_PWM_RESOLUTION_BITS); ledcAttachPin(RIGHT_DRIVE_INPUT_B_PIN, 1);
  ledcSetup(2, MOTOR_PWM_FREQUENCY_HZ, MOTOR_PWM_RESOLUTION_BITS); ledcAttachPin(LEFT_DRIVE_INPUT_A_PIN, 2);
  ledcSetup(3, MOTOR_PWM_FREQUENCY_HZ, MOTOR_PWM_RESOLUTION_BITS); ledcAttachPin(LEFT_DRIVE_INPUT_B_PIN, 3);
  stopWheelMotors();

  pinMode(LEFT_WHEEL_ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(LEFT_WHEEL_ENCODER_B_PIN, INPUT_PULLUP);
  pinMode(RIGHT_WHEEL_ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(RIGHT_WHEEL_ENCODER_B_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LEFT_WHEEL_ENCODER_A_PIN), isrEncLeftA,  CHANGE);
  attachInterrupt(digitalPinToInterrupt(LEFT_WHEEL_ENCODER_B_PIN), isrEncLeftB,  CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_WHEEL_ENCODER_A_PIN), isrEncRightA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_WHEEL_ENCODER_B_PIN), isrEncRightB, CHANGE);
  delay(100);
  clearWheelEncoderCounters();

  Wire.begin(21, 22); // SDA=21, SCL=22
  Wire.setClock(400000);
  // Scan I2C complet
  Serial.print("Peripheriques I2C detectes : ");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("adresse 0x"); Serial.print(addr, HEX); Serial.print(" ");
    }
  }
  Serial.println();
  imuOk = initializeInertialSensor();
  magOk = initializeMagneticSensor();
  Serial.print("Verification LSM6DS3 : "); Serial.println(imuOk ? "capteur present" : "capteur absent");
  Serial.print("Verification LIS3MDL : "); Serial.println(magOk ? "capteur present" : "capteur absent");
  odoEncL = readLeftWheelEncoderTicks();
  odoEncR = readRightWheelEncoderTicks();

  WiFi.softAP(WIFI_ACCESS_POINT_NAME, WIFI_ACCESS_POINT_PASSWORD);
  Serial.print("Reseau WiFi cree : "); Serial.println(WIFI_ACCESS_POINT_NAME);
  Serial.print("Interface disponible a : "); Serial.println(WiFi.softAPIP());

  staticFilesReady = LittleFS.begin(true);
  Serial.print("Systeme de fichiers web : ");
  Serial.println(staticFilesReady ? "pret" : "indisponible");

  server.on("/", serveMainInterfacePage);
  server.on("/style.css", serveInterfaceStylesheet);
  server.on("/main.js", serveInterfaceJavascript);
  server.on("/assets/logo_gyrobot.png", serveInterfaceLogoImage);

  server.on("/api/status", serveApiStatusRequest);
  server.on("/api/move", serveApiMoveRequest);
  server.on("/api/power", serveApiPowerRequest);
  server.on("/api/trim", serveApiTrimRequest);
  server.on("/api/reset-pose", serveApiPoseResetRequest);
  server.on("/api/calibrate-gyro", serveApiGyroCalibrationRequest);
  server.on("/api/calibrate-mag", serveApiMagCalibrationRequest);
  server.on("/api/sequence/stair", serveApiStairSequenceRequest);
  server.on("/api/sequence/seq1", serveApiStairSequenceRequest);
  server.on("/api/sequence/circle", serveApiCircleSequenceRequest);
  server.on("/api/sequence/seq2", serveApiCircleSequenceRequest);
  server.on("/api/sequence/north-arrow", serveApiNorthArrowSequenceRequest);
  server.on("/api/sequence/fleche-nord", serveApiNorthArrowSequenceRequest);
  server.on("/api/sequence/seq3", serveApiNorthArrowSequenceRequest);
  server.on("/api/turn-in-place", serveApiTurnInPlaceRequest);
  server.on("/api/tourner", serveApiTurnInPlaceRequest);

  server.on("/status", serveLegacyStatusRequest);
  server.on("/log", serveLegacyLogRequest);
  server.on("/seq1", serveLegacyStairSequenceRequest);
  server.on("/seq2", serveLegacyCircleSequenceRequest);
  server.on("/seq3", serveLegacyNorthArrowSequenceRequest);
  server.on("/diag", serveDiagnosticStartRequest);
  server.on("/stop", serveStopRequest);
  server.on("/results", serveResultsReportRequest);
  server.on("/calib", serveDistanceCalibrationStartRequest);
  server.on("/telecommande", serveLegacyRemotePageRequest);
  server.on("/tc", serveLegacyRemoteCommandRequest);
  server.on("/enc", serveEncoderSpeedRequest);
  server.on("/capteurs",     serveLegacySensorPageRequest);
  server.on("/sensors.json", serveLegacySensorJsonRequest);
  server.on("/resetpos",     serveLegacyPoseResetRequest);
  server.on("/tests",        serveLegacyTestPageRequest);
  server.on("/testdata",     serveTestDataRequest);
  server.on("/runcalib",     serveActuatorCalibrationStartRequest);
  server.on("/config",       servePlainConfigurationNotice);
  server.on("/setparams",    serveParameterUpdate);
  server.on("/record",       serveLegacyRecordPageRequest);
  server.on("/recstart",     serveRecordingStartRequest);
  server.on("/recstop",      serveRecordingStopRequest);
  server.on("/recplay",      serveRecordingPlaybackRequest);
  server.on("/recclear",     serveRecordingClearRequest);
  server.on("/recdata",      serveRecordingDataRequest);
  server.onNotFound(serveNotFoundRequest);
  server.begin();

  Serial.print("Resolution encodeur="); Serial.print(ENCODER_COUNTS_PER_WHEEL_REV);
  Serial.print(" conversion mm/tick="); Serial.println(MILLIMETERS_PER_ENCODER_COUNT, 3);
  Serial.println("Touches serie disponibles : s escalier, n nord, d controle");

  for (int i = 0; i < 3; i++) {
    digitalWrite(ROBOT_READY_LED_PIN, HIGH); delay(200);
    digitalWrite(ROBOT_READY_LED_PIN, LOW);  delay(200);
  }
  statusMessage = "Pret";
}

void runGyrobotFirmwareCycle() {
  server.handleClient();

  // Mise a jour IMU + odometrie toutes les 50 ms
  static unsigned long lastSensorMs = 0;
  if (millis() - lastSensorMs >= 50) {
    lastSensorMs = millis();
    refreshRobotSensors();
  }

  switch (robotState) {
    case ROBOT_STATE_IDLE:
      if (Serial.available()) {
        char c = Serial.read();
        if (c == 's' || c == 'S') robotState = ROBOT_STATE_SEQUENCE_1;
        if (c == 'n' || c == 'N') robotState = ROBOT_STATE_SEQUENCE_3;
        if (c == 'd' || c == 'D') robotState = ROBOT_STATE_DIAGNOSTIC;
        if (c == 'c' || c == 'C') robotState = ROBOT_STATE_DISTANCE_CALIBRATION;
        if (c == 'e' || c == 'E') {
          Serial.println("=== Lecture directe des encodeurs pendant 10 s ===");
          for (int i = 0; i < 100; i++) {
            Serial.print("gauche="); Serial.print(readLeftWheelEncoderTicks());
            Serial.print(" | compteur droit="); Serial.print(readRightWheelEncoderTicks());
            Serial.print(" (droit brut="); Serial.print(-readRightWheelEncoderTicks()); Serial.println(" fin)");
            delay(100);
          }
          Serial.println("=== Fin de lecture directe encodeurs ===");
        }
      }
      break;

    case ROBOT_STATE_SEQUENCE_1:
      executeStairDrawingSequence();
      robotState = ROBOT_STATE_DONE;
      break;

    case ROBOT_STATE_SEQUENCE_2:
      executeCircleDrawingSequence();
      robotState = ROBOT_STATE_DONE;
      break;  

    case ROBOT_STATE_SEQUENCE_3:
      executeNorthArrowDrawingSequence();
      robotState = ROBOT_STATE_DONE;
      break;

    case ROBOT_STATE_DIAGNOSTIC:
      executeDriveEncoderInspection();
      robotState = ROBOT_STATE_DONE;
      break;

    case ROBOT_STATE_DISTANCE_CALIBRATION:
      executeStraightDistanceMeasurement();
      robotState = ROBOT_STATE_DONE;
      break;

    case ROBOT_STATE_ACTUATOR_CALIBRATION:
      executeMotorResponseMeasurement();
      robotState = ROBOT_STATE_DONE;
      break;

    case ROBOT_STATE_RECORDING:
      // Initialisation au premier passage (isRecording == false)
      if (!isRecording) {
        stopWheelMotors();
        recStartL    = readLeftWheelEncoderTicks();
        recStartR    = readRightWheelEncoderTicks();
        recordCount  = 0;
        recordBuffer[0] = {0, 0};
        recordCount  = 1;
        isRecording  = true;
        lastRecordMs = millis();
        statusMessage = "Enreg: 1/" + String(RECORDING_MAX_SAMPLE_COUNT) + " pts";
        Serial.println("=== Debut de memorisation du parcours ===");
      }
      // Echantillonnage toutes les 100ms
      if (millis() - lastRecordMs >= 100) {
        lastRecordMs = millis();
        if (recordCount < RECORDING_MAX_SAMPLE_COUNT) {
          recordBuffer[recordCount] = {
            readLeftWheelEncoderTicks()  - recStartL,
            readRightWheelEncoderTicks() - recStartR
          };
          recordCount++;
          statusMessage = "Enreg: " + String(recordCount) + "/" + String(RECORDING_MAX_SAMPLE_COUNT) + " pts";
        } else {
          // Buffer plein — arret auto
          isRecording = false;
          robotState  = ROBOT_STATE_DONE;
          statusMessage = "Buffer plein: " + String(recordCount) + " pts enregistres";
          Serial.println("=== Stockage parcours rempli, fin automatique ===");
        }
      }
      break;

    case ROBOT_STATE_PLAYBACK:
      executeRecordedTrajectoryPlayback();
      robotState = ROBOT_STATE_DONE;
      break;

    case ROBOT_STATE_DONE:
      digitalWrite(ROBOT_READY_LED_PIN, HIGH);
      if (Serial.available()) {
        char c = Serial.read();
        if (c == 's' || c == 'S') { digitalWrite(ROBOT_READY_LED_PIN, LOW); robotState = ROBOT_STATE_SEQUENCE_1; }
        if (c == 'n' || c == 'N') { digitalWrite(ROBOT_READY_LED_PIN, LOW); robotState = ROBOT_STATE_SEQUENCE_3; }
        if (c == 'd' || c == 'D') { digitalWrite(ROBOT_READY_LED_PIN, LOW); robotState = ROBOT_STATE_DIAGNOSTIC; }
        if (c == 'c' || c == 'C') { digitalWrite(ROBOT_READY_LED_PIN, LOW); robotState = ROBOT_STATE_DISTANCE_CALIBRATION; }
      }
      break;
  }
}

void setup() {
  initializeGyrobotFirmware();
}

void loop() {
  runGyrobotFirmwareCycle();
}
