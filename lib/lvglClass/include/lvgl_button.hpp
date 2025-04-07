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
#include "lvgl_style.hpp"
#include "lvgl_image.hpp"
#include "lvgl_label.hpp"
#include "lvgl_event.hpp"
#include "lvgl_color.hpp"

#define ALFA_AVALIABLE 0

namespace lvgl
{
  /**
   * @class    Button
   * @brief    Create a button class with some predefined things inside
   */
  class Button : public Object
  {
  public:
/**
 * @brief   Start object with some configuration predefined
 * @param   text Text to be showed on button object
 * @param   icon1 First icon to be showed on button object
 * @param   icon2 Second icon to be showed on button object
 * @note    If no icons are passsed, or passed as NULL, only text will be showed on button.
 *          If two icons are passed they can be switched using Button:iconSwap() method
 */
#if ALFA_AVALIABLE
    Button(const char *text = NULL, icon_t icon1 = NULL, icon_t icon2 = NULL)
#else
    Button(const char *text, icon_t icon1 = NULL, icon_t icon2 = NULL)
#endif
        : _text_scr(text), _icon1_scr(icon1), _icon2_scr(icon2) {};

    /**
     * @brief  Create a new button
     * @param  parent object parent of the new button
     * @note   The new button will be created into parent
     * @note   If any parent is passed the button will be created into main screen
     */
    void create(object_t *parent = NULL)
    {
      if (object != NULL)
        return;
      port::mutex_take();
      if (parent == NULL)
        parent = lv_scr_act();
      object = lv_btn_create(parent);
      _init();
      port::mutex_give();
    } // create(object_t *parent)

    /**
     * @brief  Create a new button
     * @param  parent object parent of the new button
     * @note   The new button will be created into parent
     */
    void create(Object &parent)
    {
      if (object != NULL)
        return;
      port::mutex_take();
      object = lv_btn_create(parent.get_object());
      _init();
      port::mutex_give();
    } // create(Object &parent)

    void buttonRound(lvgl::coordinates_t radius)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      _style_button.set_radius(radius);
      port::mutex_give();
    }

    void buttonOpacity(lvgl::opacity::opacity_t opacity)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      _style_button.set_bg_opa(opacity);
      if (opacity > lvgl::opacity::OPA_50)
        _style_button.set_shadow_opa(lvgl::opacity::OPA_30);
      else if (opacity > lvgl::opacity::OPA_30)
        _style_button.set_shadow_opa(lvgl::opacity::OPA_20);
      else
        _style_button.set_shadow_opa(lvgl::opacity::OPA_0);
      port::mutex_give();
    }

    void buttonShaddow(bool state)
    {
      if (state)
      {
        _style_button.set_shadow_width(5);
        _style_buttonPressed.set_shadow_width(1);
      }
      else
      {
        _style_button.set_shadow_width(0);
        _style_buttonPressed.set_shadow_width(0);
      }
    }

    /**
     * @brief   Change button color
     * @param   color Palette of button color
     */
    void buttonColor(lvgl::palette::palette_t color)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      _style_button.set_bg_color(lvgl::palette::main(color));
      _style_buttonPressed.set_bg_color(lvgl::palette::darken(color, 2));
      port::mutex_give();
    } // color

    /**
     * @brief   Change button color
     * @param   color Palette of button color
     */
    void pinnedButtonColor(lvgl::palette::palette_t color)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      _style_buttonPinned.set_bg_color(lvgl::palette::main(color));
      port::mutex_give();
    } // color

    /**
     * @brief   Change icon color
     * @param   color Palette of icon color
     */
    void iconColor(lvgl::palette::palette_t color)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      _style_icon.set_img_recolor(color);
      port::mutex_give();
    } // iconColor

    /**
     * @brief   Change icon color
     * @param   color Palette of icon color
     */
    void pinnedIconColor(lvgl::palette::palette_t color)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      _style_pinnedIcon.set_img_recolor(color);
      port::mutex_give();
    } // iconColor

    /**
     * @brief   Change text color
     * @param   color Palette of text color
     */
    void textColor(lvgl::palette::palette_t color)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      _style_button.set_text_color(color);
      port::mutex_give();
    } // iconColor

    /**
     * @sa      lvgl::event::code_t
     * @brief   Register a function callback on button
     * @param   callback Function callbatk that will be called when button capture an action
     * @param   code Button action the will trigger the callback function
     * @note    The callback function need be void return type and receive an event::event_t parameter
     */
    template <typename T>
    void callback(lvgl::event::callback_t callback, lvgl::event::code_t code, T user_data)
    {
      if (object == NULL)
        return;
      lvgl::port::mutex_take();
      add_event_cb(callback, code, (void *)user_data);
      lvgl::port::mutex_give();
    } // callback

    /**
     * @brief   If using two icons change between then
     */
    void iconSwap()
    {
      if (object == NULL)
        return;
      if (_icon1_scr == NULL || _icon2_scr == NULL)
        return;
      _state.icon_now ^= true;
      if (_state.icon_now == true)
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
    } // iconSwap

    /**
     * @brief   Pin the button
     */
    void pin()
    {
      add_style(_style_buttonPinned, lvgl::state::STATE_DEFAULT);
      add_style(_style_buttonPinned, lvgl::state::STATE_PRESSED);
      _icon.remove_style(_style_icon, lvgl::part::MAIN);
      _icon.add_style(_style_pinnedIcon, lvgl::part::MAIN);
      _state.pinnedState = true;
    } // pin

    /**
     * @brief   Unpin the button
     */
    void unpin()
    {
      remove_style(_style_buttonPinned, lvgl::state::STATE_DEFAULT);
      remove_style(_style_buttonPinned, lvgl::state::STATE_PRESSED);
      _icon.remove_style(_style_pinnedIcon, lvgl::part::MAIN);
      _icon.add_style(_style_icon, lvgl::part::MAIN);
      _state.pinnedState = false;
    } // unpin

    /**
     * @brief   Verify if button is pinned
     * @return  Return button pin state
     *            0 - Button is not pinned
     *            1 - Button is pinned
     */
    bool pinned() { return _state.pinnedState; }

  protected:
    /**
     * @brief   Internal init button members
     * @note    This is a protected member
     */
    void _init()
    {
      lvgl::color_t color_alt = lvgl::color::make(41, 45, 50);

      _style_button.set_radius(6);

      _style_button.set_bg_opa(lvgl::opacity::OPA_100);
      _style_button.set_bg_color(lvgl::palette::CYAN);

      _style_button.set_shadow_width(5);
      _style_button.set_shadow_ofs_y(3);
      _style_button.set_shadow_ofs_x(3);
      _style_button.set_shadow_opa(lvgl::opacity::OPA_30);
      _style_button.set_shadow_color(lvgl::color::black());

      _style_button.set_text_color(lvgl::color::white());
      _style_button.set_pad_all(10);

      _style_buttonPinned.set_bg_color(color_alt);
      _style_buttonPinned.set_outline_color(color_alt);

      _style_buttonPressed.set_translate_y(5);
      _style_buttonPressed.set_shadow_width(1);
      _style_buttonPressed.set_shadow_ofs_y(5);
      _style_buttonPressed.set_bg_color(lvgl::palette::darken(lvgl::palette::CYAN, 2));

      remove_style_all();
      add_style(_style_button, lvgl::state::STATE_DEFAULT);
      add_style(_style_buttonPressed, lvgl::state::STATE_PRESSED);
      set_size(128, 128);

      if (_icon1_scr != NULL)
      {
        _icon.create(*this);
        _icon.center();
        _icon.set_src(_icon1_scr);
        _style_icon.set_img_recolor(lvgl::color::white());
        _style_icon.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
        _style_pinnedIcon.set_img_recolor(lvgl::color::white());
        _style_pinnedIcon.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
      }
      else if (_icon2_scr != NULL)
      {
        _icon.create(*this);
        _icon.center();
        _icon.set_src(_icon2_scr);
        _style_icon.set_img_recolor(lvgl::color::white());
        _style_icon.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
        _style_pinnedIcon.set_img_recolor(lvgl::color::white());
        _style_pinnedIcon.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
      }
      else
      {
        _label.create(*this);
        _label.set_text(_text_scr);
        _label.center();
      }
    } // Button::init

    /**
     * @var     _label
     * @brief   Button label
     * @details Used if button have no icon
     * @note    This is a protected member
     */
    Label _label;

    /**
     * @var     _icon
     * @brief   Button icon
     * @note    This is a protected member
     */
    Image _icon;

    /**
     * @var     _text_scr
     * @brief   Label text source
     * @note    This is a protected member
     */
    const char *_text_scr;

    /**
     * @var     _icon1_scr
     * @brief   Icon 1 source of icon
     * @note    This is a protected member
     */
    icon_t _icon1_scr;

    /**
     * @var     _icon2_scr
     * @brief   Icon 2 source of icon
     * @note    This is a protected member
     */
    icon_t _icon2_scr;

    /**
     * @struct  state_pack
     * @brief   Keep the icon states
     * @note    This is a protected member
     */
    struct state_pack
    {
      uint8_t icon_now : 1 = true;
      uint8_t pinnedState : 1 = false;
    } _state;

    /**
     * @object  _style_button
     * @brief   Normal button style
     * @note    This is a protected member
     */
    Style _style_button;

    /**
     * @object  _style_buttonPressed
     * @brief   Pressed button style
     * @note    This is a protected member
     */
    Style _style_buttonPressed;

    /**
     * @object  _style_buttonPinned
     * @brief   Pinned button style
     * @note    This is a protected member
     */
    Style _style_buttonPinned;

    /**
     * @object  _style_icon
     * @brief   Normal icon style
     * @note    This is a protected member
     */
    Style _style_icon;

    /**
     * @object  _style_pinnedIcon
     * @brief   Pinned icon style
     * @note    This is a protected member
     */
    lvgl::Style _style_pinnedIcon;

  }; // class Button

} // namespace lvgl

#endif
