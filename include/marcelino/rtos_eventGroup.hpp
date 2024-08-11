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

#ifndef _EVENT_GROUP_HPP_
#define _EVENT_GROUP_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "rtos_chrono.hpp"

namespace rtos {

class EventGroup {

public:
  EventGroup() { _handle = xEventGroupCreate(); }

  ~EventGroup() {
    vEventGroupDelete(_handle);
  }

  inline EventBits_t set(const EventBits_t flags) {
    return xEventGroupSetBits(_handle, flags);
  }

  inline EventBits_t setFromISR(const EventBits_t flags) {
    return xEventGroupSetBitsFromISR(_handle, flags, NULL);
  }

  inline EventBits_t clear(const EventBits_t flags) {
    return xEventGroupClearBits(_handle, flags);
  }

  inline EventBits_t clearFromISR(const EventBits_t flags) {
    return xEventGroupClearBitsFromISR(_handle, flags);
  }

  inline EventBits_t get() { return xEventGroupGetBits(_handle); }

  inline EventBits_t getFromISR() {
    return xEventGroupGetBitsFromISR(_handle);
  }

  inline EventBits_t wait() {
    return xEventGroupWaitBits(_handle, 0xFFFFFF, pdTRUE, pdFALSE, portMAX_DELAY);
  }

  inline EventBits_t wait(const EventBits_t flags) {
    return xEventGroupWaitBits(_handle, flags, pdTRUE, pdTRUE, portMAX_DELAY);
  }

  inline EventBits_t wait(const EventBits_t flags, bool clearFlags, bool allFlags) {
    return xEventGroupWaitBits(_handle, flags, clearFlags, allFlags, portMAX_DELAY);
  }

  inline EventBits_t wait(const EventBits_t flags, bool clearFlags, bool allFlags, milliseconds timeout) {
    return xEventGroupWaitBits(_handle, flags, clearFlags, allFlags, CHRONO_TO_TICK(timeout));
  }

private:
  EventGroupHandle_t _handle;
};

} // namespace rtos

#endif
