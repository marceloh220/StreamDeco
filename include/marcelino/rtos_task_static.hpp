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

#ifndef _RTOS_TASK_STATIC_HPP_
#define _RTOS_TASK_STATIC_HPP_

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

#include "rtos_chrono.hpp"

/**
 * @typedef  taskStaticArg_t
 * @brief    Parameters passed to task handler
 */
typedef void *taskStaticArg_t;

/**
 * @namespace  rtos
 * @brief      Name space with freeRTOS functions
 */
namespace rtos {

/**
 * @class    TaskStatic
 * @brief    Create a new task.
 * @details  This class is similar to xTaskCreate in a self conteined object 
 * @details  with all proprieties needed by a task in SMP system.
 *
 * @param name A descriptive name for the task.  This is mainly used to
 * facilitate debugging.  Max length defined by configMAX_TASK_NAME_LEN - default
 * is 16.
 *
 * @param priority The priority at which the task should run. Systems that
 * include MPU support can optionally create tasks in a privileged (system)
 * mode by setting bit portPRIVILEGE_BIT of the priority parameter. For example,
 * to create a privileged task at priority 2 the uxPriority parameter
 * should be set to ( 2 | portPRIVILEGE_BIT ).
 * 
 * @param stackSize The size of the task stack specified as the number of
 * bytes. Note that this differs from vanilla FreeRTOS.
 *
 * @param core If the value is tskNO_AFFINITY, the created task is not
 * pinned to any CPU, and the scheduler can run it on any core available.
 * Values 0 or 1 indicate the index number of the CPU which the task should
 * be pinned to. Specifying values larger than (portNUM_PROCESSORS - 1) will
 * cause the function to fail.
 * 
 * @code
 * rtos::TaskStatic<2048> task_one("Task One");
 * 
 * rtos::TaskStatic<2048> task_two("Task Two");
 * 
 * void task_handler(taskStaticArg_t arg)
 * {
 * 
 *     while(true)
 *     {
 * 
 *          printf("Runing: %s\n", static_cast<const char*>(arg));
 * 
 *          rtos::sleep(1s);
 * 
 *     }
 * 
 * }
 * 
 * extern 'C' main_app()
 * {
 * 
 *    task_one.attach(task_handler, task_one.name());
 * 
 *    task_two.attach(task_handler, task_two.name());
 * 
 * } 
 *
 */
template<const uint32_t SIZE>
class TaskStatic {
public:
  typedef enum pinCore {
    CORE_0,
    CORE_1,
    NO_AFINITY = tskNO_AFFINITY,
  } pinCore_t;

  TaskStatic(const char *name, UBaseType_t priority = 0, pinCore_t core = NO_AFINITY) 
  : _name(name), _priority(priority), _core(core)
  {

  }

  /**
   * @brief Remove a task from the RTOS real time kernel's management
   */
  ~TaskStatic() {
    taskDelete();
  }

  /**
   * @brief    Attach the handler callback on task
   * @details  Affter instanciate the task still not running
   *           and must have an handler attached to enter in execution
   * @param    callback  Function callback to handler the task execution
   * @param    args      Parameter that can be passed to task handler
   * @note     The handler function must be type void and receive arg type taskStaticArg_t
   *           and are not allowed to return, running into a infinit loop.
   * @code
   *           void task_handler(taskStaticArg_t arg)
   *           {
   *                while(true)
   *                {
   * 
   *                   printf("Task runing =)\n");
   * 
   *                   rtos::sleep(1s);
   * 
   *                }
   * 
   *           }
   */
  void attach(TaskFunction_t callback, taskStaticArg_t args = NULL) {
    if(_handle != NULL)
      return;
    _handle = xTaskCreateStatic(callback, _name, _stackSize, args, _priority, _stackBuffer, &_stak);
  }

  /**
   * @see      rtos::Task::attach(TaskFunction_t callback, taskStaticArg_t args)
   * @brief    Shortcut template to attach method
   * @details  Work as attach function but receivint any type of args with auto convertion
   */
  template <typename T>
  void attach(TaskFunction_t callback, T args) {
    attach(callback, (taskStaticArg_t)args);
  }

  /**
   * @see      rtos::Task::takeNotify()
   * @see      rtos::Task::takeNotify(millisecond time)
   * @brief    Sends a direct to task notification to task
   * @details  The task will wait for a notification using method takeNotify
   * @param    notification  Data index sended to notify task
   * @note     Similar to giving a counter semaphore
  */
  void sendNotify(uint32_t notification) {
    if (_handle == NULL)
      return;
    xTaskNotify(_handle, notification, eSetBits);
  }

  /**
   * @see      rtos::Task::takeNotify()
   * @see      rtos::Task::takeNotify(millisecond time)
   * @brief    Sends a direct to task notification to task from a ISR
   * @details  The task will wait for a notification using method takeNotify
   * @param    notification  Data index sended to notify task
   * @note     Similar to giving a counter semaphore
  */
  void sendNotifyFromISR(uint32_t notification) {
    if (_handle == NULL)
      return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(_handle, notification, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  /**
   *
   * @brief    Wait until task receive a notification
   * @return   Unsigned interger number with notification passed
   * @note     Similar to waiting for a counter semaphore
   *           The task will not consume any processing time while 
   *           it is in the Blocked state
  */
  uint32_t takeNotify() {
    if (_handle == NULL)
      return 0;
    return ulTaskNotifyTake(true, portMAX_DELAY);
  }

  /**
   * @brief    Wait until task receive a notification until the passed time
   * @param    time  Time to wait notification be received
   * @return   Unsigned interger number with notification passed
   *           Return 0 in the end of block time
   * @note     Similar to waiting for a counter semaphore
   *           The task will not consume any processing time while 
   *           it is in the Blocked state, run in the end of this time
  */
  uint32_t takeNotify(milliseconds time) {
    if (_handle == NULL)
      return 0;
    return ulTaskNotifyTake(true, CHRONO_TO_TICK(time));
  }

  /**
   * @brief   Put task in Blocked until a specified time.
   * @details This method can be used by periodic
   *          tasks to ensure a constant execution frequency.
   * @param time The cycle time period. The task will be unblocked at
   *             time last sleep time + time. Calling sleepUntil with the
   *             same time parameter value will cause the task to execute with
   *             a fixed interface period.
   */
  void sleepUntil(milliseconds time) {
    if (_handle == NULL)
      return;
    vTaskDelayUntil(&_previousTime, CHRONO_TO_TICK(time));
  }

  /**
   * @brief   Wakeup the task from a sleep changing they state from Blocked to Ready.
   * @return  If the task was not in the Blocked state then pdFAIL is returned.
   *          Otherwise pdPASS is returned.
   */
  BaseType_t wakeup() {
    if (_handle == NULL)
      return 0;
    return xTaskAbortDelay(_handle);
  }

  /**
   * @brief    Suspend the task.
   * @details  When suspended a task will never get any microcontroller
   *           processing time, no matter what its priority.
   * @note     A suspended task can be resumed with resume method
   *           Calls to suspend are not accumulative -
   *           i.e. calling suspend() twice on the same task still only requires one
   *           call to resume() to ready the suspended task.
   */
  void suspend() {
    if (_handle == NULL)
      return;
    vTaskSuspend(_handle);
  }

  /**
   * @brief    Resumes a suspended task.
   * @details  A task that has been suspended by one or more calls to suspend()
   *           will be made available for running again by a single call to
   *           resume().
   */
  void resume() {
    if (_handle == NULL)
      return;
    vTaskResume(_handle);
  }

  /**
   * @brief    Resumes a suspended task that can be called from within an ISR.
   * @details  A task that has been suspended by one or more calls to suspend()
   *           will be made available for running again by a single call to
   *           resume().
   */
  void resumeFromISR() {
    if (_handle == NULL)
      return;
    xTaskResumeFromISR(_handle);
  }

  /**
   * @brief Remove a task from the RTOS real time kernel's management. The task being
   * deleted will be removed from all ready, blocked, suspended and event lists.
   *
   * @note The memory allocated for task is static and will not be free,
   * the task can be recreated using the same stak space using rtos::attach() method, 
   * but theres no point to a static task be deleted.
   */
  void taskDelete() {
    if (_handle == NULL)
      return;
    vTaskDelete(_handle);
    _handle = NULL;
  }
  
  /**
   * @brief   Get task name
   * @return  The text (human readable) name of the task
   * @note    Safe to be called in ISR, but not usable =P
   */
  inline const char *name() {return _name;}

  /**
   * @brief  Set the priority of task.
   * @param  priority The priority to which the task will be set.
   * @note   A context switch will occur before the method returns if the priority
   *         being set is higher than the currently executing task.
   */
  void priority(UBaseType_t priority) {
    if (_handle == NULL)
      return;
    _priority = priority;
    vTaskPrioritySet(_handle, priority);
  }

  /**
   * @brief   Get the priority of task.
   * @return  The priority of task
   * @note    Higher number means higher priority
   */
  inline UBaseType_t priority() {return _priority;}

  /**
   * @brief   Get the priority of task that can be called from within an ISR.
   * @return  The priority of task
   * @note    Higher number means higher priority
   */
  inline UBaseType_t priorityFromISR() {return _priority;}

  /**
   * @brief   Get task stack size specified as the number of bytes.
   * @return  Unsigned integer number of stack size specified as the number of bytes.
   * @note    Safe to be called in ISR
   */
  inline uint32_t stackSize() {return _stackSize;}

  /**
   * @brief   Get core where task are pinned
   * @return  Core where task are pinned
   * @note    Safe to be called in ISR
   */
  inline pinCore_t core() {return static_cast<pinCore_t>(_core);}

  /**
   * @brief   Get task memory used in stack specified as the number of bytes.
   * @return  Unsigned integer number of stack memory used specified as the number of bytes.
   * @note    NSFI - Not Safe For ISR
   */
  uint32_t memUsage() {
    if (_handle == NULL)
      return 0;
    return _stackSize - uxTaskGetStackHighWaterMark(_handle);
  }

  /**
   * @brief   Get task memory avaliable in stack specified as the number of bytes.
   * @return  Unsigned integer number of stack memory avaliable specified as the number of bytes.
   * @note    NSFI - Not Safe For ISR
   */
  uint32_t memFree() {
    if (_handle == NULL)
      return 0;
    return uxTaskGetStackHighWaterMark(_handle);
  }

private:
  /**
   * @var    _name
   * @brief  Pointer to keep task name into object to easy access
   */
  const char *_name;

  /**
   * @var    _priority
   * @brief  Keep task priority into object to easy access
   */
  UBaseType_t _priority;

  /**
   * @var    _core
   * @brief  Keep task pinned core into object to easy access
   */
  UBaseType_t _core;

  /**
   * @var    _stackSize
   * @brief  Keep task stack size into object to easy access
   */
  uint32_t _stackSize = SIZE;

  /**
   * @var      _handle
   * @brief    Keep task handle into object to easy access
   * @details  
   */
  TaskHandle_t _handle = NULL;

  /**
   * @var    _stak
   * @brief  Keep task stak into object to easy access
   */
  StaticTask_t _stak;

  /**
   * @var    _stakBuffer
   * @brief  Keep task stak buffer into object to easy access
   */
  StackType_t  _stackBuffer[SIZE];

  /**
   * @var    _previousTime
   * @brief  Previous time used to call sleepUntil method
   */
  TickType_t _previousTime = 0;
  
}; // class TaskStatic

} // namespace rtos

  #endif
