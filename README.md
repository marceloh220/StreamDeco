# StreamDeco
 ESP32 8048s43c board touch screen keypad

 <img src="http://url/image.png" height="60" width="60" >

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
       LVGL 8.4.0 https://lvgl.io
       esp32_smartdisplay 2.0.9 https://github.com/rzeldent/esp32-smartdisplay
       platformio-espressif32-sunton https://github.com/rzeldent/platformio-espressif32-sunton
       GFX Library for Arduino 1.2.8 https://github.com/moononournation/Arduino_GFX

    Touch screen:
       TAMC_GT911 1.0.2 https://github.com/tamctec/gt911-arduino

    Bluetooth and keyboard:
       ESP32 BLE Keyboard 0.3.2 https://github.com/T-vK/ESP32-BLE-Keyboard
       NimBLE-Arduino 1.4.1 https://github.com/h2zero/NimBLE-Arduino


 
