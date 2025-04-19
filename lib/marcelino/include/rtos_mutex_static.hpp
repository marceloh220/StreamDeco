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

#ifndef _RTOS_MUTEX_STATIC_HPP_
#define _RTOS_MUTEX_STATIC_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "rtos_chrono.hpp"

namespace rtos
{

  /**
   * @sa       TaskStatic
   * @brief    Creates a new mutex instance
   * @details  Internally, within the FreeRTOS implementation, mutex semaphores use a block
   * of memory, in which the mutex structure is stored. Then the required memory is automatically
   * static allocated inside the object.
   * @code
   * rtos::TaskStatic<2048> task_receiver("Task Receiver");
   *
   * rtos::MutexStatic mutex;
   *
   *
   * void task_receiver_handler(taskStaticArg_t arg) {
   *
   *      while(true) {
   *
   *          if(mutex.take(500ms)) {
   *
   *              printf("Mutex take by Task receiver\n");
   *
   *              mutex.give();
   *
   *          }
   *          else {
   *
   *              printf("Mutex could not be take by Task receiver\n");
   *
   *          }
   *
   *          rtos::sleep(500ms);
   *
   *      }
   *
   * }
   *
   * extern "C" app_main() {
   *
   *
   *      task_receiver.attach(task_receiver_handler);
   *
   *      while(true) {
   *
   *          if(mutex.take(500ms)) {
   *
   *              printf("Mutex take by app_main\n");
   *
   *              rtos::sleep(1s);
   *
   *              mutex.give();
   *
   *          }
   *          else {
   *
   *              printf("Mutex could not be take by app_main\n");
   *
   *          }
   *
   *      }
   *
   * }
   */
  class MutexStatic
  {

  public:
    MutexStatic();
    ~MutexStatic();

    /**
     * @brief    Try to obtain the mutex
     * @details  Block the task until mutex can be taken
     * @return   This return is not utilizable once the task will wait by the mutex forever,
     * only for legacy  compatibility
     */
    bool take();

    /**
     * @brief    Try to obtain the mutex
     * @details  Block the task until mutex can be obteined
     * @param    timeout The time in ticks to wait for the mutex to become available
     * @return   true if the mutex was obtained
     *           false if timeout expired without the mutex becoming available
     */
    bool take(milliseconds timeout);

    /**
     * @brief  Release the mutex to be used by others tasks
     * @return true if the mutex was released.
     *         false if an error occurred.
     *         Mutex are implemented using queues.  An error can occur if there is
     *         no space on the queue to post a message - indicating that the
     *         mutex was not first obtained correctly.
     */
    bool give();

  private:
    SemaphoreHandle_t _handle = nullptr;
    StaticSemaphore_t _stack;
  };

  /**
   * @sa       TaskStatic
   * @brief    Creates a new recursive mutex instance
   * @details  Internally, within the FreeRTOS implementation, mutex semaphores use a block
   * of memory, in which the mutex structure is stored. Then the required memory is automatically
   * static allocated inside the object.
   * @details  Recursive mutexs are indicate to be safety called by functions or methods many times as nedded.
   *           Once obteined a number of times, the mutex must be released by the same number of times.
   *           For example, if a task successfully takes the same mutex 5 times then the mutex will
   *           not be available to any other task until it has also given the mutex back exactly five times.
   * @code
   * rtos::TaskStatic<2048> task_receiver("Task Receiver");
   *
   * rtos::MutexRecursive mutex;
   *
   * void print_helloWorld() {
   *
   *      mutex.take();
   *
   *      printf("Hello World!\n");
   *
   *      mutex.give();
   *
   * }
   *
   *
   * void task_receiver_handler(taskStaticArg_t arg) {
   *
   *      while(true) {
   *
   *          if(mutex.take(500ms)) {
   *
   *              printf("Mutex take by Task receiver\n");
   *
   *              print_helloWorld();
   *
   *              mutex.give();
   *
   *          }
   *          else {
   *
   *              printf("Mutex could not be take by Task receiver\n");
   *
   *          }
   *
   *          rtos::sleep(500ms);
   *
   *      }
   *
   * }
   *
   * extern "C" app_main() {
   *
   *
   *      task_receiver.attach(task_receiver_handler);
   *
   *      while(true) {
   *
   *          if(mutex.take(500ms)) {
   *
   *              printf("Mutex take by app_main\n");
   *
   *              rtos::sleep(1s);
   *
   *              mutex.give();
   *
   *          }
   *          else {
   *
   *              printf("Mutex could not be take by app_main\n");
   *
   *          }
   *
   *      }
   *
   * }
   */
  class MutexRecursiveStatic
  {

  public:
    MutexRecursiveStatic();
    ~MutexRecursiveStatic();

    /**
     * @brief    Try to obtain the mutex
     * @details  Block the task until mutex can be taken
     * @return   This return is not utilizable once the task will wait by the mutex forever,
     * only for legacy  compatibility
     */
    bool take();

    /**
     * @brief    Try to obtain the mutex
     * @details  Block the task until mutex can be obteined
     * @param    timeout The time in ticks to wait for the mutex to become available
     * @return   true if the mutex was obtained
     *           false if timeout expired without the mutex becoming available
     */
    bool take(int timeout);

    /**
     * @brief  Release the mutex to be used by others tasks
     * @return true if the mutex was released.
     *         false if an error occurred.
     *         Mutex are implemented using queues.  An error can occur if there is
     *         no space on the queue to post a message - indicating that the
     *         mutex was not first obtained correctly.
     */
    bool give();

  private:
    SemaphoreHandle_t _handle = nullptr;
    StaticSemaphore_t _stack;
  };

} // namespace rtos

#endif
