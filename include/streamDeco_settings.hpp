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
#include "lvgl_color.hpp"
#include "lvgl_types.hpp"

namespace streamDeco
{

    /**
     * @namespace settings
     * @brief     Offer some settings configurations
     */
    namespace settings
    {

        /**
         * @struct   settings_s
         * @typedef  settings_t
         * @brief    Typedef of StreamDeco settings
         * @details  Organize StreamDeco settings into one struct
         **/
        typedef struct settings_s
        {
            /**
             * @var      initied
             * @brief    Verify if flash space are iniciated
             * @details  The system will initiate flash space with standart values
             */
            bool initied;

            /**
             * @var      rotation
             * @brief    StreamDeco screen rotation
             * @details  Used to change rotation of StreamDeco screen
             **/
            lvgl::screen::rotation_t rotation;

            /**
             * @var      color_background
             * @brief    StreamDeco background color
             * @details  Used to change color of StreamDeco background
             **/
            lv_color_t color_background;

            /**
             * @var      color_buttons
             * @brief    StreamDeco buttons color
             * @details  Used to change color of StreamDeco buttons
             **/
            lvgl::palette::palette_t color_buttons;

            /**
             * @var      color_background_index
             * @brief    StreamDeco background color index
             * @details  Used to select next color of StreamDeco background
             **/
            uint8_t color_background_index;

            /**
             * @var      color_buttons_index
             * @brief    StreamDeco buttons color index
             * @details  Used to select next color of StreamDeco buttons
             **/
            uint8_t color_buttons_index;

            /**
             * @var lcd_bright
             * @brief    Backlight bright value
             * @details  Used to change StreamDeco backlight bright
             * @details  port configure PWM with 4095 max value and 12-bits resolution */
            int lcd_bright;

        } settings_t;

        enum
        {
            SIZE_CONTENT = LV_SIZE_CONTENT
        };

        /**
         * @brief   Select one background color on colors pool
         * @note    This colors pool are defined in streamDeco_settings.cpp file
         *          on colors_background array, new colors can be add to this pool
         */
        lvgl::color_t nextBackgroundColor(uint8_t &autoColor);

        /**
         * @brief   Select one streamDecoButtons color on colors pool
         * @note    This colors pool are defined in streamDeco_settings.cpp file
         *          on palette_buttons array, and I dont have a clue to make dhis like an color =/
         */
        lvgl::palette::palette_t nextButtonColor(uint8_t &autoColor);

        void initCache();
        void saveCache();

    } // namespace settings

} // namespace streamDeco

#endif
