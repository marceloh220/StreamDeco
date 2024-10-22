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

#include "lvgl/lvgl_button.hpp"

namespace lvgl
{
  void Button::create(lv_obj_t *parent)
  {
    if (object != NULL)
    {
      return;
    }
    port::mutex_take();
    if (parent == NULL)
      parent = lv_scr_act();
    object = lv_btn_create(parent);
    _init();
    port::mutex_give();
  } // Button::create(lv_obj_t *parent)

  void Button::create(Object &parent)
  {
    if (object != NULL)
      return;
    port::mutex_take();
    object = lv_btn_create(parent.get_object());
    _init();
    port::mutex_give();
  } // Button::create(Object &parent)

  void Button::_init()
  {
    lv_color_t color = lv_color_make(41, 45, 50);
    buttonFixed.set_bg_color(color);
    buttonFixed.set_outline_color(color);
    buttonFixed.set_border_color(lv_color_black());
    button.set_radius(6);
    button.set_bg_opa(LV_OPA_100);
    button.set_bg_color(LV_PALETTE_PURPLE);
    button.set_border_opa(LV_OPA_40);
    button.set_border_width(2);
    button.set_border_color(lv_palette_main(LV_PALETTE_GREY));
    button.set_shadow_width(0);
    button.set_shadow_color(lv_palette_main(LV_PALETTE_GREY));
    button.set_shadow_ofs_y(0);
    button.set_outline_opa(LV_OPA_COVER);
    button.set_outline_color(lv_palette_main(LV_PALETTE_PURPLE));
    button.set_text_color(lv_color_white());
    button.set_pad_all(10);
    buttonPressed.set_outline_width(30);
    buttonPressed.set_outline_opa(LV_OPA_TRANSP);
    buttonPressed.set_translate_y(5);
    buttonPressed.set_shadow_ofs_y(3);
    buttonPressed.set_bg_color(lv_palette_darken(LV_PALETTE_PURPLE, 2));
    remove_style_all();
    add_style(button, LV_STATE_DEFAULT);
    add_style(buttonPressed, LV_STATE_PRESSED);
    set_size(128, 128);
    if (_icon1_scr != NULL)
    {
      _icon.create(*this);
      _icon.center();
      _icon.set_src(_icon1_scr);
      icon.set_img_recolor(lv_color_white());
      icon.set_img_recolor_opa(100);
    }
    else
    {
      _label.create(*this);
      _label.set_text(_text_scr);
      _label.center();
    }
  } // Button::init

  void Button::text(const char *text)
  {
    if (object == NULL)
      return;
    port::mutex_take();
    _label.set_text(text);
    port::mutex_give();
  } // Button::text

  void Button::buttonColor(lv_palette_t color)
  {
    if (object == NULL)
      return;
    port::mutex_take();
    button.set_bg_color(lv_palette_main(color));
    button.set_outline_color(lv_palette_main(color));
    buttonPressed.set_bg_color(lv_palette_darken(color, 2));
    port::mutex_give();
  } // Button::color

  void Button::iconColor(lv_palette_t color)
  {
    if (object == NULL)
      return;
    port::mutex_take();
    icon.set_img_recolor(color);
    port::mutex_give();
  } // Button::iconColor

  void Button::swapIcon()
  {
    if (object == NULL)
      return;
    if (_icon1_scr == NULL || _icon2_scr == NULL)
      return;
    _icon_now ^= true;
    if(_icon_now == true)
    {
      port::mutex_take();
      _icon.set_src(_icon1_scr);
      port::mutex_give();
    }
    else
    {
      port::mutex_take();
      _icon.set_src(_icon2_scr);
      port::mutex_give();
    }
  } // Button::swapIcon

  void Button::pin()
  {
    add_style(buttonFixed, LV_STATE_DEFAULT);
    add_style(buttonFixed, LV_STATE_PRESSED);
    _icon.add_style(icon, LV_PART_MAIN);
    _pinnedState = true;
  } // Button::pin

  void Button::unpin()
  {
    remove_style(buttonFixed, LV_STATE_DEFAULT);
    remove_style(buttonFixed, LV_STATE_PRESSED);
    _icon.remove_style(icon, LV_PART_MAIN);
    _pinnedState = false;
  } // Button::unpin

  bool Button::pinned() { return _pinnedState; }

} // namespace lvgl
