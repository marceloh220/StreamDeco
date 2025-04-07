/**
 * Copyright © 2024 Marcelo H Moraes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _STREAMDECO_MONITOR_HPP_
#define _STREAMDECO_MONITOR_HPP_

#include "marcelino.hpp"
#include "lvgl.hpp"
#include "time.h"

namespace streamDeco
{

    namespace metric
    {

        class Complete : public lvgl::Object
        {
        public:
            Complete(const char *text, lvgl::icon_t icon = NULL) : text_scr(text), icon_scr(icon) {}
            void create(lvgl::object_t *parent, lvgl::palette::palette_t color);
            void create(Object &parent, lvgl::palette::palette_t color);
            void set_bg_color(lv_color_t color);
            void set_bg_color(lvgl::palette::palette_t color);
            void color(lvgl::palette::palette_t color);
            void arc_set_value(int16_t value);
            void bar1_set_range(int32_t min, int32_t max);
            void bar2_set_range(int32_t min, int32_t max);
            void bar1_set_value(int32_t value, const char *prefix, const char *sufix = "");
            void bar2_set_value(int32_t value, const char *prefix, const char *sufix = "");
        protected:
            virtual void init_conf(lvgl::palette::palette_t color);
            const char *text_scr;
            lvgl::icon_t icon_scr;
            lvgl::Style monitor_style;
            lvgl::Style metric_style;
            lvgl::Style metric_indicator_style;
            lvgl::Style warning_style;
            lvgl::Label monitor_label;
            lvgl::Image monitor_icon;
            lvgl::Arc arc;
            lvgl::Label arc_label;
            lvgl::Bar bar1;
            lvgl::Label bar1_label;
            lvgl::Bar bar2;
            lvgl::Label bar2_label;
        }; // class Complete

        class Basic : public lvgl::Object
        {
        public:
            Basic(const char *text, lvgl::icon_t icon = NULL) : text_scr(text), icon_scr(icon) {}
            void create(lvgl::object_t *parent, lvgl::palette::palette_t color);
            void create(Object &parent, lvgl::palette::palette_t color);
            void set_bg_color(lv_color_t color);
            void set_bg_color(lvgl::palette::palette_t color);
            void color(lvgl::palette::palette_t color);
            void bar1_set_range(int32_t min, int32_t max);
            void bar2_set_range(int32_t min, int32_t max);
            void bar1_set_value(int32_t value, const char *prefix, const char *sufix = "");
            void bar2_set_value(int32_t value, int32_t value2, const char *prefix, const char *sufix = "");
        private:
            void init_conf(lvgl::palette::palette_t color);
            const char *text_scr;
            lvgl::icon_t icon_scr;
            lvgl::Style monitor_style;
            lvgl::Style metric_style;
            lvgl::Style metric_indicator_style;
            lvgl::Style warning_style;
            lvgl::Label monitor_label;
            lvgl::Image monitor_icon;
            lvgl::Bar bar1;
            lvgl::Label bar1_label;
            lvgl::Bar bar2;
            lvgl::Label bar2_label;
        }; // class Basic

        class Clock : public lvgl::Object
        {
        public:
            Clock(const char *text, lvgl::icon_t icon = NULL) : text_scr(text), icon_scr(icon) {}
            void create(lvgl::object_t *parent, lvgl::palette::palette_t color);
            void create(Object &parent, lvgl::palette::palette_t color);
            void set_bg_color(lv_color_t color);
            void set_bg_color(lvgl::palette::palette_t color);
            void color(lvgl::palette::palette_t color);
            void set_time(struct tm &value);
        private:
            void init_conf(lvgl::palette::palette_t color);
            const char *text_scr;
            lvgl::icon_t icon_scr;
            lvgl::Label monitor_label;
            lvgl::Image monitor_icon;
            lvgl::Label date;
            lvgl::Label hour;
            lvgl::Style monitor_style;
            lvgl::Style metric_style;
        }; // class Clock

    } // namespace StreamDecoMonitor

} // namespace streamDeco

#endif
