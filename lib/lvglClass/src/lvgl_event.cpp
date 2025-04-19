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

namespace lvgl
{

  namespace event
  {

    lv_res_t send(object_t *object, code_t code, void *param)
    {
      lv_res_t ret = 0;
      port::mutex_take();
      ret = lv_event_send(object, (lv_event_code_t)code, param);
      port::mutex_give();
      return ret;
    }

    lv_res_t send(Object &object, code_t code, void *param)
    {
      lv_res_t ret = 0;
      port::mutex_take();
      ret = lv_event_send(object.get_object(), (lv_event_code_t)code, param);
      port::mutex_give();
      return ret;
    }

    object_t *get_object(event_t e)
    {
      object_t *ret = nullptr;
      port::mutex_take();
      ret = lv_event_get_current_target((lv_event_t *)e);
      port::mutex_give();
      return ret;
    }

    code_t get_code(event_t e)
    {
      port::mutex_take();
      code_t ret = (code_t)lv_event_get_code((lv_event_t *)e);
      port::mutex_give();
      return ret;
    }

    void stop_bubbling(event_t e)
    {
      port::mutex_take();
      lv_event_stop_bubbling((lv_event_t *)e);
      port::mutex_give();
    }

    void stop_processing(event_t e)
    {
      port::mutex_take();
      lv_event_stop_processing((lv_event_t *)e);
      port::mutex_give();
    }

  } // namespace event

} // namespace lvgl
