from flask import Flask, request, jsonify
import csv
import os
from datetime import datetime

app = Flask(__name__)

import argparse

parser = argparse.ArgumentParser(description="Run PZEM Flask server")
parser.add_argument("--csv", type=str, default="pzem_log.csv", help="Tên file CSV để lưu dữ liệu")
args = parser.parse_args()

CSV_FILE = args.csv

# Nếu chưa có file, tạo và thêm header
if not os.path.exists(CSV_FILE):
    with open(CSV_FILE, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["timestamp", "voltage_V", "current_A", "power_W", "energy_kWh", "frequency_Hz", "pf"])

@app.route("/pzem", methods=["POST"])
def receive_pzem():
    try:
        data = request.get_json()
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        voltage = data.get("voltage", 0)
        current = data.get("current", 0)
        power   = data.get("power", 0)
        energy  = data.get("energy", 0)
        frequency = data.get("frequency", 0)
        pf = data.get("pf", 0)

        # Ghi vào CSV
        with open(CSV_FILE, "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerow([timestamp, voltage, current, power, energy, frequency, pf])

        print(f"[{timestamp}] Data logged: V={voltage}, I={current}, P={power}, E={energy}, F={frequency}, PF={pf}")
        return jsonify({"status":"ok"}), 200
    except Exception as e:
        print("Error:", e)
        return jsonify({"status":"error", "msg": str(e)}), 500

if __name__ == "__main__":
    # Chạy server trên mọi interface, port 5000
    app.run(host="0.0.0.0", port=5000)