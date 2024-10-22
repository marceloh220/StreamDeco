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

#ifndef _RTOS_SEMAPHORE_HPP_
#define _RTOS_SEMAPHORE_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "rtos_chrono.hpp"

namespace rtos {

/**
 * @sa       Task
 * @brief    Creates a new semathore instance
 * @details  Internally, within the FreeRTOS implementation, counting semaphores use a
 * block of memory, in which the counting semaphore structure is stored. If a 
 * counting semaphore is created using rtos::Semaphore then the required 
 * memory is automatically dynamically allocated inside the object.
 * @param    count The maximum count value that can be reached. 
 * When the semaphore reaches this value it can no longer be 'given'.
 * @param    initialValue The count value assigned to the semaphore when it is created.
 * @code
 * rtos::Task task_receiver("Task Receiver");
 * 
 * rtos::Semaphore semaphore;
 * 
 * void task_receiver_handler(taskStaticArg_t arg) {
 * 
 *  int counter = 0;
 * 
 *  while(true) {
 *    
 *    if(semaphore.take(500ms)) {
 * 
 *      printf("Semaphore received\n");
 *    
 *    } else {
 *    
 *      printf("Semaphore not received\n");
 *    
 *    }
 *    
 *  }
 * 
 * }
 * 
 * extern "C" app_main() {
 *  
 *  task_receiver.attach(task_receiver_handler);
 * 
 *  while(true) {
 *  
 *    semaphore.give();
 *  
 *    rtos::sleep(1s);
 * 
 *  }
 * 
 * }
 */
class Semaphore {

public:

  Semaphore(int count = 1, int initialValue = 0);
  ~Semaphore();

  /**
   * @brief  Delete a semaphore. This function must be used with care,
   * the task waiting for the semaphore will be blocked forever and ever.
   * And ever... and ever.. and ever... and ever... and ever...
   * I dont eve know why I left this method herer LOLOLOLOLOL
   */
  void semaphoreDelete();

  /**
   * @sa      giveFromISR()
   * @brief   Release a semaphore. The semaphore must be obtained using take().
   * @return  pdTRUE if the semaphore was released
   *          pdFALSE if an error occurred
   * @note    This macro must not be used from an ISR. See giveFromISR() for
   * an alternative which can be used from an ISR.
   */
  bool give();

  /**
   * @brief   Release a semaphore. It is safe to use this function from
   * within an interrupt service routine.
   * @return  pdTRUE if the semaphore was released
   *          pdFALSE if an error occurred
   */
  BaseType_t giveFromISR();

  /**
   * @brief    Obtain a semaphore
   * @details  Block the task until semaphore be received
   * @return   This return is not utilizable, only for legacy  compatibility
   */
  bool take();

  /**
   * @brief    Obtain a semaphore
   * @details  Block the task until semaphore be received or timeout expire
   * @param    timeout The time in ticks to wait for the semaphore to become available
   * @return   pdTRUE if the semaphore was obtained
   *           pdFALSE if timeout expired without the semaphore becoming available
   */
  bool take(milliseconds timeout);

private:

  SemaphoreHandle_t _handle = NULL;

};

} // namespace rtos

#endif
