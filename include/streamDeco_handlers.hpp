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

#ifndef _STREAMDECO_HANDLERS_HPP_
#define _STREAMDECO_HANDLERS_HPP_

#include "streamDeco_objects.hpp"

namespace streamDeco {

  /** 
   * @brief   Handle the buttons task, made buttons configurations and manager buttons events
   * @details The events can generate a keyboard code to send to computer throug BLE Bluetooth
   *          or change StreamDeco configurations
   */
  void handleButtons(taskArg_t arg);

  /**
   * @brief   Handle the ui reset task
   * @details Each 10 seconds a timer will pass a event to this task 
   *          to hidden additional screens and return to Main screen
   */
  void handleUiReset(taskArg_t arg);

  /** 
   * @brief   Handle the monitor task
   * @details Update computer metrics on Monitor screen
   */
  void handleMonitor(taskArg_t arg);

  /**
   * @brief   Handle the clock task
   * @details Update clock on Monitor screen
   */
  void handleClock(taskArg_t arg);

} // nasmespasce streamDeco

#endif
