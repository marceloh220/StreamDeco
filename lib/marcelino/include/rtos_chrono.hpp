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

#ifndef _CHRONO_HPP_
#define _CHRONO_HPP_

#include <chrono>

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "time.h"

/** User literals for time
 * us to microseconds
 * ms to milliseconds
 * s to seconds
 * min to minutes
 * h to hour
 */
using namespace std::literals::chrono_literals;

using std::chrono::hours;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::minutes;
using std::chrono::seconds;

namespace rtos
{

  constexpr uint64_t chronoToTick(milliseconds timeout)
  {
    return pdMS_TO_TICKS(timeout.count());
  }

  template <typename t>
  [[nodiscard]] int64_t duration_cast(milliseconds time)
  {
    return std::chrono::duration_cast<t>(time).count();
  }

  template <typename t>
  [[nodiscard]] int64_t duration_cast(seconds time)
  {
    return std::chrono::duration_cast<t>(time).count();
  }

  template <typename t>
  [[nodiscard]] int64_t duration_cast(minutes time)
  {
    return std::chrono::duration_cast<t>(time).count();
  }

  template <typename t>
  [[nodiscard]] int64_t duration_cast(hours time)
  {
    return std::chrono::duration_cast<t>(time).count();
  }

  inline void sleep(milliseconds time) { vTaskDelay(chronoToTick(time)); }
  inline void sleep(uint32_t time) { vTaskDelay(pdMS_TO_TICKS(time)); }

  template <typename t>
  [[nodiscard]] inline t time()
  {
    microseconds micros(esp_timer_get_time());
    return std::chrono::duration_cast<t>(micros);
  }

} // namespace rtos

#endif
