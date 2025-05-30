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
 * @file    streamDeco_HandlerUIReset.cpp
 * @brief   Hidden canvas
 * @details This handler hidden canvas or put backlight brigrht to minimum
 * @details if buttons are not pinned affter receive a notify sended by a timer
 */

#include "streamDeco_objects.hpp"

namespace streamDeco
{

  /* Handler of UI reset task,
   * hidden canvas if they are not pinned or
   * put backlight on rest mode reducing the bright to minimum. */
  void handleUIReset(taskArg_t task_arg)
  {
    while (1)
    {
      uint32_t e = streamDecoTasks::uiReset.takeNotify();
      switch (e)
      {
      case hidden_canvas_event:
        if (!streamDecoButtons::applications_canvas.pinned())
        {
          streamDecoCanvas::applications.hidden();
        }
        if (!streamDecoButtons::multimedia_canvas.pinned())
        {
          streamDecoCanvas::multimedia.hidden();
        }
        /* Aways hidden Configurations canvas
         * and never hidden Monitor canvas */
        streamDecoCanvas::configurations.hidden();
        break;
      case rest_backlight_event:
        // only change backlight bright if are no pinned canvas
        if (streamDecoButtons::applications_canvas.pinned())
          break;
        if (streamDecoButtons::multimedia_canvas.pinned())
          break;
        if (streamDecoButtons::configurations_canvas.pinned())
          break;
        lvgl::port::backlight_set(.1);
        break;
      }
    }
  }

} // namespace streamDeco