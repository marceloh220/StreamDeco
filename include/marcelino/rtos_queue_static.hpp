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

template <typename type, const uint32_t SIZE>
class QueueStatic {

public:
  QueueStatic() {
    if(_handle != NULL)
      return;
    _handle = xQueueCreateStatic(SIZE, sizeof(type), _stakBuffer, &_stak);
  }

  ~QueueStatic() { 
    if(_handle == NULL)
      return;
    vQueueDelete(_handle);
    _handle = NULL;
  }

  bool send(type &data) {
    if(_handle == NULL)
      return false;
    return xQueueSend(_handle, (void *)&data, portMAX_DELAY);
  }

  bool send(type &data, milliseconds timeout) {
    if(_handle == NULL)
      return false;
    return xQueueSend(_handle, (void *)&data, CHRONO_TO_TICK(timeout));
  }

  bool sendToBack(type &data) {
    if(_handle == NULL)
      return false;
    return xQueueSendToBack(_handle, (void *)&data, portMAX_DELAY);
  }

  bool sendToBack(type &data, milliseconds timeout) {
    if(_handle == NULL)
      return false;
    return xQueueSendToBack(_handle, (void *)&data, CHRONO_TO_TICK(timeout));
  }

  bool sendToFont(type &data) {
    if(_handle == NULL)
      false;
    return xQueueSendToFront(_handle, (void *)&data, portMAX_DELAY);
  }

  bool sendToFront(type &data, milliseconds timeout) {
    if(_handle == NULL)
      return false;
    return xQueueSendToFront(_handle, (void *)&data, CHRONO_TO_TICK(timeout));
  }

  bool sendFromISR(type &data) {
    if(_handle == NULL)
      return false;
    return xQueueSendFromISR(_handle, (void *)&data, NULL);
  }

  bool sendToBackFromISR(type &data) {
    if(_handle == NULL)
      return false;
    return xQueueSendToBackFromISR(_handle, (void *)&data, NULL);
  }

  bool sendToFrontFromISR(type &data) {
    if(_handle == NULL)
      false;
    return xQueueSendToFrontFromISR(_handle, (void *)&data, NULL);
  }

  bool receive(type &data) {
    if(_handle == NULL)
      return false;
    return xQueueReceive(_handle, (void *)&data, portMAX_DELAY);
  }

  bool receive(type &data, milliseconds timeout) {
    if(_handle == NULL)
      return false;
    return xQueueReceive(_handle, (void *)&data, CHRONO_TO_TICK(timeout));
  }

  UBaseType_t messagesWaiting() { 
    if(_handle == NULL)
      return 0;
    return uxQueueMessagesWaiting(_handle);
  }

  UBaseType_t messagesWaitingFromISR() {
    if(_handle == NULL)
      return 0;
    return uxQueueMessagesWaitingFromISR(_handle);
  }

  UBaseType_t spacesAvaliable() { 
    if(_handle == NULL)
      return 0;
    return uxQueueSpacesAvailable(_handle);
  }

  void reset() { 
    if(_handle == NULL)
      return;
    xQueueReset(_handle);
  }

  int size() {
    if(_handle == NULL)
      return 0;
    return _size;
  }

private:
  
  QueueHandle_t _handle = NULL;
  StaticQueue_t _stak;
  uint8_t       _stakBuffer[sizeof(type) * SIZE];
  const int _size = SIZE;

}; // clss QueueStatic

} // namespace rtos

#endif
