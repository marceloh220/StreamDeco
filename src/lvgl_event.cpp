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

#include "lvgl_event.hpp"
#include "lvgl_port.hpp"

namespace lvgl {

namespace event {

lv_res_t send(lv_obj_t *object, lv_event_code_t code, void *param) {
  lv_res_t ret = 0;
  port::mutex_take();
  ret = lv_event_send(object, code, param);
  port::mutex_give();
  return ret;
}

lv_res_t send(Object &object, lv_event_code_t code, void *param) {
  lv_res_t ret = 0;
  port::mutex_take();
  ret = lv_event_send(object.get_object(), code, param);
  port::mutex_give();
  return ret;
}

lv_obj_t *get_object(lv_event_t *e) {
  lv_obj_t *ret = NULL;
  port::mutex_take();
  ret = lv_event_get_current_target(e);
  port::mutex_give();
  return ret;
}

lv_event_code_t get_code(lv_event_t *e) {
  port::mutex_take();
  lv_event_code_t ret = lv_event_get_code(e);
  port::mutex_give();
  return ret;
}

void stop_bubbling(lv_event_t *e) {
  port::mutex_take();
  lv_event_stop_bubbling(e);
  port::mutex_give();
}

void stop_processing(lv_event_t *e) {
  port::mutex_take();
  lv_event_stop_processing(e);
  port::mutex_give();
}

} // namespace event

} // namespace lvgl