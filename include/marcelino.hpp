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

#include "marcelino/button.hpp"
#include "marcelino/chrono.hpp"
#include "marcelino/eventGroup.hpp"
#include "marcelino/gpio.hpp"
#include "marcelino/input.hpp"
#include "marcelino/interrupt.hpp"
#include "marcelino/mutex.hpp"
#include "marcelino/output.hpp"
#include "marcelino/queue.hpp"
#include "marcelino/semaphore.hpp"
#include "marcelino/task.hpp"
#include "marcelino/timer.hpp"

#include "lvgl/lvgl_arc.hpp"
#include "lvgl/lvgl_bar.hpp"
#include "lvgl/lvgl_button.hpp"
#include "lvgl/lvgl_event.hpp"
#include "lvgl/lvgl_image.hpp"
#include "lvgl/lvgl_label.hpp"
#include "lvgl/lvgl_layer.hpp"
#include "lvgl/lvgl_port.hpp"
#include "lvgl/lvgl_screen.hpp"
#include "lvgl/lvgl_slider.hpp"
#include "lvgl/lvgl_style.hpp"

#define byte_k(_b) (_b*1024)

#endif
