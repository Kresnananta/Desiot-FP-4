# Sistem Prediksi Bahaya Gas Beracun dan Anomali Termal pada Tambang Bawah Tanah berbasis AIoT

[![Tech Stack](https://img.shields.io/badge/Status-Project%20Development-blue)](https://github.com/)

---

##  Nama Anggota Kelompok

Azfarafi Gustiar Jati - 5024241037  
Ilan Hawwari Prasojo - 5024241039  
Anak Agung Ngurah Agung Kresna Ananta - 5024241085


---

##  Deskripsi Proyek

### Latar Belakang & Masalah
Sektor tambang mineral dan batubara merupakan salah satu penopang utama perekonomian Indonesia. Namun, seiring menipisnya cadangan di permukaan, industri dipaksa beralih ke metode **tambang bawah tanah (*underground mining*)**. Lingkungan bawah tanah yang tertutup dan minim sirkulasi udara memiliki risiko kecelakaan kerja yang sangat tinggi.

Proyek ini dilatarbelakangi oleh tragedi nyata, salah satunya **Tragedi Tambang Emas Pongkor, Bogor pada Januari 2026**. Kebocoran gas beracun dan ledakan di area tambang tersebut menjadi bukti nyata bahwa metode monitoring konvensional masih memiliki celah fatal. Dua ancaman utama yang kami fokuskan adalah:
1. **Gas Beracun Tak Kasatmata:** Akumulasi gas berbahaya seperti Karbon Monoksida (CO) dan Hidrogen Sulfida ($\text{H}_2\text{S}$) yang memicu keracunan massal hingga ledakan tanpa disadari oleh pekerja.
2. **Anomali Termal:** Fluktuasi suhu abnormal yang menjadi indikator awal mesin mengalami *overheating* atau adanya potensi kebakaran tersembunyi di balik dinding tambang.

### Solusi yang Ditawarkan
Kami mengembangkan solusi berbasis **AIoT (Artificial Intelligence of Things)** yang mampu mendeteksi tren bahaya secara *real-time*. Alat ini tidak hanya bekerja sebagai alarm biasa, melainkan memanfaatkan pemrosesan data (AI) untuk membaca tren kenaikan parameter sebelum mencapai batas fatal manusia. 

Sistem kami merujuk pada standar ambang batas **NIOSH (National Institute for Occupational Safety and Health)**:
* **$\le$ 35 PPM:** Batas aman paparan 8 jam (*8-hour TWA*).
* **> 200 PPM:** Mulai menimbulkan gejala pusing, mual, dan kelelahan.
* **$\ge$ 800 PPM:** Paparan selama 2 jam dapat berakibat fatal.

#### Ambang Batas Suhu Lingkungan & Kerja (Anomali Termal)
* **≤ 30°C:** Batas suhu kerja normal dan optimal bagi manusia di area bawah tanah (kondisi sirkulasi udara baik).
* **31°C – 37°C:** Zona Waspada (*Heat Strain Zone*); pekerja berisiko tinggi mengalami dehidrasi berat, kram, dan kelelahan ekstrem (*heat exhaustion*). Produktivitas menurun tajam dan membutuhkan rotasi kerja atau pendinginan tambahan.
* **≥ 38°C:** Zona Bahaya Akut (*Anomali Termal*); mengindikasikan kegagalan sistem ventilasi, pemanasan ekstrem pada mesin (*overheating*), atau indikasi awal titik api/kebakaran tersembunyi. Area harus segera dievakuasi.

Dengan sistem ini, peringatan dini (*early warning*) dapat diberikan lebih cepat sehingga evakuasi pekerja dapat dilakukan sebelum nyawa melayang..

---

## Flow System

# Sistem Prediksi Bahaya Gas Beracun dan Anomali Termal Tambang Bawah Tanah Berbasis AIoT

Sistem pemantauan dan prediksi berbasis AIoT (*Artificial Intelligence of Things*) yang dirancang untuk mendeteksi risiko gas berbahaya dan anomali termal di area tambang bawah tanah secara *real-time*.

---

## Arsitektur Sistem (Berdasarkan Flowchart)

Sistem ini dirancang dengan alur data sekuensial dari pengumpulan data fisik hingga visualisasi akhir dan sistem peringatan:

1. **Edge Layer (Pengumpulan Data)**
   * **MQ-2 Sensor:** Mendeteksi konsentrasi gas/asap di area tambang.
   * **DH-22 Sensor:** Mengukur suhu (Celsius) dan kelembaban udara.
   * **ESP 32:** Membaca data dari kedua sensor secara *real-time*.

2. **Network Layer (Transmisi Data)**
   * **MQTT Protocol:** ESP 32 mengirim data sensor dalam format JSON yang ringan.
   * **Raspberry Pi (Broker):** Bertindak sebagai MQTT Broker (misalnya Mosquitto) untuk menerima dan meneruskan *traffic* data.

3. **Backend & Storage Layer (Pengolahan Utama)**
   * **n8n (Backend):** Mengambil data dari MQTT broker dan bertindak sebagai mesin pengolah di sisi *backend*.
   * **Database (Raw Data):** n8n memasukkan data sensor mentah yang diterima ke dalam database untuk diarsipkan.

4. **AI & Intelligence Layer (Analisis Prediktif)**
   * **AI Model Prediction:** Model AI mengambil data dari database untuk menganalisis tren anomali dan mengklasifikasikan status lingkungan.
   * **Telegram (Alert):** Jika AI mendeteksi adanya indikasi bahaya atau anomali termal, model AI akan langsung memicu *alert* ke Telegram.
   * **Database (Prediction Data):** Hasil klasifikasi dan prediksi dari AI Model disimpan kembali ke dalam database.

5. **Frontend & Visualization Layer (Monitoring)**
   * **Metabase (Frontend):** Bertindak sebagai *user interface* utama yang menarik data hasil prediksi dari database untuk disajikan dalam bentuk *dashboard* visual interaktif bagi manajemen tambang.
   
## Parameter Standar Acuan Status Keamanan

Model AI mengklasifikasikan kondisi tambang bawah tanah ke dalam 3 tingkatan status berdasarkan kombinasi pembacaan fisik sensor dan analisis tren waktu (*time-series anomaly detection*):

| Status | Batas Suhu (DH-22) | Batas Gas (MQ-2) | Logika AI & Aksi Sistem |
| :--- | :--- | :--- | :--- |
| **AMAN** | < 30°C | < 200 PPM | **Kondisi Normal.** Suhu udara dan kadar gas berada dalam batas wajar untuk pekerja tambang. <br> *Aksi:* Data diarsipkan ke Database, Metabase diperbarui berkala. |
| **WASPADA** | 30°C - 38°C <br>*(Atau lonjakan > 2°C/menit)* | 200 PPM - 500 PPM <br>*(Ada tren kenaikan grafik)* | **Deteksi Anomali Awal.** AI mendeteksi adanya indikasi anomali termal atau kebocoran gas tipis secara bertahap.<br> *Aksi:* Mengirim notifikasi awal ke Telegram. |
| **BAHAYA** | > 38°C <br>*(Panas ekstrem/kebakaran)* | > 500 PPM <br>*(Gas beracun pekat)* | **Kondisi Kritis.** Risiko tinggi kebakaran bawah tanah atau kontaminasi gas yang mengancam nyawa.<br> *Aksi:* **Telegram langsung memicu alarm darurat secara otomatis** untuk evakuasi pekerja. |

---

## Fitur Chatbot & Alert Telegram

Telegram Bot dalam sistem ini bekerja secara dua arah: **Push Notification (Alerting)** dan **Pull Request (Chatbot Interaktif)**. Pekerja atau pengawas tambang dapat mengirimkan perintah berikut untuk mendapatkan informasi *on-demand*:

* `/status` — **Data Sensor Terkini:** Menampilkan nilai real-time dari sensor MQ-2, DH-22, serta status prediksi keamanan terakhir dari AI (`Aman` / `Waspada` / `Bahaya`).
* `/history` — **Riwayat 1 Jam:** Menampilkan grafik singkat atau ringkasan tren fluktuasi gas dan suhu selama 1 jam terakhir yang diambil dari database.
* `/alerts` — **Alert Terbaru:** Menampilkan log atau daftar peringatan bahaya/anomali termal terakhir yang sempat terpicu oleh sistem.
* `/stats` — **Statistik Hari Ini:** Menyajikan metrik harian seperti suhu rata-rata, kadar gas tertinggi (puncak), dan persentase durasi status tambang dalam kondisi aman sepanjang hari ini.
---

## Foto Alat

Berikut adalah dokumentasi fisik dari alat/perangkat keras yang telah dirakit dan digunakan dalam proyek ini:

| Pandangan Atas | Pandangan Samping / Detail Komponen |
| :---: | :---: |
| <!-- Masukkan tag foto di sini, contoh: --> <img src="https://via.placeholder.com/350x250" width="350" alt="Alap Tampak Atas"> | <img src="https://via.placeholder.com/350x250" width="350" alt="Detail Komponen"> |

*Keterangan: Berikan sedikit penjelasan mengenai foto alat di atas, misalnya wadah yang digunakan atau posisi peletakan sensor.*

---

## Stack / Tech yang Digunakan

### Perangkat Keras (Hardware & Komponen Elektronika)
* **Mikrokontroler:** ESP32 (Internal Wi-Fi module)
* **SBC / Gateway:** Raspberry Pi 4
* **Sensor:** Sensor Gas MQ-6 & Sensor Suhu/Kelembaban DHT22
* **Komponen Pendukung:**
  * Breadboard (Papan Prototiping)
  * Kabel Jumper (Male-to-Male / Male-to-Female)
  * Kabel USB Type-C (Power & Flashing)
  * Resistor 220 Ohm & Resistor 1k Ohm (Pull-up / Pembagi Tegangan)

### Perangkat Lunak & Cloud (Software Stack)
* **Protokol Komunikasi:** MQTT (Format Payload: JSON)
* **Backend Integration Engine:** n8n Workflow Automation
* **Artificial Intelligence:** Gemini 2.5 LLM/Prediction Model via API
* **Database Management System:** PostgreSQL
* **Data Visualization Tool:** Metabase (Frontend Dashboard)
* **Alerting Platform:** Telegram Bot API

---

## Video Demo & Dashboard Metabase

