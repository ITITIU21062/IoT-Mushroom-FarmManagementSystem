import paho.mqtt.client as mqtt
import json
import sqlite3
from datetime import datetime

# === MQTT Configuration ===
BROKER = "localhost"
PORT = 1883
TOPIC = "sensor/readings"

# === SQLite Database Path ===
import os
DB_PATH = os.path.join(os.path.dirname(__file__), "instance", "smart_home.db")


def insert_sensor_reading(sensor_id, value):
    try:
        conn = sqlite3.connect(DB_PATH)
        cursor = conn.cursor()
        timestamp = datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')
        cursor.execute("INSERT INTO sensor_readings (sensor_id, value1, timestamp) VALUES (?, ?, ?)",
               (sensor_id, value, timestamp))
        conn.commit()
        conn.close()
        print(f"✔️ Đã ghi sensor_id={sensor_id}, value={value}")
    except Exception as e:
        print(f"❌ Lỗi ghi DB: {e}")

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✅ Đã kết nối MQTT Broker.")
        client.subscribe(TOPIC)
    else:
        print("❌ Kết nối MQTT thất bại. Mã lỗi:", rc)

def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode()
        data = json.loads(payload)
        print("📥 Nhận:", data)

        sensor_id = int(data['sensor_id'])
        value = float(data['value'])

        insert_sensor_reading(sensor_id, value)

    except Exception as e:
        print("❌ Lỗi xử lý message:", e)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT, 60)
client.loop_forever()
