/**
 * Copyright © 2024 Marcelo H Moraes
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the “Software”), to deal in 
 * the Software without restriction, including without limitation the rights to 
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _LVGL_PORT_MAP_H_
#define _LVGL_PORT_MAP_H_

#include <TAMC_GT911.h>

#include "driver/ledc.h"
#include "hal/gpio_types.h"

namespace lvgl {

namespace port {

typedef enum map_e {
  GPIO_LCD_DE = GPIO_NUM_40,
  GPIO_LCD_VSYNC = GPIO_NUM_41,
  GPIO_LCD_HSYNC = GPIO_NUM_39,
  GPIO_LCD_PCLK = GPIO_NUM_42,
  GPIO_LCD_R0 = GPIO_NUM_45,
  GPIO_LCD_R1 = GPIO_NUM_48,
  GPIO_LCD_R2 = GPIO_NUM_47,
  GPIO_LCD_R3 = GPIO_NUM_21,
  GPIO_LCD_R4 = GPIO_NUM_14,
  GPIO_LCD_G0 = GPIO_NUM_5,
  GPIO_LCD_G1 = GPIO_NUM_6,
  GPIO_LCD_G2 = GPIO_NUM_7,
  GPIO_LCD_G3 = GPIO_NUM_15,
  GPIO_LCD_G4 = GPIO_NUM_16,
  GPIO_LCD_G5 = GPIO_NUM_4,
  GPIO_LCD_B0 = GPIO_NUM_8,
  GPIO_LCD_B1 = GPIO_NUM_3,
  GPIO_LCD_B2 = GPIO_NUM_46,
  GPIO_LCD_B3 = GPIO_NUM_9,
  GPIO_LCD_B4 = GPIO_NUM_1,
  GPIO_LCD_DISP = GPIO_NUM_NC,
  GPIO_LCD_BL = GPIO_NUM_2,
  GPIO_LCD_CS = GPIO_NUM_NC,
  GPIO_LCD_SDA = GPIO_NUM_NC,
  GPIO_LCD_SCK = GPIO_NUM_NC,

  GPIO_TOUCH_SDA = GPIO_NUM_19,
  GPIO_TOUCH_SCL = GPIO_NUM_20,
  GPIO_TOUCH_RST = GPIO_NUM_38,
  GPIO_TOUCH_INT = GPIO_NUM_NC,

  LCD_HSYNC_POLARITY = 0,
  LCD_HSYNC_FRONT_PORCH = 8,
  LCD_HSYNC_BACK_PORCH = 8,
  LCD_HSYNC_PULSE_WIDTH = 4,

  LCD_VSYNC_POLARITY = 0,
  LCD_VSYNC_FRONT_PORCH = 8,
  LCD_VSYNC_BACK_PORCH = 8,
  LCD_VSYNC_PULSE_WIDTH = 4,

  LCD_PCLK_ACTIVE_NEG = 1,
  LCD_PREFER_SPEED = 14000000,
  LCD_AUTO_FLUSH = true,

  LCD_WIDTH = 800,
  LCD_HEIGHT = 480,
  LCD_NUM_FRAMEBUFFER = 2,

  BACKLIGHT_SPEED_MODE = LEDC_LOW_SPEED_MODE,
  BACKLIGHT_TIMER = LEDC_TIMER_0,
  BACKLIGHT_CHANNEL = LEDC_CHANNEL_0,
  BACKLIGHT_FREQUENCY = 1000,

  TOUCH_H_RES_MIN = 0,
  TOUCH_H_RES_MAX = 477,
  TOUCH_V_RES_MIN = 0,
  TOUCH_V_RES_MAX = 269,

  TOUCH_GT911_SCL = 20,
  TOUCH_GT911_SDA = 19,
  TOUCH_GT911_INT = GPIO_NUM_NC,
  TOUCH_GT911_RST = 38,
  TOUCH_GT911_ROTATION = ROTATION_NORMAL,
  TOUCH_MAP_X1 = 480,
  TOUCH_MAP_X2 = 0,
  TOUCH_MAP_Y1 = 272,
  TOUCH_MAP_Y2 = 0,

  LVGL_DOUBLE_BUFFER = 1,
  LVGL_REDRAW_PARTIAL = 1,
  LVGL_STACK_SIZE = 4 * 1024,

} map_t;

} // namespace port

} // namespace lvgl

#endif
