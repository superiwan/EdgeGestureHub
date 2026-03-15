import csv
from pathlib import Path

src_dir = Path(r"C:\Users\prohibit\Desktop\stm32f4_project\logs\dataset")
out_dir = src_dir / "neai"
out_dir.mkdir(parents=True, exist_ok=True)

for p in src_dir.glob("*.csv"):
    rows_out = 0
    out = out_dir / p.name
    with p.open("r", encoding="utf-8", newline="") as f, out.open("w", encoding="utf-8", newline="") as g:
        r = csv.reader(f)
        w = csv.writer(g)
        for row in r:
            if not row:
                continue
            # skip header if present
            if row[0].strip().lower() == "ax":
                continue
            if len(row) < 6:
                continue
            w.writerow(row[:6])
            rows_out += 1
    print(f"{p.name} -> {out.name}, rows={rows_out}")
