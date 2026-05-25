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

#include "streamDeco_monitor.hpp"

namespace streamDeco
{

    namespace metric
    {

        namespace
        {

            template <typename InitializeFn>
            void create_object(lvgl::object_t *&object, lvgl::object_t *parent, lvgl::palette::palette_t color, InitializeFn initialize)
            {
                if (object != nullptr) return;
                lvgl::port::mutex_take();
                object = lv_obj_create(parent != nullptr ? parent : lv_scr_act());
                initialize(color);
                lvgl::port::mutex_give();
            }

            template <typename ColorT>
            void set_background_and_invalidate(lvgl::Style &style, lvgl::Object &object, ColorT color)
            {
                style.set_bg_color(color);
                object.invalidate();
            }

            void setup_monitor_style(lvgl::Style &monitor_style, lvgl::palette::palette_t color)
            {
                monitor_style.set_bg_color(lvgl::color::make(41, 45, 50));
                monitor_style.set_bg_opa(lvgl::opacity::OPA_100);
                monitor_style.set_border_width(1);
                monitor_style.set_border_color(lvgl::palette::darken(color, 0));
                monitor_style.set_pad_all(5);
            }

            void setup_metric_style(lvgl::Style &metric_style, lvgl::palette::palette_t color)
            {
                metric_style.set_bg_color(color);
                metric_style.set_img_recolor(color);
                metric_style.set_img_recolor_opa(lvgl::opacity::OPA_100);
                metric_style.set_text_color(color);
                metric_style.set_arc_color(color);
            }

            void setup_warning_style(lvgl::Style &warning_style)
            {
                warning_style.set_bg_color(lvgl::color::make(160, 0, 0));
                warning_style.set_img_recolor(lvgl::color::make(160, 0, 0));
                warning_style.set_img_recolor_opa(lvgl::opacity::OPA_100);
                warning_style.set_text_color(lvgl::color::make(160, 0, 0));
                warning_style.set_arc_color(lvgl::color::make(160, 0, 0));
                warning_style.set_line_color(lvgl::color::make(160, 0, 0));
            }

            void setup_metric_indicator_style(lvgl::Style &metric_indicator_style, lvgl::palette::palette_t color)
            {
                metric_indicator_style.set_bg_color(lvgl::palette::lighten(color, 3));
                metric_indicator_style.set_arc_color(lvgl::palette::lighten(color, 3));
            }

        } // namespace

        const char* week_name_pt[] = {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"};

        #define WEEK_NAME week_name_pt

        void Complete::create(lvgl::palette::palette_t color)
        {
            create_object(object, nullptr, color, [this](lvgl::palette::palette_t current_color) {
                init_conf(current_color);
            });
        } // void Complete::create

        void Complete::create(lvgl::object_t *parent, lvgl::palette::palette_t color)
        {
            create_object(object, parent, color, [this](lvgl::palette::palette_t current_color) {
                init_conf(current_color);
            });
        } // void Complete::create

        void Complete::create(Object &parent, lvgl::palette::palette_t color)
        {
            create_object(object, parent.get_object(), color, [this](lvgl::palette::palette_t current_color) {
                init_conf(current_color);
            });
        } // Complete::create

        void Complete::set_bg_color(lvgl::color_t color)
        {
            with_lock([&]() {
                set_background_and_invalidate(monitor_style, *this, color);
            });
        } // Complete::set_bg_color

        void Complete::set_bg_color(lvgl::palette::palette_t color)
        {
            with_lock([&]() {
                set_background_and_invalidate(monitor_style, *this, color);
            });
        } // Complete::set_bg_color

        void Complete::color(lvgl::palette::palette_t color)
        {
            with_lock([&]() {
                setup_metric_indicator_style(metric_indicator_style, color);
                setup_metric_style(metric_style, color);
                invalidate();
            });
        } // Complete::color

        void Complete::arc_set_value(int16_t value)
        {
            arc.set_value(value);
            arc_label.set_text_fmt("%d%%", value);
        } // Complete::arc_set_value

        void Complete::bar1_set_range(int32_t min, int32_t max)
        {
            bar1.set_range(min, max);
        } // Complete::bar1_set_range

        void Complete::bar2_set_range(int32_t min, int32_t max)
        {
            bar2.set_range(min, max);
        } // Complete::bar2_set_range

        void Complete::bar1_set_value(int32_t value, const char *prefix, const char *sufix)
        {
            bar1.set_value(value, lvgl::animation::OFF);
            bar1_label.set_text_fmt("%s %d %s", prefix, value, sufix);
        } // Complete::bar1_set_value

        void Complete::bar2_set_value(int32_t value, const char *prefix, const char *sufix)
        {
            bar2.set_value(value, lvgl::animation::OFF);
            bar2_label.set_text_fmt("%s %d %s", prefix, value, sufix);
        } // Complete::bar2_set_value

        void Complete::init_conf(lvgl::palette::palette_t color)
        {
            setup_monitor_style(monitor_style, color);
            setup_metric_indicator_style(metric_indicator_style, color);
            setup_metric_style(metric_style, color);
            setup_warning_style(warning_style);

            auto font = lvgl::font::montserrat_22;

            monitor_label.create(*this);
            monitor_label.set_style_text_font(font);
            monitor_label.set_pos(0, 0);
            monitor_label.add_style(metric_style, lvgl::part::MAIN);
            monitor_label.set_text(text_scr);

            if (icon_scr != nullptr)
            {
                monitor_icon.create(*this);
                monitor_icon.set_pos(0, 0);
                monitor_icon.add_style(metric_style, lvgl::part::MAIN);
                monitor_icon.set_src(icon_scr);
                monitor_label.align_to(monitor_icon, lvgl::alignment::OUT_RIGHT_MID, 10, 0);
            }

            arc.create(*this);
            arc.align_to(monitor_label, lvgl::alignment::OUT_BOTTOM_RIGHT, 50, 10);
            arc.set_rotation(180);
            arc.set_bg_angles(0, 360);
            arc.set_range(0, 100);
            arc.set_size(96, 96);
            arc.add_style(metric_style, lvgl::part::MAIN);
            arc.add_style(metric_indicator_style, lvgl::part::INDICATOR);
            arc.remove_style(nullptr, lvgl::part::KNOB);
            arc.clear_flag(LV_OBJ_FLAG_CLICKABLE);

            arc_label.create(arc);
            arc_label.center();
            arc_label.set_style_text_font(font);
            arc_label.add_style(metric_style, lvgl::part::MAIN);

            bar1.create(*this);
            bar1.align_to(arc, lvgl::alignment::OUT_RIGHT_MID, 10, -40);
            bar1.set_height(20);
            bar1.set_width(130);
            bar1.add_style(metric_style, lvgl::part::MAIN);
            bar1.add_style(metric_indicator_style, lvgl::part::INDICATOR);

            bar1_label.create(*this);
            bar1_label.align_to(bar1, lvgl::alignment::BOTTOM_LEFT, 0, 25);
            bar1_label.set_style_text_font(font);
            bar1_label.add_style(metric_style, lvgl::part::MAIN);

            bar2.create(*this);
            bar2.align_to(bar1_label, lvgl::alignment::BOTTOM_LEFT, 0, 25);
            bar2.set_height(20);
            bar2.set_width(130);
            bar2.add_style(metric_style, lvgl::part::MAIN);
            bar2.add_style(metric_indicator_style, lvgl::part::INDICATOR);

            bar2_label.create(*this);
            bar2_label.align_to(bar2, lvgl::alignment::BOTTOM_LEFT, 0, 25);
            bar2_label.set_style_text_font(font);
            bar2_label.add_style(metric_style, lvgl::part::MAIN);

            arc.set_value(0);
            arc_label.set_text("NA");
            bar1.set_value(0, lvgl::animation::OFF);
            bar1_label.set_text("NA °C");
            bar2.set_value(0, lvgl::animation::OFF);
            bar2_label.set_text("NA MHz");

            set_size(250, 200);
            add_style(monitor_style, lvgl::part::MAIN);
        } // Complete::init_conf

        void Basic::create(lvgl::object_t *parent, lvgl::palette::palette_t color)
        {
            create_object(object, parent, color, [this](lvgl::palette::palette_t current_color) {
                init_conf(current_color);
            });
        } // Basic::create

        void Basic::create(Object &parent, lvgl::palette::palette_t color)
        {
            create_object(object, parent.get_object(), color, [this](lvgl::palette::palette_t current_color) {
                init_conf(current_color);
            });
        } // Basic::create

        void Basic::set_bg_color(lvgl::color_t color)
        {
            with_lock([&]() {
                set_background_and_invalidate(monitor_style, *this, color);
            });
        } // Basic::set_bg_color

        void Basic::set_bg_color(lvgl::palette::palette_t color)
        {
            with_lock([&]() {
                set_background_and_invalidate(monitor_style, *this, color);
            });
        } // Basic::set_bg_color

        void Basic::color(lvgl::palette::palette_t color)
        {
            with_lock([&]() {
                setup_metric_indicator_style(metric_indicator_style, color);
                setup_metric_style(metric_style, color);
                invalidate();
            });
        } // Basic::color

        void Basic::bar1_set_range(int32_t min, int32_t max)
        {
            bar1.set_range(min, max);
        } // Basic::bar1_set_range

        void Basic::bar2_set_range(int32_t min, int32_t max)
        {
            bar2.set_range(min, max);
        } //  Basic::bar2_set_range

        void Basic::bar1_set_value(int32_t value, const char *prefix, const char *sufix)
        {
            bar1.set_value(value, lvgl::animation::OFF);
            bar1_label.set_text_fmt("%s%d%s", prefix, value, sufix);
        } //  Basic::bar1_set_value

        void Basic::bar2_set_value(int32_t value, int32_t value2, const char *prefix, const char *sufix)
        {
            bar2.set_value(value, lvgl::animation::OFF);
            bar2_label.set_text_fmt("%s%d/%d%s", prefix, value, value2, sufix);
        } //  Basic::bar2_set_value

        void Basic::init_conf(lvgl::palette::palette_t color)
        {
            setup_monitor_style(monitor_style, color);
            setup_metric_indicator_style(metric_indicator_style, color);
            setup_metric_style(metric_style, color);
            setup_warning_style(warning_style);

            auto font = lvgl::font::montserrat_22;

            monitor_label.create(*this);
            monitor_label.set_style_text_font(font);
            monitor_label.set_pos(0, 0);
            monitor_label.add_style(metric_style, lvgl::part::MAIN);
            monitor_label.set_text(text_scr);

            if (icon_scr != nullptr)
            {
                monitor_icon.create(*this);
                monitor_icon.set_pos(0, 0);
                monitor_icon.add_style(metric_style, lvgl::part::MAIN);
                monitor_icon.set_src(icon_scr);
                monitor_label.align_to(monitor_icon, lvgl::alignment::OUT_RIGHT_MID, 10, 0);
            }

            bar1.create(*this);
            bar1.set_pos(15, 22 + 12);
            bar1.set_height(20);
            bar1.set_width(180);
            bar1.add_style(metric_style, lvgl::part::MAIN);
            bar1.add_style(metric_indicator_style, lvgl::part::INDICATOR);

            bar1_label.create(*this);
            bar1_label.align_to(bar1, lvgl::alignment::BOTTOM_LEFT, 0, 25);
            bar1_label.set_style_text_font(font);
            bar1_label.add_style(metric_style, lvgl::part::MAIN);

            bar2.create(*this);
            bar2.align_to(bar1_label, lvgl::alignment::BOTTOM_LEFT, 0, 25);
            bar2.set_height(20);
            bar2.set_width(180);
            bar2.add_style(metric_style, lvgl::part::MAIN);
            bar2.add_style(metric_indicator_style, lvgl::part::INDICATOR);

            bar2_label.create(*this);
            bar2_label.align_to(bar2, lvgl::alignment::BOTTOM_LEFT, 0, 25);
            bar2_label.set_style_text_font(font);
            bar2_label.add_style(metric_style, lvgl::part::MAIN);

            bar1.set_value(0, lvgl::animation::OFF);
            bar1_label.set_text("RAM: NA");

            bar2.set_value(0, lvgl::animation::OFF);
            bar2_label.set_text("C: NA");

            set_size(250, 200);
            add_style(monitor_style, lvgl::part::MAIN);
        }  // Basic::init_conf

        void Clock::create(lvgl::object_t *parent, lvgl::palette::palette_t color)
        {
            create_object(object, parent, color, [this](lvgl::palette::palette_t current_color) {
                init_conf(current_color);
            });
        } // Clock::create

        void Clock::create(Object &parent, lvgl::palette::palette_t color)
        {
            create_object(object, parent.get_object(), color, [this](lvgl::palette::palette_t current_color) {
                init_conf(current_color);
            });
        } //  Clock::create

        void Clock::set_bg_color(lvgl::color_t color)
        {
            with_lock([&]() {
                set_background_and_invalidate(monitor_style, *this, color);
            });
        } // Clock::set_bg_color

        void Clock::set_bg_color(lvgl::palette::palette_t color)
        {
            with_lock([&]() {
                set_background_and_invalidate(monitor_style, *this, color);
            });
        } // Clock::set_bg_color

        void Clock::color(lvgl::palette::palette_t color)
        {
            with_lock([&]() {
                metric_style.set_bg_color(color);
                metric_style.set_img_recolor(color);
                metric_style.set_text_color(color);
                metric_style.set_arc_color(color);
                weekActual_style.set_text_color(color);
                invalidate();
            });
        } // Clock::color

        void Clock::set_time(struct tm &rtc_time)
        {

            constexpr size_t kDateBufferSize = 11; // dd/mm/yyyy + null terminator
            constexpr size_t kTimeBufferSize = 9;  // hh:mm:ss + null terminator
            char buffer[kDateBufferSize];

            strftime(buffer, kDateBufferSize, "%d/%m/%Y", &rtc_time);
            date.set_text(buffer);

            strftime(buffer, kTimeBufferSize, "%H:%M:%S", &rtc_time);
            hour.set_text(buffer);

            if(wday != rtc_time.tm_wday) {
                week[wday].remove_style(weekActual_style, lvgl::part::MAIN);
                week[wday].add_style(week_style, lvgl::part::MAIN);
                wday = rtc_time.tm_wday;
                week[wday].remove_style(week_style, lvgl::part::MAIN);
                week[wday].add_style(weekActual_style, lvgl::part::MAIN);
            }

        } // Clock::set_time

        void Clock::init_conf(lvgl::palette::palette_t color)
        {
            setup_monitor_style(monitor_style, color);
            setup_metric_style(metric_style, color);

            auto font = lvgl::font::montserrat_22;

            monitor_label.create(*this);
            monitor_label.set_style_text_font(font);
            monitor_label.set_pos(0, 0);
            monitor_label.add_style(metric_style, lvgl::part::MAIN);
            monitor_label.set_text(text_scr);

            if (icon_scr != nullptr)
            {
                monitor_icon.create(*this);
                monitor_icon.set_pos(0, 0);
                monitor_icon.add_style(metric_style, lvgl::part::MAIN);
                monitor_icon.set_src(icon_scr);
                monitor_label.align_to(monitor_icon, lvgl::alignment::OUT_RIGHT_MID, 10, 0);
            }

            hour.create(*this);
            hour.align(lvgl::alignment::CENTER, -22, -18);
            hour.set_style_text_font(lvgl::font::montserrat_40);
            hour.add_style(metric_style, lvgl::part::MAIN);
            hour.set_text("NO SYNC");

            date.create(*this);
            date.align(lvgl::alignment::CENTER, -22, 90 - 22 - 18 - 18 );
            date.set_style_text_font(font);
            date.add_style(metric_style, lvgl::part::MAIN);
            date.set_text("NO SYNC");

            font = lvgl::font::montserrat_12;
            week_style.set_text_color(lvgl::palette::darken(color, 0));
            week_style.set_text_font(font);

            font = lvgl::font::montserrat_16;
            weekActual_style.set_text_color(color);
            weekActual_style.set_text_font(font);

            int index = 0;
            for(auto &_week : this->week) {
                _week.create(*this);
                _week.align(lvgl::alignment::RIGHT_MID, -5, -54 + font->line_height * index);
                _week.set_text(WEEK_NAME[index]);
                _week.add_style(week_style, lvgl::part::MAIN);
                index++;
            }

            week[0].remove_style(week_style, lvgl::part::MAIN);
            week[0].add_style(weekActual_style, lvgl::part::MAIN);

            add_style(monitor_style, lvgl::part::MAIN);
        } // Clock::init_conf

    } // namespace StreamDecoMonitor

} // namespace streamDeco
