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

  void MainButton::create(uint8_t pos)
  {
    if (assertPosition(pos) == false) return;
    if (object != NULL) return;
    lvgl::port::mutex_take();
    object = lv_btn_create(lv_scr_act());
    init();
    position(pos);
    lvgl::port::mutex_give();
  } // MainButton::create(uint8_t pos)

  void MainButton::create(Object &parent, uint8_t pos)
  {
    if (assertPosition(pos) == false) return;
    if (object != NULL) return;
    lvgl::port::mutex_take();
    object = lv_btn_create(parent.get_object());
    init();
    position(pos);
    lvgl::port::mutex_give();
  } // MainButton::create(Object &parent, uint8_t pos)

  void MainButton::init()
  {
    lv_color_t color = lv_color_make(41, 45, 50);
    style_buttonFixed.set_bg_color(color);
    style_buttonFixed.set_outline_color(color);
    style_buttonFixed.set_border_color(lv_color_black());
    style_button.set_radius(6);
    style_button.set_bg_opa(LV_OPA_100);
    style_button.set_bg_color(LV_PALETTE_PURPLE);
    style_button.set_border_opa(LV_OPA_40);
    style_button.set_border_width(2);
    style_button.set_border_color(lv_palette_main(LV_PALETTE_GREY));
    style_button.set_shadow_width(0);
    style_button.set_shadow_color(lv_palette_main(LV_PALETTE_GREY));
    style_button.set_shadow_ofs_y(0);
    style_button.set_outline_opa(LV_OPA_COVER);
    style_button.set_outline_color(lv_palette_main(LV_PALETTE_PURPLE));
    style_button.set_text_color(lv_color_white());
    style_button.set_pad_all(10);
    style_buttonPressed.set_outline_width(30);
    style_buttonPressed.set_outline_opa(LV_OPA_TRANSP);
    style_buttonPressed.set_translate_y(5);
    style_buttonPressed.set_shadow_ofs_y(3);
    style_buttonPressed.set_bg_color(lv_palette_darken(LV_PALETTE_PURPLE, 2));
    if(!flat_icons) {
      style_buttonFixed.set_bg_grad_color(color);
      style_button.set_bg_grad_color(lv_palette_darken(LV_PALETTE_PURPLE, 2));
      style_button.set_bg_grad_dir(LV_GRAD_DIR_VER);
      style_buttonPressed.set_bg_grad_color(lv_palette_darken(LV_PALETTE_PURPLE, 4));
    }
    remove_style_all();
    add_style(style_button, LV_STATE_DEFAULT);
    add_style(style_buttonPressed, LV_STATE_PRESSED);
    set_size(128, 128);
    if (icon1_scr != NULL)
    {
      icon.create(*this);
      icon.center();
      icon.set_src(icon1_scr);
      style_iconFixed.set_img_recolor(lv_color_white());
      style_iconFixed.set_img_recolor_opa(100);
    } else if (icon2_scr != NULL) {
      icon.create(*this);
      icon.center();
      icon.set_src(icon2_scr);
      style_iconFixed.set_img_recolor(lv_color_white());
      style_iconFixed.set_img_recolor_opa(100);
    } else
    {
      label.create(*this);
      label.set_text(text_scr);
      label.center();
    }
  }

  void MainButton::text(const char *text)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    label.set_text(text);
    lvgl::port::mutex_give();
  } // MainButton::text

  void MainButton::color(lv_palette_t color)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    style_button.set_bg_color(lv_palette_main(color));
    style_button.set_outline_color(lv_palette_main(color));
    style_buttonPressed.set_bg_color(lv_palette_darken(color, 2));
    if (!flat_icons) {
      style_button.set_bg_grad_color(lv_palette_darken(color, 2));
      style_buttonPressed.set_bg_grad_color(lv_palette_darken(color, 4));
    }
    lvgl::port::mutex_give();
  } // MainButton::color

  void MainButton::iconColor(lv_palette_t color)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    style_iconFixed.set_img_recolor(color);
    lvgl::port::mutex_give();
  } // MainButton::iconColor

  void MainButton::iconColor(lv_color_t color)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    style_iconFixed.set_img_recolor(color);
    lvgl::port::mutex_give();
  } // MainButton::iconColor

  void MainButton::callback(lvgl::event::callback_t callback, lv_event_code_t code, int user_data)
  {
    if (object == NULL) return;
    lvgl::port::mutex_take();
    add_event_cb(callback, code, (void *)user_data);
    lvgl::port::mutex_give();
  } // MainButton::callback

  void MainButton::swapIcon()
  {
    if (object == NULL) return;
    if (icon1_scr == NULL) return;
    if (icon2_scr == NULL) return;
    icon_now ^= true;
    lvgl::port::mutex_take();
    icon_now ? icon.set_src(icon1_scr) : icon.set_src(icon2_scr);
    lvgl::port::mutex_give();
  } // MainButton::swapIcon

  void MainButton::pin()
  {
    if (object == NULL) return;
    add_style(style_buttonFixed, LV_STATE_DEFAULT);
    add_style(style_buttonFixed, LV_STATE_PRESSED);
    icon.add_style(style_iconFixed, LV_PART_MAIN);
    pinnedState = true;
  } // MainButton::pin

  void MainButton::unpin()
  {
    if (object == NULL) return;
    remove_style(style_buttonFixed, LV_STATE_DEFAULT);
    remove_style(style_buttonFixed, LV_STATE_PRESSED);
    icon.remove_style(style_iconFixed, LV_PART_MAIN);
    pinnedState = false;
  } // MainButton::unpin

  bool MainButton::pinned() { return pinnedState; }

  void MainButton::position(uint8_t pos)
  {
    if (assertPosition(pos) == false) return;
    if (object == NULL) return;
    lvgl::port::mutex_take();
    lv_disp_rot_t rotation = lvgl::screen::get_rotation();
    int x = 0, y = 0;
    switch(rotation) {
      case LV_DISP_ROT_NONE:
      case LV_DISP_ROT_180:
        x = mainButtons_position_map_landscape[pos].x;
        y = mainButtons_position_map_landscape[pos].y;
        break;
      case LV_DISP_ROT_90:
      case LV_DISP_ROT_270:
        x = mainButtons_position_map_porttrait[pos].x;
        y = mainButtons_position_map_porttrait[pos].y;
        break;
    }
    align(LV_ALIGN_CENTER, x, y);
    update_layout();
    lvgl::port::mutex_give();
  } // MainButton::position

  void CanvasButton::position(uint8_t pos)
  {
    if (assertPosition(pos) == false) return; 
    if (object == NULL) return;
    lvgl::port::mutex_take();
    lv_disp_rot_t rotation = lvgl::screen::get_rotation();
    int x = 0, y = 0;
    switch(rotation) {
      case LV_DISP_ROT_NONE:
      case LV_DISP_ROT_180:
        x = canvasButtons_position_map_landscape[pos].x;
        y = canvasButtons_position_map_landscape[pos].y;
        break;
      case LV_DISP_ROT_90:
      case LV_DISP_ROT_270:
        x = canvasButtons_position_map_porttrait[pos].x;
        y = canvasButtons_position_map_porttrait[pos].y;
        break;
    }
    align(LV_ALIGN_CENTER, x, y);
    update_layout();
    lvgl::port::mutex_give();
  } // CanvasButton::position

  void ConfigButton::position(uint8_t pos)
  {
    if (assertPosition(pos) == false) return;
    if (object == NULL) return;
    lvgl::port::mutex_take();
    lv_disp_rot_t rotation = lvgl::screen::get_rotation();
    int x = 0, y = 0;
    switch(rotation) {
      case LV_DISP_ROT_NONE:
      case LV_DISP_ROT_180:
        x = canvasButtons_position_map_landscape[pos].x - 42;
        y = canvasButtons_position_map_landscape[pos].y;
        break;
      case LV_DISP_ROT_90:
      case LV_DISP_ROT_270:
        x = canvasButtons_position_map_porttrait[pos].x;
        y = canvasButtons_position_map_porttrait[pos].y - 42;
        break;
    }
    align(LV_ALIGN_CENTER, x, y);
    update_layout();
    lvgl::port::mutex_give();
  } // ConfigButton::position

} // namespace streamDeco
