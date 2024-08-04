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

#ifndef _LVGL_BUTTON_HPP_
#define _LVGL_BUTTON_HPP_

#include "lvgl_object.hpp"
#include "lvgl_event.hpp"

namespace lvgl {

class Button : public Object {

public:
  Button(const char *text, const void *icon1 = NULL, const void *icon2 = NULL)
      : text_scr(text), icon1_scr(icon1), icon2_scr(icon2) {}

  void create() {

    if (created)
      return;
    port::mutex_take();

    lv_style_init(&this->style);
    lv_style_set_radius(&this->style, 6);
    lv_style_set_bg_opa(&this->style, LV_OPA_100);
    lv_style_set_bg_color(&this->style, lv_palette_main(LV_PALETTE_PURPLE));
    lv_style_set_bg_grad_color(&this->style, lv_palette_darken(LV_PALETTE_PURPLE, 2));
    lv_style_set_bg_grad_dir(&this->style, LV_GRAD_DIR_VER);
    lv_style_set_border_opa(&this->style, LV_OPA_40);
    lv_style_set_border_width(&this->style, 2);
    lv_style_set_border_color(&this->style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_width(&this->style, 0);
    lv_style_set_shadow_color(&this->style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_ofs_y(&this->style, 0);
    lv_style_set_outline_opa(&this->style, LV_OPA_COVER);
    lv_style_set_outline_color(&this->style, lv_palette_main(LV_PALETTE_PURPLE));
    lv_style_set_text_color(&this->style, lv_color_white());
    lv_style_set_pad_all(&this->style, 10);

    lv_style_init(&this->style_pressed);
    lv_style_set_outline_width(&this->style_pressed, 30);
    lv_style_set_outline_opa(&this->style_pressed, LV_OPA_TRANSP);
    lv_style_set_translate_y(&this->style_pressed, 5);
    lv_style_set_shadow_ofs_y(&this->style_pressed, 3);
    lv_style_set_bg_color(&this->style_pressed, lv_palette_darken(LV_PALETTE_PURPLE, 2));
    lv_style_set_bg_grad_color(&this->style_pressed, lv_palette_darken(LV_PALETTE_PURPLE, 4));

#ifdef BUTTON_PRESSED_ANIMATION
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH,
                                      LV_STYLE_OUTLINE_OPA, LV_STYLE_PROP_INV};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);
    lv_style_set_transition(&this->style_pressed, &trans);
#endif

    this->object = lv_btn_create(lv_scr_act());

    lv_obj_remove_style_all(this->object);
    lv_obj_add_style(this->object, &this->style, 0);
    lv_obj_add_style(this->object, &this->style_pressed, LV_STATE_PRESSED);

    lv_obj_set_size(this->object, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    // set_pos(40 + (128 + 20) * button_col, 20 + (128 + 20) * button_row);
    set_size(128, 128);

    if (icon1_scr != NULL) {
      this->icon = lv_img_create(object);
      lv_obj_center(this->icon);
      lv_img_set_src(this->icon, icon1_scr);
      lv_img_dsc_t *ic = (lv_img_dsc_t *)icon1_scr;
      if (ic->header.cf == LV_IMG_CF_ALPHA_1BIT) {
      }
    } else {
      label = lv_label_create(object);
      lv_label_set_text(label, text_scr);
      lv_obj_center(label);
    }

    port::mutex_give();
    created = true;
  }

  void text(const char *text) {
    if (!created)
      return;
    port::mutex_take();
    lv_label_set_text(label, text);
    port::mutex_give();
  }

  void color(lv_palette_t color) {
    if (!created)
      return;
    port::mutex_take();
    lv_style_set_bg_color(&this->style, lv_palette_main(color));
    lv_style_set_bg_grad_color(&this->style, lv_palette_darken(color, 2));
    lv_style_set_outline_color(&this->style, lv_palette_main(color));
    lv_style_set_bg_color(&this->style_pressed, lv_palette_darken(color, 2));
    lv_style_set_bg_grad_color(&this->style_pressed, lv_palette_darken(color, 4));
    port::mutex_give();
  }

  void callback(event::callback_t callback, int flag) {
    create();
    if (!created)
      return;
    port::mutex_take();
    lv_obj_add_event_cb(object, callback, LV_EVENT_ALL, (void *)flag);
    port::mutex_give();
  }

  void swapIcon() {
    if (!created)
      return;
    port::mutex_take();
    static bool icon_now = 0;

    if (icon1_scr == NULL || icon2_scr == NULL) {
      return;
    }

    icon_now = !icon_now;

    if (icon_now) {
      if (icon2_scr != NULL)
        lv_img_set_src(this->icon, icon2_scr);
    } else {
      if (icon1_scr != NULL)
        lv_img_set_src(this->icon, icon1_scr);
    }
    port::mutex_give();
  }

private:
  lv_obj_t *label;
  lv_obj_t *icon;
  lv_style_t style;
  lv_style_t style_pressed;

  const char *text_scr;
  const void *icon1_scr;
  const void *icon2_scr;
};

} // namespace lvgl

#endif
