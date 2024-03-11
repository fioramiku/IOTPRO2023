=========================
ชื่อผู้พัฒนาโครงงาน 
-นายทศพร ลีฬพงษ์ 
-นายวรุตม์ มาศสุวรรณ
-นายธนพรรธน์ หวังเพ็ชรงาม
ชื่อวิชา Hardware Develoopment
ภาควิชา คอมพิวเตอร์
คณะวิศวกรรมศาสตร์ มหาวิทยาลัยเกษตรศาสตร์
========================
อธิบายไฟล์หรือไดเรคตอรี
│   license.txt
│   README.txt
│
├───schematic # folder for store schematic 
│       Schematic_for_bucket.pdf #Schematic which use in esp32 that connect to pc for notification and send/receive data between nodered
│       Schematic_for_notification_system.pdf #Schematic which use in esp32 that place in bucket 
│
└───source_code
    ├───for_esp32  #code that exucute in esp32
    │       code_for_bucket.py #source code that prepare for esp32 that place in bucket
    │       code_for_notification.ino #source code that prepare for esp32 connect or place near working place
    │       config.h #config file
    │
    └───for_nodered
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

