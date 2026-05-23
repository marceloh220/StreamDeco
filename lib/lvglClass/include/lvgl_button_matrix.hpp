#ifndef __LVGL_BUTTON_MATRIX_HPP__
#define __LVGL_BUTTON_MATRIX_HPP__

#include "lvgl_object.hpp"

namespace lvgl
{
  class ButtonMatrix : public Object
  {

    public:

    /**********************
     * GLOBAL PROTOTYPES
     **********************/

    /**
     * Create a Button Matrix object
     * @param parent pointer to an object, it will be the parent of the new button matrix
     * @return pointer to the created button matrix
     */
    inline void create(object_t *parent = nullptr)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      if (parent == nullptr)
        parent = lv_scr_act();
      object = lv_btnmatrix_create(parent);
      port::mutex_give();
    }

    inline void create(Object &parent)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      object = lv_btnmatrix_create(parent.get_object());
      port::mutex_give();
    }

    /*=====================
    * Setter functions
    *====================*/

    /**
     * Set a new map for the button matrix. The map is an array of string pointers. Each string is the text of a button.
     * New line characters ('\n') can be inserted into the strings to create buttons in a new line.
     * E.g. `const char * map[] = {"1", "2", "3", "\n", "4", "5", "6", ""};`
     * The last element of the array has to be an empty string (`""`) to indicate the end of the map.
     * @param map pointer to a string array to describe the map.
     */
    inline void set_map(const char * map[])
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_btnmatrix_set_map(object, map);
      port::mutex_give();
    }

    /**
     * Set the button control map (hidden, disabled etc.) for a button matrix.
     * The control map array will be copied and so may be deallocated after this
     * function returns.
     * @param ctrl_map  pointer to an array of `lv_btn_ctrl_t` control bytes. The
     *                  length of the array and position of the elements must match
     *                  the number and order of the individual buttons (i.e. excludes
     *                  newline entries).
     *                  An element of the map should look like e.g.:
     *                 `ctrl_map[0] = width | LV_BTNMATRIX_CTRL_NO_REPEAT |  LV_BTNMATRIX_CTRL_TGL_ENABLE`
     */
    inline void set_ctrl_map(const lv_btnmatrix_ctrl_t ctrl_map[])
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_btnmatrix_set_ctrl_map(object, ctrl_map);
      port::mutex_give();
    }

    /**
     * Set the selected buttons
     * @param btn_id         0 based index of the button to modify. (Not counting new lines)
     */
    inline void set_selected_btn(uint16_t btn_id)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_btnmatrix_set_selected_btn(object, btn_id);
      port::mutex_give();
    }

    /**
     * Set the attributes of a button of the button matrix
     * @param btn_id    0 based index of the button to modify. (Not counting new lines)
     * @param ctrl      OR-ed attributs. E.g. `LV_BTNMATRIX_CTRL_NO_REPEAT | LV_BTNMATRIX_CTRL_CHECKABLE`
     */
    inline void set_btn_ctrl(uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_btnmatrix_set_btn_ctrl(object, btn_id, ctrl);
      port::mutex_give();
    }

    /**
     * Clear the attributes of a button of the button matrix
     * @param btn_id    0 based index of the button to modify. (Not counting new lines)
     * @param ctrl      OR-ed attributs. E.g. `LV_BTNMATRIX_CTRL_NO_REPEAT | LV_BTNMATRIX_CTRL_CHECKABLE`
     */
    inline void clear_btn_ctrl(uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_btnmatrix_clear_btn_ctrl(object, btn_id, ctrl);
      port::mutex_give();
    }

    /**
     * Set attributes of all buttons of a button matrix
     * @param ctrl      attribute(s) to set from `lv_btnmatrix_ctrl_t`. Values can be ORed.
     */
    inline void set_btn_ctrl_all(lv_btnmatrix_ctrl_t ctrl)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_btnmatrix_set_btn_ctrl_all(object, ctrl);
      port::mutex_give();
    }

    /**
     * Clear the attributes of all buttons of a button matrix
     * @param ctrl      attribute(s) to set from `lv_btnmatrix_ctrl_t`. Values can be ORed.
     * @param en        true: set the attributes; false: clear the attributes
     */
    inline void clear_btn_ctrl_all(lv_btnmatrix_ctrl_t ctrl)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_btnmatrix_clear_btn_ctrl_all(object, ctrl);
      port::mutex_give();
    }

    /**
     * Set a single button's relative width.
     * This method will cause the matrix be regenerated and is a relatively
     * expensive operation. It is recommended that initial width be specified using
     * `lv_btnmatrix_set_ctrl_map` and this method only be used for dynamic changes.
     * @param btn_id    0 based index of the button to modify.
     * @param width     relative width compared to the buttons in the same row. [1..7]
     */
    inline void set_btn_width(uint16_t btn_id, uint8_t width)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_btnmatrix_set_btn_width(object, btn_id, width);
      port::mutex_give();
    }

    /**
     * Make the button matrix like a selector widget (only one button may be checked at a time).
     * `LV_BTNMATRIX_CTRL_CHECKABLE` must be enabled on the buttons to be selected using
     * `lv_btnmatrix_set_ctrl()` or `lv_btnmatrix_set_btn_ctrl_all()`.
     * @param en        whether "one check" mode is enabled
     */
    inline void set_one_checked(bool en)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_btnmatrix_set_one_checked(object, en);
      port::mutex_give();
    }

    /*=====================
    * Getter functions
    *====================*/

    /**
     * Get the current map of a button matrix
     * @return          the current map
     */
    const char ** get_map() const
    {
      if (object == nullptr)
        return nullptr;
      port::mutex_take();
      const char ** map = lv_btnmatrix_get_map(object);
      port::mutex_give();
      return map;
    }

    /**
     * Get the index of the lastly "activated" button by the user (pressed, released, focused etc)
     * Useful in the `event_cb` to get the text of the button, check if hidden etc.
     * @return          index of the last released button (LV_BTNMATRIX_BTN_NONE: if unset)
     */
    uint16_t get_selected_btn() const
    {
      if (object == nullptr)
        return 0;
      port::mutex_take();
      uint16_t btn_id = lv_btnmatrix_get_selected_btn(object);
      port::mutex_give();
      return btn_id;
    }

    /**
     * Get the button's text
     * @param btn_id    the index a button not counting new line characters.
     * @return          text of btn_index` button
     */
    const char * get_btn_text(uint16_t btn_id) const
    {
      if (object == nullptr)
        return nullptr;
      port::mutex_take();
      const char * text = lv_btnmatrix_get_btn_text(object, btn_id);
      port::mutex_give();
      return text;
    }

    /**
     * Get the whether a control value is enabled or disabled for button of a button matrix
     * @param btn_id    the index of a button not counting new line characters.
     * @param ctrl      control values to check (ORed value can be used)
     * @return          true: the control attribute is enabled false: disabled
     */
    bool has_btn_ctrl(uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl) const
    {
      if (object == nullptr)
        return false;
      port::mutex_take();
      bool has_ctrl = lv_btnmatrix_has_btn_ctrl(object, btn_id, ctrl);
      port::mutex_give();
      return has_ctrl;
    }

    /**
     * Tell whether "one check" mode is enabled or not.
     * @return          true: "one check" mode is enabled; false: disabled
     */
    bool get_one_checked() const
    {
      if (object == nullptr)
        return false;
      port::mutex_take();
      bool one_checked = lv_btnmatrix_get_one_checked(object);
      port::mutex_give();
      return one_checked;
    }
    
  }; // class ButtonMatrix 
} // namespace lvgl

#endif