<div align="center">

  <br/>
  <img src="https://upload.wikimedia.org/wikipedia/commons/8/87/Arduino_Logo.svg" alt="Logo" width="200"/><br/>
  
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
  <p>Pada tahap awal, sistem dibuat sebagai prototype menggunakan Wokwi sebagai simulator ESP32. Fokusnya adalah membuat MVP yang bisa membaca nilai analog dari sensor MQ-2, menyalakan LED hijau saat kondisi normal, lalu mengaktifkan LED merah dan buzzer ketika nilai sensor mendeteksi gas yang melebihi batas atau treshold. Program pada tahap ini masih berjalan dengan satu loop() tunggal tanpa FreeRTOS dan belum terhubung ke MQTT, dan hanya cukup untuk memverifikasi logika deteksi dasar sebelum sistem dikembangkan lebih lanjut.</p>
    
  ### Code Snippet
  https://github.com/SidqiRaafi/Kelompok-3-Mikrokontroler/blob/58384752943e762da85316145fe5a0d2bd0d5b04/docs/code/prototype1.ino#L16-L72
  </div>
<br/>

  ## Prototype RTOS
  <div align="justify">
  <p>Setelah logika dasar berjalan, sistem diubah menggunakan FreeRTOS agar pembacaan sensor tidak terganggu oleh proses lain. Program dipisah menjadi dua task mandiri yaitu GasTask dengan prioritas tertinggi bertugas membaca sensor, menentukan status kebocoran, dan mengendalikan LED dan buzzer untuk deteksi kebocoran, sementara StatusTask berprioritas rendah mengambil data terbaru dari queue dan menampilkannya ke Serial Monitor. Dengan pemisahan ini, alarm tetap responsif meskipun ada beban kerja lain dan pembacaan sensor tidak lagi harus menunggu proses logging selesai menghasilkan sistem yang lebih responsif.</p>
    
  ### Code Snippet
  https://github.com/SidqiRaafi/Kelompok-3-Mikrokontroler/blob/11510bfc183ed84b13ad66a08daaf8a23b0afe15/docs/code/prototype_FreeRTOS.ino#L16-L109
  </div>
<br/>

  ## Prototype + MQTT
  <div align="justify">
  <p>Tahap ini menambahkan MQTTTask di atas program FreeRTOS yang sudah dibuat sebelumnya. Task baru ini mengelola koneksi WiFi di wokwi, koneksi ke broker di broker.emqx.io, dan pengiriman data secara berkala. Data dari sensor tetap dikemas dalam struct GasData dan dikirim lewat gasQueue, tugas MQTTTask cukup mengambil nilai terbaru dari antrian, menyusunnya menjadi payload JSON, lalu mempublikasikannya ke topik gasleak/data di broker menggunakan library tambahan di wokwi yaitu PubSubClient.

disini Prioritas task tetap dipertahankan: GasTask (prioritas 3) fokus pada sensor dan alarm, MQTTTask (prioritas 2) mengurus koneksi jaringan, dan StatusTask (prioritas 1) menangani logging lokal. Hasilnya, sistem berkembang dari alarm lokal sederhana menjadi node sensor yang bisa dimonitor dari dashboard berbasis web yang akan di implementasikan.</p>

  <div align="center">
    
  <img src="docs/img/Broker Config.webp" alt="brokerconf" width="1024"/><br/>**Konfigurasi broker agar terhubung ke simulasi di wokwi**<br/><br/>
  
  <img src="docs/img/Subscription Config.webp" alt="subsconf" width="512"/><br/>**Konfigurasi subscriptions di broker**<br/><br/>
  
  <img src="docs/img/Sending Data to Broker.webp" alt="sendata" width="512"/><br/>**Simulasi di wokwi mengirimkan data ke broker**<br/><br/>
  
  <img src="docs/img/Data Received in Broker.webp" alt="receiveddata" width="512"/><br/>**Data yang diterima di broker**<br/>
  
  </div>
  
### Code Snippet
https://github.com/SidqiRaafi/Kelompok-3-Mikrokontroler/blob/11510bfc183ed84b13ad66a08daaf8a23b0afe15/docs/code/prototype_FreeRTOS%2BMQTT.ino#L16-L177
</div>
<br/>

  ## Implementasi menggunakan ESP32
  
  ### Hardware yang digunakan
- Wemos D1 R32 ESP32
- Sensor MQ2
- Active Buzzer
- LED Hijau
- LED Merah
- Breadboard
- 2 × Resistor 220 Ω (pembatas arus LED)
- Breadboard
- Kabel jumper

  ### Dokumentasi Rangkaian
  <p align="center">
  <img width="450" alt="WhatsApp Image 2026-06-25 at 13 52 29" src="https://github.com/user-attachments/assets/fdfb0c93-e11a-49ee-9263-04355d1556bc" />
<img width="450" alt="WhatsApp Image 2026-06-25 at 13 52 29 (1)" src="https://github.com/user-attachments/assets/275c4496-9431-46e7-b177-66db961daf5b" />

  <br/><br/><br/>
<br/>
  ## Implementasi Web Interface
  <br/><br/><br/>
<br/>
  ## Final Product
  <br/><br/><br/>
<br/>
  ## Kesimpulan
  <br/><br/><br/>
<br/>
  ## Kontributor
- [@Sidqi Raafi Al Fauzan](https://github.com/SidqiRaafi) - 23552011395
- [@Muhammad Rizal Afrizal](https://github.com/Afrizal8) - 23552011376 
- [@Ardika Nurdiansyah](https://github.com/ardikaanurdiansyah) - 23552011311
</div>
