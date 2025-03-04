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
    void create(object_t *parent = NULL);

    /**
    * @brief  Create a new button
    * @param  parent object parent of the new button
    * @note   The new button will be created into parent
    */
    void create(Object &parent);

    #if ALFA_AVALIABLE
    /** This three methods are in beta... in alfa stage
     *  I need think better on they usabilitie, use for your own risk
     */
    void text(const char *text);

    void icon1(icon_t icon);
    void icon2(icon_t icon);
    #endif

    /**
     * @brief   Change button color
     * @param   color Palette of button color
     */
    void buttonColor(lvgl::palette::palette_t color);

    /**
     * @brief   Change icon color
     * @param   color Palette of icon color
     */
    void iconColor(lvgl::palette::palette_t color);

    /**
     * @sa      lvgl::event::code_t
     * @brief   Register a function callback on button
     * @param   callback Function callbatk that will be called when button capture an action
     * @param   code Button action the will trigger the callback function
     * @note    The callback function need be void return type and receive an event::event_t parameter
     */
    void callback(lvgl::event::callback_t callback, lvgl::event::code_t code, int user_data);
    
    /**
     * @brief   If using two icons change between then
     */
    void iconSwap();

    /**
     * @brief   Pin the button
     */
    void pin();

    /**
     * @brief   Unpin the button
     */
    void unpin();

    /**
     * @brief   Verify if button is pinned
     * @return  Return button pin state
     *            0 - Button is not pinned
     *            1 - Button is pinned
     */
    bool pinned();

  protected:
    
    /**
     * @brief   Internal init button members
     * @note    This is a protected member
     */
    void _init();
    
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
    struct state_pack{
      uint8_t icon_now:1 = true;
      uint8_t pinnedState:1 = false;
    } _state;


    /**
     * @object  style_button
     * @brief   Normal button style
     * @note    This is a protected member
     */
    Style style_button;

    /**
     * @object  style_buttonPressed
     * @brief   Pressed button style
     * @note    This is a protected member
     */
     Style style_buttonPressed;
    
    /**
     * @object  style_buttonPinned
     * @brief   Pinned button style
     * @note    This is a protected member
     */
    Style style_buttonPinned;
    
    /**
     * @object  style_icon
     * @brief   Normal icon style
     * @note    This is a protected member
     */
    Style style_icon;

  }; // class Button
  
} // namespace lvgl

#endif
