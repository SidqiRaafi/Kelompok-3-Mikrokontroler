<div align="center">

  # Sistem Deteksi Kebocoran Gas Secara Realtime Menggunakan ESP32, FreeRTOS, dan MQTT
  
  [![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white)](https://www.espressif.com/en/products/socs/esp32)
  [![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
  [![FreeRTOS](https://img.shields.io/badge/RTOS-006600?style=for-the-badge&logo=freertos&logoColor=white)](https://www.freertos.org/)
  [![MQTT](https://img.shields.io/badge/MQTT-660066?style=for-the-badge&logo=mqtt&logoColor=white)](https://mqtt.org/)
  [![HTML5](https://img.shields.io/badge/Dashboard-FF5722?style=for-the-badge&logo=html5&logoColor=white)](#)
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)

  <img src="docs/img/proto.webp" alt="Prototype" width="670"/><br/>**Prototype Sistem Deteksi Kebocoran Gas di Wokwi**
  
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

#
</div>
