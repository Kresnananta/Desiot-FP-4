#include <Arduino.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <MQUnifiedsensor.h>

// Definisikan Pin sesuai wiring kita tadi
#define DHTPIN 32        // Pin D2 untuk DHT22
#define DHTTYPE DHT22   // Tipe sensor DHT22
#define MQ6_PIN 36      // Pin VP (GPIO 36 / ADC1_CH0) untuk gas MQ-6

// --- Konfigurasi MQUnifiedsensor untuk ESP32 ---
#define Board "ESP-32"
#define Type "MQ-6"
#define Voltage_Resolution 3.3
#define ADC_Bit_Resolution 12 // ESP32 menggunakan ADC 12-bit (0-4095)
#define RatioMQ6CleanAir 10.0 // Rasio Rs/R0 standar MQ-6 di udara bersih

// Inisialisasi sensor DHT
DHT dht(DHTPIN, DHTTYPE);
MQUnifiedsensor MQ6(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ6_PIN, Type);

void setup() {
  // Samakan baudrate dengan di file .env gateway.py kamu
  Serial.begin(115200);

  // --- Setup MQ-6 ---
  MQ6.setRegressionMethod(1); // Metode Regresi: PPM = a * ratio^b
  
  // Parameter Kurva untuk MQ-6 mendeteksi gas LPG/Butana (korek gas)
  MQ6.setA(1007.2); 
  MQ6.setB(-2.18);
  
  MQ6.init();

  // --- Proses Kalibrasi Awal (Wajib di udara bersih!) ---
  Serial.println("{\"info\": \"Mengkalibrasi MQ-6. Tolong jangan beri gas dulu!\"}");
  float calcR0 = 0;
  for(int i = 1; i <= 10; i++) {
    MQ6.update(); // Update data
    calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    delay(500);
  }
  MQ6.setR0(calcR0/10);
  
  // Cek apakah pin terhubung dengan benar
  if(isinf(calcR0) || calcR0 == 0) {
    Serial.println("{\"error\": \"Kabel MQ-6 putus atau tidak terhubung ke pin 36!\"}");
    while(1); // Berhenti di sini kalau error
  }
  
  Serial.println("{\"info\": \"Kalibrasi Selesai! Mulai membaca data...\"}");
  
  dht.begin();
  
  // Beri waktu 2 detik agar sensor stabil saat pertama kali menyala
  delay(2000); 
}

void loop() {
  // 1. Baca data dari DHT22
  float suhu = dht.readTemperature();
  
  // 2. Baca data analog dari MQ-6 (Nilai mentah 0 - 4095 di ESP32)
  // int gas_raw = analogRead(MQ6_PIN);

  // 3. Validasi: Pastikan DHT22 berhasil terbaca (bukan NaN / Not a Number)
  if (isnan(suhu)) {
    Serial.println("{\"error\": \"Gagal membaca sensor DHT22! Cek kabel atau resistor pull-up.\"}");
    delay(2000);
    return; // Ulangi loop dari awal
  }

  // --- Baca MQ-6 dalam satuan PPM ---
  MQ6.update(); // Perbarui nilai tegangan
  float gas_ppm = MQ6.readSensor(); // Dapatkan nilai PPM!

  // 5. Buat objek JSON
  JsonDocument doc;
  doc["suhu"] = suhu;
  doc["gas_co"] = gas_ppm;

  // 6. Cetak JSON ke Serial (Nanti akan ditangkap oleh gateway.py)
  serializeJson(doc, Serial);
  Serial.println(); // Beri enter (newline) agar gateway.py tahu satu data sudah selesai

  // Tunggu 5 detik sebelum membaca data lagi
  delay(5000); 
}