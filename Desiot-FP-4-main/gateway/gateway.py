import serial
import time
import json
import paho.mqtt.client as mqtt
import os
from dotenv import load_dotenv

load_dotenv()

PORT = os.getenv('SERIAL_PORT')
BAUDRATE = os.getenv('BAUDRATE')

MQTT_BROKER = os.getenv('MQTT_BROKER')
MQTT_PORT = int(os.getenv('MQTT_PORT'))
MQTT_TOPIC = os.getenv('MQTT_TOPIC')

mqtt_client = mqtt.Client()

try:
    print(f"Menghubungkan ke MQTT Broker {MQTT_BROKER}...")
    mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
    mqtt_client.loop_start() # Jalankan MQTT di background
    print("Berhasil terhubung ke MQTT Server!\n")


    ser = serial.Serial(PORT, BAUDRATE, timeout=1)
    ser.setDTR(False)
    ser.setRTS(False)
    ser.reset_input_buffer()

    print(f"--- Terhubung ke ESP32 di {PORT} ---")

    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8', errors='ignore').strip()

            if line:
                try:
                    data = json.loads(line)
                    print(f'Data dari ESP32: {data}')

                    payload = json.dumps(data)
                    mqtt_client.publish(MQTT_TOPIC, payload)
                    print(f'   [MQTT] Terkirim -> {MQTT_TOPIC}')

                except json.JSONDecodeError:
                    print(f'Bukan JSON: {line}')
                
        time.sleep(0.1)

except Exception as e:
    print(f'Error: {e}')

except KeyboardInterrupt:
    print('\nProgram dihentikan user')
    mqtt_client.loop_stop()
    mqtt_client.disconnect()