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

#ifndef _LVGL_PORT_HPP_
#define _LVGL_PORT_HPP_

#include <lvgl.h>

namespace lvgl
{

    namespace port
    {

        /**
         * @brief  Init the LVGL port for this platform
         */
        void init();

        uint32_t backlight_max();

        /**
         * @brief    set backlight bright
         * @param    bright  Backlight bright in float percent unity value
         * @details  The bright value is percent unity based
         *           0.0 means 0%   of bright
         *           0.5 means 50%  of bright
         *           1.0 menas 100% of bright
         */
        void backlight_set(float bright);

        /**
         * @brief    set backlight bright
         * @param    bright Backlight bright in raw value
         * @details  PWM bright control is configured to 12-bits resolution
         *           The miminum value of bright is 0 ans max are 4095
         */
        void backlight_setRaw(int bright);

        /**
         * @brief    take the LVGL mutex
         * @details  this mutex prevents display alterations while
         *           internal timer are processed
         * @note     Affter take, the mutex must be released as soon as possible
         */
        void mutex_take();

        /**
         * @brief    release LVGL mutex
         * @details  Must be called affter the mutex are taked to release
         *           LVGL interface to do other process
         */
        void mutex_give();

        /**
         * @brief   Send LVGL used RAM memory through Serial interface
         */
        void print_task_memory_usage();

        /**
         * @brief   Set screen rotations
         * @param   rotation An lv_disp_rot_t type
         *    LV_DISP_ROT_0   for 0 degree rotation, generaly landscape screen
         *    LV_DISP_ROT_90  for 90 degrees rotation, generaly portrait screen
         *    LV_DISP_ROT_180 for 180 degrees rotation, generaly mirror landscape screen
         *    LV_DISP_ROT_270 for 270 degrees rotation, generally mirror portrait screen
         *
         */
        void set_screen_rotation(lv_disp_rot_t rotation);
        lv_disp_rot_t get_screen_rotation();

    } // namespace port

} // namespace lvgl

#endif
