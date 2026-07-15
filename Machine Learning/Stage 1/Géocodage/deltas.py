import pandas as pd

df = pd.read_parquet(r"C:\Users\arsla\PycharmProjects\ML\Géocodage\series_commune_92.parquet")
deltas = df.sort_values("period_start")["period_start"].drop_duplicates().diff().dropna()
print(deltas.dt.days.describe())
print(df["period_start"].sort_values().head(10).to_string(index=False))
print(df["period_start"].sort_values().tail(10).to_string(index=False))
