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

#ifndef _LVGL_SLIDER_HPP_
#define _LVGL_SLIDER_HPP_

#include "lvgl_object.hpp"
#include "lvgl_port.hpp"
#include <lvgl.h>

namespace lvgl {

class Slider : public Object {

public:
  inline void create(lv_obj_t *parent = NULL) {
    if (created)
      return;
    port::mutex_take();
    if (parent == NULL)
      parent = lv_scr_act();
    object = lv_slider_create(parent);
    port::mutex_give();
    created = true;
  }

  inline void create(Object &parent) {
    if (created)
      return;
    port::mutex_take();
    object = lv_slider_create(parent.get_object());
    port::mutex_give();
    created = true;
  }

  /**
   * Set a new value on the slider
   * @param value     the new value
   * @param anim      LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF:
   * change the value immediately
   */
  inline void set_value(int32_t value, lv_anim_enable_t anim = LV_ANIM_OFF) {
    if (!created)
      return;
    port::mutex_take();
    lv_bar_set_value(object, value, anim);
    port::mutex_give();
  }

  /**
   * Set a new value for the left knob of a slider
   * @param value     new value
   * @param anim      LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF:
   * change the value immediately
   */
  inline void set_left_value(int32_t value, lv_anim_enable_t anim) {
    if (!created)
      return;
    port::mutex_take();
    lv_bar_set_start_value(object, value, anim);
    port::mutex_give();
  }

  /**
   * Set minimum and the maximum values of a bar
   * @param min       minimum value
   * @param max       maximum value
   */
  inline void set_range(int32_t min, int32_t max) {
    if (!created)
      return;
    port::mutex_take();
    lv_bar_set_range(object, min, max);
    port::mutex_give();
  }

  /**
   * Set the mode of slider.
   * @param mode      the mode of the slider. See ::lv_slider_mode_t
   */
  inline void set_mode(lv_slider_mode_t mode) {
    if (!created)
      return;
    port::mutex_take();
    lv_bar_set_mode(object, (lv_bar_mode_t)mode);
    port::mutex_give();
  }

  /**
   * Get the value of the main knob of a slider
   * @return          the value of the main knob of the slider
   */
  inline int32_t get_value() {
    if (!created)
      return 0;
    port::mutex_take();
    int32_t ret = lv_bar_get_value(object);
    port::mutex_give();
    return ret;
  }

  /**
   * Get the value of the left knob of a slider
   * @return          the value of the left knob of the slider
   */
  inline int32_t get_left_value() {
    if (!created)
      return 0;
    port::mutex_take();
    int32_t ret = lv_bar_get_start_value(object);
    port::mutex_give();
    return ret;
  }

  /**
   * Get the minimum value of a slider
   * @return          the minimum value of the slider
   */
  inline int32_t get_min_value() {
    if (!created)
      return 0;
    port::mutex_take();
    int32_t ret = lv_bar_get_min_value(object);
    port::mutex_give();
    return ret;
  }

  /**
   * Get the maximum value of a slider
   * @return          the maximum value of the slider
   */
  inline int32_t get_max_value() {
    if (!created)
      return 0;
    port::mutex_take();
    int32_t ret = lv_bar_get_max_value(object);
    port::mutex_give();
    return ret;
  }

  /**
   * Give the slider is being dragged or not
   * @return          true: drag in progress false: not dragged
   */
  bool is_dragged() {
    if (!created)
      return false;
    port::mutex_take();
    bool ret = lv_slider_is_dragged(object);
    port::mutex_give();
    return ret;
  }

  /**
   * Get the mode of the slider.
   * @return          see ::lv_slider_mode_t
   */
  inline lv_slider_mode_t get_mode() {
    if (!created)
      return false;
    port::mutex_take();
    lv_bar_mode_t mode = lv_bar_get_mode(object);
    port::mutex_give();
    if (mode == LV_BAR_MODE_SYMMETRICAL)
      return LV_SLIDER_MODE_SYMMETRICAL;
    else if (mode == LV_BAR_MODE_RANGE)
      return LV_SLIDER_MODE_RANGE;
    else
      return LV_SLIDER_MODE_NORMAL;
  }

}; // class Slider

} // namespace lvgl

#endif