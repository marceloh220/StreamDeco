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

#include "rtos_eventGroup.hpp"

namespace rtos
{

  EventGroup::EventGroup()
  {
    _handle = xEventGroupCreate();
  }

  EventGroup::~EventGroup()
  {
    vEventGroupDelete(_handle);
  }

  bool EventGroup::set(const EventBits_t flags)
  {
    if (_handle == nullptr)
      return false;
    EventBits_t bits = xEventGroupSetBits(_handle, flags);
    return (bits & flags) == flags ? true : false;
  }

  bool EventGroup::setFromISR(const EventBits_t flags)
  {
    if (_handle == nullptr)
      return false;
    EventBits_t bits = xEventGroupSetBitsFromISR(_handle, flags, nullptr);
    return (bits & flags) == flags ? true : false;
  }

  bool EventGroup::clear(const EventBits_t flags)
  {
    if (_handle == nullptr)
      return false;
    EventBits_t bits = xEventGroupClearBits(_handle, flags);
    return (bits & flags) == flags ? false : true;
  }

  bool EventGroup::clearFromISR(const EventBits_t flags)
  {
    if (_handle == nullptr)
      return false;
    EventBits_t bits = xEventGroupClearBitsFromISR(_handle, flags);
    return (bits & flags) == flags ? false : true;
  }

  EventBits_t EventGroup::get()
  {
    if (_handle == nullptr)
      return 0;
    return xEventGroupGetBits(_handle);
  }

  EventBits_t EventGroup::getFromISR()
  {
    if (_handle == nullptr)
      return 0;
    return xEventGroupGetBitsFromISR(_handle);
  }

  EventBits_t EventGroup::wait()
  {
    if (_handle == nullptr)
      return 0;
    return xEventGroupWaitBits(_handle, 0xFFFFFF, false, false, portMAX_DELAY);
  }

  EventBits_t EventGroup::wait(milliseconds timeout)
  {
    if (_handle == nullptr)
      return 0;
    return xEventGroupWaitBits(_handle, 0xFFFFFF, false, false, CHRONO_TO_TICK(timeout));
  }

  bool EventGroup::waitAllFlags(const EventBits_t flags)
  {
    if (_handle == nullptr)
      return 0;
    EventBits_t bits = xEventGroupWaitBits(_handle, flags, true, true, portMAX_DELAY);
    return (bits & flags) == flags ? true : false;
  }

  bool EventGroup::waitAllFlags(const EventBits_t flags, milliseconds timeout)
  {
    if (_handle == nullptr)
      return 0;
    EventBits_t bits = xEventGroupWaitBits(_handle, flags, true, true, CHRONO_TO_TICK(timeout));
    return (bits & flags) == flags ? true : false;
  }

  bool EventGroup::waitAnyFlags(const EventBits_t flags)
  {
    if (_handle == nullptr)
      return 0;
    EventBits_t bits = xEventGroupWaitBits(_handle, flags, true, false, portMAX_DELAY);
    return (bits & flags) ? true : false;
  }

  bool EventGroup::waitAnyFlags(const EventBits_t flags, milliseconds timeout)
  {
    if (_handle == nullptr)
      return 0;
    EventBits_t bits = xEventGroupWaitBits(_handle, flags, true, false, CHRONO_TO_TICK(timeout));
    return (bits & flags) ? true : false;
  }

} // namespace rtos
