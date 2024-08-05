/**
 * Copyright © 2024 Marcelo H Moraes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  streamDeco_HandlerButtons.cpp
 * @brief Button task handler
 */

#include "streamDeco_objects.hpp"

namespace streamDeco
{

  /**
   * @brief   Handle the buttons task to manager buttons events
   * @details The events can generate a keyboard code to send to computer throug BLE Bluetooth
   *          or change StreamDeco configurations
   */
  void handleButtons(arg_t arg)
  {

    /**
     * @var      settings_t settings
     * @brief    Wallpaper color, buttons theme and screen bright
     * @details  lv_color_t color_background
     *           lv_palette_t color_buttons
     *           int lcd_bright
     * @note     This arg reside on heap and is passed in attach method during StreamDeco::init
     **/
    settings_t *settings = (settings_t *)arg;

    while (1)
    {

      /* wait for a notification
       * this notification is sent by LVGL button with a event code */
      uint32_t button_event = task.buttons.takeNotify();

      /* function in streamDeco_shortcuts.cpp */
      process_event(button_event, settings);

      /**
       * if some event is received the UI is not inactive
       * backlight bright change to setpoint value
       * and timers are manting reseted */
      lvgl::port::backlight_setRaw(settings->lcd_bright);
      timer_ui.backlight.reset();
      timer_ui.uiReset.reset();

      rtos::delay(100ms);

    } // end handleButtons's infinit loop

  } // end handleButtons

} // namespace streamDeco