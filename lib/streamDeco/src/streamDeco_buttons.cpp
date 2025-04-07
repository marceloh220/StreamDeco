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

#include "streamDeco_buttons.hpp"

namespace streamDeco
{

  typedef struct
  {
    lv_coord_t x;
    lv_coord_t y;
  } lv_position_t;

  static const lv_position_t mainButtons_position_map_landscape[] = {
      {-296, -148}, {-148, -148}, {0, -148}, {148, -148}, {296, -148},
      {-296,    0}, {-148,    0}, {0,    0}, {148,    0}, {296,    0},
      {-296,  148}, {-148,  148}, {0,  148}, {148,  148}, {296,  148},
  };

  static const lv_position_t mainButtons_position_map_porttrait[] = {
      {-148, -296}, {0, -296}, {148, -296},
      {-148, -148}, {0, -148}, {148, -148},
      {-148,    0}, {0,    0}, {148,    0},
      {-148,  148}, {0,  146}, {148,  148},
      {-148,  296}, {0,  296}, {148,  296},
  };

  static const lv_position_t canvasButtons_position_map_landscape[]{
      {-148, -148}, {0, -148}, {148, -148},
      {-148,    0}, {0,    0}, {148,    0},
      {-148,  148}, {0,  148}, {148,  148},
  };

  static const lv_position_t canvasButtons_position_map_porttrait[]{
      {-148, -148}, {0, -148}, {148, -148},
      {-148,    0}, {0,    0}, {148,    0},
      {-148,  148}, {0,  148}, {148,  148},
  };

  void MainButton::create(uint8_t pos, lvgl::palette::palette_t color)
  {
    if (assertPosition(pos) == false) return;
    if (object != NULL) return;
    lvgl::port::mutex_take();
    object = lv_btn_create(lv_scr_act());
    init(color);
    position(pos);
    lvgl::port::mutex_give();
  } // MainButton::create(uint8_t pos)

  void MainButton::create(Object &parent, uint8_t pos, lvgl::palette::palette_t color)
  {
    if (assertPosition(pos) == false) return;
    if (object != NULL) return;
    lvgl::port::mutex_take();
    object = lv_btn_create(parent.get_object());
    init(color);
    position(pos);
    lvgl::port::mutex_give();
  } // MainButton::create(Object &parent, uint8_t pos)

  void MainButton::init(lvgl::palette::palette_t color)
  {
    lvgl::color_t color_alt = lvgl::color::make(41, 45, 50);

    style_button.set_radius(6);

    style_button.set_bg_opa(lvgl::opacity::OPA_100);
    style_button.set_bg_color(color);
    
    style_button.set_shadow_width(5);
    style_button.set_shadow_ofs_y(3);
    style_button.set_shadow_ofs_x(3);
    style_button.set_shadow_opa(lvgl::opacity::OPA_30);
    style_button.set_shadow_color(lvgl::color::black());
    
    style_button.set_text_color(lvgl::color::white());
    style_button.set_pad_all(10);

    style_buttonPinned.set_bg_color(color_alt);
    style_buttonPinned.set_outline_color(color_alt);

    style_buttonPressed.set_translate_y(5);
    style_buttonPressed.set_shadow_width(1);
    style_buttonPressed.set_shadow_ofs_y(5);
    style_buttonPressed.set_bg_color(lvgl::palette::darken(color, 2));

    remove_style_all();
    add_style(style_button, lvgl::state::STATE_DEFAULT);
    add_style(style_buttonPressed, lvgl::state::STATE_PRESSED);
    set_size(128, 128);
    
    if (icon1_scr != NULL)
    {
      icon.create(*this);
      icon.center();
      icon.set_src(icon1_scr);
      style_icon.set_img_recolor(lvgl::color::black());
      style_icon.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
      style_iconPinned.set_img_recolor(lvgl::color::white());
      style_iconPinned.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
    } else if (icon2_scr != NULL) {
      icon.create(*this);
      icon.center();
      icon.set_src(icon2_scr);
      style_icon.set_img_recolor(lvgl::color::black());
      style_icon.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
      style_iconPinned.set_img_recolor(lvgl::color::white());
      style_iconPinned.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
    } else
    {
      label.create(*this);
      label.set_text(text_scr);
      label.center();
    }
  }

  #if ALFA_AVALIABLE

  /** This three methods are in beta... in alfa stage
     *  I need think better on they usabilitie, use for your own risk
     */
  void MainButton::text(const char *text)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    label.set_text(text);
    lvgl::port::mutex_give();
  } // MainButton::text

  #endif

  void MainButton::buttonColor(lvgl::palette::palette_t color)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    style_button.set_bg_color(lvgl::palette::main(color));
    style_button.set_outline_color(lvgl::palette::main(color));
    style_buttonPressed.set_bg_color(lvgl::palette::darken(color, 2));
    lvgl::port::mutex_give();
  } // MainButton::buttonColor

  void MainButton::buttonPinnedColor(lvgl::palette::palette_t color)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    style_buttonPinned.set_bg_color(lvgl::palette::main(color));
    style_buttonPinned.set_outline_color(lvgl::palette::main(color));
    lvgl::port::mutex_give();
  } // MainButton::buttonPinnedColor

  void MainButton::iconColor(lvgl::palette::palette_t color)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    style_icon.set_img_recolor(color);
    lvgl::port::mutex_give();
  } // MainButton::iconColor

  void MainButton::iconColor(lvgl::color_t color)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    style_icon.set_img_recolor(color);
    lvgl::port::mutex_give();
  } // MainButton::iconColor

  void MainButton::iconPinnedColor(lvgl::palette::palette_t color)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    style_iconPinned.set_img_recolor(color);
    lvgl::port::mutex_give();
  } // MainButton::iconPinnedColor

  void MainButton::iconPinnedColor(lvgl::color_t color)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    style_iconPinned.set_img_recolor(color);
    lvgl::port::mutex_give();
  } // MainButton::iconPinnedColor

  void MainButton::callback(lvgl::event::callback_t callback, lvgl::event::code_t code, int user_data)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    add_event_cb(callback, code, (void *)user_data);
    lvgl::port::mutex_give();
  } // MainButton::callback

  void MainButton::iconSwap()
  {
    if (object == NULL) return;
    if (icon1_scr == NULL) return;
    if (icon2_scr == NULL) return;
    state.icon_now ^= true;
    lvgl::port::mutex_take();
    state.icon_now ? icon.set_src(icon1_scr) : icon.set_src(icon2_scr);
    lvgl::port::mutex_give();
  } // MainButton::iconSwap

  void MainButton::pin()
  {
    if (object == NULL) return;
    add_style(style_buttonPinned, lvgl::state::STATE_DEFAULT);
    add_style(style_buttonPinned, lvgl::state::STATE_PRESSED);
    icon.remove_style(style_icon, lvgl::part::MAIN);
    icon.add_style(style_iconPinned, lvgl::part::MAIN);
    state.pinnedState = true;
  } // MainButton::pin

  void MainButton::unpin()
  {
    if (object == NULL) return;
    remove_style(style_buttonPinned, lvgl::state::STATE_DEFAULT);
    remove_style(style_buttonPinned, lvgl::state::STATE_PRESSED);
    icon.remove_style(style_iconPinned, lvgl::part::MAIN);
    icon.add_style(style_icon, lvgl::part::MAIN);
    state.pinnedState = false;
  } // MainButton::unpin

  bool MainButton::pinned() { return state.pinnedState; }

  void MainButton::position(uint8_t pos)
  {
    if (assertPosition(pos) == false) return;
    if (object == NULL) return;
    lvgl::port::mutex_take();
    lvgl::screen::rotation_t rotation = lvgl::screen::get_rotation();
    int x = 0, y = 0;
    switch(rotation) {
      case lvgl::screen::LANDSCAPE:
      case lvgl::screen::MIRROR_LANDSCAPE:
        x = mainButtons_position_map_landscape[pos].x;
        y = mainButtons_position_map_landscape[pos].y;
        break;
      case lvgl::screen::PORTRAIT:
      case lvgl::screen::MIRROR_PORTRAIT:
        x = mainButtons_position_map_porttrait[pos].x;
        y = mainButtons_position_map_porttrait[pos].y;
        break;
    }
    align(lvgl::alignment::CENTER, x, y);
    update_layout();
    lvgl::port::mutex_give();
  } // MainButton::position

  void CanvasButton::position(uint8_t pos)
  {
    if (assertPosition(pos) == false) return; 
    if (object == NULL) return;
    lvgl::port::mutex_take();
    lvgl::screen::rotation_t rotation = lvgl::screen::get_rotation();
    int x = 0, y = 0;
    switch(rotation) {
      case lvgl::screen::LANDSCAPE:
      case lvgl::screen::MIRROR_LANDSCAPE:
        x = canvasButtons_position_map_landscape[pos].x;
        y = canvasButtons_position_map_landscape[pos].y;
        break;
      case lvgl::screen::PORTRAIT:
      case lvgl::screen::MIRROR_PORTRAIT:
        x = canvasButtons_position_map_porttrait[pos].x;
        y = canvasButtons_position_map_porttrait[pos].y;
        break;
    }
    align(lvgl::alignment::CENTER, x, y);
    update_layout();
    lvgl::port::mutex_give();
  } // CanvasButton::position

  void ConfigButton::position(uint8_t pos)
  {
    if (assertPosition(pos) == false) return;
    if (object == NULL) return;
    lvgl::port::mutex_take();
    lvgl::screen::rotation_t rotation = lvgl::screen::get_rotation();
    int x = 0, y = 0;
    switch(rotation) {
      case lvgl::screen::LANDSCAPE:
      case lvgl::screen::MIRROR_LANDSCAPE:
        x = canvasButtons_position_map_landscape[pos].x - 42;
        y = canvasButtons_position_map_landscape[pos].y;
        break;
      case lvgl::screen::PORTRAIT:
      case lvgl::screen::MIRROR_PORTRAIT:
        x = canvasButtons_position_map_porttrait[pos].x;
        y = canvasButtons_position_map_porttrait[pos].y - 42;
        break;
    }
    align(lvgl::alignment::CENTER, x, y);
    update_layout();
    lvgl::port::mutex_give();
  } // ConfigButton::position

} // namespace streamDeco
