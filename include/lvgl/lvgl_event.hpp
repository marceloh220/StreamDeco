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

#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include "lvgl_object.hpp"

namespace lvgl {

namespace event {

typedef lv_event_t *event_t;

/**
 * @brief Event callback.
 * Events are used to notify user of some action being taken on the object.
 */
typedef void callback_t(event_t);

/**
 * Send an event to the object
 * @param object      pointer to an object
 * @param code        the type of the event from `lv_event_t`
 * @param param       arbitrary data depending on the widget type and the event.
 * (Usually `NULL`)
 * @return LV_RES_OK: `obj` was not deleted in the event; LV_RES_INV: `obj` was
 * deleted in the event_code
 */
lv_res_t send(lv_obj_t *object, lv_event_code_t code, void *param);

/**
 * Get the current target of the event. It's the object which event handler
 * being called. If the event is not bubbled it's the same as "normal" target.
 * @param e     pointer to the event descriptor
 * @return      pointer to the current target of the event_code
 */
lv_obj_t *get_object(lv_event_t *e);

/**
 * Get the event code of an event
 * @param e     pointer to the event descriptor
 * @return      the event code. (E.g. `LV_EVENT_CLICKED`, `LV_EVENT_FOCUSED`,
 * etc)
 */
lv_event_code_t get_code(lv_event_t *e);

/**
 * Get the parameter passed when the event was sent
 * @param e     pointer to the event descriptor
 * @return      pointer to the parameter
 */
template <typename type> type get_param(lv_event_t *e) {
  return (type)lv_event_get_param(e);
}

/**
 * Get the user_data passed when the event was registered on the object
 * @param e     pointer to the event descriptor
 * @return      pointer to the user_data
 */
template <typename type> type get_user_data(lv_event_t *e) {
  return (type)lv_event_get_user_data(e);
}

/**
 * Stop the event from bubbling.
 * This is only valid when called in the middle of an event processing chain.
 * @param e     pointer to the event descriptor
 */
void stop_bubbling(lv_event_t *e);

/**
 * Stop processing this event.
 * This is only valid when called in the middle of an event processing chain.
 * @param e     pointer to the event descriptor
 */
void stop_processing(lv_event_t *e);

} // namespace event

} // namespace lvgl

#endif
