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

  template <std::size_t size>
  lv_position_t get_position(const lv_position_t (&landscape)[size],
                             const lv_position_t (&portrait)[size],
                             uint8_t pos)
  {
    switch (lvgl::screen::get_rotation())
    {
      case lvgl::screen::LANDSCAPE:
      case lvgl::screen::MIRROR_LANDSCAPE:
        return landscape[pos];
      case lvgl::screen::PORTRAIT:
      case lvgl::screen::MIRROR_PORTRAIT:
        return portrait[pos];
    }

    return landscape[pos];
  }

  void setup_icon_styles(lvgl::Style &style_icon, lvgl::Style &style_iconPinned)
  {
    style_icon.set_img_recolor(lvgl::color::black());
    style_icon.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
    style_iconPinned.set_img_recolor(lvgl::color::white());
    style_iconPinned.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
  }

  template <typename ColorT>
  void set_icon_color(lvgl::Style &style, ColorT color)
  {
    style.set_img_recolor(color);
  }

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

  static const lv_position_t canvasButtons_position_map[]{
      {-148, -148}, {0, -148}, {148, -148},
      {-148,    0}, {0,    0}, {148,    0},
      {-148,  148}, {0,  148}, {148,  148},
  };

  void MainButton::create(uint8_t pos, lvgl::palette::palette_t color)
  {
    create_impl(nullptr, pos, color);
  } // MainButton::create(uint8_t pos)

  void MainButton::create(Object &parent, uint8_t pos, lvgl::palette::palette_t color)
  {
    create_impl(&parent, pos, color);
  } // MainButton::create(Object &parent, uint8_t pos)

  void MainButton::create_impl(Object *parent, uint8_t pos, lvgl::palette::palette_t color)
  {
    if (assertPosition(pos) == false) return;
    if (object != nullptr) return;
    lvgl::port::mutex_take();
    object = lv_btn_create(resolve_parent_object(parent));
    init(color);
    position(pos);
    lvgl::port::mutex_give();
  }

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

    init_content();
  }

  void MainButton::init_content()
  {
    const lvgl::icon_t icon_source = icon1_scr != nullptr ? icon1_scr : icon2_scr;
    if (icon_source != nullptr)
    {
      icon.create(*this);
      icon.center();
      icon.set_src(icon_source);
      setup_icon_styles(style_icon, style_iconPinned);
      return;
    }

    label.create(*this);
    label.set_text(text_scr);
    label.center();
  }

  #if ALFA_AVALIABLE

  /** This three methods are in beta... in alfa stage
     *  I need think better on they usabilitie, use for your own risk
     */
  void MainButton::text(const char *text)
  {
    with_lock([&]() { label.set_text(text); });
  } // MainButton::text

  #endif

  void MainButton::buttonColor(lvgl::palette::palette_t color)
  {
    with_lock([&]() { apply_button_color(color); });
  } // MainButton::buttonColor

  void MainButton::buttonPinnedColor(lvgl::palette::palette_t color)
  {
    with_lock([&]() { apply_button_pinned_color(color); });
  } // MainButton::buttonPinnedColor

  void MainButton::iconColor(lvgl::palette::palette_t color)
  {
    with_lock([&]() { set_icon_color(style_icon, color); });
  } // MainButton::iconColor

  void MainButton::iconColor(lvgl::color_t color)
  {
    with_lock([&]() { set_icon_color(style_icon, color); });
  } // MainButton::iconColor

  void MainButton::iconPinnedColor(lvgl::palette::palette_t color)
  {
    with_lock([&]() { set_icon_color(style_iconPinned, color); });
  } // MainButton::iconPinnedColor

  void MainButton::iconPinnedColor(lvgl::color_t color)
  {
    with_lock([&]() { set_icon_color(style_iconPinned, color); });
  } // MainButton::iconPinnedColor

  void MainButton::callback(lvgl::event::callback_t callback, lvgl::event::code_t code, int user_data)
  {
    with_lock([&]() { add_event_cb(callback, code, (void *)user_data); });
  } // MainButton::callback

  void MainButton::iconSwap()
  {
    if (icon1_scr == nullptr) return;
    if (icon2_scr == nullptr) return;
    state.icon_now ^= true;
    with_lock([&]() { state.icon_now ? icon.set_src(icon1_scr) : icon.set_src(icon2_scr); });
  } // MainButton::iconSwap

  void MainButton::apply_pin_state(bool pinned)
  {
    if (pinned)
    {
      add_style(style_buttonPinned, lvgl::state::STATE_DEFAULT);
      add_style(style_buttonPinned, lvgl::state::STATE_PRESSED);
      icon.remove_style(style_icon, lvgl::part::MAIN);
      icon.add_style(style_iconPinned, lvgl::part::MAIN);
      state.pinnedState = true;
      return;
    }

    remove_style(style_buttonPinned, lvgl::state::STATE_DEFAULT);
    remove_style(style_buttonPinned, lvgl::state::STATE_PRESSED);
    icon.remove_style(style_iconPinned, lvgl::part::MAIN);
    icon.add_style(style_icon, lvgl::part::MAIN);
    state.pinnedState = false;
  }

  void MainButton::apply_button_color(lvgl::palette::palette_t color)
  {
    const lvgl::color_t main_color = lvgl::palette::main(color);
    style_button.set_bg_color(main_color);
    style_button.set_outline_color(main_color);
    style_buttonPressed.set_bg_color(lvgl::palette::darken(color, 2));
  }

  void MainButton::apply_button_pinned_color(lvgl::palette::palette_t color)
  {
    const lvgl::color_t main_color = lvgl::palette::main(color);
    style_buttonPinned.set_bg_color(main_color);
    style_buttonPinned.set_outline_color(main_color);
  }

  void MainButton::pin()
  {
    with_lock([&]() { apply_pin_state(true); });
  } // MainButton::pin

  void MainButton::unpin()
  {
    with_lock([&]() { apply_pin_state(false); });
  } // MainButton::unpin

  bool MainButton::pinned() { return state.pinnedState; }

  void MainButton::position(uint8_t pos)
  {
    if (assertPosition(pos) == false) return;
    with_lock([&]() {
      lv_position_t position = get_position(mainButtons_position_map_landscape, mainButtons_position_map_porttrait, pos);
      align_position(position.x, position.y);
    });
  } // MainButton::position

  void CanvasButton::position(uint8_t pos)
  {
    if (assertPosition(pos) == false) return; 
    with_lock([&]() {
      lv_position_t position = get_position(canvasButtons_position_map, canvasButtons_position_map, pos);
      align_position(position.x, position.y);
    });
  } // CanvasButton::position

  void ConfigButton::position(uint8_t pos)
  {
    if (assertPosition(pos) == false) return;
    with_lock([&]() {
      lv_position_t position = get_position(canvasButtons_position_map, canvasButtons_position_map, pos);
      lvgl::screen::rotation_t rotation = lvgl::screen::get_rotation();
      if (rotation == lvgl::screen::LANDSCAPE || rotation == lvgl::screen::MIRROR_LANDSCAPE)
      {
        position.x -= 42;
      }
      else
      {
        position.y -= 42;
      }
      align_position(position.x, position.y);
    });
  } // ConfigButton::position

} // namespace streamDeco
