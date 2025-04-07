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

#ifndef _LVGL_SCREEN_HPP_
#define _LVGL_SCREEN_HPP_

#include "lvgl_types.hpp"
#include "lvgl_color.hpp"
#include "lvgl_object.hpp"

namespace lvgl
{

    /**
     * @namespace  screen
     * @brief      Screen resources avaliable for this port framework
     */
    namespace screen
    {

        /**
         * @brief   Set screen rotation
         * @param   rotation rotation_t parameter
         *          screen::LANDSCAPE to landscape mode
         *          screen::PORTRAIT  to portrait mode
         *          screen::MIRROR_LANDSCAPE to mirror landscape mode
         *          screen::MIRROR_PORTRAIT to mirror portrait mode
         */
        void set_rotation(screen::rotation_t rotation);

        /**
         * @brief   Get screen rotation
         * @return
         */
        screen::rotation_t get_rotation();

        /**
         * @brief   Set the background color
         * @param   color Color of background in display format
         */
        void set_bg_color(color_t color);

        /**
         * @brief   Set the background color
         * @param   color Color of background on lvgl paletton
         */
        void set_bg_color(palette::palette_t color);

        /**
         * @brief  Align the screen to the center of display
         */
        void center();

        /**
         * @brief Mark all screen to be redraw
         */
        void refresh();

    } // namespace screen

} // namespace lvgl

#endif