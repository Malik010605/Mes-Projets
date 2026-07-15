from __future__ import annotations

import math
from dataclasses import dataclass
from io import BytesIO
from pathlib import Path
from typing import Dict, List, Optional, Tuple

import numpy as np
import pandas as pd


# ----------------------------
# Configuration principale
# ----------------------------

@dataclass
class DetectConfig:
    recent_window: int = 8
    hist_window: int = 16

    min_points: int = 10
    min_points_before: int = 5
    min_points_after: int = 5

    min_sales_recent: int = 20
    penalty: float = 0.5

    # Annualisation : si >0 on force (ex: 4 trimestriel, 12 mensuel), si 0 on déduit à partir des deltas de period_start uniques
    steps_per_year: float = 0.0

    # Poids du score
    w_accel: float = 1.0
    w_surperf: float = 1.0
    w_conf: float = 0.5

    benchmark: str = "global"


# ----------------------------
# Helpers I/O
# ----------------------------

def _read_table(path: Path) -> pd.DataFrame:
    if not path.exists():
        raise FileNotFoundError(f"Fichier introuvable: {path}")

    suf = path.suffix.lower()
    if suf == ".parquet":
        try:
            return pd.read_parquet(path)
        except Exception as e:
            raise RuntimeError(
                f"Echec lecture Parquet: {e}. "
                "Installe pyarrow (pip install pyarrow) ou exporte en CSV."
            ) from e
    if suf == ".csv":
        # Détection simple du séparateur
        with path.open("r", encoding="utf-8", errors="ignore") as f:
            head = "".join([next(f, "") for _ in range(5)])
        sep = max({";": head.count(";"), ",": head.count(","), "\t": head.count("\t")}.items(), key=lambda kv: kv[1])[0]
        return pd.read_csv(path, sep=sep, low_memory=False)
    raise ValueError("Extension invalide. Utilise .parquet ou .csv")


def _ensure_columns(df: pd.DataFrame, required: List[str]) -> None:
    missing = [c for c in required if c not in df.columns]
    if missing:
        raise ValueError(f"Colonnes manquantes: {missing}. Colonnes disponibles: {list(df.columns)}")


def _to_datetime(s: pd.Series) -> pd.Series:
    return pd.to_datetime(s, errors="coerce")


# ----------------------------
# Helpers maths
# ----------------------------

def _linear_fit(x: np.ndarray, y: np.ndarray) -> Tuple[float, float, float]:
    """Ajuste y = a + b*x. Retourne (a, b, sse)."""
    if x.size != y.size or x.size < 2:
        return (np.nan, np.nan, np.inf)

    x_mean = float(np.mean(x))
    y_mean = float(np.mean(y))
    denom = float(np.sum((x - x_mean) ** 2))
    if denom <= 0.0:
        sse = float(np.sum((y - y_mean) ** 2))
        return (y_mean, 0.0, sse)

    b = float(np.sum((x - x_mean) * (y - y_mean)) / denom)
    a = float(y_mean - b * x_mean)
    y_hat = a + b * x
    sse = float(np.sum((y - y_hat) ** 2))
    return (a, b, sse)


def _annualize_log_slope(slope_per_step: float, steps_per_year: float) -> float:
    """Si y = log(prix), la pente approxime la croissance relative par pas; annualisation: exp(pente * steps_per_year) - 1."""
    if not np.isfinite(slope_per_step) or steps_per_year <= 0:
        return np.nan
    return float(math.exp(slope_per_step * steps_per_year) - 1.0)


def _infer_steps_per_year_from_unique_periods(periods: pd.Series) -> float:
    """Inférence robuste sur données panel: on déduplique period_start, puis delta < 60 jours => mensuel (12), sinon trimestriel (4)."""
    p = pd.to_datetime(periods, errors="coerce").dropna().drop_duplicates().sort_values()
    if len(p) < 3:
        return 4.0
    deltas = (p.diff().dt.days.dropna()).to_numpy(dtype=float)
    if deltas.size == 0:
        return 4.0
    med = float(np.median(deltas))
    return 12.0 if med < 60.0 else 4.0


# ----------------------------
# Benchmark
# ----------------------------

def compute_benchmark_global(series_df: pd.DataFrame) -> pd.DataFrame:
    """Benchmark interne global: médiane sur l'ensemble des zones par period_start."""
    bench = (
        series_df.groupby("period_start", observed=True, sort=True)
        .agg(
            bench_log=("price_m2_log", "median"),
            bench_price=("price_m2_median", "median"),
            bench_n=("zone_id", "nunique"),
        )
        .reset_index()
        .sort_values("period_start")
        .reset_index(drop=True)
    )
    return bench


def confidence_score(volume_recent: float, rupture_is_recent: int, min_sales_recent: int) -> float:
    """Confiance dans [0,1]: le volume sature à min_sales_recent, et la récence pénalise les ruptures trop anciennes."""
    if not np.isfinite(volume_recent):
        base = 0.3
    else:
        base = min(1.0, float(volume_recent) / float(max(1, min_sales_recent)))

    rec = 1.0 if rupture_is_recent == 1 else 0.6
    return float(max(0.0, min(1.0, base * rec)))


# ----------------------------
# Détection de rupture (1 changepoint)
# ----------------------------

def detect_zone_break(zone_df: pd.DataFrame, cfg: DetectConfig, steps_per_year: float) -> Dict[str, object]:
    """Régression linéaire par morceaux (1 point de rupture) sur log(prix) dans les cfg.hist_window derniers points."""
    z = zone_df.sort_values("period_start").copy()
    if len(z) < cfg.min_points:
        return {"ok": 0}

    z = z.tail(cfg.hist_window).copy()
    if len(z) < cfg.min_points:
        return {"ok": 0}

    y = z["price_m2_log"].to_numpy(dtype=float)
    x = np.arange(len(z), dtype=float)

    n = len(z)
    k_min = cfg.min_points_before - 1
    k_max = n - cfg.min_points_after - 1
    if k_min > k_max:
        return {"ok": 0}

    best_cost = np.inf
    best_k = None
    best_b1 = np.nan
    best_b2 = np.nan

    for k in range(k_min, k_max + 1):
        x1, y1 = x[: k + 1], y[: k + 1]
        x2, y2 = x[k + 1 :], y[k + 1 :]

        _, b1, sse1 = _linear_fit(x1, y1)
        _, b2, sse2 = _linear_fit(x2, y2)
        if not np.isfinite(b1) or not np.isfinite(b2):
            continue

        cost = float(sse1 + sse2 + cfg.penalty)
        if cost < best_cost:
            best_cost = cost
            best_k = k
            best_b1 = float(b1)
            best_b2 = float(b2)

    if best_k is None:
        return {"ok": 0}

    rupture_date = z.iloc[best_k + 1]["period_start"]
    slope_before = best_b1
    slope_after = best_b2
    acceleration = float(slope_after - slope_before)

    recent = z.tail(cfg.recent_window).copy()
    if len(recent) >= 2:
        xr = np.arange(len(recent), dtype=float)
        yr = recent["price_m2_log"].to_numpy(dtype=float)
        _, br, _ = _linear_fit(xr, yr)
        growth_recent = _annualize_log_slope(float(br), steps_per_year)
    else:
        growth_recent = np.nan

    if "n_sales" in z.columns:
        volume_recent = float(recent["n_sales"].sum(skipna=True))
    else:
        volume_recent = np.nan

    idx_rupt = best_k + 1
    dist_from_end = (n - 1) - idx_rupt
    rupture_is_recent = 1 if dist_from_end <= (cfg.recent_window - 1) else 0

    return {
        "ok": 1,
        "rupture_date": rupture_date,
        "slope_before": slope_before,
        "slope_after": slope_after,
        "acceleration": acceleration,
        "growth_recent": growth_recent,
        "volume_recent": volume_recent,
        "rupture_is_recent": rupture_is_recent,
        "n_points": n,
        "best_cost": best_cost,
    }


# ----------------------------
# Pipeline
# ----------------------------

def run_detection(series_df: pd.DataFrame, cfg: DetectConfig) -> Tuple[pd.DataFrame, pd.DataFrame, float]:
    _ensure_columns(series_df, ["zone_id", "period_start", "price_m2_median", "price_m2_log"])

    df = series_df.copy()
    df["period_start"] = _to_datetime(df["period_start"])
    df = df.dropna(subset=["zone_id", "period_start", "price_m2_log"]).copy()

    if "n_sales" not in df.columns:
        df["n_sales"] = np.nan

    if cfg.steps_per_year and cfg.steps_per_year > 0:
        steps_per_year = float(cfg.steps_per_year)
    else:
        steps_per_year = _infer_steps_per_year_from_unique_periods(df["period_start"])

    if cfg.benchmark != "global":
        raise ValueError("Benchmark non supporte dans cette version. Utilise 'global'.")

    bench = compute_benchmark_global(df)

    bench_recent = bench.tail(cfg.recent_window).copy()
    if len(bench_recent) >= 2:
        xb = np.arange(len(bench_recent), dtype=float)
        yb = bench_recent["bench_log"].to_numpy(dtype=float)
        _, bb, _ = _linear_fit(xb, yb)
        growth_benchmark = _annualize_log_slope(float(bb), steps_per_year)
    else:
        growth_benchmark = np.nan

    results: List[Dict[str, object]] = []

    for zone_id, zdf in df.groupby("zone_id", observed=True, sort=True):
        r = detect_zone_break(zdf, cfg, steps_per_year)
        if r.get("ok", 0) != 1:
            continue

        conf = confidence_score(
            volume_recent=float(r["volume_recent"]) if np.isfinite(r["volume_recent"]) else np.nan,
            rupture_is_recent=int(r["rupture_is_recent"]),
            min_sales_recent=cfg.min_sales_recent,
        )

        if np.isfinite(r["growth_recent"]) and np.isfinite(growth_benchmark):
            surperf = float(r["growth_recent"] - growth_benchmark)
        else:
            surperf = np.nan

        accel = float(r["acceleration"])
        score = (
            cfg.w_accel * accel
            + cfg.w_surperf * (surperf if np.isfinite(surperf) else 0.0)
            + cfg.w_conf * conf
        )

        results.append(
            {
                "zone_id": str(zone_id),
                "rupture_date": pd.to_datetime(r["rupture_date"], errors="coerce"),
                "slope_before_log_per_step": float(r["slope_before"]),
                "slope_after_log_per_step": float(r["slope_after"]),
                "acceleration_log_per_step": accel,
                "growth_recent_annualized": float(r["growth_recent"]),
                "growth_benchmark_annualized": float(growth_benchmark),
                "surperf_annualized": surperf,
                "volume_recent": float(r["volume_recent"]) if np.isfinite(r["volume_recent"]) else np.nan,
                "confidence": float(conf),
                "n_points_used": int(r["n_points"]),
                "rupture_is_recent": int(r["rupture_is_recent"]),
                "best_cost": float(r["best_cost"]),
                "score": float(score),
            }
        )

    if not results:
        raise RuntimeError(
            "Aucun resultat. Causes possibles: fenetres trop longues, min_points trop eleve, "
            "ou series insuffisantes pour detecter une rupture."
        )

    ranking = pd.DataFrame(results).sort_values(["score", "confidence"], ascending=[False, False]).reset_index(drop=True)
    return ranking, bench, steps_per_year


def export_csv_bytes(df: pd.DataFrame) -> bytes:
    bio = BytesIO()
    df.to_csv(bio, index=False)
    return bio.getvalue()


# ----------------------------
# Carte: calcul des centroïdes
# ----------------------------

def _find_col(df: pd.DataFrame, candidates: List[str]) -> Optional[str]:
    cols_lower = {c.lower(): c for c in df.columns}
    for cand in candidates:
        if cand.lower() in cols_lower:
            return cols_lower[cand.lower()]
    return None


def compute_zone_centroids_from_dvf_clean(
    dvf_df: pd.DataFrame,
    zone_id_col_in_dvf: str,
    zone_id_as_string: bool = True,
) -> pd.DataFrame:
    """Construit un centroïde par zone à partir de dvf_clean (médiane lat/lon); nécessite zone_id_col_in_dvf, latitude, longitude."""
    col_lat = _find_col(dvf_df, ["latitude", "lat"])
    col_lon = _find_col(dvf_df, ["longitude", "lon", "lng"])
    if col_lat is None or col_lon is None:
        raise ValueError("DVF clean doit contenir latitude et longitude (ou alias lat/lon).")

    tmp = dvf_df.copy()
    tmp[col_lat] = pd.to_numeric(tmp[col_lat], errors="coerce")
    tmp[col_lon] = pd.to_numeric(tmp[col_lon], errors="coerce")
    tmp = tmp.dropna(subset=[zone_id_col_in_dvf, col_lat, col_lon]).copy()

    if zone_id_as_string:
        tmp[zone_id_col_in_dvf] = tmp[zone_id_col_in_dvf].astype("string")

    cent = (
        tmp.groupby(zone_id_col_in_dvf, observed=True, sort=True)
        .agg(
            lat=("latitude" if col_lat == "latitude" else col_lat, "median"),
            lon=("longitude" if col_lon == "longitude" else col_lon, "median"),
            n_geo=(col_lat, "count"),
        )
        .reset_index()
        .rename(columns={zone_id_col_in_dvf: "zone_id"})
    )

    cent["lat"] = pd.to_numeric(cent["lat"], errors="coerce")
    cent["lon"] = pd.to_numeric(cent["lon"], errors="coerce")
    cent = cent.dropna(subset=["lat", "lon"]).copy()
    return cent


def attach_map_coords(
    ranking_df: pd.DataFrame,
    series_df: pd.DataFrame,
    dvf_clean_path: Optional[Path],
    zone_level: str,
) -> Tuple[pd.DataFrame, str]:
    """Retourne (ranking_avec_coords, message_statut) avec priorité: séries lat/lon, sinon centroïdes via dvf_clean, sinon sans coords."""
    col_lat = _find_col(series_df, ["lat", "latitude"])
    col_lon = _find_col(series_df, ["lon", "lng", "longitude"])
    if col_lat and col_lon:
        tmp = series_df[["zone_id", col_lat, col_lon]].copy()
        tmp[col_lat] = pd.to_numeric(tmp[col_lat], errors="coerce")
        tmp[col_lon] = pd.to_numeric(tmp[col_lon], errors="coerce")
        tmp = tmp.dropna(subset=[col_lat, col_lon]).copy()
        cent = tmp.groupby("zone_id", observed=True, sort=True).agg(lat=(col_lat, "median"), lon=(col_lon, "median")).reset_index()
        out = ranking_df.merge(cent, on="zone_id", how="left")
        ok = out["lat"].notna().sum()
        if ok > 0:
            return out, "Coordonnées trouvées dans le fichier de séries."

    if dvf_clean_path is not None:
        dvf_df = _read_table(dvf_clean_path)

        if zone_level == "commune":
            zone_col = _find_col(dvf_df, ["code_commune", "commune", "insee_com", "code_insee"])
            if zone_col is None:
                return ranking_df.copy(), "Impossible de calculer des centroïdes: DVF clean n'a pas de colonne code_commune."
        elif zone_level == "cp":
            zone_col = _find_col(dvf_df, ["code_postal", "cp"])
            if zone_col is None:
                return ranking_df.copy(), "Impossible de calculer des centroïdes: DVF clean n'a pas de colonne code_postal."
        elif zone_level == "iris":
            zone_col = _find_col(dvf_df, ["code_iris", "iris", "insee_iris"])
            if zone_col is None:
                return ranking_df.copy(), "Impossible de calculer des centroïdes: DVF clean n'a pas de colonne IRIS."
        else:
            return ranking_df.copy(), "Niveau de zone non supporté pour la carte."

        cent = compute_zone_centroids_from_dvf_clean(dvf_df, zone_col, zone_id_as_string=True)
        out = ranking_df.merge(cent[["zone_id", "lat", "lon", "n_geo"]], on="zone_id", how="left")
        ok = out["lat"].notna().sum()
        if ok > 0:
            return out, "Centroïdes calculés à partir du DVF clean (médiane lat/lon par zone)."
        return ranking_df.copy(), "Centroïdes non calculables (lat/lon manquants ou invalides dans DVF clean)."

    return ranking_df.copy(), "Aucune coordonnée disponible: fournis un chemin DVF clean pour calculer les centroïdes."


# ----------------------------
# UI Streamlit
# ----------------------------

def _ui_css() -> str:
    return """
    <style>
      .app-title { font-size: 1.6rem; font-weight: 800; margin-bottom: 0.25rem; }
      .app-subtitle { color: rgba(0,0,0,0.6); margin-bottom: 1rem; }
      .card {
        border: 1px solid rgba(120, 90, 255, 0.18);
        background: rgba(120, 90, 255, 0.05);
        border-radius: 16px;
        padding: 14px 14px;
        margin: 10px 0px;
      }
      .metric-grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 10px; }
      .metric {
        border: 1px solid rgba(0,0,0,0.08);
        border-radius: 14px;
        padding: 10px 12px;
        background: white;
      }
      .metric .k { font-size: 0.8rem; color: rgba(0,0,0,0.55); margin-bottom: 2px; }
      .metric .v { font-size: 1.15rem; font-weight: 800; }
      .small-note { font-size: 0.85rem; color: rgba(0,0,0,0.65); }
      .hr { height: 1px; background: rgba(0,0,0,0.08); margin: 12px 0; }
      code { font-size: 0.9rem; }
    </style>
    """


def streamlit_app():
    import streamlit as st
    import plotly.express as px
    import pydeck as pdk

    st.set_page_config(page_title="Gentrification Scanner", layout="wide")
    st.markdown(_ui_css(), unsafe_allow_html=True)

    st.markdown('<div class="app-title">Gentrification Scanner</div>', unsafe_allow_html=True)
    st.markdown(
        '<div class="app-subtitle">Détection de ruptures de tendance + accélération + surperformance (benchmark interne) sur séries DVF agrégées.</div>',
        unsafe_allow_html=True,
    )

    with st.sidebar:
        st.header("Fichiers")

        default_series = r"C:\Users\arsla\PycharmProjects\ML\Géocodage\series_commune_75_92.parquet"
        series_path_str = st.text_input("Chemin séries (.parquet/.csv)", value=default_series)

        default_dvf = r"C:\Users\arsla\PycharmProjects\ML\Géocodage\dvf_92_clean_v2.csv"
        dvf_clean_path_str = st.text_input("Chemin DVF clean (optionnel, pour la carte)", value=default_dvf)

        zone_level = st.selectbox("Niveau de zone (pour la carte)", options=["commune", "cp", "iris"], index=0)

        st.divider()
        st.header("Paramètres algo")

        recent_window = st.slider("Fenêtre récente (périodes)", 4, 16, 8, 1)
        hist_window = st.slider("Fenêtre historique (périodes)", 10, 40, 16, 1)

        st.subheader("Qualité")
        min_points = st.slider("Min points par zone", 6, 30, 10, 1)
        min_points_before = st.slider("Min points avant rupture", 3, 20, 5, 1)
        min_points_after = st.slider("Min points après rupture", 3, 20, 5, 1)
        min_sales_recent = st.slider("Seuil volume récent (confiance)", 0, 500, 20, 1)

        st.subheader("Stabilisation")
        penalty = st.number_input("Pénalité (coût)", min_value=0.0, max_value=100.0, value=0.5, step=0.1)

        st.subheader("Annualisation")
        steps_mode = st.radio("Mode", ["Auto (déduit)", "Force"], index=0)
        steps_per_year = 0.0
        if steps_mode == "Force":
            steps_per_year = st.number_input("Steps/an (4 trimestriel, 12 mensuel)", min_value=1.0, max_value=365.0, value=4.0, step=1.0)

        st.subheader("Score")
        w_accel = st.number_input("Poids accélération", min_value=0.0, max_value=100.0, value=1.0, step=0.1)
        w_surperf = st.number_input("Poids surperformance", min_value=0.0, max_value=100.0, value=1.0, step=0.1)
        w_conf = st.number_input("Poids confiance", min_value=0.0, max_value=100.0, value=0.5, step=0.1)

        st.divider()
        st.header("Carte")
        top_k_map = st.slider("Top K affiché sur la carte", 5, 50, 20, 1)

        run_btn = st.button("Exécuter", type="primary")

    if not run_btn:
        st.info("Configure les paramètres dans la barre latérale, puis clique sur Exécuter.")
        return

    series_path = Path(series_path_str)
    dvf_clean_path = Path(dvf_clean_path_str) if dvf_clean_path_str.strip() else None
    if dvf_clean_path is not None and not dvf_clean_path.exists():
        dvf_clean_path = None  # On laisse l'app tourner; le calcul des centroïdes sera désactivé

    cfg = DetectConfig(
        recent_window=int(recent_window),
        hist_window=int(hist_window),
        min_points=int(min_points),
        min_points_before=int(min_points_before),
        min_points_after=int(min_points_after),
        min_sales_recent=int(min_sales_recent),
        penalty=float(penalty),
        steps_per_year=float(steps_per_year),
        w_accel=float(w_accel),
        w_surperf=float(w_surperf),
        w_conf=float(w_conf),
        benchmark="global",
    )

    try:
        series_df = _read_table(series_path)
        ranking_df, bench_df, steps_used = run_detection(series_df, cfg)

        zones_total = int(series_df["zone_id"].nunique()) if "zone_id" in series_df.columns else -1
        zones_out = int(ranking_df["zone_id"].nunique())
        periods_unique = int(pd.to_datetime(series_df["period_start"], errors="coerce").dropna().drop_duplicates().shape[0])

        st.markdown('<div class="metric-grid">', unsafe_allow_html=True)
        st.markdown(f'<div class="metric"><div class="k">Zones totales</div><div class="v">{zones_total}</div></div>', unsafe_allow_html=True)
        st.markdown(f'<div class="metric"><div class="k">Zones retenues</div><div class="v">{zones_out}</div></div>', unsafe_allow_html=True)
        st.markdown(f'<div class="metric"><div class="k">Périodes uniques</div><div class="v">{periods_unique}</div></div>', unsafe_allow_html=True)
        st.markdown(f'<div class="metric"><div class="k">Steps/an utilisés</div><div class="v">{steps_used:.1f}</div></div>', unsafe_allow_html=True)
        st.markdown('</div>', unsafe_allow_html=True)

        st.markdown('<div class="hr"></div>', unsafe_allow_html=True)

        tabs = st.tabs(["Carte", "Classement", "Détails zone", "Benchmark", "Diagnostics"])

        with tabs[0]:
            st.subheader("Carte — Quartiers de demain")

            ranking_map, map_status = attach_map_coords(
                ranking_df=ranking_df,
                series_df=series_df,
                dvf_clean_path=dvf_clean_path,
                zone_level=zone_level,
            )

            st.markdown(f"<div class='small-note'>{map_status}</div>", unsafe_allow_html=True)

            pts = ranking_map.dropna(subset=["lat", "lon"]).copy()
            if len(pts) == 0:
                st.warning(
                    "Impossible d'afficher la carte sans lat/lon par zone. "
                    "Vérifie le chemin DVF clean et la colonne de zone (commune/cp/iris)."
                )
            else:
                pts = pts.head(int(top_k_map)).copy()
                pts["rank"] = np.arange(1, len(pts) + 1)
                pts["radius"] = (pts["confidence"].fillna(0.3) * 600).clip(120, 800)

                s = pts["score"].astype(float)
                s_norm = (s - s.min()) / (s.max() - s.min() + 1e-12)
                pts["color_r"] = (160 + 80 * s_norm).astype(int)
                pts["color_g"] = (80 + 40 * (1 - s_norm)).astype(int)
                pts["color_b"] = (255 - 60 * s_norm).astype(int)

                view_state = pdk.ViewState(
                    latitude=float(pts["lat"].median()),
                    longitude=float(pts["lon"].median()),
                    zoom=10,
                    pitch=0,
                )

                layer = pdk.Layer(
                    "ScatterplotLayer",
                    data=pts,
                    get_position="[lon, lat]",
                    get_radius="radius",
                    get_fill_color="[color_r, color_g, color_b, 160]",
                    pickable=True,
                    auto_highlight=True,
                )

                tooltip = {
                    "html": """
                    <b>Zone:</b> {zone_id}<br/>
                    <b>Score:</b> {score}<br/>
                    <b>Rupture:</b> {rupture_date}<br/>
                    <b>Surperf:</b> {surperf_annualized}<br/>
                    <b>Confiance:</b> {confidence}<br/>
                    <b>Vol. récent:</b> {volume_recent}
                    """,
                    "style": {"backgroundColor": "white", "color": "black"},
                }

                deck = pdk.Deck(
                    layers=[layer],
                    initial_view_state=view_state,
                    tooltip=tooltip,
                    map_provider="carto",
                    map_style="light",
                )

                st.pydeck_chart(deck, use_container_width=True)

                st.download_button(
                    "Télécharger les points carte (CSV)",
                    data=export_csv_bytes(pts[[
                        "zone_id", "lat", "lon", "score", "rupture_date", "surperf_annualized", "confidence", "volume_recent"
                    ]]),
                    file_name="gentrif_map_points.csv",
                    mime="text/csv",
                )

        with tabs[1]:
            st.subheader("Classement (zones à potentiel)")
            st.dataframe(ranking_df, use_container_width=True, height=420)

            st.download_button(
                "Télécharger le classement (CSV)",
                data=export_csv_bytes(ranking_df),
                file_name="gentrif_ranking.csv",
                mime="text/csv",
            )

            top_k = min(20, len(ranking_df))
            fig = px.bar(ranking_df.head(top_k), x="zone_id", y="score", title=f"Top {top_k} zones par score")
            st.plotly_chart(fig, use_container_width=True)

        with tabs[2]:
            st.subheader("Détails par zone")

            zone_list = ranking_df["zone_id"].tolist()
            selected_zone = st.selectbox("Zone", options=zone_list, index=0 if zone_list else None)

            if selected_zone:
                z = series_df[series_df["zone_id"].astype(str) == str(selected_zone)].copy()
                z["period_start"] = pd.to_datetime(z["period_start"], errors="coerce")
                z = z.dropna(subset=["period_start"]).sort_values("period_start")

                rrow = ranking_df[ranking_df["zone_id"] == selected_zone].iloc[0].to_dict()

                st.markdown('<div class="card">', unsafe_allow_html=True)
                rd = pd.to_datetime(rrow["rupture_date"], errors="coerce")
                st.markdown(f"**Rupture**: {rd.date() if pd.notnull(rd) else 'NA'}")
                st.markdown(
                    f"- slope_before (log/step): {rrow['slope_before_log_per_step']:.6f}\n"
                    f"- slope_after (log/step): {rrow['slope_after_log_per_step']:.6f}\n"
                    f"- accélération (log/step): {rrow['acceleration_log_per_step']:.6f}\n"
                    f"- surperformance annualisée: {rrow['surperf_annualized'] if pd.notnull(rrow['surperf_annualized']) else 'NA'}\n"
                    f"- confiance: {rrow['confidence']:.3f}\n"
                    f"- volume récent: {rrow['volume_recent'] if pd.notnull(rrow['volume_recent']) else 'NA'}"
                )
                st.markdown("</div>", unsafe_allow_html=True)

                fig1 = px.line(z, x="period_start", y="price_m2_median", title="Prix €/m² (médiane) dans le temps")
                st.plotly_chart(fig1, use_container_width=True)

                fig2 = px.line(z, x="period_start", y="price_m2_log", title="log(Prix €/m² médian) dans le temps")
                st.plotly_chart(fig2, use_container_width=True)

        with tabs[3]:
            st.subheader("Benchmark global interne")
            bench = bench_df.copy()
            bench["period_start"] = pd.to_datetime(bench["period_start"], errors="coerce")
            bench = bench.dropna(subset=["period_start"]).sort_values("period_start")

            st.dataframe(bench, use_container_width=True, height=280)

            figb = px.line(bench, x="period_start", y="bench_price", title="Benchmark €/m² (médiane des zones) par période")
            st.plotly_chart(figb, use_container_width=True)

        with tabs[4]:
            st.subheader("Diagnostics")
            st.markdown(
                "- Si trop peu de zones sortent: baisse `min_points` ou `min_points_before/after`, ou augmente `hist_window`.\n"
                "- Si annualisation incohérente: force `steps/an` (4 si trimestriel).\n"
                "- Le benchmark est interne au fichier (pas national)."
            )

            figc = px.scatter(
                ranking_df,
                x="acceleration_log_per_step",
                y="surperf_annualized",
                size="confidence",
                hover_data=["zone_id", "rupture_date", "volume_recent", "score"],
                title="Accélération vs Surperformance (taille = confiance)",
            )
            st.plotly_chart(figc, use_container_width=True)

    except Exception as e:
        import streamlit as st
        st.error(f"Erreur: {e}")


if __name__ == "__main__":
    # Point d'entrée Streamlit
    streamlit_app()
