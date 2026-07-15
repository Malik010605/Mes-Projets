from pathlib import Path
import pandas as pd

# Dossier de travail
base_dir = Path(r"C:\Users\arsla\PycharmProjects\ML\Géocodage")

# Fichiers d'entrée
file_75 = base_dir / "dvf-75_clean_v2.csv"
file_92 = base_dir / "dvf_92_clean_v2.csv"

# Fichier de sortie
out_file = base_dir / "dvf_75_92_clean_v2.csv"

# Lecture des CSV
df_75 = pd.read_csv(file_75, low_memory=False)
df_92 = pd.read_csv(file_92, low_memory=False)

# Vérification stricte des colonnes
if list(df_75.columns) != list(df_92.columns):
    raise ValueError("Les deux fichiers CSV n'ont pas exactement les mêmes colonnes (ordre ou noms).")

# Concaténation verticale
df_all = pd.concat([df_75, df_92], axis=0, ignore_index=True)

# Export CSV
df_all.to_csv(out_file, index=False)

print(f"Fichier concaténé créé : {out_file}")
print(f"Nombre total de lignes : {len(df_all)}")
