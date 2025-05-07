// Mengelola semua elemen input
const inputElements = {
  // Crossover
  fcSub: document.getElementById('fcSub'),
  fcLow: document.getElementById('fcLow'),
  fcMid: document.getElementById('fcMid'),
  fcHigh: document.getElementById('fcHigh'),
  
  // Compressor
  threshold: document.getElementById('threshold'),
  ratio: document.getElementById('ratio'),
  attack: document.getElementById('attack'),
  release: document.getElementById('release'),
  
  // Limiter
  limiterThresh: document.getElementById('limiterThresh'),
  
  // Delay
  delayLow: document.getElementById('delayLow'),
  delayLowMid: document.getElementById('delayLowMid'),
  delayHighMid: document.getElementById('delayHighMid'),
  delayHigh: document.getElementById('delayHigh'),
  
  // Preset
  presetSelect: document.getElementById('presetSelect')
};

// Menyimpan nilai terakhir yang diketahui
let lastKnownValues = {};

// Inisialisasi aplikasi
document.addEventListener('DOMContentLoaded', function() {
  // Tambahkan event listener untuk semua input
  Object.keys(inputElements).forEach(key => {
    const element = inputElements[key];
    if (element) {
      // Simpan nilai awal
      lastKnownValues[key] = element.value;
      
      // Tambahkan event listener
      if (element.type === 'range') {
        // Untuk slider, kirim perubahan saat dilepas
        element.addEventListener('change', () => updateParameter(key, element.value));
      } else if (element.type === 'number') {
        // Untuk input number, kirim saat keluar dari fokus atau tekan enter
        element.addEventListener('blur', () => updateParameter(key, element.value));
        element.addEventListener('keyup', (e) => {
          if (e.key === 'Enter') {
            updateParameter(key, element.value);
          }
        });
      }
    }
  });
  
  // Muat pengaturan awal dari ESP32
  loadCurrentSettings();
});

// Fungsi untuk memuat pengaturan saat ini dari ESP32
function loadCurrentSettings() {
  showNotification('Memuat pengaturan...');
  
  fetch('/getSettings')
    .then(response => {
      if (!response.ok) {
        throw new Error('Gagal mendapatkan pengaturan');
      }
      return response.json();
    })
    .then(data => {
      // Perbarui semua nilai input
      updateAllInputs(data);
      showNotification('Pengaturan berhasil dimuat', 'success');
    })
    .catch(error => {
      console.error('Error loading settings:', error);
      showNotification('Gagal memuat pengaturan', 'error');
    });
}

// Fungsi untuk memperbarui parameter tunggal
function updateParameter(paramName, value) {
  // Jika nilai tidak berubah, jangan kirim permintaan
  if (lastKnownValues[paramName] === value) {
    return;
  }
  
  // Perbarui nilai yang diketahui terakhir
  lastKnownValues[paramName] = value;
  
  // Kirim ke ESP32
  fetch(`/updateParam?name=${paramName}&value=${value}`)
    .then(response => {
      if (!response.ok) {
        throw new Error('Gagal memperbarui parameter');
      }
      return response.json();
    })
    .then(data => {
      console.log(`Parameter ${paramName} diperbarui:`, data);
      showNotification(`${getParameterLabel(paramName)}: ${value}`, 'success');
    })
    .catch(error => {
      console.error(`Error updating parameter ${paramName}:`, error);
      showNotification(`Gagal memperbarui ${getParameterLabel(paramName)}`, 'error');
    });
}

// Mendapatkan label yang lebih baik untuk notifikasi
function getParameterLabel(paramName) {
  const labels = {
    fcSub: 'Crossover Sub',
    fcLow: 'Crossover Low',
    fcMid: 'Crossover Mid',
    fcHigh: 'Crossover High',
    threshold: 'Threshold Compressor',
    ratio: 'Ratio Compressor',
    attack: 'Attack Compressor',
    release: 'Release Compressor',
    limiterThresh: 'Threshold Limiter',
    delayLow: 'Delay Low',
    delayLowMid: 'Delay Low-Mid',
    delayHighMid: 'Delay High-Mid',
    delayHigh: 'Delay High'
  };
  
  return labels[paramName] || paramName;
}

// Fungsi untuk memperbarui semua input dari data
function updateAllInputs(data) {
  Object.keys(data).forEach(key => {
    const element = inputElements[key];
    if (element) {
      element.value = data[key];
      lastKnownValues[key] = data[key];
    }
  });
}

// Fungsi untuk menampilkan notifikasi
function showNotification(message, type = 'info') {
  // Periksa apakah sudah ada notifikasi
  let notification = document.getElementById('notification');
  
  if (!notification) {
    // Buat elemen notifikasi jika belum ada
    notification = document.createElement('div');
    notification.id = 'notification';
    document.body.appendChild(notification);
  }
  
  // Atur kelas berdasarkan jenis notifikasi
  notification.className = `notification ${type}`;
  notification.textContent = message;
  
  // Tampilkan notifikasi
  notification.style.display = 'block';
  
  // Hilangkan notifikasi setelah 3 detik
  setTimeout(() => {
    notification.style.display = 'none';
  }, 3000);
}

// Apply preset function
function applyPreset() {
  let presetIndex = document.getElementById('presetSelect').value;
  showNotification(`Menerapkan preset ${getPresetName(presetIndex)}...`);
  
  fetch(`/applyPreset?presetIndex=${presetIndex}`)
    .then(response => {
      if (!response.ok) {
        throw new Error('Gagal menerapkan preset');
      }
      return response.json();
    })
    .then(data => {
      console.log("Preset diterapkan:", data);
      // Perbarui nilai-nilai input
      updateAllInputs(data);
      showNotification(`Preset ${getPresetName(presetIndex)} diterapkan`, 'success');
    })
    .catch(error => {
      console.error('Error applying preset:', error);
      showNotification(`Gagal menerapkan preset ${getPresetName(presetIndex)}`, 'error');
    });
}

// Save preset function
function savePreset() {
  let presetIndex = document.getElementById('presetSelect').value;
  showNotification(`Menyimpan preset ${getPresetName(presetIndex)}...`);
  
  fetch(`/savePreset?presetIndex=${presetIndex}`)
    .then(response => {
      if (!response.ok) {
        throw new Error('Gagal menyimpan preset');
      }
      return response.json();
    })
    .then(data => {
      console.log("Preset disimpan:", data);
      showNotification(`Preset ${getPresetName(presetIndex)} disimpan`, 'success');
    })
    .catch(error => {
      console.error('Error saving preset:', error);
      showNotification(`Gagal menyimpan preset ${getPresetName(presetIndex)}`, 'error');
    });
}

// Load preset function
function loadPreset() {
  let presetIndex = document.getElementById('presetSelect').value;
  showNotification(`Memuat preset ${getPresetName(presetIndex)}...`);
  
  fetch(`/loadPreset?presetIndex=${presetIndex}`)
    .then(response => {
      if (!response.ok) {
        throw new Error('Gagal memuat preset');
      }
      return response.json();
    })
    .then(data => {
      console.log("Preset dimuat:", data);
      // Perbarui nilai-nilai input
      updateAllInputs(data);
      showNotification(`Preset ${getPresetName(presetIndex)} dimuat`, 'success');
    })
    .catch(error => {
      console.error('Error loading preset:', error);
      showNotification(`Gagal memuat preset ${getPresetName(presetIndex)}`, 'error');
    });
}

// Mendapatkan nama preset berdasarkan indeks
function getPresetName(index) {
  const presetNames = {
    '0': 'Default',
    '1': 'Rock',
    '2': 'Jazz',
    '3': 'Dangdut',
    '4': 'Pop'
  };
  
  return presetNames[index] || `Preset ${index}`;
}