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
 * @file  streamDeco_CallbackTimer.cpp
 * @brief Callback for timers of StreamDeco
 */

#include "streamDeco_objects.hpp"
#include <time.h>

namespace streamDeco
{

  /**
   * @brief    Callback function to handle timers
   * @details  Set backlight bright to 10% affter timer backlight overflow and
   *           hidden streamDeco canvas affter timer uiReset overflow
   * @param    timerHandle The rtos timer send the timer handler to callback function 
   *           as argument, this parameter is used to verify what timer generate the
   *           interrupt by the timer ID
   * @note     It is therefore essential that timer callback functions never attempt to block, 
   *           to avoid trobles this implementation sends a notification to a task that handles 
   *           the backlight reset and canvas hide
   **/
  void timer_callback(TimerHandle_t timerHandle)
  {
    // verify what timer generated the event by ID
    if (timer_ui::backlight.verifyID(timerHandle))
    {
      streamDecoTasks::uiReset.sendNotify(rest_backlight_event);
    }
    if (timer_ui::uiReset.verifyID(timerHandle))
    {
      streamDecoTasks::uiReset.sendNotify(hidden_canvas_event);
    }
  }

} // namespace streamDeco