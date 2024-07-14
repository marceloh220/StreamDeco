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

#include "lvgl/lvgl_port.hpp"

#include <Arduino_GFX_Library.h>
#include <TAMC_GT911.h>
#include <Wire.h>

#include "lvgl/lvgl_port_map.h"

#include "marcelino/mutex.hpp"
#include "marcelino/task.hpp"

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"

#include "driver/gpio.h"
#include "esp_log.h"

namespace lvgl {

namespace port {

Arduino_ESP32RGBPanel bus(GPIO_LCD_CS, GPIO_LCD_SCK, GPIO_LCD_SDA, GPIO_LCD_DE,
                          GPIO_LCD_VSYNC, GPIO_LCD_HSYNC, GPIO_LCD_PCLK,
                          GPIO_LCD_R0, GPIO_LCD_R1, GPIO_LCD_R2, GPIO_LCD_R3,
                          GPIO_LCD_R4, GPIO_LCD_G0, GPIO_LCD_G1, GPIO_LCD_G2,
                          GPIO_LCD_G3, GPIO_LCD_G4, GPIO_LCD_G5, GPIO_LCD_B0,
                          GPIO_LCD_B1, GPIO_LCD_B2, GPIO_LCD_B3, GPIO_LCD_B4);

Arduino_RPi_DPI_RGBPanel gfx(&bus, LCD_WIDTH, LCD_HSYNC_POLARITY,
                             LCD_HSYNC_FRONT_PORCH, LCD_HSYNC_PULSE_WIDTH,
                             LCD_HSYNC_BACK_PORCH, LCD_HEIGHT,
                             LCD_VSYNC_POLARITY, LCD_VSYNC_FRONT_PORCH,
                             LCD_VSYNC_PULSE_WIDTH, LCD_VSYNC_BACK_PORCH,
                             LCD_PCLK_ACTIVE_NEG, LCD_PREFER_SPEED,
                             LCD_AUTO_FLUSH);

TAMC_GT911 touchscreen(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT,
                       TOUCH_GT911_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2),
                       max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *display_draw_buf1;
static lv_color_t *display_draw_buf2;

static lv_disp_drv_t display_driver;
static lv_disp_t *display;

static lv_indev_drv_t indev_driver;

MutexRecursive mutex;

Task task("Port task LVGL", 5, LVGL_STACK_SIZE);

/* Display flushing */
void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  gfx.draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);

  lv_disp_flush_ready(disp);
}

void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {

  touchscreen.read();

  if (touchscreen.isTouched) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = map(touchscreen.points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0,
                        LCD_WIDTH - 1);
    data->point.y = map(touchscreen.points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0,
                        LCD_HEIGHT - 1);
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void handle(void *arg) {

  while (1) {
    mutex.take();
    uint32_t time_till_next_run = lv_timer_handler();
    mutex.give();
    delay(time_till_next_run);
  }
}

void backlight_init(void) {

  ledc_timer_config_t ledc_timer = {.speed_mode =
                                        (ledc_mode_t)BACKLIGHT_SPEED_MODE,
                                    .duty_resolution = LEDC_TIMER_12_BIT,
                                    .timer_num = (ledc_timer_t)BACKLIGHT_TIMER,
                                    .freq_hz = BACKLIGHT_FREQUENCY,
                                    .clk_cfg = LEDC_AUTO_CLK};

  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  // Prepare and then apply the LEDC PWM channel configuration
  ledc_channel_config_t ledc_channel = {
      .gpio_num = GPIO_LCD_BL,
      .speed_mode = (ledc_mode_t)BACKLIGHT_SPEED_MODE,
      .channel = (ledc_channel_t)BACKLIGHT_CHANNEL,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = (ledc_timer_t)BACKLIGHT_TIMER,
      .duty = 2457,
      .hpoint = 0};

  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void backlight_set(float bright) {

  if (bright > 1.0f)
    bright = 1.0f;
  if (bright < 0.0f)
    bright = 0.0f;
  uint32_t pwm_value = 4095 * bright;
  ledc_set_duty((ledc_mode_t)BACKLIGHT_SPEED_MODE,
                (ledc_channel_t)BACKLIGHT_CHANNEL, pwm_value);
  ledc_update_duty((ledc_mode_t)BACKLIGHT_SPEED_MODE,
                   (ledc_channel_t)BACKLIGHT_CHANNEL);
}

void backlight_setRaw(int bright) {

  if (bright > 4095)
    bright = 4095;
  if (bright < 0)
    bright = 0;
  ledc_set_duty((ledc_mode_t)BACKLIGHT_SPEED_MODE,
                (ledc_channel_t)BACKLIGHT_CHANNEL, bright);
  ledc_update_duty((ledc_mode_t)BACKLIGHT_SPEED_MODE,
                   (ledc_channel_t)BACKLIGHT_CHANNEL);
}

void mutex_take() { mutex.take(); }

void mutex_give() { mutex.give(); }

void init() {

  gfx.begin();
  Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
  touchscreen.begin();
  touchscreen.setRotation(TOUCH_GT911_ROTATION);

  backlight_init();

  lv_init();

  uint32_t screenWidth = gfx.width();
  uint32_t screenHeight = gfx.height();

  if (LVGL_DOUBLE_BUFFER > 2) {

    display_draw_buf1 = (lv_color_t *)heap_caps_malloc(
        sizeof(lv_color_t) * screenWidth * screenHeight / LVGL_REDRAW_PARTIAL,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    display_draw_buf2 = (lv_color_t *)heap_caps_malloc(
        sizeof(lv_color_t) * screenWidth * screenHeight / LVGL_REDRAW_PARTIAL,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!display_draw_buf1 || !display_draw_buf2) {
      Serial.println("LVGL disp_draw_buf allocate failed!");
      return;
    }

    lv_disp_draw_buf_init(&draw_buf, display_draw_buf1, display_draw_buf2,
                          screenWidth * screenHeight / LVGL_REDRAW_PARTIAL);

  } else {

    display_draw_buf1 = (lv_color_t *)heap_caps_malloc(
        sizeof(lv_color_t) * screenWidth * screenHeight / LVGL_REDRAW_PARTIAL,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!display_draw_buf1) {
      Serial.println("LVGL disp_draw_buf allocate failed!");
      return;
    }

    lv_disp_draw_buf_init(&draw_buf, display_draw_buf1, NULL,
                          screenWidth * screenHeight / LVGL_REDRAW_PARTIAL);
  }

  lv_disp_drv_init(&display_driver);
  display_driver.hor_res = screenWidth;
  display_driver.ver_res = screenHeight;
  display_driver.flush_cb = disp_flush;
  display_driver.draw_buf = &draw_buf;
  display_driver.sw_rotate = true;
  display = lv_disp_drv_register(&display_driver);

  lv_indev_drv_init(&indev_driver);
  indev_driver.type = LV_INDEV_TYPE_POINTER;
  indev_driver.read_cb = touchpad_read;
  lv_indev_drv_register(&indev_driver);

  task.attach(handle);
}

void set_screen_rotation(lv_disp_rot_t rotation) {
  mutex_take();
  lv_disp_set_rotation(display, rotation);
  mutex_give();
}

lv_disp_rot_t get_screen_rotation() {
  mutex_take();
  lv_disp_rot_t ret = lv_disp_get_rotation(display);
  mutex_give();
  return ret;
}

void print_task_memory_usage() {
  printf("Task Port mem usage %d\n", task.memUsage());
}

} // namespace port

} // namespace lvgl
