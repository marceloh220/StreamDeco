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

#ifndef __LVGL_COLOR_HPP__
#define __LVGL_COLOR_HPP__

#include "lv_conf.h"

namespace lvgl
{

    /**
     * @typedef   color_t
     * @brief     Color type for lvgl warp
     * @details   Depending of display the format color can be different
     *            In some ports the display can be:
     *              Monochromatic, display support only one color
     *              RGB332, 8-bit bus color, which result in 258 colors
     *              RGB565, 16-bit bus color, which result in 65,536 colors
     *              RGB888, 24-bit bus color, which tesult in 16,777,216 colors
     *              ARGB8888, 32-bit bus color, an alfa opacity value is sended with RGB888 color
     *
     * @note      Same as lv_color_t
     * @note      The LVGL can simulate opacity in software even if display doesn't support ARGB8888 format
     */
    typedef LV_CONCAT3(lv_color, LV_COLOR_DEPTH, _t) color_t;

    /**
     * @namespace   color
     * @brief       Color tools avaliable
     */
    namespace color
    {

        /**
         * @brief   Convert color to lvgl display mode, which can be
         *          RGB332, RGB565, RGB888 or ARGB8888
         * @param   r Red color component, range from 0 to 255
         * @param   g Green color component, range from 0 to 255
         * @param   b Blue color component, range from 0 to 255
         * @return  Color in lvgl predefined format
         * @note    Passing 0 for each RGB value result in black color,
         *          as the same passing 255 for each RGB value result in white color.
         */
        constexpr color_t make(uint8_t r, uint8_t g, uint8_t b)
        {
            return _LV_COLOR_MAKE_TYPE_HELPER LV_COLOR_MAKE(r, g, b);
        }

        /**
         * @brief   Predefined white color in lvlg display format mode
         * @return  White color in display format
         */
        constexpr color_t white(void)
        {
            return make(0xff, 0xff, 0xff);
        }

        /**
         * @brief   Predefined black color in lvlg display format mode
         * @return  Black color in display format
         */
        constexpr color_t black(void)
        {
            return make(0x00, 0x00, 0x00);
        }

        /**
         * @brief   Predefined red color in lvlg display format mode
         * @return  Red color in display format
         */
        constexpr color_t red(void)
        {
            return make(0xff, 0x00, 0x00);
        }

        /**
         * @brief   Predefined green color in lvlg display format mode
         * @return  Green color in display format
         */
        constexpr color_t green(void)
        {
            return make(0x00, 0xff, 0x00);
        }

        /**
         * @brief   Predefined blue color in lvlg display format mode
         * @return  Blue color in display format
         */
        constexpr color_t blue(void)
        {
            return make(0x00, 0x00, 0xff);
        }

    } // namespace color

    /**
     * @namespace  palette
     * @brief      Avaliable paletton of colors for lvgl
     */
    namespace palette
    {

        /**
         * @typedef palette_t
         * @brief   Paleton color type, same as lv_palette_t
         */

        /**
         * @enum    palette_e
         * @brief   Some paletton of colors with main, lighter and darken color
         */
        typedef enum palette_e
        {
            RED,
            PINK,
            PURPLE,
            DEEP_PURPLE,
            INDIGO,
            BLUE,
            LIGHT_BLUE,
            CYAN,
            TEAL,
            GREEN,
            LIGHT_GREEN,
            LIME,
            YELLOW,
            AMBER,
            ORANGE,
            DEEP_ORANGE,
            BROWN,
            BLUE_GREY,
            GREY,
            LAST,
            NONE = 0xff,
        } palette_t;

        /**
         * @brief   Get the main color of paletto
         * @param   paletton  palette_t type of paletton color
         * @return  Paletton color in display format
         */
        static inline color_t main(palette_t paletton)
        {
            return (color_t)lv_palette_main((lv_palette_t)paletton);
        }

        /**
         * @brief   Get the color of paletton which can be 5 level of lighten
         * @param   paletton  palette_t type of paletton color
         * @param   level     level of lighten color, from 1 to 5, 5 means the more lighter color
         * @return  Paletton lighten color in display format
         */
        static inline color_t lighten(palette_t paletton, uint8_t level)
        {
            return (color_t)lv_palette_lighten((lv_palette_t)paletton, level);
        }

        /**
         * @brief   Get the color of paletton which can be 5 level of darken
         * @param   paletton  palette_t type of paletton color
         * @param   level     level of darken color, from 1 to 5, 5 means the more darker color
         * @return  Paletton darken color in display format
         */
        static inline color_t darken(palette_t paletton, uint8_t level)
        {
            return (color_t)lv_palette_lighten((lv_palette_t)paletton, level);
        }

    } // namespace palette

} // namespace lvgl

#endif
