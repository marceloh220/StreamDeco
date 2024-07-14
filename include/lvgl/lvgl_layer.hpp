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

#ifndef _LVGL_LAYER_HPP_
#define _LVGL_LAYER_HPP_

#include "lvgl_object.hpp"
#include "lvgl_port.hpp"
#include <lvgl.h>

namespace lvgl {

class Layer : public Object {

public:
  inline void create(lv_obj_t *parent = NULL) {
    if (created)
      return;
    port::mutex_take();
    if (parent == NULL)
      parent = lv_scr_act();
    object = lv_obj_create(parent);
    port::mutex_give();
    created = true;
  }

  inline void create(Object &parent) {
    if (created)
      return;
    port::mutex_take();
    object = lv_obj_create(parent.get_object());
    port::mutex_give();
    created = true;
  }

  void set_bg_color(lv_color_t color) {
    if (!created)
      return;
    port::mutex_take();
    lv_obj_set_style_bg_color(object, color, LV_PART_MAIN);
    invalidate();
    port::mutex_give();
  }

  void set_bg_color(lv_palette_t color) {
    if (!created)
      return;
    port::mutex_take();
    lv_obj_set_style_bg_color(object, lv_palette_main(color), LV_PART_MAIN);
    invalidate();
    port::mutex_give();
  }
};

} // namespace lvgl

#endif
