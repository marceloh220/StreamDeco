#ifndef _LVGL_CALENDAR_HPP_
#define _LVGL_CALENDAR_HPP_

#include "lvgl_object.hpp"
#include <time.h>

namespace lvgl
{
  class Calendar : public Object
  {
  public:
    void create(Object &parent, color_t background)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      object = lv_calendar_create(parent.get_object());
      lv_obj_set_size(object, lv_pct(100), lv_pct(100));
      lv_obj_center(object);
      lv_obj_set_style_pad_all(object, 8, LV_PART_MAIN);
      lv_obj_set_style_bg_color(object, lv_color_make(41, 45, 50), LV_PART_MAIN);
      lv_obj_set_style_text_color(object, lv_color_white(), LV_PART_MAIN);
      lv_obj_set_style_text_font(object, font::montserrat_22, LV_PART_MAIN);
      auto *header = lv_calendar_header_arrow_create(object);
      lv_obj_set_height(header, 64);
      lv_obj_set_style_pad_all(header, 8, LV_PART_MAIN);
      for (int32_t index : {0, 2})
      {
        auto *button = lv_obj_get_child(header, index);
        lv_obj_set_size(button, 48, 48);
        lv_obj_set_ext_click_area(button, 20);
      }
      lv_obj_t *matrix = lv_calendar_get_btnmatrix(object);
      today_style.set_bg_opa(opacity::OPA_COVER);
      today_style.set_bg_color(background);
      lv_obj_add_event_cb(matrix, draw_today, LV_EVENT_DRAW_PART_BEGIN, this);
      refresh_date(true);
      port::mutex_give();
    }

    void refresh_date(bool show_current_month = false)
    {
      if (object == nullptr)
        return;
      time_t now = time(nullptr);
      struct tm date = {};
      if (localtime_r(&now, &date) == nullptr)
        return;
      port::mutex_take();
      const auto *today = lv_calendar_get_today_date(object);
      if (today->year != date.tm_year + 1900 || today->month != date.tm_mon + 1 || today->day != date.tm_mday)
        lv_calendar_set_today_date(object, date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
      if (show_current_month)
        lv_calendar_set_showed_date(object, date.tm_year + 1900, date.tm_mon + 1);
      port::mutex_give();
    }

    void set_button_color(palette::palette_t color)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      auto *header = lv_obj_get_child(object, 0);
      for (int32_t index : {0, 2})
      {
        auto *button = lv_obj_get_child(header, index);
        lv_obj_set_style_bg_color(button, palette::main(color), LV_PART_MAIN);
        lv_obj_set_style_bg_color(button, palette::darken(color, 2), LV_PART_MAIN | LV_STATE_PRESSED);
      }
      port::mutex_give();
    }

    void set_today_color(color_t background)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      today_style.set_bg_color(background);
      lv_obj_invalidate(object);
      port::mutex_give();
    }

  private:
    Style today_style;

    static void draw_today(lv_event_t *event)
    {
      auto *descriptor = static_cast<lv_obj_draw_part_dsc_t *>(lv_event_get_param(event));
      auto *matrix = lv_event_get_target(event);
      if (descriptor->part != LV_PART_ITEMS || descriptor->id < 7 ||
          !lv_btnmatrix_has_btn_ctrl(matrix, descriptor->id, LV_BTNMATRIX_CTRL_CUSTOM_1))
        return;
      auto *calendar = static_cast<Calendar *>(lv_event_get_user_data(event));
      lv_style_value_t color;
      lv_style_get_prop(calendar->today_style.get_style(), LV_STYLE_BG_COLOR, &color);
      descriptor->rect_dsc->bg_color = color.color;
      descriptor->rect_dsc->bg_opa = LV_OPA_COVER;
      descriptor->label_dsc->color = lv_color_brightness(color.color) > 128 ? lv_color_black() : lv_color_white();
    }
  };
}

#endif