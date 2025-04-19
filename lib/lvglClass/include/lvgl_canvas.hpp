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

#ifndef _LVGL_CANVAS_HPP_
#define _LVGL_CANVAS_HPP_

#include "lvgl_object.hpp"
#include "lvgl_color.hpp"

namespace lvgl
{

  class Canvas : public Object
  {

  public:
    /**
     * @brief  Create a new canvas
     * @param  parent object parent of the new canvas
     * @note   The new canvas will be created into parent
     * @note   If any parent is passed the canvas will be created into main screen
     */
    inline void create(object_t *parent = nullptr)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      if (parent == nullptr)
        parent = lv_scr_act();
      object = lv_obj_create(parent);
      port::mutex_give();
    }

    /**
     * @brief  Create a new canvas
     * @param  parent object parent of the new canvas
     * @note   The new canvas will be created into parent
     */
    inline void create(Object &parent)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      object = lv_obj_create(parent.get_object());
      port::mutex_give();
    }

    /**
     * @brief   Set the background color of canvas
     * @param   color Color of background in display format
     */
    void set_bg_color(lvgl::color_t color)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_obj_set_style_bg_color(object, color, lvgl::part::MAIN);
      invalidate();
      port::mutex_give();
    }

    /**
     * @brief   Set the background color canvas
     * @param   color Color of background on lvgl paletton
     */
    void set_bg_color(lvgl::palette::palette_t color)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_obj_set_style_bg_color(object, lv_palette_main((lv_palette_t)color), lvgl::part::MAIN);
      invalidate();
      port::mutex_give();
    }
  };

} // namespace lvgl

#endif
