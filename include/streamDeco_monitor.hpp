#ifndef _STREAMDECO_MONITOR_HPP_
#define _STREAMDECO_MONITOR_HPP_

#include "lvgl_object.hpp"
#include "lvgl_port.hpp"

#include "lvgl_arc.hpp"
#include "lvgl_bar.hpp"
#include "lvgl_image.hpp"
#include "lvgl_label.hpp"

#include "time.h"

namespace streamDeco
{

    namespace metric
    {

        class Complete : public lvgl::Object
        {

        public:
            inline void create(lv_obj_t *parent = NULL)
            {
                if (created)
                    return;
                lvgl::port::mutex_take();
                if (parent == NULL)
                    parent = lv_scr_act();
                object = lv_obj_create(parent);
                init_conf();
                lvgl::port::mutex_give();
                created = true;
            }

            inline void create(Object &parent)
            {
                if (created)
                    return;
                lvgl::port::mutex_take();
                object = lv_obj_create(parent.get_object());
                init_conf();
                lvgl::port::mutex_give();
                created = true;
            }

            void set_bg_color(lv_color_t color)
            {
                if (!created)
                    return;
                lvgl::port::mutex_take();
                monitor_style.set_bg_color(color);
                invalidate();
                lvgl::port::mutex_give();
            }

            void set_bg_color(lv_palette_t color)
            {
                if (!created)
                    return;
                lvgl::port::mutex_take();
                monitor_style.set_bg_color(color);
                invalidate();
                lvgl::port::mutex_give();
            }

            void color(lv_palette_t color)
            {
                if (!created)
                    return;
                lvgl::port::mutex_take();
                metric_indicator_style.set_bg_color(lv_palette_lighten(color, 3));
                metric_indicator_style.set_arc_color(lv_palette_lighten(color, 3));

                metric_style.set_bg_color(color);
                metric_style.set_img_recolor(color);
                metric_style.set_text_color(color);
                metric_style.set_arc_color(color);
                invalidate();
                lvgl::port::mutex_give();
            }

            void metric_label(const char *label)
            {
                monitor_label.set_text(label);
            }

            void arc_set_value(int16_t value)
            {
                arc.set_value(value);
                arc_label.set_text_fmt("%d%%", value);
            }

            void bar1_set_range(int32_t min, int32_t max)
            {
                bar1.set_range(min, max);
            }

            void bar2_set_range(int32_t min, int32_t max)
            {
                bar2.set_range(min, max);
            }

            void bar1_set_value(int32_t value, const char *prefix, const char *sufix = "")
            {
                bar1.set_value(value, LV_ANIM_OFF);
                bar1_label.set_text_fmt("%s %d %s", prefix, value, sufix);
            }

            void bar2_set_value(int32_t value, const char *prefix, const char *sufix = "")
            {
                bar2.set_value(value, LV_ANIM_OFF);
                bar2_label.set_text_fmt("%s %d %s", prefix, value, sufix);
            }

        protected:
            virtual void init_conf()
            {

                monitor_style.set_bg_color(lv_color_make(41, 45, 50));
                monitor_style.set_bg_opa(LV_OPA_100);
                this->set_size(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                this->add_style(monitor_style, LV_PART_MAIN);

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

                arc.create(*this);
                arc.align_to(monitor_label, LV_ALIGN_BOTTOM_RIGHT, 80, 140);
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
                bar1.align_to(arc, LV_ALIGN_TOP_RIGHT, 120 + 120 + 28, 0);
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
            }

            lvgl::Style monitor_style;
            lvgl::Style metric_style;
            lvgl::Style metric_indicator_style;
            lvgl::Style warning_style;

            lvgl::Label monitor_label;

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
            inline void create(lv_obj_t *parent = NULL)
            {
                if (created)
                    return;
                lvgl::port::mutex_take();
                if (parent == NULL)
                    parent = lv_scr_act();
                object = lv_obj_create(parent);
                init_conf();
                lvgl::port::mutex_give();
                created = true;
            }

            inline void create(Object &parent)
            {
                if (created)
                    return;
                lvgl::port::mutex_take();
                object = lv_obj_create(parent.get_object());
                init_conf();
                lvgl::port::mutex_give();
                created = true;
            }
            void set_bg_color(lv_color_t color)
            {
                if (!created)
                    return;
                lvgl::port::mutex_take();
                monitor_style.set_bg_color(color);
                invalidate();
                lvgl::port::mutex_give();
            }

            void set_bg_color(lv_palette_t color)
            {
                if (!created)
                    return;
                lvgl::port::mutex_take();
                monitor_style.set_bg_color(color);
                invalidate();
                lvgl::port::mutex_give();
            }

            void color(lv_palette_t color)
            {
                if (!created)
                    return;
                lvgl::port::mutex_take();
                metric_indicator_style.set_bg_color(lv_palette_lighten(color, 3));
                metric_indicator_style.set_arc_color(lv_palette_lighten(color, 3));

                metric_style.set_bg_color(color);
                metric_style.set_img_recolor(color);
                metric_style.set_text_color(color);
                metric_style.set_arc_color(color);
                invalidate();
                lvgl::port::mutex_give();
            }

            void metric_label(const char *label)
            {
                monitor_label.set_text(label);
            }

            void bar1_set_range(int32_t min, int32_t max)
            {
                bar1.set_range(min, max);
            }

            void bar2_set_range(int32_t min, int32_t max)
            {
                bar2.set_range(min, max);
            }

            void bar1_set_value(int32_t value, const char *prefix, const char *sufix = "")
            {
                bar1.set_value(value, LV_ANIM_OFF);
                bar1_label.set_text_fmt("%s %d %s", prefix, value, sufix);
            }

            void bar2_set_value(int32_t value, int32_t value2, const char *prefix, const char *sufix = "")
            {
                bar2.set_value(value, LV_ANIM_OFF);
                bar2_label.set_text_fmt("%s %d/%d %s", prefix, value, value2, sufix);
            }

        private:
            void init_conf()
            {
                monitor_style.set_bg_color(lv_color_make(41, 45, 50));
                monitor_style.set_bg_opa(LV_OPA_100);
                this->set_size(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                this->add_style(monitor_style, LV_PART_MAIN);

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
            }

            lvgl::Style monitor_style;
            lvgl::Style metric_style;
            lvgl::Style metric_indicator_style;
            lvgl::Style warning_style;

            lvgl::Label monitor_label;

            lvgl::Bar bar1;
            lvgl::Label bar1_label;

            lvgl::Bar bar2;
            lvgl::Label bar2_label;

        }; // class Basic

        class Clock : public lvgl::Object
        {
        public:
            inline void create(lv_obj_t *parent = NULL)
            {
                if (created)
                    return;
                lvgl::port::mutex_take();
                if (parent == NULL)
                    parent = lv_scr_act();
                object = lv_obj_create(parent);
                init_conf();
                lvgl::port::mutex_give();
                created = true;
            }

            inline void create(Object &parent)
            {
                if (created)
                    return;
                lvgl::port::mutex_take();
                object = lv_obj_create(parent.get_object());
                init_conf();
                lvgl::port::mutex_give();
                created = true;
            }
            void set_bg_color(lv_color_t color)
            {
                if (!created)
                    return;
                lvgl::port::mutex_take();
                monitor_style.set_bg_color(color);
                invalidate();
                lvgl::port::mutex_give();
            }

            void set_bg_color(lv_palette_t color)
            {
                if (!created)
                    return;
                lvgl::port::mutex_take();
                monitor_style.set_bg_color(color);
                invalidate();
                lvgl::port::mutex_give();
            }

            void color(lv_palette_t color)
            {
                if (!created)
                    return;
                lvgl::port::mutex_take();
                metric_style.set_bg_color(color);
                metric_style.set_img_recolor(color);
                metric_style.set_text_color(color);
                metric_style.set_arc_color(color);
                invalidate();
                lvgl::port::mutex_give();
            }

            void set_time(struct tm &value)
            {

                char buffer[15];

                strftime(buffer, 12, "%d/%m/%Y", &value);
                date.set_text_fmt("%s", buffer);

                strftime(buffer, 12, "%H:%M:%S", &value);
                hour.set_text_fmt("%s", buffer);
            }

        private:
            void init_conf()
            {
                monitor_style.set_bg_color(lv_color_make(41, 45, 50));
                monitor_style.set_bg_opa(LV_OPA_100);
                this->set_size(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                this->add_style(monitor_style, LV_PART_MAIN);

                metric_style.set_bg_color(LV_PALETTE_PURPLE);
                metric_style.set_img_recolor(LV_PALETTE_PURPLE);
                metric_style.set_img_recolor_opa(LV_OPA_100);
                metric_style.set_text_color(LV_PALETTE_PURPLE);
                metric_style.set_arc_color(LV_PALETTE_PURPLE);
                
                hour.create(*this);
                hour.align(LV_ALIGN_CENTER, 0, -22);
                hour.set_style_text_font(&lv_font_montserrat_40);
                hour.add_style(metric_style, LV_PART_MAIN);

                date.create(*this);
                date.align(LV_ALIGN_CENTER, 0, 22+(44-22));
                date.set_style_text_font(&lv_font_montserrat_22);
                date.add_style(metric_style, LV_PART_MAIN);
            }

            lvgl::Label date;
            lvgl::Label hour;

            lvgl::Style monitor_style;
            lvgl::Style metric_style;

        }; // class Clock

    } // namespace monitor

} // namespace streamDeco

#endif
