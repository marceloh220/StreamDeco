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

#ifndef _RTOS_EVENT_GROUP_STATIC_HPP_
#define _RTOS_EVENT_GROUP_STATIC_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "rtos_chrono.hpp"

namespace rtos {

/**
 * @sa       TaskStatic
 * @brief    Creates a new event group instance.
 * @details  Internally, within the FreeRTOS implementation, event groups use a [small]
 * block of memory, in which the event group's structure is stored. If an event
 * groups is created using rtos::EventGroup then the required memory is
 * automatically static allocated inside the object.
 * @note     Each event group has 24 usable bits (bit 0 to bit 23). The EventBits_t type is used to store
 * event bits within an event group.
 * @code 
 * rtos::TaskStatic<2048> task_receiver_or("Task Receiver OR");
 * 
 * rtos::TaskStatic<2048> task_receiver_and("Task Receiver AND");
 * 
 * rtos::TaskStatic<2048> task_Transmiter("Task Transmiter");
 * 
 * rtos::EventGroupStatic event_group; 
 * 
 * 
 * #define EVENT_0 (1 << 0)
 * 
 * #define EVENT_1 (1 << 1)
 * 
 * #define EVENT_2 (1 << 2)
 * 
 * #define EVENT_3 (1 << 3)
 * 
 * 
 * void task_receiver_or_handler(taskStaticArg_t arg) {
 * 
 *    while(true) {
 * 
 *      if(event_group.waitAnyFlags(EVENT_0|EVENT_1)) {
 * 
 *        printf("Event group take by Task receiver OR\n");
 * 
 *      }
 * 
 *      rtos::sleep(500ms);
 * 
 *    }
 * 
 * }
 * 
 * 
 * void task_receiver_and_handler(taskStaticArg_t arg) {
 * 
 *    while(true) {
 * 
 *      if(event_group.waitAllFlags(EVENT_2|EVENT_3)) {
 * 
 *        printf("Event group take by Task receiver AND\n");
 * 
 *      }
 * 
 *      rtos::sleep(500ms);
 * 
 *    }
 * 
 * }
 * 
 * 
 * void task_transmiter_handler(taskStaticArg_t arg) {
 * 
 *    while(true) {
 * 
 *      event_group.set(EVENT_0);
 *    
 *      rtos::sleep(500ms);
 * 
 *    }
 * 
 * }
 * 
 * 
 * extern "C" app_main() {
 * 
 *      task_receiver_or.attach(task_receiver_or_handler);
 * 
 *      task_receiver_and.attach(task_receiver_and_handler);
 * 
 *      task_Transmiter.attach(task_transmiter_handler);
 * 
 *      while(true) {
 * 
 *          event_group.set(EVENT_1);
 * 
 *          event_group.set(EVENT_2|EVENT_3);
 * 
 *          rtos::sleep(1s);
 * 
 *      }
 * 
 * }
 */
class EventGroupStatic {

public:

  EventGroupStatic();

  ~EventGroupStatic();

  /**
   * @brief    Set bits within an event group.
   * @details  Setting bits in an event group will automatically unblock tasks that are
   *           blocked waiting for the bits.
   * @note     This function cannot be called from an interrupt. setBitsFromISR()
   *           is a version that can be called from an interrupt.
   * @param    flags A bitwise value that indicates the bit or bits to set.
   *           For example, to set bit 3 only, set uxBitsToSet to 0x08 of (1<<3). To set bit 3
   *           and bit 0 set uxBitsToSet to 0x09 or (1<<0)|(1<<3).
   * @return The value of the event group at the time the call to
   *         set() returns.
   */
  EventBits_t set(const EventBits_t flags);

  EventBits_t setFromISR(const EventBits_t flags);

  EventBits_t clear(const EventBits_t flags);

  EventBits_t clearFromISR(const EventBits_t flags);

  EventBits_t get();

  EventBits_t getFromISR();

  EventBits_t wait();

  EventBits_t wait(milliseconds timeout);

  bool waitAllFlags(const EventBits_t flags);

  bool waitAllFlags(const EventBits_t flags, milliseconds timeout);

  bool waitAnyFlags(const EventBits_t flags);

  bool waitAnyFlags(const EventBits_t flags, milliseconds timeout);

private:

  EventGroupHandle_t _handle = NULL;
  StaticEventGroup_t _stack;

}; // namespace EventGroup

} // namespace rtos

#endif
