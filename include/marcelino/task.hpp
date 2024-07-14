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

#ifndef _TASK_HPP_
#define _TASK_HPP_

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

#include "chrono.hpp"

typedef void *arg_t;

class Task {

public:
  typedef enum pinCore {
    CORE_0,
    CORE_1,
    NO_AFINITY,
  } pinCore_t;

  Task(TaskFunction_t callback, const char *name, UBaseType_t priority = 0,
       uint32_t stackSize = 6 * 1024, arg_t args = NULL,
       pinCore_t core = NO_AFINITY)
      : _callback(callback), _name(name), _priority(priority), _core(core),
        _stackSize(stackSize), _args(args) {
    attach(_callback, args);
  }

  Task(const char *name, UBaseType_t priority = 0,
       uint32_t stackSize = 2 * 1024, pinCore_t core = NO_AFINITY)
      : _name(name), _priority(priority), _core(core), _stackSize(stackSize) {}

  ~Task() { taskDelete(); }

  inline void attach(TaskFunction_t callback, arg_t args = NULL) {
    if (_created)
      return;
    _callback = callback;
    _args = args;
    esp_err_t error;
    if (_core == NO_AFINITY) {
      error =
          xTaskCreate(_callback, _name, _stackSize, _args, _priority, &_handle);
    } else
      error = xTaskCreatePinnedToCore(_callback, _name, _stackSize, _args,
                                      _priority, &_handle, _core);
    if (error == pdPASS) {
      ESP_LOGI(_name, "Task created");
      _created = true;
    } else {
      ESP_LOGE(_name, "Task faill in creation");
    }
  }

  inline void sendNotify() {
    if (!_created)
      return;
    xTaskNotifyGive(_handle);
  }

  inline void sendNotify(uint32_t sendBit) {
    if (!_created)
      return;
    xTaskNotify(_handle, sendBit, eSetBits);
  }

  inline void sendNotifyFromISR() {
    if (!_created)
      return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  inline void sendNotifyFromISR(uint32_t sendBit) {
    if (!_created)
      return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(_handle, sendBit, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  inline uint32_t takeNotify() { return ulTaskNotifyTake(true, portMAX_DELAY); }

  inline uint32_t takeNotify(TickType_t time) {
    return ulTaskNotifyTake(true, pdMS_TO_TICKS(time));
  }

  inline void delayUntil(milliseconds time) {
    vTaskDelayUntil(&previousTime, CHRONO_TO_TICK(time));
  }

  inline BaseType_t abortDelay() {
    if (!_created)
      return 0;
    return xTaskAbortDelay(_handle);
  }

  template <typename type> inline void call(type parameter = NULL) {
    xTaskCallApplicationTaskHook(_handle, (void*)parameter);
  }

  inline void suspend() {
    if (!_created)
      return;
    vTaskSuspend(_handle);
  }

  inline void resume() {
    if (!_created)
      return;
    vTaskResume(_handle);
  }

  inline BaseType_t resumeFromISR() {
    if (!_created)
      return 0;
    return xTaskResumeFromISR(_handle);
  }

  inline void taskDelete() {
    if (!_created)
      return;
    vTaskDelete(_handle);
    _created = false;
  }

  inline const char *name() { return _name; }

  inline UBaseType_t priority() { return _priority; }
  inline UBaseType_t priorityFromISR() { return _priority; }

  inline void priority(UBaseType_t priority) {
    if (!_created)
      return;
    _priority = priority;
    vTaskPrioritySet(_handle, priority);
  }

  inline uint32_t stackSize() { return _stackSize; }

  template <typename type> inline type args() {
    return static_cast<type>(_args);
  }

  template <typename type> inline void args(type args) {
    _args = static_cast<arg_t>(args);
  }

  inline BaseType_t core() { return _core; }

  inline uint32_t memUsage() {
    if (!_created)
      return 0;
    return _stackSize - uxTaskGetStackHighWaterMark(_handle);
  }

  inline uint32_t memFree() {
    if (!_created)
      return 0;
    return uxTaskGetStackHighWaterMark(_handle);
  }

private:
  TaskFunction_t _callback;
  const char *_name;
  UBaseType_t _priority;
  UBaseType_t _core;
  uint32_t _stackSize;
  arg_t _args;
  TaskHandle_t _handle;
  bool _created = false;
  TickType_t previousTime = 0;
};

#endif
