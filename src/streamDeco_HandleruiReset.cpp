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
 * @file    streamDeco_HandleruiReset.cpp
 * @brief   Hidden canvas
 * @details This handler hidden canvas if buttons are not pinned
 * @details affter receive a notify sended by a timer
 */

#include "streamDeco_objects.hpp"

namespace streamDeco
{

  /* Handler of UI reset task, 
   * hidden canvas if they are not pinned. */
  void handleUiReset(arg_t arg)
  {
    while (1)
    {
      uint32_t e = task.uiReset.takeNotify();
      if (e == hidden_canvas_event)
      {
        if (!button.applications_canvas.pinned())
        {
          canvas.applications.hidden();
        }
        if (!button.multimedia_canvas.pinned())
        {
          canvas.multimedia.hidden();
        }
        /* Aways hidden Configurations canvas
         * and never hidden Monitor canvas */
        canvas.configurations.hidden();
      }
    }
  }

} // namespace streamDeco