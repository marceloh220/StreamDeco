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

#include "settings.hpp"

namespace lvgl
{

    namespace settings
    {

        const lv_color_t colors_background[] = {
            LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_COLOR_MAKE(0xF4, 0x43, 0x36), LV_COLOR_MAKE(0xE9, 0x1E, 0x63), 
            LV_COLOR_MAKE(0x9C, 0x27, 0xB0), LV_COLOR_MAKE(0x67, 0x3A, 0xB7), LV_COLOR_MAKE(0x3F, 0x51, 0xB5), LV_COLOR_MAKE(0x21, 0x96, 0xF3), 
            LV_COLOR_MAKE(0x03, 0xA9, 0xF4), LV_COLOR_MAKE(0x00, 0xBC, 0xD4), LV_COLOR_MAKE(0x00, 0x96, 0x88), LV_COLOR_MAKE(0x4C, 0xAF, 0x50), 
            LV_COLOR_MAKE(0x8B, 0xC3, 0x4A), LV_COLOR_MAKE(0xCD, 0xDC, 0x39), LV_COLOR_MAKE(0xFF, 0xEB, 0x3B), LV_COLOR_MAKE(0xFF, 0xC1, 0x07), 
            LV_COLOR_MAKE(0xFF, 0x98, 0x00), LV_COLOR_MAKE(0xFF, 0x57, 0x22), LV_COLOR_MAKE(0x79, 0x55, 0x48), LV_COLOR_MAKE(0x60, 0x7D, 0x8B), 
            LV_COLOR_MAKE(0x9E, 0x9E, 0x9E)};

        const lv_palette_t palette_button[] = {
            LV_PALETTE_RED,
            LV_PALETTE_PINK,
            LV_PALETTE_PURPLE,
            LV_PALETTE_DEEP_PURPLE,
            LV_PALETTE_INDIGO,
            LV_PALETTE_BLUE,
            LV_PALETTE_LIGHT_BLUE,
            LV_PALETTE_CYAN,
            LV_PALETTE_TEAL,
            LV_PALETTE_GREEN,
            LV_PALETTE_LIGHT_GREEN,
            LV_PALETTE_LIME,
            LV_PALETTE_YELLOW,
            LV_PALETTE_AMBER,
            LV_PALETTE_ORANGE,
            LV_PALETTE_DEEP_ORANGE,
            LV_PALETTE_BROWN,
            LV_PALETTE_BLUE_GREY,
            LV_PALETTE_GREY,

            _LV_PALETTE_LAST,
        };

        lv_color_t backgroundColor()
        {
            static uint8_t autoColor = 0;
            int last_element = sizeof(colors_background) / sizeof(lv_color_t);
            autoColor++;
            autoColor = autoColor % last_element;
            return colors_background[autoColor];
        }

        lv_palette_t buttonColor()
        {
            static uint8_t autoColor = 3;
            autoColor++;
            autoColor = autoColor % (_LV_PALETTE_LAST - 1);
            return palette_button[autoColor];
        }

    } // namespace settings

} // namespace lvgl
