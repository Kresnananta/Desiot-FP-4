import serial
import time
import json

PORT = 'COM7'
BAUDRATE = 115200

try:
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

                except json.JSONDecodeError:
                    print(f'Bukan JSON: {line}')
                
        time.sleep(0.1)

except Exception as e:
    print(f'Error: {e}')

except KeyboardInterrupt:
    print('\nProgram dihentikan user')