#ifndef __LVGL_SWITCH_HPP__
#define __LVGL_SWITCH_HPP__

#include "lvgl.hpp"

namespace lvgl {

  class Switch : public Object {
    public:

      /**
       * @brief  Create a new slider
       * @param  parent object parent of the new slider
       * @note   The new slider will be created into parent
       * @note   If any parent is passed the slider will be created into main screen
       */
      inline void create(object_t * parent = nullptr)
      {
        if (object != nullptr)
        return;
        port::mutex_take();
        if (parent == nullptr)
          parent = lv_scr_act();
        object = lv_switch_create(parent);
        add_state(state::STATE_CHECKED|state::STATE_DISABLED);
        port::mutex_give();
      }

      inline void create(Object & parent)
      {
        if (object != nullptr)
          return;
        port::mutex_take();
        object = lv_switch_create(parent.get_object());
        port::mutex_give();
      }

      inline bool checked()
      {
        if (object == nullptr)
          return false;
        port::mutex_take();
        bool state = has_state(lvgl::state::STATE_CHECKED);
        port::mutex_give();
        return state ? true : false;
      }

      inline bool disabled()
      {
        if (object == nullptr)
          return false;
        port::mutex_take();
        bool state = has_state(lvgl::state::STATE_DISABLED);
        port::mutex_give();
        return state ? true : false;
      }

  }; // class Switch

} // namespace lvgl

#endif