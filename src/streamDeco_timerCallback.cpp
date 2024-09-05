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
 * @file  streamDeco_init.cpp
 * @brief Init task functions handlers of StreamDeco
 */

#include "streamDeco_objects.hpp"
#include <time.h>

namespace streamDeco
{

  /**
   * @brief    Callback function to handle timers
   * @details  Set backlight bright to 10% affter timer backlight overflow
   * @details  Hidden canvas affter timer uiReset overflow
   * @param    timerHandle The rtos timer send the timer handler to callback function
   * @note     The timerHandle can be used to verify what timer generate the interrupt
   * @note     by the timer ID
   **/
  void timer_callback(TimerHandle_t timerHandle)
  {
    // verify what timer generated the event by ID
    if (timer_ui.backlight.verifyID(timerHandle))
    {
      // only change backlight bright if are no pinned canvas
      bool test = button.applications_canvas.pinned();
      test |= button.multimedia_canvas.pinned();
      test |= button.configurations_canvas.pinned();
      if (!test)
      {
        lvgl::port::backlight_set(.1);
      }
    }
    else if (timer_ui.uiReset.verifyID(timerHandle))
    {
      task.uiReset.sendNotify(hidden_canvas_event);
    }
  }

  /**
   * @brief   Callback registred in buttons
   * @details Send a notifications with event code to task buttons handler
   * @param   lvglEvent  Event received by the callback
   * @note    This callback is registred on buttons and slider objects
   * @note    Each button and slider send a different event
   **/
  void buttons_callback(lvgl::event::event_t lvglEvent)
  {
    // userdata passed are int type
    int event = lvgl::event::get_user_data<int>(lvglEvent);
    task.buttons.sendNotify(event);
  }

} // namespace streamDeco