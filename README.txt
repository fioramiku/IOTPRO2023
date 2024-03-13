=========================
ชื่อผู้พัฒนาโครงงาน 
-นายทศพร ลีฬพงษ์ 
-นายวรุตม์ มาศสุวรรณ
-นายธนพรรธน์ หวังเพ็ชรงาม
ชื่อวิชา Hardware Develoopment
ภาควิชา คอมพิวเตอร์
คณะวิศวกรรมศาสตร์ มหาวิทยาลัยเกษตรศาสตร์
========================
อธิบายไฟล์/ไดเรคตอรี
│   license.txt #explain about license for this project
│   README.txt
│
├───schematic # folder for store schematic 
│       Schematic_for_bucket.pdf #Schematic which use in esp32 that connect to pc for notification and send/receive data between nodered
│       Schematic_for_notification_system.pdf #Schematic which use in esp32 that place in bucket 
│
└───source_code #contain all source code file
    ├───for_esp32  #code that exucute in esp32
    │       code_for_bucket.py #source code that prepare for esp32 that place in bucket
    │       code_for_notification.ino #source code that prepare for esp32 connect or place near working place
    │       config.h #config file
    │
    └───for_nodered #file for node-red
            flows.json  #contain a flow that work in node red
=======================
อุปกรณ์ฮาร์ดแวร์ที่ใช้
  * ESP32-S3 x 2
  * sensor Ultrasonic Module x 1
  * buzzer SFM-27 x1
  * servo moter sg90s x1
  * small bucket (3.5L) x1
  * IR sensor module x1
  * Power supply 12V x1
  * model convert voltage to 5V breadboard x1
library using
=== for micropython
  * machine - Allows control over hardware components on microcontroller boards like the ESP32
  * Pin - Controls GPIO pins for digital input/output.
  * ADC - Enables analog to digital conversion for reading analog sensors.
  * I2C - Facilitates communication over the I2C bus for interfacing with various peripherals.
  * PWM - Manages pulse-width modulation for controlling analog components.
  * Timer - Handles timing events and interrupts.
  * UART - Controls serial communication for transmitting and receiving data.
  * uasyncio - Provides asynchronous programming support for MicroPython.
  * time, utime - Deal with time-related operations and delays.
  * network - Provides network-related functionalities.
  * neopixel - Controls Neopixel (WS2812) RGB LED modules.
=== for arduino 
  * PubSubClient.h - Enables MQTT communication for publishing and subscribing.
  * WiFi.h - Provides access to ESP32's WiFi functionalities.
  * time.h - Assists in handling time-related operations, possibly synchronization with NTP servers.

