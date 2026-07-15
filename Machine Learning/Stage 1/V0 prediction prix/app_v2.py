"""
app_v2_fr.py — DVF V2 (Streamlit)
"""

from __future__ import annotations

from pathlib import Path

import numpy as np
import pandas as pd
import streamlit as st
import pydeck as pdk

import plotly.express as px
import plotly.graph_objects as go

from sklearn.cluster import KMeans
from sklearn.linear_model import Ridge
from sklearn.metrics import mean_absolute_error
from sklearn.preprocessing import OneHotEncoder
import xgboost as xgb


# =========================
# UI / THEME
# =========================

PURPLE = "#5B2A86"
CREAM = "#F7F1E8"

LOGO_PATH_USER = Path(r"ptbai.png")
LOGO_PATH_FALLBACK = Path("logoptbai.png")


def inject_css():
    """
    CSS :
    - sidebar accessible (ne pas masquer le header)
    - accents en violet (sliders inclus)
    - supprime bordures fantômes sans casser le layout
    """
    st.markdown(
        f"""
        <style>
        .block-container {{
            padding-top: 0.8rem !important;
            padding-bottom: 1.2rem !important;
        }}

        /* Ne pas masquer le header */
        footer {{
            visibility: hidden;
            height: 0px;
        }}

        h1, h2, h3 {{
            margin-top: 0.2rem !important;
            margin-bottom: 0.6rem !important;
        }}

        /* =========================
           SLIDERS (BaseWeb) -> violet
           ========================= */

        /* la piste (track) */
        div[data-baseweb="slider"] > div {{
            color: {PURPLE} !important; /* certains sous-éléments héritent */
        }}

        /* partie "remplie" (progress) */
        div[data-baseweb="slider"] div[role="progressbar"] {{
            background-color: {PURPLE} !important;
        }}

        /* poignée (thumb) */
        div[data-baseweb="slider"] div[role="slider"] {{
            background-color: {PURPLE} !important;
            border-color: {PURPLE} !important;
            box-shadow: none !important;
        }}

        /* parfois la track est un div avec background gris : on la cible par aria */
        div[data-baseweb="slider"] div[aria-label] {{
            border-color: {PURPLE} !important;
        }}

        /* =========================
           Inputs / Select focus
           ========================= */
        .stTextInput input:focus,
        .stNumberInput input:focus {{
            border-color: {PURPLE} !important;
            box-shadow: 0 0 0 0.15rem rgba(91,42,134,0.15) !important;
        }}
        div[data-baseweb="select"] > div:focus-within {{
            border-color: {PURPLE} !important;
            box-shadow: 0 0 0 0.15rem rgba(91,42,134,0.15) !important;
        }}

        /* =========================
           Boutons
           ========================= */
        div.stButton > button {{
            background: {PURPLE} !important;
            color: white !important;
            border-radius: 10px !important;
            border: 0 !important;
            padding: 0.55rem 0.9rem !important;
            font-weight: 650 !important;
        }}
        div.stButton > button:hover {{
            background: #4A1F6F !important;
            color: white !important;
        }}

        /* =========================
           Tabs
           ========================= */
        button[data-baseweb="tab"][aria-selected="true"] {{
            color: {PURPLE} !important;
        }}
        div[data-baseweb="tab-highlight"] {{
            background-color: {PURPLE} !important;
        }}

        /* Sidebar style (sans toucher aux positions/largeurs) */
        section[data-testid="stSidebar"] > div {{
            background: #FFFFFF !important;
            border-right: 1px solid rgba(0,0,0,0.06) !important;
        }}

        /* Bordures fantômes */
        div[data-testid="stVerticalBlockBorderWrapper"] {{
            border: none !important;
            background: transparent !important;
            box-shadow: none !important;
            padding: 0 !important;
        }}

        .stPlotlyChart, .stPydeckChart {{
            padding: 0 !important;
            margin: 0 !important;
        }}
        </style>
        """,
        unsafe_allow_html=True,
    )



def header_with_logo():
    left, right = st.columns([1.2, 6], vertical_alignment="center")

    logo_path = None
    if LOGO_PATH_USER.exists():
        logo_path = LOGO_PATH_USER
    elif LOGO_PATH_FALLBACK.exists():
        logo_path = LOGO_PATH_FALLBACK

    with left:
        if logo_path is not None:
            # largeur plus grande => pas de “crop”
            st.image(str(logo_path), width=150)
        else:
            st.write("")

    with right:
        st.markdown(
            f"""
            <div style="line-height:1.05; padding-top: 0.15rem;">
              <div style="font-size: 34px; font-weight: 850; color: {PURPLE};">
                PlaceToBe AI — DVF V2
              </div>
              <div style="font-size: 14px; opacity: 0.85;">
                Tendance (Ridge en log) + Clusters (KMeans) + Résidu (XGBoost en log)
              </div>
            </div>
            """,
            unsafe_allow_html=True,
        )


# =========================
# PIPELINE
# =========================

def calculer_mae(y_vrai: np.ndarray, y_predit: np.ndarray) -> float:
    """
    Cette fonction calcule la MAE (Mean Absolute Error), c’est-à-dire l’erreur absolue moyenne.
    La MAE mesure en moyenne de combien (en €/m² ici) les prédictions s’écartent des valeurs réelles.
    """
    return float(mean_absolute_error(y_vrai, y_predit))


def split_temporel(df: pd.DataFrame, fraction_train: float, fraction_val: float):
    """
    Cette fonction découpe un DataFrame trié chronologiquement en trois blocs (train/val/test).
    """
    n = len(df)
    fin_train = int(n * fraction_train)
    fin_val = int(n * (fraction_train + fraction_val))
    train = df.iloc[:fin_train].copy()
    val = df.iloc[fin_train:fin_val].copy()
    test = df.iloc[fin_val:].copy()
    return train, val, test


def ajouter_features_temps(df: pd.DataFrame, colonne_date: str) -> pd.DataFrame:
    """
    Cette fonction ajoute des variables temporelles utiles à partir d’une colonne de dates.
    """
    df = df.copy()
    d = df[colonne_date]

    df["annee_calendaire"] = d.dt.year.astype(int)
    df["mois"] = d.dt.month.astype(int)
    df["trimestre"] = d.dt.quarter.astype(int)

    date_min = d.min()
    df["t_mois"] = ((d.dt.year - date_min.year) * 12 + (d.dt.month - date_min.month)).astype(int)
    return df


def entrainer_ridge_tendance(df_train: pd.DataFrame, colonne_cible: str, alpha_ridge: float, graine: int) -> Ridge:
    """
    Cette fonction entraîne un modèle Ridge pour apprendre une tendance simple dans le temps (t_mois -> log_price_m2).
    """
    X = df_train[["t_mois"]].values
    y = df_train[colonne_cible].values
    modele = Ridge(alpha=alpha_ridge, random_state=graine)
    modele.fit(X, y)
    return modele


def predire_ridge_tendance(modele: Ridge, df: pd.DataFrame) -> np.ndarray:
    """
    Cette fonction calcule la prédiction de tendance à partir d’un modèle Ridge entraîné.
    """
    return modele.predict(df[["t_mois"]].values)


def cible_log1p(series_prix_m2: pd.Series) -> np.ndarray:
    """
    Cette fonction transforme la cible en log1p(prix_m2) avec contrôle des valeurs invalides.
    """
    y = pd.to_numeric(series_prix_m2, errors="coerce").astype(float)
    y = np.where(np.isnan(y), np.nan, y)
    y = np.where(y < 0, np.nan, y)
    return np.log1p(y)


def inverse_cible_log1p(y_log: np.ndarray) -> np.ndarray:
    """
    Cette fonction inverse log1p via expm1 pour revenir en €/m².
    """
    return np.expm1(y_log)


def entrainer_kmeans(df_train: pd.DataFrame, k_clusters: int, graine: int) -> KMeans:
    """
    Cette fonction entraîne KMeans sur les coordonnées (latitude, longitude) du train.
    """
    coords = df_train[["latitude", "longitude"]].astype(float).values
    km = KMeans(n_clusters=k_clusters, random_state=graine, n_init=10)
    km.fit(coords)
    return km


def ajouter_cluster_id(km: KMeans, df: pd.DataFrame) -> pd.DataFrame:
    """
    Cette fonction ajoute la colonne 'cluster_id' à un DataFrame via un KMeans déjà entraîné.
    """
    df = df.copy()
    coords = df[["latitude", "longitude"]].astype(float).values
    df["cluster_id"] = km.predict(coords).astype(int)
    return df


def construire_ohe(df_train: pd.DataFrame, colonnes_categ: list[str]) -> OneHotEncoder:
    """
    Cette fonction entraîne un OneHotEncoder sur les colonnes catégorielles du train
    en forçant le typage string et en remplaçant les NA par "MISSING".
    """
    X_cat = df_train[colonnes_categ].copy()
    X_cat = X_cat.fillna("MISSING")
    for c in colonnes_categ:
        X_cat[c] = X_cat[c].astype(str)

    ohe = OneHotEncoder(handle_unknown="ignore", sparse_output=True)
    ohe.fit(X_cat)
    return ohe


def transformer_features(
    df: pd.DataFrame,
    colonnes_num: list[str],
    colonnes_categ: list[str],
    ohe: OneHotEncoder,
):
    """
    Cette fonction construit la matrice de features finale (numériques + catégorielles encodées),
    en appliquant le même traitement robuste que lors du fit du OneHotEncoder.
    """
    from scipy import sparse

    X_num = df[colonnes_num].astype(float).values

    X_cat = df[colonnes_categ].copy()
    X_cat = X_cat.fillna("MISSING")
    for c in colonnes_categ:
        X_cat[c] = X_cat[c].astype(str)
    X_cat_enc = ohe.transform(X_cat)

    X_num_sp = sparse.csr_matrix(X_num)
    X = sparse.hstack([X_num_sp, X_cat_enc], format="csr")
    return X


def entrainer_xgb_residu(
    X_train,
    y_train,
    X_val,
    y_val,
    params: dict,
    early_stopping_rounds: int,
):
    """
    Cette fonction entraîne un booster XGBoost sur les résidus en log-space via xgb.train + early stopping.
    """
    dtrain = xgb.DMatrix(X_train, label=y_train)
    dval = xgb.DMatrix(X_val, label=y_val)

    num_boost_round = int(params.get("n_estimators", 2000))

    xgb_params = {
        "objective": "reg:squarederror",
        "eta": float(params.get("learning_rate", 0.05)),
        "max_depth": int(params.get("max_depth", 6)),
        "subsample": float(params.get("subsample", 0.8)),
        "colsample_bytree": float(params.get("colsample_bytree", 0.8)),
        "lambda": float(params.get("reg_lambda", 1.0)),
        "min_child_weight": float(params.get("min_child_weight", 1.0)),
        "seed": int(params.get("random_state", 42)),
    }

    booster = xgb.train(
        params=xgb_params,
        dtrain=dtrain,
        num_boost_round=num_boost_round,
        evals=[(dval, "val")],
        early_stopping_rounds=int(early_stopping_rounds),
        verbose_eval=False,
    )
    return booster


def meilleure_iteration_plus_un(booster) -> int:
    """
    Cette fonction récupère best_iteration + 1 (pour iteration_range) si early stopping a trouvé un optimum.
    """
    best_iter = getattr(booster, "best_iteration", None)
    if best_iter is None:
        return 0
    return int(best_iter) + 1


# =========================
# PLOTLY
# =========================

def _apply_plotly_theme(fig: go.Figure) -> go.Figure:
    """
    Cette fonction applique un style Plotly cohérent avec le thème (violet + fond clair).
    """
    fig.update_layout(
        template="plotly_white",
        margin=dict(l=10, r=10, t=40, b=10),
        font=dict(color="#1C1C1C"),
        paper_bgcolor="rgba(0,0,0,0)",
        plot_bgcolor="rgba(0,0,0,0)",
    )
    fig.update_xaxes(showgrid=True, gridcolor="rgba(0,0,0,0.08)", zeroline=False)
    fig.update_yaxes(showgrid=True, gridcolor="rgba(0,0,0,0.08)", zeroline=False)
    return fig


def fig_pred_vs_reel_plotly(y_vrai: np.ndarray, y_predit: np.ndarray, titre: str) -> go.Figure:
    """
    Scatter interactif Prédit vs Réel (points violet).
    """
    dfp = pd.DataFrame({"Réel": y_vrai, "Prédit": y_predit}).replace([np.inf, -np.inf], np.nan).dropna()
    fig = px.scatter(dfp, x="Réel", y="Prédit", title=titre)

    fig.update_traces(
        marker=dict(size=5, color=PURPLE, opacity=0.7),
        showlegend=False,
        selector=dict(mode="markers"),
    )
    fig = _apply_plotly_theme(fig)
    return fig


def fig_hist_residus_plotly(y_vrai: np.ndarray, y_predit: np.ndarray, titre: str) -> go.Figure:
    """
    Histogramme interactif des résidus (barres violettes) sans légende.
    """
    residus = pd.Series(y_vrai - y_predit).replace([np.inf, -np.inf], np.nan).dropna()

    fig = px.histogram(
        residus,
        nbins=60,
        title=titre,
        labels={"value": "Résidu (€/m²)"},
        color_discrete_sequence=[PURPLE],
    )
    fig.update_traces(showlegend=False)
    fig.update_layout(showlegend=False)
    fig = _apply_plotly_theme(fig)
    return fig


def fig_mae_par_mois_plotly(df: pd.DataFrame, colonne_date: str, colonne_reel: str, colonne_predit: str, titre: str) -> go.Figure:
    """
    Courbe MAE mensuelle (ligne violette).
    """
    temp = df[[colonne_date, colonne_reel, colonne_predit]].copy()
    temp = temp.dropna(subset=[colonne_date, colonne_reel, colonne_predit])
    temp["annee_mois"] = temp[colonne_date].dt.to_period("M").astype(str)

    serie_mae = temp.groupby("annee_mois").apply(
        lambda x: mean_absolute_error(x[colonne_reel].values, x[colonne_predit].values)
    ).reset_index()
    serie_mae.columns = ["annee_mois", "MAE"]

    fig = px.line(
        serie_mae,
        x="annee_mois",
        y="MAE",
        title=titre,
        markers=True,
        color_discrete_sequence=[PURPLE],
    )
    fig.update_traces(line=dict(width=3), showlegend=False)
    fig.update_layout(showlegend=False, xaxis_tickangle=-45)
    fig = _apply_plotly_theme(fig)
    return fig


# =========================
# MAP (mode fixé)
# =========================

def construire_carte_pydeck(df: pd.DataFrame, mode: str):
    """
    Carte Pydeck colorée par une valeur (mode fixé).
    """
    m = df.copy()

    mapping = {
        "Réel": "price_m2",
        "Prédit": "pred_price_m2",
        "Erreur absolue": "abs_error",
        "Erreur relative": "rel_error_pct",
    }
    colonne_valeur = mapping.get(mode, "rel_error_pct")
    if colonne_valeur not in m.columns:
        raise ValueError(f"Colonne introuvable pour la carte: {colonne_valeur}")

    if len(m) > 15000:
        m = m.sample(15000, random_state=42)

    m[colonne_valeur] = pd.to_numeric(m[colonne_valeur], errors="coerce").replace([np.inf, -np.inf], np.nan)

    if m[colonne_valeur].notna().sum() < 10:
        m[colonne_valeur] = m[colonne_valeur].fillna(0.0)
    else:
        med = float(m[colonne_valeur].median(skipna=True))
        m[colonne_valeur] = m[colonne_valeur].fillna(med)

    q1 = float(m[colonne_valeur].quantile(0.33))
    q2 = float(m[colonne_valeur].quantile(0.66))

    def couleur(v: float):
        if v <= q1:
            return [91, 42, 134, 150]   # violet
        if v <= q2:
            return [242, 201, 76, 150]  # doré
        return [200, 0, 0, 150]         # rouge

    m["couleur"] = m[colonne_valeur].astype(float).apply(couleur)
    m["rayon"] = 30

    tooltip = {
        "html": (
            "<b>Source:</b> {source_dvf}<br/>"
            "<b>Dépt:</b> {code_departement}<br/>"
            "<b>Commune:</b> {nom_commune}<br/>"
            "<b>Code postal:</b> {code_postal}<br/>"
            "<b>Type:</b> {type_local}<br/>"
            "<b>Surface bâtie:</b> {surface_reelle_bati} m²<br/>"
            "<b>Pièces:</b> {nombre_pieces_principales}<br/>"
            "<b>Nb lots effectif:</b> {nb_lots_effectif}<br/>"
            "<b>Surface Carrez totale:</b> {surface_carrez_totale} m²<br/>"
            "<b>Prix réel:</b> {price_m2} €/m²<br/>"
            "<b>Prix prédit:</b> {pred_price_m2} €/m²<br/>"
            "<b>Erreur abs:</b> {abs_error} €/m²<br/>"
            "<b>Erreur rel:</b> {rel_error_pct} %<br/>"
            "<b>Cluster:</b> {cluster_id}"
        )
    }

    couche = pdk.Layer(
        "ScatterplotLayer",
        data=m,
        get_position=["longitude", "latitude"],
        get_radius="rayon",
        get_fill_color="couleur",
        pickable=True,
        auto_highlight=True,
    )

    vue = pdk.ViewState(
        latitude=float(m["latitude"].mean()),
        longitude=float(m["longitude"].mean()),
        zoom=10.5,
        pitch=0,
    )

    deck = pdk.Deck(layers=[couche], initial_view_state=vue, tooltip=tooltip)
    return deck


def charger_csv_avec_source(chemin: str, etiquette_source: str) -> pd.DataFrame:
    """
    Charge un CSV DVF nettoyé et ajoute une colonne 'source_dvf' pour tracer l’origine.
    """
    df = pd.read_csv(chemin, low_memory=False)
    df["source_dvf"] = etiquette_source
    return df


def application_principale():
    st.set_page_config(page_title="PlaceToBe AI — DVF V2", layout="wide")

    inject_css()
    header_with_logo()

    st.sidebar.header("Paramètres")

    chemin_csv_92 = st.sidebar.text_input(
        "Chemin CSV clean (DVF 92)",
        value=r"C:\Users\arsla\PycharmProjects\ML\V0 prediction prix\dvf_92_clean_v2.csv",
    )
    chemin_csv_75 = st.sidebar.text_input(
        "Chemin CSV clean (DVF 75)",
        value=r"C:\Users\arsla\PycharmProjects\ML\V0 prediction prix\dvf-75_clean_v2.csv",
    )

    fraction_train = st.sidebar.slider("Train fraction", 0.50, 0.85, 0.70, 0.01)
    fraction_val = st.sidebar.slider("Validation fraction", 0.05, 0.30, 0.15, 0.01)

    k_clusters = st.sidebar.slider("KMeans k (clusters)", 10, 150, 40, 1)
    alpha_ridge = st.sidebar.number_input("Ridge alpha", min_value=0.0, value=1.0, step=0.5)

    profondeur_max = st.sidebar.slider("XGB max_depth", 2, 12, 6, 1)
    n_estimators_max = st.sidebar.slider("XGB n_estimators (max)", 500, 8000, 4000, 100)
    learning_rate = st.sidebar.number_input("XGB learning_rate", min_value=0.001, value=0.03, step=0.005, format="%.3f")
    subsample = st.sidebar.slider("XGB subsample", 0.4, 1.0, 0.8, 0.05)
    colsample = st.sidebar.slider("XGB colsample_bytree", 0.4, 1.0, 0.8, 0.05)
    reg_lambda = st.sidebar.number_input("XGB reg_lambda", min_value=0.0, value=1.0, step=0.5)
    min_child_weight = st.sidebar.number_input("XGB min_child_weight", min_value=0.0, value=1.0, step=0.5)
    early_stopping_rounds = st.sidebar.slider("Early stopping rounds", 20, 500, 150, 10)

    min_lignes_par_dept = st.sidebar.number_input(
        "Min rows / département (tendance Ridge)",
        min_value=50,
        value=200,
        step=50
    )

    # --- bouton run + mémoire du dernier run ---
    if "has_run" not in st.session_state:
        st.session_state.has_run = False

    bouton_run = st.sidebar.button("Entraîner et visualiser")

    if bouton_run:
        st.session_state.has_run = True

    if not st.session_state.has_run:
        st.info("Régle les paramètres dans la barre latérale puis clique sur 'Entraîner et visualiser'.")
        st.stop()

    # Chargement
    try:
        df_92 = charger_csv_avec_source(chemin_csv_92, "92")
    except Exception as e:
        st.error(f"Lecture CSV impossible (DVF 92): {e}")
        return

    try:
        df_75 = charger_csv_avec_source(chemin_csv_75, "75")
    except Exception as e:
        st.error(f"Lecture CSV impossible (DVF 75): {e}")
        return

    df = pd.concat([df_92, df_75], ignore_index=True)

    colonnes_requises = [
        "date_mutation",
        "price_m2",
        "latitude",
        "longitude",
        "surface_reelle_bati",
        "nombre_pieces_principales",
        "surface_terrain",
        "type_local",
        "code_postal",
        "code_commune",
        "nom_commune",
        "code_departement",
        "source_dvf",
        "surface_carrez_totale",
        "nb_lots_renseignes",
        "nb_lots_effectif",
        "surface_carrez_par_lot",
    ]
    colonnes_manquantes = [c for c in colonnes_requises if c not in df.columns]
    if colonnes_manquantes:
        st.error(f"Colonnes manquantes dans le CSV concaténé: {colonnes_manquantes}")
        return

    # Typage / nettoyage minimal
    df["date_mutation"] = pd.to_datetime(df["date_mutation"], errors="coerce")

    colonnes_num_a_forcer = [
        "price_m2", "latitude", "longitude",
        "surface_reelle_bati", "nombre_pieces_principales", "surface_terrain",
        "surface_carrez_totale", "nb_lots_renseignes", "nb_lots_effectif", "surface_carrez_par_lot",
    ]
    for c in colonnes_num_a_forcer:
        df[c] = pd.to_numeric(df[c], errors="coerce")

    for c in ["type_local", "code_postal", "code_commune", "nom_commune", "code_departement", "source_dvf"]:
        df[c] = df[c].astype("string")

    df = df.dropna(subset=["date_mutation", "price_m2", "latitude", "longitude"])
    df = df[df["price_m2"] > 0]

    df = df.sort_values("date_mutation").reset_index(drop=True)
    df = ajouter_features_temps(df, "date_mutation")

    df_train, df_val, df_test = split_temporel(df, fraction_train, fraction_val)
    if len(df_train) == 0 or len(df_val) == 0 or len(df_test) == 0:
        st.error("Split invalide: train/val/test vide. Ajuste train fraction / validation fraction.")
        return

    # Cible log
    df_train["log_price_m2"] = cible_log1p(df_train["price_m2"])
    df_val["log_price_m2"] = cible_log1p(df_val["price_m2"])
    df_test["log_price_m2"] = cible_log1p(df_test["price_m2"])

    df_train = df_train.dropna(subset=["log_price_m2"]).copy()
    df_val = df_val.dropna(subset=["log_price_m2"]).copy()
    df_test = df_test.dropna(subset=["log_price_m2"]).copy()

    if len(df_train) == 0 or len(df_val) == 0 or len(df_test) == 0:
        st.error("Après transformation log, un des splits est vide. Vérifie les données ou ajuste le split.")
        return

    # Tendance Ridge
    modele_tendance_global = entrainer_ridge_tendance(df_train, "log_price_m2", alpha_ridge=alpha_ridge, graine=42)

    modeles_tendance_par_dept: dict[str, Ridge] = {}
    for dept, g in df_train.groupby("code_departement", dropna=False):
        cle_dept = str(dept)
        if len(g) >= int(min_lignes_par_dept):
            modeles_tendance_par_dept[cle_dept] = entrainer_ridge_tendance(
                g, "log_price_m2", alpha_ridge=alpha_ridge, graine=42
            )

    def predire_tendance_par_dept(df_part: pd.DataFrame) -> np.ndarray:
        """
        Cette fonction prédit la tendance (log) par département si possible, sinon via le modèle global.
        """
        preds = pd.Series(index=df_part.index, dtype="float64")
        for dept, sous_df in df_part.groupby("code_departement", dropna=False):
            cle_dept = str(dept)
            modele = modeles_tendance_par_dept.get(cle_dept, modele_tendance_global)
            preds.loc[sous_df.index] = predire_ridge_tendance(modele, sous_df)
        return preds.to_numpy()

    for part in (df_train, df_val, df_test):
        part["trend_pred_log"] = predire_tendance_par_dept(part)
        part["residual_log"] = part["log_price_m2"] - part["trend_pred_log"]

    # KMeans
    kmeans = entrainer_kmeans(df_train, k_clusters=k_clusters, graine=42)
    df_train = ajouter_cluster_id(kmeans, df_train)
    df_val = ajouter_cluster_id(kmeans, df_val)
    df_test = ajouter_cluster_id(kmeans, df_test)
    for part in (df_train, df_val, df_test):
        part["cluster_id"] = part["cluster_id"].astype("string")

    colonnes_numeriques = [
        "surface_reelle_bati",
        "nombre_pieces_principales",
        "surface_terrain",
        "latitude",
        "longitude",
        "t_mois",
        "trimestre",
        "mois",
        "surface_carrez_totale",
        "nb_lots_renseignes",
        "nb_lots_effectif",
        "surface_carrez_par_lot",
    ]
    colonnes_categorielles = [
        "type_local",
        "code_postal",
        "code_commune",
        "cluster_id",
        "code_departement",
        "source_dvf",
    ]

    ohe = construire_ohe(df_train, colonnes_categorielles)

    X_train = transformer_features(df_train, colonnes_numeriques, colonnes_categorielles, ohe)
    y_train = df_train["residual_log"].values

    X_val = transformer_features(df_val, colonnes_numeriques, colonnes_categorielles, ohe)
    y_val = df_val["residual_log"].values

    X_test = transformer_features(df_test, colonnes_numeriques, colonnes_categorielles, ohe)
    y_test = df_test["residual_log"].values

    params_xgb = dict(
        n_estimators=int(n_estimators_max),
        learning_rate=float(learning_rate),
        max_depth=int(profondeur_max),
        min_child_weight=float(min_child_weight),
        subsample=float(subsample),
        colsample_bytree=float(colsample),
        reg_lambda=float(reg_lambda),
        random_state=42,
    )

    booster = entrainer_xgb_residu(
        X_train, y_train,
        X_val, y_val,
        params=params_xgb,
        early_stopping_rounds=int(early_stopping_rounds),
    )

    nb_arbres_a_utiliser = meilleure_iteration_plus_un(booster)

    df_train["pred_residual_log"] = booster.predict(xgb.DMatrix(X_train), iteration_range=(0, nb_arbres_a_utiliser))
    df_val["pred_residual_log"] = booster.predict(xgb.DMatrix(X_val), iteration_range=(0, nb_arbres_a_utiliser))
    df_test["pred_residual_log"] = booster.predict(xgb.DMatrix(X_test), iteration_range=(0, nb_arbres_a_utiliser))

    for part in (df_train, df_val, df_test):
        part["pred_log_price_m2"] = part["trend_pred_log"] + part["pred_residual_log"]
        part["pred_price_m2"] = inverse_cible_log1p(part["pred_log_price_m2"].values)
        part["pred_price_m2"] = np.where(part["pred_price_m2"] < 0, 0.0, part["pred_price_m2"])

        part["abs_error"] = (part["price_m2"] - part["pred_price_m2"]).abs()
        denom = part["price_m2"].replace(0, np.nan)
        part["rel_error_pct"] = (part["abs_error"] / denom * 100.0).replace([np.inf, -np.inf], np.nan).fillna(0.0)

    mae_train = calculer_mae(df_train["price_m2"].values, df_train["pred_price_m2"].values)
    mae_val = calculer_mae(df_val["price_m2"].values, df_val["pred_price_m2"].values)
    mae_test = calculer_mae(df_test["price_m2"].values, df_test["pred_price_m2"].values)
    gap = mae_val - mae_train

    # =========================
    # LAYOUT
    # =========================

    c1, c2, c3, c4 = st.columns(4)
    c1.metric("MAE train (€/m²)", f"{mae_train:.0f}")
    c2.metric("MAE val (€/m²)", f"{mae_val:.0f}")
    c3.metric("MAE test (€/m²)", f"{mae_test:.0f}")
    c4.metric("Gap val - train (€/m²)", f"{gap:.0f}")

    st.write("")

    tab_res, tab_map, tab_data = st.tabs(["Résultats", "Carte", "Table test"])

    with tab_res:
        left, right = st.columns(2, gap="large")

        with left:
            st.subheader("Prédit vs Réel (Test)")
            fig1 = fig_pred_vs_reel_plotly(df_test["price_m2"].values, df_test["pred_price_m2"].values, "")
            st.plotly_chart(fig1, use_container_width=True)

        with right:
            st.subheader("Résidus (Test)")
            fig2 = fig_hist_residus_plotly(df_test["price_m2"].values, df_test["pred_price_m2"].values, "")
            st.plotly_chart(fig2, use_container_width=True)

        st.write("")
        st.subheader("MAE par mois (Test)")
        fig3 = fig_mae_par_mois_plotly(df_test, "date_mutation", "price_m2", "pred_price_m2", "")
        st.plotly_chart(fig3, use_container_width=True)

    with tab_map:
        st.subheader("Carte interactive")
        mode_carte = "Erreur relative"
        deck = construire_carte_pydeck(df_test, mode=mode_carte)
        st.pydeck_chart(deck, use_container_width=True)

    with tab_data:
        st.subheader("Aperçu des prédictions (Test)")
        colonnes_affichage = [
            "date_mutation",
            "source_dvf",
            "code_departement",
            "nom_commune",
            "code_postal",
            "type_local",
            "surface_reelle_bati",
            "nombre_pieces_principales",
            "nb_lots_effectif",
            "surface_carrez_totale",
            "price_m2",
            "pred_price_m2",
            "abs_error",
            "rel_error_pct",
            "cluster_id",
        ]
        st.dataframe(df_test[colonnes_affichage].head(200), use_container_width=True)


if __name__ == "__main__":
    application_principale()
