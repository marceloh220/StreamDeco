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
 * @file  streamDeco_CallbackButtons.cpp
 * @brief Callback for buttons event of LVGL
 */

#include "streamDeco_objects.hpp"

namespace streamDeco
{

  namespace streamDecoButtons
  {

    /**
     * @brief   Callback registred in buttons
     * @details Send a notifications with event code to task buttons handler
     * @param   lvglEvent  Event received by the callback
     * @note    This callback is registred on buttons and streamDecoBrightSlider objects
     * @note    Each streamDeco button and streamDeco bright slider send a different event
     **/
    void buttons_callback(lvgl::event::event_t lvglEvent)
    {
      // userdata passed are the event generated by touch at int type
      int event = lvgl::event::get_user_data<int>(lvglEvent);
      streamDecoTasks::buttons.sendNotify(event);
    }

  } // namespace streamDecoButtons

} // namespace streamDeco