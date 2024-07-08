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

        const lv_palette_t palette_bg[] = {
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

        lv_palette_t bgColor()
        {
            static uint8_t autoColor = 3;
            autoColor++;
            autoColor = autoColor % (_LV_PALETTE_LAST - 1);
            return palette_bg[autoColor];
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
