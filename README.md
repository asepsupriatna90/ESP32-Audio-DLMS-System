# 🎛️ ESP32 Audio DSP Controller with LCD & Web UI

Sistem kontrol audio digital berbasis **ESP32** yang menggabungkan antarmuka fisik (LCD dan rotary encoder) dengan antarmuka web responsif. Dirancang untuk mengatur parameter DSP secara real-time, sistem ini cocok untuk aplikasi audio profesional maupun DIY.

---

## 📖 Deskripsi Umum

Proyek ini memungkinkan pengguna untuk mengontrol berbagai parameter Digital Signal Processing (DSP) melalui:

- **Antarmuka Fisik**: Menggunakan LCD I2C 16x2 dan rotary encoder untuk navigasi menu dan pengaturan parameter.
- **Antarmuka Web**: Mengakses dan mengatur parameter DSP melalui browser web yang responsif.

Fitur utama meliputi pengaturan crossover, compressor, limiter, delay, dan manajemen preset.

---

## 🔧 Komponen Hardware

- **ESP32 Development Board**: Otak dari sistem kontrol.
- **LCD I2C 16x2**: Menampilkan menu dan status sistem.
- **Rotary Encoder**: Navigasi menu dan pengaturan nilai parameter.
- **Pushbutton**: Konfirmasi pilihan dan navigasi tambahan.
- **Modul DSP Audio**: Modul eksternal untuk pemrosesan sinyal audio (tergantung implementasi).

---

## 🌐 Fitur Sistem

- **Crossover Control**: Pengaturan frekuensi cutoff untuk 4-way crossover (Sub, Low, Mid, High).
- **Compressor Settings**: Pengaturan threshold, ratio, attack, dan release untuk dinamika audio.
- **Limiter**: Pengaturan threshold limiter untuk membatasi output audio.
- **Delay**: Pengaturan delay untuk tiap band guna koreksi fase.
- **Preset Management**: Simpan, muat, dan terapkan preset audio sesuai kebutuhan.
- **Physical Control**: Navigasi menu menggunakan rotary encoder dan tombol fisik.
- **Web Control Panel**: Antarmuka web yang responsif untuk pengaturan DSP secara remote.

---

## 🚀 Cara Instalasi & Penggunaan

### 1. Persiapan

- Pastikan Anda telah menginstal [Arduino IDE](https://www.arduino.cc/en/software).
- Tambahkan board ESP32 ke Arduino IDE melalui Board Manager dengan URL:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
- Instal library yang diperlukan melalui Library Manager:
- `Wire` untuk komunikasi I2C.
- `ESPAsyncWebServer` dan `AsyncTCP` untuk antarmuka web.
- Library tambahan sesuai dengan modul DSP yang digunakan.

### 2. Unggah Kode

- Buka file `AudioDSP.ino` di Arduino IDE.
- Sesuaikan konfigurasi pin dan parameter sesuai dengan hardware Anda.
- Unggah kode ke board ESP32 Anda.

### 3. Unggah File Web UI

- Gunakan [ESP32 Sketch Data Upload Tool](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) untuk mengunggah folder `data/` ke SPIFFS.

### 4. Akses Antarmuka Web

- Hubungkan perangkat ke jaringan Wi-Fi ESP32 atau jaringan lokal.
- Buka browser dan akses alamat IP ESP32 yang ditampilkan di Serial Monitor.

---

## 📁 Struktur Direktori

ESP32-Audio-DLMS-System/
├── AudioDSP.ino # Kode utama ESP32
├── data/ # File HTML, CSS, dan JavaScript untuk antarmuka web
├── src/ # Kode sumber tambahan
├── README.md # Dokumentasi proyek
└── LICENSE # Lisensi proyek

---

## 💡 Keunggulan Sistem

- **Real-Time Control**: Pengaturan parameter DSP secara langsung melalui antarmuka fisik dan web.
- **Dual Interface**: Kombinasi kontrol fisik dan web memberikan fleksibilitas dalam pengoperasian.
- **Modular Design**: Mudah disesuaikan dengan berbagai modul DSP dan kebutuhan pengguna.
- **Open Source**: Kode sumber tersedia untuk dikembangkan dan disesuaikan lebih lanjut.

---
## 🤝 Kontribusi

Kontribusi sangat dihargai! Silakan fork repositori ini dan ajukan pull request untuk perbaikan atau penambahan fitur.

---

## 📬 Kontak

📧 asepsupriatna90@gmail.com  
🔗 [GitHub @asepsupriatna90](https://github.com/asepsupriatna90)

---

## 📄 Lisensi

Proyek ini dilisensikan di bawah [MIT License](LICENSE).
