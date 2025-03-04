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

#include "streamDeco_settings.hpp"

namespace streamDeco
{

    namespace settings
    {

        const lvgl::color_t colors_background[] = {
            LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_COLOR_MAKE(0xF4, 0x43, 0x36), LV_COLOR_MAKE(0xE9, 0x1E, 0x63), 
            LV_COLOR_MAKE(0x9C, 0x27, 0xB0), LV_COLOR_MAKE(0x67, 0x3A, 0xB7), LV_COLOR_MAKE(0x3F, 0x51, 0xB5), LV_COLOR_MAKE(0x21, 0x96, 0xF3), 
            LV_COLOR_MAKE(0x03, 0xA9, 0xF4), LV_COLOR_MAKE(0x00, 0xBC, 0xD4), LV_COLOR_MAKE(0x00, 0x96, 0x88), LV_COLOR_MAKE(0x4C, 0xAF, 0x50), 
            LV_COLOR_MAKE(0x8B, 0xC3, 0x4A), LV_COLOR_MAKE(0xCD, 0xDC, 0x39), LV_COLOR_MAKE(0xFF, 0xEB, 0x3B), LV_COLOR_MAKE(0xFF, 0xC1, 0x07), 
            LV_COLOR_MAKE(0xFF, 0x98, 0x00), LV_COLOR_MAKE(0xFF, 0x57, 0x22), LV_COLOR_MAKE(0x79, 0x55, 0x48), LV_COLOR_MAKE(0x60, 0x7D, 0x8B), 
            LV_COLOR_MAKE(0x9E, 0x9E, 0x9E)};

        const lvgl::palette::palette_t palette_button[] = {
            lvgl::palette::RED,
            lvgl::palette::PINK,
            lvgl::palette::PURPLE,
            lvgl::palette::DEEP_PURPLE,
            lvgl::palette::INDIGO,
            lvgl::palette::BLUE,
            lvgl::palette::LIGHT_BLUE,
            lvgl::palette::CYAN,
            lvgl::palette::TEAL,
            lvgl::palette::GREEN,
            lvgl::palette::LIGHT_GREEN,
            lvgl::palette::LIME,
            lvgl::palette::YELLOW,
            lvgl::palette::AMBER,
            lvgl::palette::ORANGE,
            lvgl::palette::DEEP_ORANGE,
            lvgl::palette::BROWN,
            lvgl::palette::BLUE_GREY,
            lvgl::palette::GREY,
            lvgl::palette::LAST,
        };

        lvgl::color_t backgroundColor()
        {
            static uint8_t autoColor = 0;
            int last_element = sizeof(colors_background) / sizeof(lv_color_t);
            autoColor++;
            autoColor = autoColor % last_element;
            return colors_background[autoColor];
        }

        lvgl::palette::palette_t buttonColor()
        {
            static uint8_t autoColor = 3;
            autoColor++;
            autoColor = autoColor % (lvgl::palette::LAST - 1);
            return palette_button[autoColor];
        }

    } // namespace settings

} // namespace lvgl
