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

#ifndef _STREAMDECO_INTERRUPTS_HPP_
#define _STREAMDECO_INTERRUPTS_HPP_

#include "streamDeco_objects.hpp"

namespace streamDeco
{

  /**
   * @brief    Callback function to handle backlight timer
   * @details  Set backlight bright to 10% affter timer overflow
   * @param    timerHandle The rtos timer send the timer handler to callback function
   **/
  void timer_callback(TimerHandle_t timerHandle);

  /**
   * @brief   Callback registred in buttons
   * @details Send a notifications with event code to streamDecoTasks buttons handler
   * @param   event  Event received by the callback
   * @note    This callback is registred on buttons and streamDecoBrightSlider objects
   * @note    Each streamDecoButtons and streamDecoBrightSlider send a different event
   **/
  void buttons_callback(lvgl::event::event_t event);

} // nasmespasce streamDeco

#endif
