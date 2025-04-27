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

#include "rtos_task.hpp"

namespace rtos
{

  Task::Task(const char *name, UBaseType_t priority, uint32_t stackSize, pinCore_t core)
      : _name(name), _priority(priority), _core(core), _stackSize(stackSize)
  {
  }

  Task::~Task()
  {
    taskDelete();
  }

  void Task::attach(TaskFunction_t callback, taskArg_t args)
  {
    if (_handle != nullptr)
      return;
    xTaskCreatePinnedToCore(callback, _name, _stackSize, args, _priority, &_handle, _core);
  }

  void Task::sendNotify(uint32_t notification)
  {
    if (_handle == nullptr)
      return;
    xTaskNotify(_handle, notification, eSetBits);
  }

  void Task::sendNotifyFromISR(uint32_t notification)
  {
    if (_handle == nullptr)
      return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(_handle, notification, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  uint32_t Task::takeNotify()
  {
    if (_handle == nullptr)
      return 0;
    return ulTaskNotifyTake(true, portMAX_DELAY);
  }

  uint32_t Task::takeNotify(milliseconds time)
  {
    if (_handle == nullptr)
      return 0;
    return ulTaskNotifyTake(true, chronoToTick(time));
  }

  void Task::sleepUntil(milliseconds time)
  {
    if (_handle == nullptr)
      return;
    vTaskDelayUntil(&_previousTime, chronoToTick(time));
  }

  BaseType_t Task::wakeup()
  {
    if (_handle == nullptr)
      return 0;
    return xTaskAbortDelay(_handle);
  }

  void Task::suspend()
  {
    if (_handle == nullptr)
      return;
    vTaskSuspend(_handle);
  }

  void Task::resume()
  {
    if (_handle == nullptr)
      return;
    vTaskResume(_handle);
  }
  void Task::resumeFromISR()
  {
    if (_handle == nullptr)
      return;
    xTaskResumeFromISR(_handle);
  }

  void Task::taskDelete()
  {
    if (_handle == nullptr)
      return;
    vTaskDelete(_handle);
    _handle = nullptr;
  }

  void Task::priority(UBaseType_t priority)
  {
    if (_handle == nullptr)
      return;
    _priority = priority;
    vTaskPrioritySet(_handle, priority);
  }

  uint32_t Task::memUsage()
  {
    if (_handle == nullptr)
      return 0;
    return _stackSize - uxTaskGetStackHighWaterMark(_handle);
  }

  uint32_t Task::memFree()
  {
    if (_handle == nullptr)
      return 0;
    return uxTaskGetStackHighWaterMark(_handle);
  }

} // namespace rtos
