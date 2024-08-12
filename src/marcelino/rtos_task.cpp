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

#include "marcelino/rtos_task.hpp"

namespace rtos {

  void Task::attach(TaskFunction_t callback, taskArg_t args)
  {
    if (_handle != NULL)
      vTaskDelete(_handle);
    _callback = callback;
    _args = args;
    xTaskCreatePinnedToCore(_callback, _name, _stackSize, this, _priority, &_handle, _core);
  }

  void Task::sendNotify() {
    if (_handle == NULL)
      return;
    xTaskNotifyGive(_handle);
  }

  void Task::sendNotify(uint32_t sendBit) {
    if (_handle == NULL)
      return;
    xTaskNotify(_handle, sendBit, eSetBits);
  }

  void Task::sendNotifyFromISR() {
    if (_handle == NULL)
      return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  void Task::sendNotifyFromISR(uint32_t sendBit) {
    if (_handle == NULL)
      return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(_handle, sendBit, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  uint32_t Task::takeNotify()
  {
    return ulTaskNotifyTake(true, portMAX_DELAY);
  }

  uint32_t Task::takeNotify(TickType_t time)
  {
    return ulTaskNotifyTake(true, pdMS_TO_TICKS(time));
  }

  void Task::sleepUntil(milliseconds time)
  {
    vTaskDelayUntil(&previousTime, CHRONO_TO_TICK(time));
  }

  BaseType_t Task::wakeup()
  {
    if (_handle == NULL)
      return 0;
    return xTaskAbortDelay(_handle);
  }

  void Task::suspend() {
    if (_handle == NULL)
      return;
    vTaskSuspend(_handle);
  }

  void Task::resume() {
    if (_handle == NULL)
      return;
    vTaskResume(_handle);
  }
  BaseType_t Task::resumeFromISR() {
    if (_handle == NULL)
      return 0;
    return xTaskResumeFromISR(_handle);
  }

  void Task::taskDelete() {
    if (_handle == NULL)
      return;
    vTaskDelete(_handle);
    _handle = NULL;
  }

  void Task::priority(UBaseType_t priority) {
    if (_handle == NULL)
      return;
    _priority = priority;
    vTaskPrioritySet(_handle, priority);
  }

  uint32_t Task::memUsage() {
    if (_handle == NULL)
      return 0;
    return _stackSize - uxTaskGetStackHighWaterMark(_handle);
  }

  uint32_t Task::memFree() {
    if (_handle == NULL)
      return 0;
    return uxTaskGetStackHighWaterMark(_handle);
  }

Task *get_task(taskArg_t arg) {
  return static_cast<Task*>(arg);
}

} // namespace rtos
