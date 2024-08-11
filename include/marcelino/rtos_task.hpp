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

#include "rtos_chrono.hpp"

typedef void *taskArg_t;

namespace rtos {

class Task {

public:
  typedef enum pinCore {
    CORE_0,
    CORE_1,
    NO_AFINITY,
  } pinCore_t;

  Task(TaskFunction_t callback, const char *name, UBaseType_t priority = 0, uint32_t stackSize = 6 * 1024, taskArg_t args = NULL, pinCore_t core = NO_AFINITY)
      : _callback(callback), _name(name), _priority(priority), _core(core),
        _stackSize(stackSize), _args(args) { attach(_callback, args); }

  Task(const char *name, UBaseType_t priority = 0, uint32_t stackSize = 2 * 1024, pinCore_t core = NO_AFINITY)
      : _name(name), _priority(priority), _core(core), _stackSize(stackSize) {}

  ~Task() { taskDelete(); }

  void attach(TaskFunction_t callback, taskArg_t args = NULL);
  void sendNotify();
  void sendNotify(uint32_t sendBit);
  void sendNotifyFromISR();
  void sendNotifyFromISR(uint32_t sendBit);
  uint32_t takeNotify();
  uint32_t takeNotify(TickType_t time);
  void delayUntil(milliseconds time);
  BaseType_t abortDelay();
  void suspend();
  void resume();
  BaseType_t resumeFromISR();
  void taskDelete();
  inline const char *name() { return _name; }
  void priority(UBaseType_t priority);
  inline UBaseType_t priority() { return _priority; }
  inline UBaseType_t priorityFromISR() { return _priority; }
  inline uint32_t stackSize() { return _stackSize; }
  template <typename type> inline type args() { return static_cast<type>(_args); }
  template <typename type> inline void args(type args) { _args = static_cast<taskArg_t>(args); }
  inline BaseType_t core() { return _core; }
  uint32_t memUsage();
  uint32_t memFree();

private:
  TaskFunction_t _callback;
  const char *_name;
  UBaseType_t _priority;
  UBaseType_t _core;
  uint32_t _stackSize;
  taskArg_t _args;
  TaskHandle_t _handle = NULL;
  TickType_t previousTime = 0;
};

Task *get_task(taskArg_t arg);

} // namespace rtos

#endif
