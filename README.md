# StreamDeco
 ESP32 8048s43c board touch screen keypad connected at BLE Bluetooth to send keys shortcuts to computer.

 <img src="https://github.com/marceloh220/StreamDeco/blob/main/assets/main_screen.jpg" height="480" width="800" >

 Board Specs:

    ESP32-S3 dual-core Xtensa 32-bit LX7 microprocessor, up to 240 MHz with 384KB ROM, 512KB SRAM. 2.4GHz WiFi and Bluetooth 5
    FLASH: 16MB
    PSRAM: 8MB
    Micro-SD card slot (SPI)
    4.3-inch 800x480 IPS TFT display - ST7262 (Parallel RGB-565 interface)
    Capacitive touch panel - GT911 (i2C - 0x5D or 0x14)
    1 USB-C to Serial 0 (CH340C)
    I2C / SPI / GPIOs

 Use Vscode and Platformio with Arduino Framework

  Librares

   LCD and Graphic:
       
   [LVGL v8.4.0](https://lvgl.io)
      
   [esp32_smartdisplay v2.0.9](https://github.com/rzeldent/esp32-smartdisplay)
       
   [platformio-espressif32-sunton](https://github.com/rzeldent/platformio-espressif32-sunton)
       
   [GFX Library for Arduino v1.2.8](https://github.com/moononournation/Arduino_GFX)

   Touch screen:
   
   [TAMC_GT911 v1.0.2](https://github.com/tamctec/gt911-arduino)

   Bluetooth and keyboard:
   
   [ESP32 BLE Keyboard v0.3.2](https://github.com/T-vK/ESP32-BLE-Keyboard)
   
   [NimBLE-Arduino v1.4.1](https://github.com/h2zero/NimBLE-Arduino)

# How to USE

 Short click on Application button open Application layer, 
 after 10 seconds of inactivity the Application Layer is hidden 
 and user interface return to Main Layer
 
 Long click on Application button fixe the Application layer on screen,
 the fixed layer will not been hidden until a short click on Application Button.
 
 <img src="https://github.com/marceloh220/StreamDeco/blob/main/assets/app_layer_fixed.jpg" height="480" width="800" >



 Short click on Multimedia button open Multimedia layer, 
 after 10 seconds of inactivity the Multimedia layer is hidden 
 and user interface return to Main layer.
 
 Long click on Multimedia button fixe the Multimedia layer on screen,
 the fixed layer will not been hidden until a short click on Multimedia Button.

 <img src="https://github.com/marceloh220/StreamDeco/blob/main/assets/mult_layer_fixed.jpg" height="480" width="800" >



 Short click on Configuration button open Configuration layer, 
 after 10 seconds of inactivity the Configuration layer is hidden 
 and user interface return to Main layer
 
 Configuration Layer on screen doesn't have fixed function.

 <img src="https://github.com/marceloh220/StreamDeco/blob/main/assets/conf_layer.jpg" height="480" width="800" >



 Slider on Configuration Layer change backlight bright, 
 the bright of backlight will be set to minimun after 30 seconds of inactivity.

# Some buttons may work without configurations, but the most part need be configured on operational system.
 
Work on every system?
   NO, apparently MacOS and iOS have issues with bluettoth connection

Can I change icons?
   YES

Can I change command shortcuts?
   YES

