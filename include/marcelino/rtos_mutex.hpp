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

#ifndef _MUTEX_HPP_
#define _MUTEX_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "rtos_chrono.hpp"

namespace rtos {

class Mutex {
public:
    Mutex() {
        _handle = xSemaphoreCreateMutex();
    }

    ~Mutex() {
        vSemaphoreDelete(_handle);
    }

    inline BaseType_t take() {
        return xSemaphoreTake(_handle, portMAX_DELAY);
    }

    inline BaseType_t take(milliseconds timeout) {
        return xSemaphoreTake(_handle, CHRONO_TO_TICK(timeout));
    }

    inline BaseType_t give() {
        return xSemaphoreGive(_handle);
    }

private:
    SemaphoreHandle_t _handle;
};


class MutexRecursive {
public:
    MutexRecursive() {
        _handle = xSemaphoreCreateRecursiveMutex();
    }

    ~MutexRecursive() {
        vSemaphoreDelete(_handle);
    }

    inline BaseType_t take() {
        return xSemaphoreTakeRecursive(_handle, portMAX_DELAY);
    }

    inline BaseType_t take(int timeout) {
        return xSemaphoreTakeRecursive(_handle, pdMS_TO_TICKS(timeout));
    }

    inline BaseType_t give() {
        return xSemaphoreGiveRecursive(_handle);
    }

private:
    SemaphoreHandle_t _handle;
};

} // namespace rtos

#endif
