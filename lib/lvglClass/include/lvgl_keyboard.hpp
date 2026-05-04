#ifndef __LVGL_KEYBOARD_HPP__
#define __LVGL_KEYBOARD_HPP__

#include "lvgl_object.hpp"
#include "lvgl_textarea.hpp"

namespace lvgl
{
  class Keyboard : public Object
  {

    public:

    enum keyboard_mode {
        MODE_TEXT_LOWER,
        MODE_TEXT_UPPER,
        MODE_SPECIAL,
        MODE_NUMBER,
        MODE_USER_1,
        MODE_USER_2,
        MODE_USER_3,
        MODE_USER_4,
    } mode_t;

    /**********************
     * GLOBAL PROTOTYPES
     **********************/

    /**
     * Create a Keyboard object
     * @param parent pointer to an object, it will be the parent of the new keyboard
     * @return pointer to the created keyboard
     */
    inline void create(object_t *parent = nullptr)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      if (parent == nullptr)
        parent = lv_scr_act();
      object = lv_keyboard_create(parent);
      port::mutex_give();
    }

    inline void create(Object &parent)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      object = lv_keyboard_create(parent.get_object());
      port::mutex_give();
    }

  /*=====================
  * Setter functions
  *====================*/

    /**
     * Assign a Text Area to the Keyboard. The pressed characters will be put there.
     * @param ta pointer to a Text Area object to write there
     */
    inline void set_textarea(TextArea &ta)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_keyboard_set_textarea(object, ta.get_object());
      port::mutex_give();
    }

    /**
     * Set a new a mode (text or number map)
     * @param mode the mode from 'lv_keyboard_mode_t'
     */
    inline void set_mode(lv_keyboard_mode_t mode)
    {
      if (object == nullptr)
            return;
      port::mutex_take();
      lv_keyboard_set_mode(object, mode);
      port::mutex_give();
    }

    /**
     * Show the button title in a popover when pressed.
     * @param en whether "popovers" mode is enabled
     */
    inline void set_popovers(bool en)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_keyboard_set_popovers(object, en);
      port::mutex_give();
    }

    /**
     * Set a new map for the keyboard
     * @param mode keyboard map to alter 'lv_keyboard_mode_t'
     * @param map pointer to a string array to describe the map.
     *            See 'lv_btnmatrix_set_map()' for more info.
     */
    inline void set_map(mode_t mode, const char * map[], const lv_btnmatrix_ctrl_t ctrl_map[])
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_keyboard_set_map(object, static_cast<lv_keyboard_mode_t>(mode), map, ctrl_map);
      port::mutex_give();
    }

    /*=====================
    * Getter functions
    *====================*/

    /**
     * Assign a Text Area to the Keyboard. The pressed characters will be put there.
     * @return pointer to the assigned Text Area object
     */
    lv_obj_t *get_textarea() const
    {
      if (object == nullptr)
        return nullptr;
      port::mutex_take();
      lv_obj_t * ta = lv_keyboard_get_textarea(object);
      port::mutex_give();
      return ta;
    }

    /**
     * Set a new a mode (text or number map)
     * @return the current mode from 'lv_keyboard_mode_t'
     */
    inline mode_t get_mode() const
    {
      if (object == nullptr)
        return MODE_TEXT_LOWER;
      port::mutex_take();
      lv_keyboard_mode_t mode = lv_keyboard_get_mode(object);
      port::mutex_give();
      return static_cast<mode_t>(mode);
    }

    /**
     * Tell whether "popovers" mode is enabled or not.
     * @return true: "popovers" mode is enabled; false: disabled
     */
    bool get_popovers() const
    {
      if (object == nullptr)
        return false;
      port::mutex_take();
      bool en = lv_btnmatrix_get_popovers(object);
      port::mutex_give();
      return en;
    }

    /**
     * Get the current map of a keyboard
     * @return the current map
     */
    static inline const char ** get_map_array()
    {
      if (object == nullptr)
        return nullptr;
        return lv_btnmatrix_get_map(object);
    }

    /**
     * Get the index of the lastly "activated" button by the user (pressed, released, focused etc)
     * Useful in the `event_cb` to get the text of the button, check if hidden etc.
     * @return          index of the last released button (LV_BTNMATRIX_BTN_NONE: if unset)
     */
    static inline uint16_t get_selected_btn()
    {
      if (object == nullptr)
        return 0;
      return lv_btnmatrix_get_selected_btn(object);
    }

    /**
     * Get the button's text
     * @param btn_id    the index a button not counting new line characters.
     * @return          text of btn_index` button
     */
    static inline const char * get_btn_text(uint16_t btn_id)
    {
        if (object == nullptr)
            return nullptr;
        return lv_btnmatrix_get_btn_text(object, btn_id);
    }

    /*=====================
    * Other functions
    *====================*/

    /**
     * Default keyboard event to add characters to the Text area and change the map.
     * If a custom `event_cb` is added to the keyboard this function can be called from it to handle the
     * button clicks
     * @param event the triggering event
     */
    inline void def_event_cb(lv_event_t * event)
    {
      if (object == nullptr)
        return;
      lv_keyboard_def_event_cb(event);
    }

  }; // class Keyboard
    
} // namespace lvgl

namespace 

#endif