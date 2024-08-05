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

#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/timers.h"
#include "rtos_chrono.hpp"

namespace rtos {

class Timer {

public:
  Timer(const char *name, TimerCallbackFunction_t callback, milliseconds periode, UBaseType_t autoreload = true)
      : _name(name), _periode(periode), _autoreload(autoreload) {
    attach(callback);
  }

  Timer(const char *name, milliseconds periode, UBaseType_t autoreload = true)
      : _name(name), _periode(periode), _autoreload(autoreload) {}

  void attach(TimerCallbackFunction_t callback) {
    _id = &_handler;
    _handler = xTimerCreate(_name, CHRONO_TO_TICK(_periode), _autoreload, _id, callback);
    start();
  }

  void start() { xTimerStart(_handler, portMAX_DELAY); }

  void startFromISR() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerStartFromISR(_handler, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  void stop() { xTimerStop(_handler, portMAX_DELAY); }

  void stopFromISR() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerStopFromISR(_handler, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  void reset() { xTimerReset(_handler, portMAX_DELAY); }

  void resetFromISR() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerResetFromISR(_handler, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  void periode(milliseconds periode) {
    _periode = periode;
    xTimerChangePeriod(_handler, CHRONO_TO_TICK(periode), portMAX_DELAY);
  }

  void periodeFromISR(milliseconds periode) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    _periode = periode;
    xTimerChangePeriodFromISR(_handler, CHRONO_TO_TICK(periode), &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  milliseconds periode() { return _periode; }

  bool verifyID(TimerHandle_t timer) {
    if (pvTimerGetTimerID(timer) == _id)
      return true;
    return false;
  }

private:
  TimerHandle_t _handler;
  const char *_name;
  milliseconds _periode;
  UBaseType_t _autoreload;
  void* _id;
};

} // namespace rtos

#endif
