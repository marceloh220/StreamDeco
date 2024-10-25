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

        void Complete::create(lv_obj_t *parent)
        {
            if (object != NULL) return;
            lvgl::port::mutex_take();
            if (parent == NULL) parent = lv_scr_act();
            object = lv_obj_create(parent);
            init_conf();
            lvgl::port::mutex_give();
        } // void Complete::create

        void Complete::create(Object &parent)
        {
            if (object != NULL) return;
            lvgl::port::mutex_take();
            object = lv_obj_create(parent.get_object());
            init_conf();
            lvgl::port::mutex_give();
        } // Complete::create

        void Complete::set_bg_color(lv_color_t color)
        {
            if (object == NULL) return;
            lvgl::port::mutex_take();
            monitor_style.set_bg_color(color);
            invalidate();
            lvgl::port::mutex_give();
        } // Complete::set_bg_color

        void Complete::set_bg_color(lv_palette_t color)
        {
            if (object == NULL) return;
            lvgl::port::mutex_take();
            monitor_style.set_bg_color(color);
            invalidate();
            lvgl::port::mutex_give();
        } // Complete::set_bg_color

        void Complete::color(lv_palette_t color)
        {
            if (object == NULL) return;
            lvgl::port::mutex_take();
            metric_indicator_style.set_bg_color(lv_palette_lighten(color, 3));
            metric_indicator_style.set_arc_color(lv_palette_lighten(color, 3));

            metric_style.set_bg_color(color);
            metric_style.set_img_recolor(color);
            metric_style.set_text_color(color);
            metric_style.set_arc_color(color);
            invalidate();
            lvgl::port::mutex_give();
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
            bar1.set_value(value, LV_ANIM_OFF);
            bar1_label.set_text_fmt("%s %d %s", prefix, value, sufix);
        } // Complete::bar1_set_value

        void Complete::bar2_set_value(int32_t value, const char *prefix, const char *sufix)
        {
            bar2.set_value(value, LV_ANIM_OFF);
            bar2_label.set_text_fmt("%s %d %s", prefix, value, sufix);
        } // Complete::bar2_set_value

        void Complete::init_conf()
        {
            monitor_style.set_bg_color(lv_color_make(41, 45, 50));
            monitor_style.set_bg_opa(LV_OPA_100);
            monitor_style.set_pad_all(5);

            metric_indicator_style.set_bg_color(lv_palette_lighten(LV_PALETTE_PURPLE, 3));
            metric_indicator_style.set_arc_color(lv_palette_lighten(LV_PALETTE_PURPLE, 3));

            metric_style.set_bg_color(LV_PALETTE_PURPLE);
            metric_style.set_img_recolor(LV_PALETTE_PURPLE);
            metric_style.set_img_recolor_opa(LV_OPA_100);
            metric_style.set_text_color(LV_PALETTE_PURPLE);
            metric_style.set_arc_color(LV_PALETTE_PURPLE);

            warning_style.set_bg_color(lv_color_make(160, 0, 0));
            warning_style.set_img_recolor(lv_color_make(160, 0, 0));
            warning_style.set_img_recolor_opa(LV_OPA_100);
            warning_style.set_text_color(lv_color_make(160, 0, 0));
            warning_style.set_arc_color(lv_color_make(160, 0, 0));
            warning_style.set_line_color(lv_color_make(160, 0, 0));

            monitor_label.create(*this);
            monitor_label.set_style_text_font(&lv_font_montserrat_22);
            monitor_label.set_pos(0, 0);
            monitor_label.add_style(metric_style, LV_PART_MAIN);
            monitor_label.set_text(text_scr);

            if (icon_scr != NULL)
            {
                monitor_icon.create(*this);
                monitor_icon.set_pos(0, 0);
                monitor_icon.add_style(metric_style, LV_PART_MAIN);
                monitor_icon.set_src(icon_scr);
                monitor_label.align_to(monitor_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
            }

            arc.create(*this);
            arc.align_to(monitor_label, LV_ALIGN_OUT_BOTTOM_RIGHT, 45, 10);
            arc.set_rotation(180);
            arc.set_bg_angles(0, 360);
            arc.set_range(0, 100);
            arc.set_size(96, 96);
            arc.add_style(metric_style, LV_PART_MAIN);
            arc.add_style(metric_indicator_style, LV_PART_INDICATOR);
            arc.remove_style(NULL, LV_PART_KNOB);
            arc.clear_flag(LV_OBJ_FLAG_CLICKABLE);

            arc_label.create(arc);
            arc_label.center();
            arc_label.set_style_text_font(&lv_font_montserrat_22);
            arc_label.add_style(metric_style, LV_PART_MAIN);

            bar1.create(*this);
            bar1.align_to(arc, LV_ALIGN_OUT_RIGHT_MID, 10, -40);
            bar1.set_height(20);
            bar1.set_width(130);
            bar1.add_style(metric_style, LV_PART_MAIN);
            bar1.add_style(metric_indicator_style, LV_PART_INDICATOR);

            bar1_label.create(*this);
            bar1_label.align_to(bar1, LV_ALIGN_BOTTOM_LEFT, 0, 25);
            bar1_label.set_style_text_font(&lv_font_montserrat_22);
            bar1_label.add_style(metric_style, LV_PART_MAIN);

            bar2.create(*this);
            bar2.align_to(bar1_label, LV_ALIGN_BOTTOM_LEFT, 0, 25);
            bar2.set_height(20);
            bar2.set_width(130);
            bar2.add_style(metric_style, LV_PART_MAIN);
            bar2.add_style(metric_indicator_style, LV_PART_INDICATOR);

            bar2_label.create(*this);
            bar2_label.align_to(bar2, LV_ALIGN_BOTTOM_LEFT, 0, 25);
            bar2_label.set_style_text_font(&lv_font_montserrat_22);
            bar2_label.add_style(metric_style, LV_PART_MAIN);

            arc.set_value(0);
            arc_label.set_text("NA");
            bar1.set_value(0, LV_ANIM_OFF);
            bar1_label.set_text("NA °C");
            bar2.set_value(0, LV_ANIM_OFF);
            bar2_label.set_text("NA MHz");

            set_size(250, 200);
            add_style(monitor_style, LV_PART_MAIN);
        } // Complete::init_conf

        void Basic::create(lv_obj_t *parent)
        {
            if (object != NULL) return;
            lvgl::port::mutex_take();
            if (parent == NULL) parent = lv_scr_act();
            object = lv_obj_create(parent);
            init_conf();
            lvgl::port::mutex_give();
        } // Basic::create

        void Basic::create(Object &parent)
        {
            if (object != NULL) return;
            lvgl::port::mutex_take();
            object = lv_obj_create(parent.get_object());
            init_conf();
            lvgl::port::mutex_give();
        } // Basic::create

        void Basic::set_bg_color(lv_color_t color)
        {
            if (object == NULL) return;
            lvgl::port::mutex_take();
            monitor_style.set_bg_color(color);
            invalidate();
            lvgl::port::mutex_give();
        } // Basic::set_bg_color

        void Basic::set_bg_color(lv_palette_t color)
        {
            if (object == NULL) return;
            lvgl::port::mutex_take();
            monitor_style.set_bg_color(color);
            invalidate();
            lvgl::port::mutex_give();
        } // Basic::set_bg_color

        void Basic::color(lv_palette_t color)
        {
            if (object == NULL) return;
            lvgl::port::mutex_take();
            metric_indicator_style.set_bg_color(lv_palette_lighten(color, 3));
            metric_indicator_style.set_arc_color(lv_palette_lighten(color, 3));

            metric_style.set_bg_color(color);
            metric_style.set_img_recolor(color);
            metric_style.set_text_color(color);
            metric_style.set_arc_color(color);
            invalidate();
            lvgl::port::mutex_give();
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
            bar1.set_value(value, LV_ANIM_OFF);
            bar1_label.set_text_fmt("%s%d%s", prefix, value, sufix);
        } //  Basic::bar1_set_value

        void Basic::bar2_set_value(int32_t value, int32_t value2, const char *prefix, const char *sufix)
        {
            bar2.set_value(value, LV_ANIM_OFF);
            bar2_label.set_text_fmt("%s%d/%d%s", prefix, value, value2, sufix);
        } //  Basic::bar2_set_value

        void Basic::init_conf()
        {
            monitor_style.set_bg_color(lv_color_make(41, 45, 50));
            monitor_style.set_bg_opa(LV_OPA_100);
            monitor_style.set_pad_all(5);

            metric_indicator_style.set_bg_color(lv_palette_lighten(LV_PALETTE_PURPLE, 3));
            metric_indicator_style.set_arc_color(lv_palette_lighten(LV_PALETTE_PURPLE, 3));

            metric_style.set_bg_color(LV_PALETTE_PURPLE);
            metric_style.set_img_recolor(LV_PALETTE_PURPLE);
            metric_style.set_img_recolor_opa(LV_OPA_100);
            metric_style.set_text_color(LV_PALETTE_PURPLE);
            metric_style.set_arc_color(LV_PALETTE_PURPLE);

            warning_style.set_bg_color(lv_color_make(160, 0, 0));
            warning_style.set_img_recolor(lv_color_make(160, 0, 0));
            warning_style.set_img_recolor_opa(LV_OPA_100);
            warning_style.set_text_color(lv_color_make(160, 0, 0));
            warning_style.set_arc_color(lv_color_make(160, 0, 0));
            warning_style.set_line_color(lv_color_make(160, 0, 0));

            monitor_label.create(*this);
            monitor_label.set_style_text_font(&lv_font_montserrat_22);
            monitor_label.set_pos(0, 0);
            monitor_label.add_style(metric_style, LV_PART_MAIN);
            monitor_label.set_text(text_scr);

            if (icon_scr != NULL)
            {
                monitor_icon.create(*this);
                monitor_icon.set_pos(0, 0);
                monitor_icon.add_style(metric_style, LV_PART_MAIN);
                monitor_icon.set_src(icon_scr);
                monitor_label.align_to(monitor_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
            }

            bar1.create(*this);
            bar1.set_pos(15, 22 + 12);
            bar1.set_height(20);
            bar1.set_width(180);
            bar1.add_style(metric_style, LV_PART_MAIN);
            bar1.add_style(metric_indicator_style, LV_PART_INDICATOR);

            bar1_label.create(*this);
            bar1_label.align_to(bar1, LV_ALIGN_BOTTOM_LEFT, 0, 25);
            bar1_label.set_style_text_font(&lv_font_montserrat_22);
            bar1_label.add_style(metric_style, LV_PART_MAIN);

            bar2.create(*this);
            bar2.align_to(bar1_label, LV_ALIGN_BOTTOM_LEFT, 0, 25);
            bar2.set_height(20);
            bar2.set_width(180);
            bar2.add_style(metric_style, LV_PART_MAIN);
            bar2.add_style(metric_indicator_style, LV_PART_INDICATOR);

            bar2_label.create(*this);
            bar2_label.align_to(bar2, LV_ALIGN_BOTTOM_LEFT, 0, 25);
            bar2_label.set_style_text_font(&lv_font_montserrat_22);
            bar2_label.add_style(metric_style, LV_PART_MAIN);

            bar1.set_value(0, LV_ANIM_OFF);
            bar1_label.set_text("RAM: NA");

            bar2.set_value(0, LV_ANIM_OFF);
            bar2_label.set_text("C: NA");

            set_size(250, 200);
            add_style(monitor_style, LV_PART_MAIN);
        }  // Basic::init_conf

        void Clock::create(lv_obj_t *parent)
        {
            if (object != NULL) return;
            lvgl::port::mutex_take();
            if (parent == NULL) parent = lv_scr_act();
            object = lv_obj_create(parent);
            init_conf();
            lvgl::port::mutex_give();
        } // Clock::create

        void Clock::create(Object &parent)
        {
            if (object != NULL) return;
            lvgl::port::mutex_take();
            object = lv_obj_create(parent.get_object());
            init_conf();
            lvgl::port::mutex_give();
        } //  Clock::create

        void Clock::set_bg_color(lv_color_t color)
        {
            if (object == NULL) return;
            lvgl::port::mutex_take();
            monitor_style.set_bg_color(color);
            invalidate();
            lvgl::port::mutex_give();
        } // Clock::set_bg_color

        void Clock::set_bg_color(lv_palette_t color)
        {
            if (object == NULL) return;
            lvgl::port::mutex_take();
            monitor_style.set_bg_color(color);
            invalidate();
            lvgl::port::mutex_give();
        } // Clock::set_bg_color

        void Clock::color(lv_palette_t color)
        {
            if (object == NULL) return;
            lvgl::port::mutex_take();
            metric_style.set_bg_color(color);
            metric_style.set_img_recolor(color);
            metric_style.set_text_color(color);
            metric_style.set_arc_color(color);
            invalidate();
            lvgl::port::mutex_give();
        } // Clock::color

        void Clock::set_time(struct tm &value)
        {

            char buffer[15];

            strftime(buffer, 12, "%d/%m/%Y", &value);
            date.set_text_fmt("%s", buffer);

            strftime(buffer, 12, "%H:%M:%S", &value);
            hour.set_text_fmt("%s", buffer);
        } // Clock::set_time

        void Clock::init_conf()
        {
            monitor_style.set_bg_color(lv_color_make(41, 45, 50));
            monitor_style.set_bg_opa(LV_OPA_100);
            monitor_style.set_pad_all(5);

            metric_style.set_bg_color(LV_PALETTE_PURPLE);
            metric_style.set_img_recolor(LV_PALETTE_PURPLE);
            metric_style.set_img_recolor_opa(LV_OPA_100);
            metric_style.set_text_color(LV_PALETTE_PURPLE);
            metric_style.set_arc_color(LV_PALETTE_PURPLE);

            monitor_label.create(*this);
            monitor_label.set_style_text_font(&lv_font_montserrat_22);
            monitor_label.set_pos(0, 0);
            monitor_label.add_style(metric_style, LV_PART_MAIN);
            monitor_label.set_text(text_scr);

            if (icon_scr != NULL)
            {
                monitor_icon.create(*this);
                monitor_icon.set_pos(0, 0);
                monitor_icon.add_style(metric_style, LV_PART_MAIN);
                monitor_icon.set_src(icon_scr);
                monitor_label.align_to(monitor_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
            }

            hour.create(*this);
            hour.align(LV_ALIGN_CENTER, 0, -22);
            hour.set_style_text_font(&lv_font_montserrat_40);
            hour.add_style(metric_style, LV_PART_MAIN);
            hour.set_text("NO SYNC");

            date.create(*this);
            date.align(LV_ALIGN_CENTER, 0, 22 + (44 - 22));
            date.set_style_text_font(&lv_font_montserrat_22);
            date.add_style(metric_style, LV_PART_MAIN);
            date.set_text("NO SYNC");

            add_style(monitor_style, LV_PART_MAIN);
        } // Clock::init_conf

    } // namespace monitor

} // namespace streamDeco
