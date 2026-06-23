<div align="center">

  # Sistem Deteksi Kebocoran Gas Secara Realtime Menggunakan ESP32, FreeRTOS, dan MQTT
  
  [![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white)](https://www.espressif.com/en/products/socs/esp32)
  [![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
  [![FreeRTOS](https://img.shields.io/badge/RTOS-006600?style=for-the-badge&logo=freertos&logoColor=white)](https://www.freertos.org/)
  [![MQTT](https://img.shields.io/badge/MQTT-660066?style=for-the-badge&logo=mqtt&logoColor=white)](https://mqtt.org/)
  [![HTML5](https://img.shields.io/badge/Dashboard-FF5722?style=for-the-badge&logo=html5&logoColor=white)](#)
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)

<div align="justify">
<p>Kebocoran gas pada ruangan tertutup dapat menimbulkan risiko kebakaran dan ledakan jika tidak terdeteksi sejak dini. Proyek ini membangun sistem deteksi kebocoran gas secara realtime yang memantau konsentrasi gas menggunakan sensor MQ-2 dan memberi peringatan melalui LED, buzzer, serta notifikasi data yang dikirim ke server melalui protokol MQTT. Dengan memanfaatkan FreeRTOS pada board ESP32, pembacaan sensor dan pengiriman data diatur dalam beberapa *task* sehingga proses pemantauan tetap responsif meskipun ada beban tugas lain seperti komunikasi jaringan dan dashboard web.</p>
</div>

## Daftar Komponen

| Komponen                    | Kuantitas | Penjelasan                                                                 |
| :-------------------------- | :-------: | :------------------------------------------------------------------------- |
| WEMOS D1 R32 (ESP32)        |   1 pcs   | Board mikrokontroler utama berbasis ESP32 yang mengolah data dan koneksi.  |
| Sensor Gas MQ-2 (modul)     |   1 pcs   | Sensor gas yang mendeteksi LPG, butane, metana, dan asap untuk kebocoran.  |
| Buzzer Piezo Aktif          |   1 pcs   | Buzzer aktif sebagai alarm suara saat terdeteksi kebocoran gas.            |
| LED Merah                   |   1 pcs   | Indikator visual kondisi bahaya (gas terdeteksi).                          |
| LED Hijau                   |   1 pcs   | Indikator visual kondisi normal (sistem standby / aman).                   |
| Resistor 220 Ω              |   2 pcs   | Pembatas arus untuk LED agar tidak rusak saat tersambung ke ESP32.         |
| Breadboard                  |   1 pcs   | Media perakitan rangkaian tanpa perlu menyolder komponen.                  |
| Kabel Jumper Male-to-Male   |   1 pak   | Kabel penghubung antara pin ESP32, sensor, LED, buzzer, dan breadboard.    |


<br/>

  ## Prototype Awal
  <img src="docs/img/proto.webp" alt="Prototype" width="670"/><br/>**Prototype Pertama Sistem Deteksi Kebocoran Gas Menggunakan Wokwi**
  <div align="justify">
  <p>Pada tahap awal, rancangan sistem dibuat dalam bentuk prototype menggunakan Wokwi sebagai simulator rangkaian berbasis ESP32. Prototype ini berfokus pada pembuatan minimum viable product (MVP) untuk membaca nilai analog dari sensor gas MQ-2, menyalakan LED hijau saat kondisi normal, serta mengaktifkan LED merah dan buzzer ketika nilai sensor melewati ambang batas. Pada versi ini, program masih berjalan dengan loop tunggal tanpa FreeRTOS dan belum terhubung ke broker MQTT, sehingga lebih mudah untuk menguji logika dasar deteksi kebocoran sebelum ditingkatkan menjadi arsitektur multitask dengan pengiriman data ke server.</p>
    
  ### Code Snippet
  https://github.com/SidqiRaafi/Kelompok-3-Mikrokontroler/blob/58384752943e762da85316145fe5a0d2bd0d5b04/docs/code/prototype1.ino#L16-L72
  </div>
<br/>

  ## Prototype RTOS
  <br/><br/><br/>
<br/>
  ## Prototype + MQTT
  <br/><br/><br/>
<br/>
  ## Prototype Final
  <br/><br/><br/>
<br/>
  ## Kontributor
- [@Sidqi Raafi Al Fauzan](https://github.com/SidqiRaafi) - 23552011395
- [@Muhammad Rizal Afrizal](https://github.com/Afrizal8) - 23552011376 
- [@Ardika Nurdiansyah](https://github.com/ardikaanurdiansyah) - 23552011311
</div>
