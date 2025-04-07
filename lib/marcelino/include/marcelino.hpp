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

#ifndef _MARCELINO_HPP_
#define _MARCELINO_HPP_

#include <Arduino.h>

#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"

#include "rtos_chrono.hpp"
#include "hardware_esp.hpp"

#include "hardware_button.hpp"
#include "hardware_gpio.hpp"
#include "hardware_input.hpp"
#include "hardware_interrupt.hpp"
#include "hardware_output.hpp"

#include "rtos_eventGroup.hpp"
#include "rtos_eventGroup_static.hpp"
#include "rtos_mutex.hpp"
#include "rtos_mutex_static.hpp"
#include "rtos_queue.hpp"
#include "rtos_queue_static.hpp"
#include "rtos_semaphore.hpp"
#include "rtos_semaphore_static.hpp"
#include "rtos_task.hpp"
#include "rtos_task_static.hpp"
#include "rtos_timer.hpp"
#include "rtos_timer_static.hpp"

#include "hardware_file.hpp"

constexpr unsigned long long operator"" _kHz(unsigned long long x)
{
  return x * 1000;
}

constexpr unsigned long long operator"" _MHz(unsigned long long x)
{
  return x * 1000 * 1000;
}

constexpr unsigned long long operator"" _GHz(unsigned long long x)
{
  return x * 1000 * 1000 * 1000;
}

constexpr unsigned long long operator"" _kBit(unsigned long long x)
{
  return x * 1024;
}

constexpr unsigned long long operator"" _MBit(unsigned long long x)
{
  return x * 1024 * 1024;
}

constexpr unsigned long long operator"" _GBit(unsigned long long x)
{
  return x * 1024 * 1024 * 1024;
}

constexpr unsigned long long operator"" _kB(unsigned long long x)
{
  return x * 1024;
}

constexpr unsigned long long operator"" _MB(unsigned long long x)
{
  return x * 1024 * 1024;
}

constexpr unsigned long long operator"" _GB(unsigned long long x)
{
  return x * 1024 * 1024 * 1024;
}

#endif
