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

#include "marcelino/rtos_timer.hpp"

namespace rtos {

  void Timer::attach(TimerCallbackFunction_t callback) {
    _id = &_handler;
    _handler = xTimerCreate(_name, CHRONO_TO_TICK(_periode), _autoreload, _id, callback);
    start();
  }

  void Timer::startFromISR() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerStartFromISR(_handler, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  void Timer::stopFromISR() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerStopFromISR(_handler, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  void Timer::resetFromISR() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerResetFromISR(_handler, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  void Timer::periode(milliseconds periode) {
    _periode = periode;
    xTimerChangePeriod(_handler, CHRONO_TO_TICK(periode), portMAX_DELAY);
  }

  void Timer::periodeFromISR(milliseconds periode) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    _periode = periode;
    xTimerChangePeriodFromISR(_handler, CHRONO_TO_TICK(periode), &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  bool Timer::verifyID(TimerHandle_t timer) {
    if (pvTimerGetTimerID(timer) == _id)
      return true;
    return false;
  }

} // namespace rtos
