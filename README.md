# ESP32 Digital Signal Processor (DSP) Controller

Sistem kontrol audio DSP dengan ESP32 yang menampilkan antarmuka berbasis web dan kontrol fisik menggunakan LCD dan encoder.

## Fitur

- **Kontrol Crossover**: Pengaturan frekuensi cutoff untuk 4-way crossover (Sub, Low, Mid, High)
- **Pengaturan Compressor**: Threshold, ratio, attack dan release untuk dinamika audio
- **Limiter**: Threshold limiter untuk membatasi output audio
- **Delay**: Pengaturan delay untuk tiap band untuk koreksi fase
- **Preset**: Simpan, muat, dan terapkan preset audio
- **Kontrol Fisik**: Navigasi menu menggunakan encoder dan tombol
- **Panel Kontrol Web**: Antarmuka web yang responsif untuk pengaturan DSP

## Komponen Hardware

- ESP32 Development Board
- LCD I2C 16x2
- Rotary Encoder
- Tombol Pushbutton
- Modul DSP Audio (eksternel - tergantung implementasi)

## Penggunaan

### Kontrol Fisik

1. Putar encoder untuk memilih preset
2. Tekan encoder sekali untuk masuk ke mode edit gain
3. Dalam mode edit gain:
   - Putar encoder untuk mengubah nilai gain
   - Tekan singkat untuk berpindah ke band berikutnya
   - Tekan dan tahan untuk menyimpan pengaturan

### Kontrol Web

1. Hubungkan ke jaringan Wi-Fi ESP32-AUDIO (password: 12345678)
2. Akses panel kontrol di alamat IP 192.168.4.1
3. Sesuaikan parameter menggunakan antarmuka web
4. Gunakan preset untuk menyimpan dan memuat pengaturan

## Setup Firmware

1. Install Arduino IDE dan tambahkan dukungan ESP32
2. Install library yang diperlukan:
   - WiFi.h
   - WebServer.h
   - EEPROM.h
   - LiquidCrystal_I2C.h
   - Encoder.h
   - ArduinoJson.h
   - SPIFFS.h
3. Unggah kode ke ESP32
4. Unggah file web ke SPIFFS:
   - index.html
   - style.css
   - script.js

## Persiapan File Web

1. Install alat SPIFFS untuk Arduino IDE
2. Buat folder `data` di direktori sketch Arduino
3. Salin file frontend (index.html, style.css, script.js) ke folder data
4. Unggah file menggunakan "ESP32 Sketch Data Upload" dari menu Tools

## Struktur Data EEPROM

- Alamat 0-511: Preset gain manual (GainPreset)
- Alamat 512+: Preset DSP untuk web UI (DSPSettings)

## API Endpoints

- `/getSettings`: Mendapatkan pengaturan terkini sebagai JSON
- `/updateParam`: Memperbarui parameter tunggal (name, value)
- `/applyPreset`: Menerapkan preset (presetIndex)
- `/savePreset`: Menyimpan preset saat ini (presetIndex)
- `/loadPreset`: Memuat preset (presetIndex)

## Troubleshooting

- **Tidak dapat terhubung ke Wi-Fi**: Pastikan ESP32 menyala dan dalam jangkauan
- **LCD tidak menampilkan teks**: Periksa koneksi I2C dan alamat LCD
- **Encoder tidak berfungsi**: Periksa koneksi pin dan nilai pullup
- **Web UI tidak berfungsi**: Pastikan file sudah diunggah ke SPIFFS dengan benar
