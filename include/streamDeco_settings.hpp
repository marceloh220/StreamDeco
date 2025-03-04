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

#ifndef _STREAMDECO_SETTTING_HPP_
#define _STREAMDECO_SETTTING_HPP_

#include <lvgl.h>
#include "lvgl/lvgl_color.hpp"

namespace streamDeco {

/**
 * @namespace settings
 * @brief     Offer some settings configurations
 */
namespace settings {

enum { SIZE_CONTENT = LV_SIZE_CONTENT };

/**
 * @brief   Select one background color on colors pool
 * @note    This colors pool are defined in streamDeco_settings.cpp file
 *          on colors_background array, new colors can be add to this pool
 */
lvgl::color_t backgroundColor();

/**
 * @brief   Select one button color on colors pool
 * @note    This colors pool are defined in streamDeco_settings.cpp file
 *          on palette_buttons array, and I dont have a clue to make dhis like an color =/
 */
lvgl::palette::palette_t buttonColor();

} // namespace settings

} // namespace streamDeco

#endif
