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

#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

namespace rtos {

template <typename type> class Queue {

public:
  Queue(uint32_t length) { _handle = xQueueCreate(length, sizeof(type)); }

  ~Queue() { vQueueDelete(_handle); }

  BaseType_t send(type *data) {
    return xQueueSend(_handle, (void *)data, portMAX_DELAY);
  }

  BaseType_t send(type *data, milliseconds timeout) {
    return xQueueSend(_handle, (void *)data, CHRONO_TO_TICK(timeout));
  }

  BaseType_t sendToBack(type *data) {
    return xQueueSendToBack(_handle, (void *)data, portMAX_DELAY);
  }

  BaseType_t sendToBack(type *data, milliseconds timeout) {
    return xQueueSendToBack(_handle, (void *)data, CHRONO_TO_TICK(timeout));
  }

  BaseType_t sendToFont(type *data) {
    return xQueueSendToFront(_handle, (void *)data, portMAX_DELAY);
  }

  BaseType_t sendToFont(type *data, milliseconds timeout) {
    return xQueueSendToFront(_handle, (void *)data, CHRONO_TO_TICK(timeout));
  }

  BaseType_t sendFromISR(type *data) {
    return xQueueSendFromISR(_handle, (void *)data, NULL);
  }

  BaseType_t sendToBackFromISR(type *data) {
    return xQueueSendToBackFromISR(_handle, (void *)data, NULL);
  }

  BaseType_t sendToFrontFromISR(type *data) {
    return xQueueSendToFrontFromISR(_handle, (void *)data, NULL);
  }

  BaseType_t receive(type *data) {
    return xQueueReceive(_handle, (void *)data, portMAX_DELAY);
  }

  BaseType_t receive(type *data, milliseconds timeout) {
    return xQueueReceive(_handle, (void *)data, CHRONO_TO_TICK(timeout));
  }

  UBaseType_t messagesWaiting() { return uxQueueMessagesWaiting(_handle); }

  UBaseType_t messagesWaitingFromISR() {
    return uxQueueMessagesWaitingFromISR(_handle);
  }

  UBaseType_t spacesAvaliable() { return uxQueueSpacesAvailable(_handle); }

  BaseType_t reset() { return xQueueReset(_handle); }

private:
  QueueHandle_t _handle;
};

} // namespace rtos

#endif
