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

#include "rtos_semaphore_static.hpp"

namespace rtos
{

  SemaphoreStatic::SemaphoreStatic(int count, int initialValue)
  {
    if (_handle != nullptr)
      return;
    if (count > 1)
      _handle = xSemaphoreCreateCountingStatic(count, initialValue, &_stack);
    else
      _handle = xSemaphoreCreateBinaryStatic(&_stack);
  }

  SemaphoreStatic::~SemaphoreStatic()
  {
    if (_handle == nullptr)
      return;
    vSemaphoreDelete(_handle);
    _handle = nullptr;
  }

  bool SemaphoreStatic::give()
  {
    if (_handle == nullptr)
      return false;
    return xSemaphoreGive(_handle);
  }

  BaseType_t SemaphoreStatic::giveFromISR()
  {
    if (_handle == nullptr)
      return false;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t ret = xSemaphoreGiveFromISR(_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    return ret;
  }

  bool SemaphoreStatic::take()
  {
    if (_handle == nullptr)
      return false;
    return xSemaphoreTake(_handle, portMAX_DELAY);
  }

  bool SemaphoreStatic::take(milliseconds timeout)
  {
    if (_handle == nullptr)
      return false;
    return xSemaphoreTake(_handle, chronoToTick(timeout));
  }

} // namespace rtos
