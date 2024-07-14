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

#include "lvgl/lvgl_screen.hpp"

namespace lvgl {

namespace screen {

    void set_rotation(lv_disp_rot_t rotation) {
        port::set_screen_rotation(rotation);
    }

    lv_disp_rot_t get_rotation() {
        return port::get_screen_rotation();
    }

    void set_bg_color(lv_color_t color) {
        port::mutex_take();
        lv_obj_t *object = lv_scr_act();
        lv_obj_set_style_bg_color(object, color, LV_PART_MAIN);
        lv_obj_invalidate(object);
        port::mutex_give();
    }

    void set_bg_color(lv_palette_t color) {
        port::mutex_take();
        lv_obj_t *object = lv_scr_act();
        lv_obj_set_style_bg_color(object, lv_palette_main(color), LV_PART_MAIN);
        lv_obj_invalidate(object);
        port::mutex_give();
    }

    void center() {
        port::mutex_take();
        lv_obj_t *object = lv_scr_act();
        lv_obj_center(object);
        lv_obj_invalidate(object);
        port::mutex_give();
    }

    void refresh() {
        port::mutex_take();
        lv_obj_t *object = lv_scr_act();
        lv_obj_invalidate(object);
        port::mutex_give();
    }

} // namespace screen

} // namespace lgvl