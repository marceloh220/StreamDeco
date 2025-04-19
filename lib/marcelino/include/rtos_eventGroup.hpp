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

#ifndef _RTOS_EVENT_GROUP_HPP_
#define _RTOS_EVENT_GROUP_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "rtos_chrono.hpp"

namespace rtos
{

  /**
   * @sa       Task
   * @brief    Creates a new event group instance.
   * @details  Internally, within the FreeRTOS implementation, event groups use a [small]
   * block of memory, in which the event group's structure is stored. If an event
   * groups is created using rtos::EventGroup then the required memory is
   * automatically dynamically allocated inside the object.
   * @note     Each event group has 24 usable bits (bit 0 to bit 23). The EventBits_t type is used to store
   * event bits within an event group.
   * @code
   * rtos::Task task_receiver_or("Task Receiver OR");
   *
   * rtos::Task task_receiver_and("Task Receiver AND");
   *
   * rtos::Task task_Transmiter("Task Transmiter");
   *
   * rtos::EventGroup event_group;
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
  class EventGroup
  {

  public:
    EventGroup();

    ~EventGroup();

    /**
     * @brief    Set bits within an event group.
     * @details  Setting bits in an event group will automatically unblock tasks that are
     *           blocked waiting for the bits.
     * @note     This method cannot be called from an interrupt. setBitsFromISR()
     *           is a version that can be called from an interrupt.
     * @param    flags A bitwise value that indicates the bit or bits to set.
     *           For example, to set bit 3 only, set flags to 0x08 of (1<<3). To set bit 3
     *           and bit 0 set flags to 0x09 or (1<<0)|(1<<3).
     * @return   true if flags are seted correctly
     *           false if not seted
     */
    bool set(const EventBits_t flags);

    /**
     * @brief    Set bits within an event group.
     * @details  Setting bits in an event group will automatically unblock tasks that are
     *           blocked waiting for the bits.
     * @note     This method is a version of set() that can be called from an interrupt.
     * @param    flags A bitwise value that indicates the bit or bits to set.
     *           For example, to set bit 3 only, set flags to 0x08 of (1<<3). To set bit 3
     *           and bit 0 set flags to 0x09 or (1<<0)|(1<<3).
     * @return   true if flags are seted correctly
     *           false if not seted
     */
    bool setFromISR(const EventBits_t flags);

    /**
     * @brief    Clear bits within an event group.
     * @note     This method cannot be called from an interrupt. clearFromISR()
     *           is a version that can be called from an interrupt.
     * @param    flags A bitwise value that indicates the bit or bits to clear.
     *           For example, to set bit 3 only, set flags to 0x08 of (1<<3). To set bit 3
     *           and bit 0 set flags to 0x09 or (1<<0)|(1<<3).
     * @return   true if flags are seted correctly
     *           false if not seted
     */
    bool clear(const EventBits_t flags);

    /**
     * @brief    Clear bits within an event group.
     * @note     This method is a version of clear() that can be called from an interrupt.
     * @param    flags A bitwise value that indicates the bit or bits to clear.
     *           For example, to set bit 3 only, set flags to 0x08 of (1<<3). To set bit 3
     *           and bit 0 set flags to 0x09 or (1<<0)|(1<<3).
     * @return   true if flags are seted correctly
     *           false if not seted
     */
    bool clearFromISR(const EventBits_t flags);

    /**
     * @brief   Get the current value of the bits in an event group.
     * @note    This method cannot be called from an interrupt. getBitsFromISR()
     *          is a version that can be called from an interrupt.
     * @return  The event group bits at the time get() was called.
     */
    EventBits_t get();

    /**
     * @brief   Get the current value of the bits in an event group.
     * @note    This method is a version of get() that can be called from an interrupt.
     * @return  The event group bits at the time get() was called.
     */
    EventBits_t getFromISR();

    /**
     * @brief   Block the task to wait for one or more bits to be set within a event group.
     * @note    This method do not test or clear, this must be done by the user.
     * @return  The value of the event group at the time either the bits being waited
     *          for became set.
     * @code
     * #define FLAG_0 (1 << 0)
     *
     * #define FLAG_1 (1 << 1)
     *
     * #define FLAG_2 (1 << 2)
     *
     * void task_waiting(taskArg_t arg) {
     *
     *    rtos::EventGroup *eventGroup = static_cast<rtos::EventGroup>(arg);
     *
     *    while(true) {
     *
     *      EventBits_t flags = eventGroup->wait();
     *
     *      if(flags & FLAG_0) {
     *
     *        eventGroup->clear(FLAG_0)
     *
     *        printf("Flag Zero received\n");
     *
     *      }
     *
     *      if(flags & FLAG_1) {
     *
     *        eventGroup->clear(FLAG_1
     *
     *        printf("Flag One received\n");
     *
     *      }
     *
     *      if(flags & FLAG_2) {
     *
     *        eventGroup->clear(FLAG_2)
     *
     *        printf("Flag Two received\n");
     *
     *      }
     *
     *    }
     *
     * }
     */
    EventBits_t wait();

    /**
     * @brief   Block the task to wait for one or more bits to be set within a event group
     *          or for the time expire.
     * @param   timeour The maximum amount of time to wait for the bits specified become set.
     * @note    This method do not test or clear, this must be done by the user.
     * @return  The value of the event group at the time either the bits being waited
     *          for became set.
     * @code
     * #define FLAG_0 (1 << 0)
     *
     * #define FLAG_1 (1 << 1)
     *
     * #define FLAG_2 (1 << 2)
     *
     * void task_waiting(taskArg_t arg) {
     *
     *    rtos::EventGroup *eventGroup = static_cast<rtos::EventGroup>(arg);
     *
     *    while(true) {
     *
     *      EventBits_t flags = eventGroup->wait(1s);
     *
     *      if(flags) {
     *
     *        if(flags & FLAG_0) {
     *
     *          eventGroup->clear(FLAG_0)
     *
     *          printf("Flag Zero received\n");
     *
     *        }
     *
     *        if(flags & FLAG_1) {
     *
     *          eventGroup->clear(FLAG_1
     *
     *          printf("Flag One received\n");
     *
     *        }
     *
     *        if(flags & FLAG_2) {
     *
     *          eventGroup->clear(FLAG_2)
     *
     *          printf("Flag Two received\n");
     *
     *        }
     *
     *      }
     *      else {
     *
     *        printf("No flags received in time");
     *
     *      }
     *
     *    }
     *
     * }
     */
    EventBits_t wait(milliseconds timeout);

    /**
     * @brief   Block the task to wait for all bits to be set within a event group.
     * @param   flags A bitwise value that indicates the bit or bits to wait.
     *          For example, to set bit 3 only, set flags to 0x08 of (1<<3). To set bit 3
     *          and bit 0 set flags to 0x09 or (1<<0)|(1<<3).
     * @note    This method test and clear the waiting flags.
     * @return  true if all flags are setted
     *          false if an error occour
     */
    bool waitAllFlags(const EventBits_t flags);

    /**
     * @brief   Block the task to wait for all bits to be set within a event group
     *          or for the time expire.
     * @param   flags A bitwise value that indicates the bit or bits to wait.
     *          For example, to set bit 3 only, set flags to 0x08 of (1<<3). To set bit 3
     *          and bit 0 set flags to 0x09 or (1<<0)|(1<<3).
     * @param   timeout The maximum amount of time to wait for the bits specified become set.
     * @note    This method test and clear the waiting flags.
     * @return  true if all flags are setted
     *          false if an error occour or timeout expire.
     */
    bool waitAllFlags(const EventBits_t flags, milliseconds timeout);

    /**
     * @brief   Block the task to wait for any bits to be set within a event group.
     * @param   flags A bitwise value that indicates the bit or bits to wait.
     *          For example, to set bit 3 only, set flags to 0x08 of (1<<3). To set bit 3
     *          and bit 0 set flags to 0x09 or (1<<0)|(1<<3).
     * @note    This method test and clear the waiting flags.
     * @return  true if any flag are setted
     *          false if an error occour
     */
    bool waitAnyFlags(const EventBits_t flags);

    /**
     * @brief   Block the task to wait for any bits to be set within a event group
     *          or for the time expire.
     * @param   flags A bitwise value that indicates the bit or bits to wait.
     *          For example, to set bit 3 only, set flags to 0x08 of (1<<3). To set bit 3
     *          and bit 0 set flags to 0x09 or (1<<0)|(1<<3).
     * @param   timeout The maximum amount of time to wait for the bits specified become set.
     * @note    This method test and clear the waiting flags.
     * @return  true if any flag are setted
     *          false if an error occour or timeout expire.
     */
    bool waitAnyFlags(const EventBits_t flags, milliseconds timeout);

  private:
    EventGroupHandle_t _handle = nullptr;

  }; // namespace EventGroup

} // namespace rtos

#endif
