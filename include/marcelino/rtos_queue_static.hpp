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

#ifndef _RTOS_QUEUE_STATIC_HPP_
#define _RTOS_QUEUE_STATIC_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

namespace rtos {

/**
 * @sa       TaskStatic
 * @brief    Creates a new queue instance
 * @details  Internally, within the FreeRTOS implementation, queues use two blocks of
 * memory. The first block is used to hold the queue's data structures and the
 * second block is used to hold items placed into the queue, the class keep this blocks
 * internaly then both blocks of memory are automatically static allocated.
 * This class is a template thaths allow creation or any type of data queue.
 * @tparam   type  Queue's data type
 * @tparam   SIZE  Queue's size, determine the number of elemtents that can be passed to Queue
 * @code
 * rtos::TaskStatic<2048> task_receiver("Task Receiver");
 * 
 * rtos::QueueStatic<int, 2> queue_intData;
 * 
 * void task_receiver_handler(taskStaticArg_t arg) {
 * 
 *  int counter = 0;
 * 
 *  while(true) {
 *    
 *    if(queue_intData.receive(counter, 500ms)) {
 * 
 *      printf("Message received: %d\n", counter);
 *    
 *    } else {
 *    
 *      printf("Message not received\n");
 *    
 *    }
 *    
 *  }
 * 
 * }
 * 
 * extern "C" app_main() {
 * 
 *  int counter = 0;
 *  
 *  task_receiver.attach(task_receiver_handler);
 * 
 *  while(true) {
 *  
 *    queue_intData.send(counter);
 * 
 *    counter++;
 *  
 *    rtos::sleep(1s);
 * 
 *  }
 * 
 * }
 */
template <typename type, const uint32_t SIZE>
class QueueStatic {

public:
  QueueStatic() {
    if(_handle != NULL)
      return;
    _handle = xQueueCreateStatic(SIZE, sizeof(type), _stakBuffer, &_stack);
  }

  ~QueueStatic() { 
    if(_handle == NULL)
      return;
    vQueueDelete(_handle);
    _handle = NULL;
  }

  /**
   * @brief   Post an item on a queue.
   * @details  Block the task until a intem be posted on queue or an error occour.
   * @param   data Data to the item that is to be placed on the queue.
   * The size of the items the queue will hold was defined when the queue was created, 
   * so this many bytes will be copied from data into the queue storage area.
   * @note    This function must not be called from an interrupt service routine.
   * See rtos::Queue::sendFromISR () for an alternative which may be used in an ISR.
   * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
   */
  bool send(type &data) {
    if(_handle == NULL)
      return false;
    return xQueueSend(_handle, (void *)&data, portMAX_DELAY);
  }

  /**
   * @brief   Post an item on a queue.
   * @details  Block the task until a intem be posted on queue,
   * the timeout expire or an error occour.
   * @param   data Data to the item that is to be placed on the queue.
   * The size of the items the queue will hold was defined when the queue was created, 
   * so this many bytes will be copied from data into the queue storage area.
   * @param   timeout The maximum amount of time the task should block
   * waiting for space to become available on the queue, should it already
   * be full.
   * @note    This function must not be called from an interrupt service routine.
   * See rtos::Queue::sendFromISR () for an alternative which may be used in an ISR.
   * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
   */
  bool send(type &data, milliseconds timeout) {
    if(_handle == NULL)
      return false;
    return xQueueSend(_handle, (void *)&data, CHRONO_TO_TICK(timeout));
  }

  /**
   * @brief   Post an item to the back of a queue.
   * @details  Block the task until a intem be posted on queue or an error occour.
   * @param   data Data to the item that is to be placed on the queue.
   * The size of the items the queue will hold was defined when the queue was created, 
   * so this many bytes will be copied from data into the queue storage area.
   * @note    This function must not be called from an interrupt service routine.
   * See rtos::Queue::sendFromISR () for an alternative which may be used in an ISR.
   * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
   */
  bool sendToBack(type &data) {
    if(_handle == NULL)
      return false;
    return xQueueSendToBack(_handle, (void *)&data, portMAX_DELAY);
  }

  /**
   * @brief   Post an item to the back of a queue.
   * @details  Block the task until a intem be posted on queue,
   * the timeout expire or an error occour.
   * @param   data Data to the item that is to be placed on the queue.
   * The size of the items the queue will hold was defined when the queue was created, 
   * so this many bytes will be copied from data into the queue storage area.
   * @param   timeout The maximum amount of time the task should block
   * waiting for space to become available on the queue, should it already
   * be full.
   * @note    This function must not be called from an interrupt service routine.
   * See rtos::Queue::sendFromISR () for an alternative which may be used in an ISR.
   * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
   */
  bool sendToBack(type &data, milliseconds timeout) {
    if(_handle == NULL)
      return false;
    return xQueueSendToBack(_handle, (void *)&data, CHRONO_TO_TICK(timeout));
  }

  /**
   * @brief   Post an item to the front of a queue.
   * @details  Block the task until a intem be posted on queue or an error occour.
   * @param   data Data to the item that is to be placed on the queue.
   * The size of the items the queue will hold was defined when the queue was created, 
   * so this many bytes will be copied from data into the queue storage area.
   * @note    This function must not be called from an interrupt service routine.
   * See rtos::Queue::sendFromISR () for an alternative which may be used in an ISR.
   * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
   */
  bool sendToFront(type &data) {
    if(_handle == NULL)
      false;
    return xQueueSendToFront(_handle, (void *)&data, portMAX_DELAY);
  }

  /**
   * @brief   Post an item to the front of a queue.
   * @details  Block the task until a intem be posted on queue,
   * the timeout expire or an error occour.
   * @param   data Data to the item that is to be placed on the queue.
   * The size of the items the queue will hold was defined when the queue was created, 
   * so this many bytes will be copied from data into the queue storage area.
   * @param   timeout The maximum amount of time the task should block
   * waiting for space to become available on the queue, should it already
   * be full.
   * @note    This function must not be called from an interrupt service routine.
   * See rtos::Queue::sendFromISR () for an alternative which may be used in an ISR.
   * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
   */
  bool sendToFront(type &data, milliseconds timeout) {
    if(_handle == NULL)
      return false;
    return xQueueSendToFront(_handle, (void *)&data, CHRONO_TO_TICK(timeout));
  }

  /**
   * @brief   Post an item on a queue. It is safe to use this function from
   * within an interrupt service routine.
   * @param   data Data to the item that is to be placed on the queue.
   * The size of the items the queue will hold was defined when the queue was created, 
   * so this many bytes will be copied from data into the queue storage area.
   * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
   */
  bool sendFromISR(type &data) {
    if(_handle == NULL)
      return false;
    return xQueueSendFromISR(_handle, (void *)&data, NULL);
  }

  /**
   * @brief   Post an item to the back of a queue. It is safe to use this function from
   * within an interrupt service routine.
   * @param   data Data to the item that is to be placed on the queue.
   * The size of the items the queue will hold was defined when the queue was created, 
   * so this many bytes will be copied from data into the queue storage area.
   * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
   */
  bool sendToBackFromISR(type &data) {
    if(_handle == NULL)
      return false;
    return xQueueSendToBackFromISR(_handle, (void *)&data, NULL);
  }

  /**
   * @brief   Post an item to the front of a queue. It is safe to use this function from
   * within an interrupt service routine.
   * @param   data Data to the item that is to be placed on the queue.
   * The size of the items the queue will hold was defined when the queue was created, 
   * so this many bytes will be copied from data into the queue storage area.
   * @return pdTRUE if the item was successfully posted, otherwise errQUEUE_FULL.
   */
  bool sendToFrontFromISR(type &data) {
    if(_handle == NULL)
      false;
    return xQueueSendToFrontFromISR(_handle, (void *)&data, NULL);
  }

  /**
   * @brief   Receive an item from a queue. The item is received by copy so a buffer of
   * adequate size must be provided. The number of bytes copied into the buffer
   * was defined when the queue was instanciate.
   * @details  Block the task until a intem be received from queue or an error occour.
   * @param   data Pointer to the buffer into which the received item will be copied.
   * @return  pdTRUE if an item was successfully received from the queue, otherwise pdFALSE.
   * @note    This function must not be called from an interrupt service routine.
   */
  bool receive(type &data) {
    if(_handle == NULL)
      return false;
    return xQueueReceive(_handle, (void *)&data, portMAX_DELAY);
  }

  /**
   * @brief   Receive an item from a queue. The item is received by copy so a buffer of
   * adequate size must be provided. The number of bytes copied into the buffer
   * was defined when the queue was instanciate.
   * @details  Block the task until a intem be received from queue,
   * the timeout expire or an error occour.
   * @param   data Pointer to the buffer into which the received item will be copied.
   * @param   timeout The maximum amount of time the task should block
   * waiting for space to become available on the queue, should it already
   * be full.
   * @return  pdTRUE if an item was successfully received from the queue, otherwise pdFALSE.
   * @note    This function must not be called from an interrupt service routine.
   */
  bool receive(type &data, milliseconds timeout) {
    if(_handle == NULL)
      return false;
    return xQueueReceive(_handle, (void *)&data, CHRONO_TO_TICK(timeout));
  }

  /**
   * @brief   Return the number of free spaces available in a queue. This is equal to the
   * number of items that can be sent to the queue before the queue becomes full if no items are removed.
   * @return  The number of messages available in the queue.
   */
  UBaseType_t messagesWaiting() { 
    if(_handle == NULL)
      return 0;
    return uxQueueMessagesWaiting(_handle);
  }

  /**
   * @brief   Return the number of free spaces available in a queue. This is equal to the
   * number of items that can be sent to the queue before the queue becomes full if no items are removed.
   * @return  The number of messages available in the queue.
   * @note    Utilities to query queues that are safe to use from an ISR.
   */
  UBaseType_t messagesWaitingFromISR() {
    if(_handle == NULL)
      return 0;
    return uxQueueMessagesWaitingFromISR(_handle);
  }

  /**
   * @brief   Return the number of free spaces available in a queue. This is equal to the
   * number of items that can be sent to the queue before the queue becomes full if no items are removed.
   * @return  The number of spaces available in the queue.
   */
  UBaseType_t spacesAvaliable() { 
    if(_handle == NULL)
      return 0;
    return uxQueueSpacesAvailable(_handle);
  }

  /**
   * @brief  Reset a queue back to its original empty state.
   */
  void reset() { 
    if(_handle == NULL)
      return;
    xQueueReset(_handle);
  }

  /**
   * @brief   Return queue size
   * @return  The number of item each queue can receive.
   */
  int size() {
    if(_handle == NULL)
      return 0;
    return _size;
  }

private:
  
  QueueHandle_t _handle = NULL;
  StaticQueue_t _stack;
  uint8_t       _stakBuffer[sizeof(type) * SIZE];
  const int _size = SIZE;

}; // class QueueStatic

} // namespace rtos

#endif
