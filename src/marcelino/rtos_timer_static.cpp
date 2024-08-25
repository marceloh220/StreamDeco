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

#include "marcelino/rtos_timer_static.hpp"

namespace rtos {

  TimerStatic::TimerStatic(const char *name, milliseconds changePeriode)
  : _name(name), _periode(changePeriode)
  {

  }

  void TimerStatic::attach(TimerCallbackFunction_t callback, UBaseType_t autoreload) {
    if(_handler != NULL)
      return;
    _handler = xTimerCreate(_name, CHRONO_TO_TICK(_periode), autoreload, &_handler, callback);
  }

  void TimerStatic::start()
  {
    if(_handler == NULL)
      return;
    xTimerStart(_handler, portMAX_DELAY);
  }

  void TimerStatic::startFromISR() {
    if(_handler == NULL)
      return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerStartFromISR(_handler, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  void TimerStatic::stop()
  {
    if(_handler == NULL)
      return;
    xTimerStop(_handler, portMAX_DELAY);
  }

  void TimerStatic::stopFromISR() {
    if(_handler == NULL)
      return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerStopFromISR(_handler, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  void TimerStatic::reset()
  {
    if(_handler == NULL)
      return;
    xTimerReset(_handler, portMAX_DELAY);
  }

  void TimerStatic::resetFromISR() {
    if(_handler == NULL)
      return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerResetFromISR(_handler, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  void TimerStatic::changePeriode(milliseconds periode) {
    if(_handler == NULL)
      return;
    _periode = periode;
    xTimerChangePeriod(_handler, CHRONO_TO_TICK(periode), portMAX_DELAY);
  }

  void TimerStatic::changePeriodeFromISR(milliseconds periode) {
    if(_handler == NULL)
      return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    _periode = periode;
    xTimerChangePeriodFromISR(_handler, CHRONO_TO_TICK(periode), &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  inline milliseconds TimerStatic::getPeriode() {
    return _periode;
  }

  inline int64_t TimerStatic::getPeriodeIntMS() {
    return _periode.count();
  }


  bool TimerStatic::verifyID(TimerHandle_t timer) {
    if(_handler == NULL)
      return false;
    return pvTimerGetTimerID(timer) == &_handler ? true : false;
  }

  inline const char *TimerStatic::name() {
    return _name;
  }

} // namespace rtos
