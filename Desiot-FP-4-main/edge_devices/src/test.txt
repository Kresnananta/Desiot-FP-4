#include <Arduino.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Definisikan Pin sesuai wiring kita tadi
#define DHTPIN 32        // Pin D2 untuk DHT22
#define DHTTYPE DHT22   // Tipe sensor DHT22
#define MQ6_PIN 36      // Pin VP (GPIO 36 / ADC1_CH0) untuk gas MQ-6

// Inisialisasi sensor DHT
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Samakan baudrate dengan di file .env gateway.py kamu
  Serial.begin(115200);
  
  dht.begin();
  
  // Beri waktu 2 detik agar sensor stabil saat pertama kali menyala
  delay(2000); 
}

void loop() {
  // 1. Baca data dari DHT22
  float suhu = dht.readTemperature();
  
  // 2. Baca data analog dari MQ-6 (Nilai mentah 0 - 4095 di ESP32)
  int gas_raw = analogRead(MQ6_PIN);

  // 3. Validasi: Pastikan DHT22 berhasil terbaca (bukan NaN / Not a Number)
  if (isnan(suhu)) {
    Serial.println("{\"error\": \"Gagal membaca sensor DHT22! Cek kabel atau resistor pull-up.\"}");
    delay(2000);
    return; // Ulangi loop dari awal
  }

  // 4. Konversi nilai mentah gas ke simulasi tegangan atau PPM sederhana
  // (Untuk saat ini kita ubah jadi format desimal agar mirip seperti di n8n kamu: 2.6, 3.1, dst)
  float gas_co = gas_raw * (3.3 / 4095.0); 

  // 5. Buat objek JSON
  JsonDocument doc;
  doc["suhu"] = suhu;
  doc["gas_co"] = gas_co;

  // 6. Cetak JSON ke Serial (Nanti akan ditangkap oleh gateway.py)
  serializeJson(doc, Serial);
  Serial.println(); // Beri enter (newline) agar gateway.py tahu satu data sudah selesai

  // Tunggu 5 detik sebelum membaca data lagi
  delay(5000); 
}