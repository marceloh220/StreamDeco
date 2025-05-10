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

#include "lvgl_port.hpp"
#include "const_user.hpp"

#include "rtos_mutex_static.hpp"
#include "rtos_task_static.hpp"

#include "driver/ledc.h"

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_interface.h"

#include <esp_lcd_touch.h>
#include <esp_lcd_types.h>
#include "esp_lcd_touch_gt911.h"
#include "driver/i2c.h"

#include "esp_log.h"

namespace lvgl
{

  namespace port
  {

    const char *log_tag = "LVGL PORT";

    /**
     * @brief    Display flush
     * @details  Send LVGL ready framebuffer to display
     * */
    static void display_flush(lv_disp_drv_t *driver, const lv_area_t *area, lv_color_t *framebuffer)
    {
      const esp_lcd_panel_handle_t display_handle = static_cast<const esp_lcd_panel_handle_t>(driver->user_data);
      ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(display_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, framebuffer));
      lv_disp_flush_ready(driver);
    }

    /**
     * @brief    Touchpad read
     * @details  Read touchpad and send coordinates to LVGL if touch is pressed
     * */
    static void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
    {
      auto touchpad_panel = (esp_lcd_touch_handle_t)indev_driver->user_data;
      assert(touchpad_panel);

      uint16_t touchpad_x;
      uint16_t touchpad_y;
      uint8_t touchpad_cnt = 0;

      esp_lcd_touch_read_data(touchpad_panel);

      bool touchpad_pressed = esp_lcd_touch_get_coordinates(touchpad_panel, &touchpad_x, &touchpad_y, nullptr, &touchpad_cnt, 1);
      if (touchpad_pressed && touchpad_cnt > 0)
      {
        data->point.x = math::map<uint16_t>(touchpad_x, 0, GT911_TOUCH_CONFIG_X_MAX, 0, DISPLAY_WIDTH);
        data->point.y = math::map<uint16_t>(touchpad_y, 0, GT911_TOUCH_CONFIG_Y_MAX, 0, DISPLAY_HEIGHT);
        data->state = LV_INDEV_STATE_PRESSED;
      }
      else
      {
        data->state = LV_INDEV_STATE_RELEASED;
      }
    }

    static rtos::MutexRecursiveStatic mutex;
    static rtos::TaskStatic<4_kB> task("Port task LVGL", 5);

    /**
     * @brief    Handle LVGL timer
     * @details  Task to handle LVGL timer
     */
    static void task_handle(void *arg)
    {
      while (1)
      {
        mutex.take();
        uint32_t time_till_next_run = lv_timer_handler();
        mutex.give();
        rtos::sleep(time_till_next_run);
      }
    }

    /**
     * @brief    Pointer to backlight PWM channel configurations
     * @details  Pass to PWM set functions speed mode and channel of PWM pin
     */
    ledc_channel_config_t *backlight_channel;

    /**
     * @brief    Get backlight PWM resolution
     * @details  PWM resolution can be configured herer
     * @return   Maximun PWM value
     */
    constexpr ledc_timer_bit_t backlight_resolution()
    {
      return LEDC_TIMER_8_BIT;
    }

    /**
     * @brief    Get maximun PWM value based on PWM resolution
     * @return   Maximun PWM value
     */
    uint32_t backlight_max()
    {
      return ((uint32_t)1 << backlight_resolution()) - 1;
    }

    /**
     * @brief    Set backlight bright
     * @details  Set backlight bright for 0 to 1
     * @param    bright   0 to zero bright 1 to maximum bright
     *                    0.5 mean halfh bright
     */
    void backlight_set(float bright)
    {
      if (bright > 1.0f)
        bright = 1.0f;
      if (bright < 0.0f)
        bright = 0.0f;
      uint32_t pwm_value = backlight_max() * bright;
      ledc_set_duty(backlight_channel->speed_mode, backlight_channel->channel, pwm_value);
      ledc_update_duty(backlight_channel->speed_mode, backlight_channel->channel);
    }

    /**
     * @sa       backlight_max
     * @brief    Set backlight bright
     * @details  Set backlight bright in raw number from 0 to backlight_max()
     * @param    bright   0 to zero bright and maximun to maximun bright
     *                    half bright can be achived with backlight_max()/2
     */
    void backlight_setRaw(int bright)
    {
      uint32_t max = backlight_max();
      if (bright > max)
        bright = max;
      if (bright < 0)
        bright = 0;
      ledc_set_duty(backlight_channel->speed_mode, backlight_channel->channel, bright);
      ledc_update_duty(backlight_channel->speed_mode, backlight_channel->channel);
    }

    /**
     * @brief    take the LVGL mutex
     * @details  this mutex prevents display alterations while
     *           internal timer are processed, changes cant no be done while LVGL handle
     * @note     Affter take, the mutex must be released as soon as possible
     */
    void mutex_take() { mutex.take(); }

    /**
     * @brief    release LVGL mutex
     * @details  Must be called affter the mutex are taked to release
     *           LVGL interface to do other process
     */
    void mutex_give() { mutex.give(); }

    /**
     * @brief    Init display panel, touchpad panel and LVGL port
     * @details  Must be called before any LVGL object be createdm
     *           e.g. in system initiation
     */
    void init()
    {

      static lv_disp_draw_buf_t draw_buffer;
      static lv_disp_drv_t display_driver;
      static lv_indev_drv_t indev_driver;

      esp_lcd_panel_handle_t display_handle;
      esp_lcd_panel_io_handle_t touchpad_bus_handle;
      esp_lcd_touch_handle_t touchpad_handle;

      /**
       * Panel RGB driver configure
       */
      const esp_lcd_rgb_panel_config_t rgb_panel_config = {
          .clk_src = ST7262_PANEL_CONFIG_CLK_SRC,
          .timings = {
              .pclk_hz = static_cast<int>(ST7262_PANEL_CONFIG_TIMINGS_PCLK_HZ),
              .h_res = ST7262_PANEL_CONFIG_TIMINGS_H_RES,
              .v_res = ST7262_PANEL_CONFIG_TIMINGS_V_RES,
              .hsync_pulse_width = ST7262_PANEL_CONFIG_TIMINGS_HSYNC_PULSE_WIDTH,
              .hsync_back_porch = ST7262_PANEL_CONFIG_TIMINGS_HSYNC_BACK_PORCH,
              .hsync_front_porch = ST7262_PANEL_CONFIG_TIMINGS_HSYNC_FRONT_PORCH,
              .vsync_pulse_width = ST7262_PANEL_CONFIG_TIMINGS_VSYNC_PULSE_WIDTH,
              .vsync_back_porch = ST7262_PANEL_CONFIG_TIMINGS_VSYNC_BACK_PORCH,
              .vsync_front_porch = ST7262_PANEL_CONFIG_TIMINGS_VSYNC_FRONT_PORCH,
              .flags = {
                  .hsync_idle_low = ST7262_PANEL_CONFIG_TIMINGS_FLAGS_HSYNC_IDLE_LOW == 0 ? 1 : 0,
                  .vsync_idle_low = ST7262_PANEL_CONFIG_TIMINGS_FLAGS_VSYNC_IDLE_LOW == 0 ? 1 : 0,
                  .de_idle_high = ST7262_PANEL_CONFIG_TIMINGS_FLAGS_DE_IDLE_HIGH,
                  .pclk_active_neg = ST7262_PANEL_CONFIG_TIMINGS_FLAGS_PCLK_ACTIVE_NEG,
                  .pclk_idle_high = ST7262_PANEL_CONFIG_TIMINGS_FLAGS_PCLK_IDLE_HIGH},
          },
          .data_width = ST7262_PANEL_CONFIG_DATA_WIDTH,
          .sram_trans_align = ST7262_PANEL_CONFIG_SRAM_TRANS_ALIGN,
          .psram_trans_align = ST7262_PANEL_CONFIG_PSRAM_TRANS_ALIGN,
          .hsync_gpio_num = ST7262_PANEL_CONFIG_HSYNC_GPIO_NUM,
          .vsync_gpio_num = ST7262_PANEL_CONFIG_VSYNC_GPIO_NUM,
          .de_gpio_num = ST7262_PANEL_CONFIG_DE_GPIO_NUM,
          .pclk_gpio_num = ST7262_PANEL_CONFIG_PCLK_GPIO_NUM,
          .data_gpio_nums = {
              ST7262_PANEL_CONFIG_DATA_GPIO_R0,
              ST7262_PANEL_CONFIG_DATA_GPIO_R1,
              ST7262_PANEL_CONFIG_DATA_GPIO_R2,
              ST7262_PANEL_CONFIG_DATA_GPIO_R3,
              ST7262_PANEL_CONFIG_DATA_GPIO_R4,
              ST7262_PANEL_CONFIG_DATA_GPIO_G0,
              ST7262_PANEL_CONFIG_DATA_GPIO_G1,
              ST7262_PANEL_CONFIG_DATA_GPIO_G2,
              ST7262_PANEL_CONFIG_DATA_GPIO_G3,
              ST7262_PANEL_CONFIG_DATA_GPIO_G4,
              ST7262_PANEL_CONFIG_DATA_GPIO_G5,
              ST7262_PANEL_CONFIG_DATA_GPIO_B0,
              ST7262_PANEL_CONFIG_DATA_GPIO_B1,
              ST7262_PANEL_CONFIG_DATA_GPIO_B2,
              ST7262_PANEL_CONFIG_DATA_GPIO_B3,
              ST7262_PANEL_CONFIG_DATA_GPIO_B4,
          },
          .disp_gpio_num = ST7262_PANEL_CONFIG_DISP_GPIO_NUM,
          .on_frame_trans_done = nullptr,
          .user_ctx = &display_driver,
          .flags = {.disp_active_low = ST7262_PANEL_CONFIG_FLAGS_DISP_ACTIVE_LOW, .relax_on_idle = ST7262_PANEL_CONFIG_FLAGS_RELAX_ON_IDLE, .fb_in_psram = ST7262_PANEL_CONFIG_FLAGS_FB_IN_PSRAM}};

      ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&rgb_panel_config, &display_handle));
      ESP_ERROR_CHECK(esp_lcd_panel_reset(display_handle));
      ESP_ERROR_CHECK(esp_lcd_panel_init(display_handle));

      /**
       * Panel touch driver configure
       */
      const i2c_config_t i2c_config = {
          .mode = I2C_MODE_MASTER,
          .sda_io_num = GT911_I2C_CONFIG_SDA_IO_NUM,
          .scl_io_num = GT911_I2C_CONFIG_SCL_IO_NUM,
          .sda_pullup_en = GT911_I2C_CONFIG_SDA_PULLUP_EN,
          .scl_pullup_en = GT911_I2C_CONFIG_SCL_PULLUP_EN,
          .master = {
              .clk_speed = GT911_I2C_CONFIG_MASTER_CLK_SPEED},
          .clk_flags = GT911_I2C_CONFIG_CLK_FLAGS};

      const esp_lcd_panel_io_i2c_config_t io_i2c_config = {
          .dev_addr = ESP_LCD_TOUCH_IO_I2C_GT911_ADDRESS,
          .on_color_trans_done = nullptr,
          .user_ctx = nullptr,
          .control_phase_bytes = GT911_IO_I2C_CONFIG_CONTROL_PHASE_BYTES,
          .dc_bit_offset = GT911_IO_I2C_CONFIG_DC_BIT_OFFSET,
          .lcd_cmd_bits = GT911_IO_I2C_CONFIG_LCD_CMD_BITS,
          .lcd_param_bits = GT911_IO_I2C_CONFIG_LCD_PARAM_BITS,
          .flags = {
              .dc_low_on_data = GT911_IO_I2C_CONFIG_FLAGS_DC_LOW_ON_DATA,
              .disable_control_phase = GT911_IO_I2C_CONFIG_FLAGS_DISABLE_CONTROL_PHASE,
          }};

      const esp_lcd_touch_config_t touch_config = {
          .x_max = GT911_TOUCH_CONFIG_X_MAX,
          .y_max = GT911_TOUCH_CONFIG_Y_MAX,
          .rst_gpio_num = static_cast<gpio_num_t>(GT911_TOUCH_CONFIG_RST_GPIO_NUM),
          .int_gpio_num = static_cast<gpio_num_t>(GT911_TOUCH_CONFIG_INT_GPIO_NUM),
          .levels = {
              .reset = GT911_TOUCH_CONFIG_LEVELS_RESET,
              .interrupt = GT911_TOUCH_CONFIG_LEVELS_INTERRUPT},
          .flags = {.swap_xy = TOUCH_SWAP_XY, .mirror_x = TOUCH_MIRROR_X, .mirror_y = TOUCH_MIRROR_Y},
          .process_coordinates = nullptr,
          .interrupt_callback = nullptr,
          .user_data = nullptr,
          .driver_data = nullptr};

      ESP_ERROR_CHECK(i2c_param_config(GT911_I2C_HOST, &i2c_config));
      ESP_ERROR_CHECK(i2c_driver_install(GT911_I2C_HOST, i2c_config.mode, 0, 0, 0));
      ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(GT911_I2C_HOST, &io_i2c_config, &touchpad_bus_handle));
      ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_gt911(touchpad_bus_handle, &touch_config, &touchpad_handle));

      /**
       * Backlight diver configure
       */
      const ledc_timer_config_t ledc_timer = {
          .speed_mode = LEDC_LOW_SPEED_MODE,
          .duty_resolution = backlight_resolution(),
          .timer_num = LEDC_TIMER_0,
          .freq_hz = 22_kHz,
          .clk_cfg = LEDC_AUTO_CLK};
      ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

      static ledc_channel_config_t ledc_channel = {
          .gpio_num = GPIO_BCKL,
          .speed_mode = ledc_timer.speed_mode,
          .channel = LEDC_CHANNEL_0,
          .intr_type = LEDC_INTR_DISABLE,
          .timer_sel = ledc_timer.timer_num,
          .duty = backlight_max() / 2,
          .hpoint = 0};
      ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
      backlight_channel = &ledc_channel;

      /**
       * LVGL framebuffer configure
       */
      lv_init();

      lv_color_t *display_draw_buffer1 = memory::alloc<lv_color_t>(LVGL_BUFFER_PIXELS, LVGL_BUFFER_MALLOC_FLAGS);
      lv_color_t *display_draw_buffer2 = memory::alloc<lv_color_t>(LVGL_BUFFER_PIXELS, LVGL_BUFFER_MALLOC_FLAGS);

      if (display_draw_buffer1 == nullptr || display_draw_buffer2 == nullptr)
      {
        return;
      }
      lv_disp_draw_buf_init(&draw_buffer, display_draw_buffer1, display_draw_buffer2, LVGL_BUFFER_PIXELS);

      /**
       * LVGL display driver link
       */
      lv_disp_drv_init(&display_driver);
      display_driver.user_data = display_handle;
      display_driver.hor_res = DISPLAY_WIDTH;
      display_driver.ver_res = DISPLAY_HEIGHT;
      display_driver.flush_cb = display_flush;
      display_driver.draw_buf = &draw_buffer;
      display_driver.sw_rotate = true;
      display_driver.drv_update_cb = nullptr;
      lv_disp_drv_register(&display_driver);

      /**
       * LVGL touch driver link
       */
      lv_indev_drv_init(&indev_driver);
      indev_driver.disp = lv_disp_get_default();
      indev_driver.type = LV_INDEV_TYPE_POINTER;
      indev_driver.user_data = touchpad_handle;
      indev_driver.read_cb = touchpad_read;
      lv_indev_drv_register(&indev_driver);

      /**
       * LVGL update task init
       */
      task.attach(task_handle);
    }

    /**
     * @brief   Set screen rotations
     * @param   rotation An lv_disp_rot_t type
     *    LV_DISP_ROT_0   for 0 degree rotation - landscape screen
     *    LV_DISP_ROT_90  for 90 degrees rotation - portrait screen
     *    LV_DISP_ROT_180 for 180 degrees rotation - mirror landscape screen
     *    LV_DISP_ROT_270 for 270 degrees rotation - mirror portrait screen
     *
     */
    void set_screen_rotation(lv_disp_rot_t rotation)
    {
      mutex_take();
      lv_disp_set_rotation(lv_disp_get_default(), rotation);
      mutex_give();
    }

    /**
     * @brief    Check display orientation
     * @return   Rotation  display orientation
     *            LV_DISP_ROT_NONE - Landscape orientation
     *            LV_DISP_ROT_90   - Portrait orientation
     *            LV_DISP_ROT_180  - Mirror Landscape orientation
     *            LV_DISP_ROT_270  - Mirror Portrait orientation
     */
    lv_disp_rot_t get_screen_rotation()
    {
      mutex_take();
      lv_disp_rot_t ret = lv_disp_get_rotation(lv_disp_get_default());
      mutex_give();
      return ret;
    }

    /**
     * @brief    Print LVGL port's task memory used
     */
    void print_task_memory_usage()
    {
      ESP_LOGI(log_tag, "Task memory used %d kB\n", task.memUsage());
    }

  } // namespace port

} // namespace lvgl
