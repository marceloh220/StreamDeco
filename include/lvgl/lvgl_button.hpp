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

namespace lvgl
{
  class Button : public Object
  {
  public:
    Button(const char *text, const void *icon1 = NULL, const void *icon2 = NULL)
    : _text_scr(text), _icon1_scr(icon1), _icon2_scr(icon2) {};
    void create(lv_obj_t *parent = NULL);
    void create(Object &parent);
    void text(const char *text);
    void buttonColor(lv_palette_t color);
    void iconColor(lv_palette_t color);
    void swapIcon();
    void pin();
    void unpin();
    bool pinned();
    Style button;
    Style buttonPressed;
    Style buttonFixed;
    Style icon;
  protected:
    void _init();
    Label _label;
    Image _icon;
    const char *_text_scr;
    const void *_icon1_scr;
    const void *_icon2_scr;
    bool _icon_now = true; // start with icon 1
    bool _pinnedState = false;
  }; // class Button
} // namespace lvgl

#endif
