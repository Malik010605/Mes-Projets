"""
gentrif_build_series.py

Script 1/2 (Gentrification) - Construction des series temporelles par zone.

Entree:
- Un CSV DVF nettoye (typiquement le meme que celui utilise pour XGBoost V3)

Sortie:
- Un fichier series (Parquet recommande) contenant, par zone et par periode:
    zone_id, period_start, price_m2_median, price_m2_mean, price_m2_trimmed_mean,
    price_m2_log, n_sales, total_value, total_surface
"""

from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Tuple

import numpy as np
import pandas as pd


ALIAS_COLS: Dict[str, List[str]] = {
    "date_mutation": ["date_mutation", "date", "date_vente", "mutation_date"],
    "valeur_fonciere": ["valeur_fonciere", "valeur", "prix", "price", "montant"],
    "surface": [
        "surface_reelle_bati",
        "surface",
        "surface_bati",
        "surface_reelle",
        "surface_totale",
    ],
    "type_local": ["type_local", "type", "nature", "nature_mutation"],
    "code_iris": ["code_iris", "iris", "iris_code", "insee_iris"],
    "code_commune": ["code_commune", "insee_com", "code_insee", "commune_code"],
    "code_postal": ["code_postal", "cp", "postal_code"],
    "latitude": ["latitude", "lat"],
    "longitude": ["longitude", "lon", "lng"],
}


@dataclass
class BuildConfig:
    input_path: Path
    output_path: Path
    zone: str
    freq: str
    min_sales: int
    filter_type_local: Optional[str]
    trim_q: float
    keep_negative_or_zero: bool


def _sniff_sep(path: Path, n_lines: int = 5) -> str:
    """
    Essaie de deviner le separateur CSV parmi: ; , tab.
    """
    with path.open("r", encoding="utf-8", errors="ignore") as f:
        head = "".join([next(f, "") for _ in range(n_lines)])
    counts = {
        ";": head.count(";"),
        ",": head.count(","),
        "\t": head.count("\t"),
    }
    return max(counts.items(), key=lambda kv: kv[1])[0] if head else ","


def _find_col(df: pd.DataFrame, canonical: str) -> Optional[str]:
    """
    Trouve le nom de colonne present dans df parmi les alias definis.
    """
    candidates = ALIAS_COLS.get(canonical, [canonical])
    cols_lower = {c.lower(): c for c in df.columns}
    for cand in candidates:
        c_lower = cand.lower()
        if c_lower in cols_lower:
            return cols_lower[c_lower]
    return None


def _require_col(df: pd.DataFrame, canonical: str) -> str:
    col = _find_col(df, canonical)
    if col is None:
        aliases = ALIAS_COLS.get(canonical, [canonical])
        raise ValueError(
            f"Colonne manquante: '{canonical}'. "
            f"Alias acceptes: {aliases}. Colonnes disponibles: {list(df.columns)}"
        )
    return col


def _select_zone_column(df: pd.DataFrame, zone: str) -> str:
    zone = zone.strip().lower()
    if zone == "iris":
        return _require_col(df, "code_iris")
    if zone == "commune":
        return _require_col(df, "code_commune")
    if zone in ("cp", "code_postal", "postal"):
        return _require_col(df, "code_postal")
    if zone in ("latlon", "geo"):
        lat = _require_col(df, "latitude")
        lon = _require_col(df, "longitude")
        return f"{lat}|{lon}"
    raise ValueError("zone invalide. Valeurs: iris, commune, cp, latlon")


def _make_zone_id(df: pd.DataFrame, zone_col: str) -> pd.Series:
    if "|" in zone_col:
        lat_col, lon_col = zone_col.split("|", 1)
        lat = pd.to_numeric(df[lat_col], errors="coerce").round(5)
        lon = pd.to_numeric(df[lon_col], errors="coerce").round(5)
        return lat.astype("string") + "," + lon.astype("string")
    return df[zone_col].astype("string")


def _to_period_start(s: pd.Series, freq: str) -> pd.Series:
    dt = pd.to_datetime(s, errors="coerce", utc=False)
    if freq.upper() == "M":
        return dt.dt.to_period("M").dt.to_timestamp(how="start")
    if freq.upper() == "Q":
        return dt.dt.to_period("Q").dt.to_timestamp(how="start")
    raise ValueError("freq invalide. Valeurs: M ou Q")


def _trimmed_mean(x: np.ndarray, q: float) -> float:
    if x.size == 0:
        return np.nan
    if q <= 0.0:
        return float(np.mean(x))
    lo = np.quantile(x, q)
    hi = np.quantile(x, 1.0 - q)
    y = x[(x >= lo) & (x <= hi)]
    return float(np.mean(y)) if y.size else np.nan


def build_series(cfg: BuildConfig) -> pd.DataFrame:
    """
    Cette fonction build_series nous fournit une table agregee par zone et periode.
    Elle nous sert a produire un artefact intermediaire stable, relisible par le script 2.
    """
    if not cfg.input_path.exists():
        raise FileNotFoundError(f"Fichier introuvable: {cfg.input_path}")

    sep = _sniff_sep(cfg.input_path)
    df = pd.read_csv(cfg.input_path, sep=sep, low_memory=False)

    col_date = _require_col(df, "date_mutation")
    col_value = _require_col(df, "valeur_fonciere")
    col_surface = _require_col(df, "surface")

    zone_col = _select_zone_column(df, cfg.zone)

    if cfg.filter_type_local is not None:
        col_type = _find_col(df, "type_local")
        if col_type is None:
            raise ValueError(
                "filter_type_local fourni mais colonne 'type_local' introuvable "
                f"(alias: {ALIAS_COLS.get('type_local')})."
            )
        df = df[df[col_type].astype("string").str.lower() == cfg.filter_type_local.lower()]

    df = df.copy()

    df["period_start"] = _to_period_start(df[col_date], cfg.freq)
    df["valeur_fonciere_num"] = pd.to_numeric(df[col_value], errors="coerce")
    df["surface_num"] = pd.to_numeric(df[col_surface], errors="coerce")
    df["zone_id"] = _make_zone_id(df, zone_col)

    df = df.dropna(subset=["period_start", "valeur_fonciere_num", "surface_num", "zone_id"])

    if not cfg.keep_negative_or_zero:
        df = df[(df["valeur_fonciere_num"] > 0) & (df["surface_num"] > 0)]

    df["price_m2"] = df["valeur_fonciere_num"] / df["surface_num"]
    df = df.replace([np.inf, -np.inf], np.nan).dropna(subset=["price_m2"])

    if not cfg.keep_negative_or_zero:
        df = df[df["price_m2"] > 0]

    def agg_group(g: pd.DataFrame) -> pd.Series:
        x = g["price_m2"].to_numpy(dtype=float)
        return pd.Series(
            {
                "price_m2_median": float(np.median(x)) if x.size else np.nan,
                "price_m2_mean": float(np.mean(x)) if x.size else np.nan,
                "price_m2_trimmed_mean": _trimmed_mean(x, cfg.trim_q),
                "n_sales": int(x.size),
                "total_value": float(g["valeur_fonciere_num"].sum()),
                "total_surface": float(g["surface_num"].sum()),
            }
        )

    out = (
        df.groupby(["zone_id", "period_start"], observed=True, sort=True)
        .apply(agg_group)
        .reset_index()
    )

    out = out[out["n_sales"] >= int(cfg.min_sales)].copy()

    out["price_m2_log"] = np.log(out["price_m2_median"].astype(float))
    out = out.replace([np.inf, -np.inf], np.nan).dropna(subset=["price_m2_log"])

    out = out.sort_values(["zone_id", "period_start"]).reset_index(drop=True)
    return out


def _write_output(df: pd.DataFrame, output_path: Path) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    suffix = output_path.suffix.lower()

    if suffix == ".parquet":
        try:
            df.to_parquet(output_path, index=False)
            return
        except Exception as e:
            fallback = output_path.with_suffix(".csv")
            df.to_csv(fallback, index=False)
            raise RuntimeError(
                f"Echec ecriture Parquet: {e}. Export CSV de secours: {fallback}"
            ) from e

    if suffix == ".csv":
        df.to_csv(output_path, index=False)
        return

    raise ValueError("Extension sortie invalide. Utilise .parquet ou .csv")


def parse_args(argv: Optional[List[str]] = None) -> BuildConfig:
    p = argparse.ArgumentParser(
        description="Construit des series temporelles DVF agregees par zone pour detection de gentrification."
    )
    p.add_argument("--input", required=True, help="Chemin vers dvf_clean.csv")
    p.add_argument(
        "--out",
        required=True,
        help="Chemin de sortie (recommande: .parquet). Exemple: data/series_iris.parquet",
    )
    p.add_argument(
        "--zone",
        default="iris",
        choices=["iris", "commune", "cp", "latlon"],
        help="Niveau spatial. iris recommande si la colonne existe.",
    )
    p.add_argument(
        "--freq",
        default="Q",
        choices=["M", "Q"],
        help="Frequence d'aggregation: M (mensuel) ou Q (trimestriel).",
    )
    p.add_argument(
        "--min_sales",
        type=int,
        default=8,
        help="Nombre minimal de ventes par zone-periode pour conserver le point de serie.",
    )
    p.add_argument(
        "--filter_type_local",
        default=None,
        help="Optionnel. Filtre sur type_local exact (ex: Appartement, Maison).",
    )
    p.add_argument(
        "--trim_q",
        type=float,
        default=0.05,
        help="Quantile de trimming pour la moyenne tronquee (0.05 = retire 5%% bas et 5%% haut).",
    )
    p.add_argument(
        "--keep_negative_or_zero",
        action="store_true",
        help="Si active, ne filtre pas les valeurs <= 0 (non recommande).",
    )

    a = p.parse_args(argv)
    return BuildConfig(
        input_path=Path(a.input),
        output_path=Path(a.out),
        zone=a.zone,
        freq=a.freq,
        min_sales=a.min_sales,
        filter_type_local=a.filter_type_local,
        trim_q=float(a.trim_q),
        keep_negative_or_zero=bool(a.keep_negative_or_zero),
    )


def main(argv: Optional[List[str]] = None) -> int:
    """
    Cette fonction main nous fournit l'execution CLI du script.
    Elle nous sert a produire un fichier de series robuste, sans melanger la logique de scoring.
    """
    try:
        cfg = parse_args(argv)
        out = build_series(cfg)
        _write_output(out, cfg.output_path)

        print("OK")
        print(f"Input:  {cfg.input_path}")
        print(f"Output: {cfg.output_path}")
        print(f"Rows:   {len(out)}")
        print(f"Zones:  {out['zone_id'].nunique() if 'zone_id' in out.columns else 'NA'}")
        return 0
    except Exception as e:
        print(f"ERREUR: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())