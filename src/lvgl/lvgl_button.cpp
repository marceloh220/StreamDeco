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
  void Button::create(object_t *parent)
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
  } // Button::create(object_t *parent)

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
    lvgl::color_t color = color::make(41, 45, 50);
    style_buttonPinned.set_bg_color(color);
    style_buttonPinned.set_outline_color(color);
    style_buttonPinned.set_border_color(color::black());
    style_button.set_radius(6);
    style_button.set_bg_opa(lvgl::opacity::OPA_100);
    style_button.set_bg_color(lvgl::palette::PURPLE);
    style_button.set_border_opa(lvgl::opacity::OPA_40);
    style_button.set_border_width(2);
    style_button.set_border_color(lvgl::palette::main(lvgl::palette::GREY));
    style_button.set_shadow_width(0);
    style_button.set_shadow_color(lvgl::palette::main(lvgl::palette::GREY));
    style_button.set_shadow_ofs_y(0);
    style_button.set_outline_opa(lvgl::opacity::OPA_COVER);
    style_button.set_outline_color(lvgl::palette::main(lvgl::palette::PURPLE));
    style_button.set_text_color(lvgl::color::black());
    style_button.set_pad_all(10);
    style_buttonPressed.set_outline_width(30);
    style_buttonPressed.set_outline_opa(lvgl::opacity::OPA_TRANSP);
    style_buttonPressed.set_translate_y(5);
    style_buttonPressed.set_shadow_ofs_y(3);
    style_buttonPressed.set_bg_color(lvgl::palette::darken(lvgl::palette::PURPLE, 2));
    remove_style_all();
    add_style(style_button, lvgl::state::STATE_DEFAULT);
    add_style(style_buttonPressed, lvgl::state::STATE_PRESSED);
    set_size(128, 128);
    if (_icon1_scr != NULL)
    {
      _icon.create(*this);
      _icon.center();
      _icon.set_src(_icon1_scr);
      style_icon.set_img_recolor(lvgl::color::white());
      style_icon.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
    }
    else if (_text_scr != NULL)
    {
      _label.create(*this);
      _label.set_text(_text_scr);
      _label.center();
    }
  } // Button::init

  #if ALFA_AVALIABLE
  void Button::text(const char *text)
  {
    if (object == NULL)
      return;
    port::mutex_take();
    if (_label.get_object() == NULL)
    {
      _label.create(*this);
      _label.set_text(text);
      _label.center();
    } 
    else 
    {
      _label.set_text(text);
    }
    port::mutex_give();
  } // Button::text

  void Button::icon1(icon_t icon)
  {
    if (object == NULL)
      return;
    port::mutex_take();
    _icon1_scr = icon;
    if (_icon.get_object() == NULL)
    {
      _icon.create(*this);
      _icon.center();
      _icon.set_src(icon);
      style_icon.set_img_recolor(lvgl::color::white());
      style_icon.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
    }
    port::mutex_give();
  } // Button::icon1

  void Button::icon2(icon_t icon)
  {
    if (object == NULL)
      return;
    port::mutex_take();
    _icon2_scr = icon;
    if (_icon.get_object() == NULL)
    {
      _icon.create(*this);
      _icon.center();
      _icon.set_src(icon);
      style_icon.set_img_recolor(lvgl::color::white());
      style_icon.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
    }
    port::mutex_give();
  } // Button::icon1
  #endif

  void Button::buttonColor(lvgl::palette::palette_t color)
  {
    if (object == NULL)
      return;
    port::mutex_take();
    style_button.set_bg_color(lv_palette_main((lv_palette_t)color));
    style_button.set_outline_color(lv_palette_main((lv_palette_t)color));
    style_buttonPressed.set_bg_color(lv_palette_darken((lv_palette_t)color, 2));
    port::mutex_give();
  } // Button::color

  void Button::iconColor(lvgl::palette::palette_t color)
  {
    if (object == NULL)
      return;
    port::mutex_take();
    style_icon.set_img_recolor(color);
    port::mutex_give();
  } // Button::iconColor

  void Button::callback(lvgl::event::callback_t callback, lvgl::event::code_t code, int user_data)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    add_event_cb(callback, code, (void *)user_data);
    lvgl::port::mutex_give();
  } // Button::callback

  void Button::iconSwap()
  {
    if (object == NULL)
      return;
    if (_icon1_scr == NULL || _icon2_scr == NULL)
      return;
    _state.icon_now ^= true;
    if(_state.icon_now == true)
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
  } // Button::iconSwap

  void Button::pin()
  {
    add_style(style_buttonPinned, lvgl::state::STATE_DEFAULT);
    add_style(style_buttonPinned, lvgl::state::STATE_PRESSED);
    _icon.add_style(style_icon, lvgl::part::MAIN);
    _state.pinnedState = true;
  } // Button::pin

  void Button::unpin()
  {
    remove_style(style_buttonPinned, lvgl::state::STATE_DEFAULT);
    remove_style(style_buttonPinned, lvgl::state::STATE_PRESSED);
    _icon.remove_style(style_icon, lvgl::part::MAIN);
    _state.pinnedState = false;
  } // Button::unpin

  bool Button::pinned() { return _state.pinnedState; }

} // namespace lvgl
