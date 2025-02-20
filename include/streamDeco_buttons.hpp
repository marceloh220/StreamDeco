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

#ifndef _STREAMDECO_BUTTONS_HPP_
#define _STREAMDECO_BUTTONS_HPP_

#include <lvgl.h>
#include "marcelino.hpp"

namespace streamDeco
{
  
  class MainButton : public lvgl::Object
  {
  public:
    MainButton(const char *text, const void *icon1 = NULL, const void *icon2 = NULL, bool flatIcons = true)
    : text_scr(text), icon1_scr(icon1), icon2_scr(icon2), flat_icons(flatIcons) {};
    void create(uint8_t pos);
    void create(Object &parent, uint8_t pos);
    void text(const char *text);
    void colorButton(lv_palette_t color);
    void colorButtonPinned(lv_palette_t color);
    void colorIcon(lv_palette_t color);
    void colorIcon(lv_color_t color);
    void colorIconPinned(lv_palette_t color);
    void colorIconPinned(lv_color_t color);
    void callback(lvgl::event::callback_t callback, lv_event_code_t code, int user_data);
    void swapIcon();
    void pin();
    void unpin();
    bool pinned();
    virtual void position(uint8_t pos);
  protected:
    void init();
    virtual bool assertPosition(uint8_t pos)
    {
      return (pos >= 15) ? false : true;
    }
    lvgl::Label label;
    lvgl::Image icon;
    lvgl::Style style_button;
    lvgl::Style style_buttonPressed;
    lvgl::Style style_buttonPinned;
    lvgl::Style style_icon;
    lvgl::Style style_iconPinned;
    const char *text_scr;
    const void *icon1_scr;
    const void *icon2_scr;
    bool icon_now = true; // start with icon 1
    bool pinnedState = false;
    bool flat_icons;
  }; // class MainButton

  class CanvasButton : public MainButton
  {
  public:
    CanvasButton(const char *text, const void *icon1 = NULL, const void *icon2 = NULL, bool flatIcons = true)
        : MainButton(text, icon1, icon2, flatIcons) {}
    void position(uint8_t pos);
  private:
    bool assertPosition(uint8_t pos)
    {
      return (pos >= 9) ? false : true;
    }
  }; // class CanvasButton

  class ConfigButton : public MainButton
  {
  public:
    ConfigButton(const char *text, const void *icon1 = NULL, const void *icon2 = NULL, bool flatIcons = true)
        : MainButton(text, icon1, icon2, flatIcons) {}
    void position(uint8_t pos);
  private:
    bool assertPosition(uint8_t pos)
    {
      return (pos >= 9) ? false : true;
    }
  }; // class ConfigButton

} // namespace streamDeco

#endif
