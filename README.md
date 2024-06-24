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
       LVGL 8.4.0 https://lvgl.io
       esp32_smartdisplay 2.0.9 https://github.com/rzeldent/esp32-smartdisplay
       platformio-espressif32-sunton https://github.com/rzeldent/platformio-espressif32-sunton
       GFX Library for Arduino 1.2.8 https://github.com/moononournation/Arduino_GFX

    Touch screen:
       TAMC_GT911 1.0.2 https://github.com/tamctec/gt911-arduino

    Bluetooth and keyboard:
       ESP32 BLE Keyboard 0.3.2 https://github.com/T-vK/ESP32-BLE-Keyboard
       NimBLE-Arduino 1.4.1 https://github.com/h2zero/NimBLE-Arduino

# How to USE

 Short click on Application button open Application Layer, 
 after 10 seconds of inactivity the Application Layer is hidden 
 and user interface return to Main Layer
 
 Long click on Application button fixe the Application Layer on screen,
 the fixed layer will not been hidden until a short click on Application Button.
 
 <img src="https://github.com/marceloh220/StreamDeco/blob/main/assets/app_layer_fixed.jpg" height="480" width="800" >



 Short click on Multimedia button open Multimedia Layer, 
 after 10 seconds of inactivity the Multimedia Layer is hidden 
 and user interface return to Main Layer.
 
 Long click on Multimedia button fixe the Multimedia Layer on screen,
 the fixed layer will not been hidden until a short click on Multimedia Button.

 <img src="https://github.com/marceloh220/StreamDeco/blob/main/assets/mult_layer_fixed.jpg" height="480" width="800" >



 Short click on Configuration button open Configuration Layer, 
 after 10 seconds of inactivity the Configuration Layer is hidden 
 and user interface return to Main Layer
 
 Configuration Layer on screen doesn't have fixed function.

 <img src="https://github.com/marceloh220/StreamDeco/blob/main/assets/conf_layer.jpg" height="480" width="800" >



 Slider on Configuration Layer change backlight bright, 
 the bright of backlight will be set to minimun after 30 seconds of inactivity.
 
 
Work on every system?

   NO

Can I change icons?

   YES

Can I change command shortcuts?

   YES

Some buttons may work without configurations, but the most part need be configured on operational system.

