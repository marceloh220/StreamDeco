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

#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"

#include "task.hpp"

#include "gpio.hpp"
#include "output.hpp"
#include "input.hpp"
#include "interrupt.hpp"

#include "mutex.hpp"
#include "semaphore.hpp"
#include "eventGroup.hpp"
#include "queue.hpp"
#include "timer.hpp"

#include "chrono.hpp"

#include "icons.h"

#include "lvgl_port.hpp"
#include "lvgl_screen.hpp"
#include "lvgl_label.hpp"
#include "lvgl_image.hpp"
#include "lvgl_slider.hpp"
#include "lvgl_layer.hpp"
#include "lvgl_style.hpp"

#define byte_k(_b) (_b*1024)

#endif
