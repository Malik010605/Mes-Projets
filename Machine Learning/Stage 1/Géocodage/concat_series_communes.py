from pathlib import Path
import pandas as pd

# Chemins des fichiers d'entrée
base_dir = Path(r"C:\Users\arsla\PycharmProjects\ML\Géocodage")
file_75 = base_dir / "series_commune_75.parquet"
file_92 = base_dir / "series_commune_92.parquet"

# Chemin de sortie
out_file = base_dir / "series_commune_75_92.parquet"

# Lecture des deux fichiers
df_75 = pd.read_parquet(file_75)
df_92 = pd.read_parquet(file_92)

# Vérification minimale de cohérence des colonnes
if set(df_75.columns) != set(df_92.columns):
    raise ValueError("Les deux fichiers n'ont pas exactement les mêmes colonnes.")

# Concaténation verticale
df_all = pd.concat([df_75, df_92], axis=0, ignore_index=True)

# Optionnel : tri par zone puis par date (recommandé)
df_all = df_all.sort_values(["zone_id", "period_start"]).reset_index(drop=True)

# Export Parquet
df_all.to_parquet(out_file, index=False)

print(f"Fichier concaténé créé : {out_file}")
print(f"Nombre total de lignes : {len(df_all)}")
print(f"Nombre de zones uniques : {df_all['zone_id'].nunique()}")
