#ifndef __LVGL_KEYBOARD_HPP__
#define __LVGL_KEYBOARD_HPP__

namespace lvgl
{
    class TextArea: public Object
    {

        /**
         * Create a text area object
         * @param parent    pointer to an object, it will be the parent of the new text area
         * @return          pointer to the created text area
         */
        inline void create(object_t *parent = nullptr)
        {
          if (object != nullptr)
            return;
          port::mutex_take();
          if (parent == nullptr)
            parent = lv_scr_act();
          object = lv_textarea_create(parent);
          port::mutex_give();
        }

        inline void create(Object &parent)
        {
          if (object != nullptr)
            return;
          port::mutex_take();
          object = lv_textarea_create(parent.get_object());
          port::mutex_give();
        }

        /*======================
        * Add/remove functions
        *=====================*/

        /**
         * Insert a character to the current cursor position.
         * To add a wide char, e.g. 'Á' use `_lv_txt_encoded_conv_wc('Á')`
         * @param c         a character (e.g. 'a')
         */
        inline void add_char(uint32_t c)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_add_char(object, c);
          port::mutex_give();
        }

        /**
         * Insert a text to the current cursor position
         * @param txt       a '\0' terminated string to insert
         */
        inline void add_text(const char * txt)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_add_text(object, txt);
          port::mutex_give();
        }

        /**
         * Delete a the left character from the current cursor position
         */
        inline void del_char()
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_del_char(object);
          port::mutex_give();
        }

        /**
         * Delete the right character from the current cursor position
         */
        inline void del_char_forward()
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_del_char_forward(object);
          port::mutex_give();
        }

        /*=====================
        * Setter functions
        *====================*/

        /**
         * Set the text of a text area
         * @param txt       pointer to the text
         */
        inline void set_text(const char * txt)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_text(object, txt);
          port::mutex_give();
        }

        /**
         * Set the placeholder text of a text area
         * @param txt       pointer to the text
         */
        inline void set_placeholder_text(const char * txt)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_placeholder_text(object, txt);
          port::mutex_give();
        }

        /**
         * Set the cursor position
         * @param pos       the new cursor position in character index
         *                  < 0 : index from the end of the text
         *                  LV_TEXTAREA_CURSOR_LAST: go after the last character
         */
        inline void set_cursor_pos(int32_t pos)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_cursor_pos(object, pos);
          port::mutex_give();
        }

        /**
         * Enable/Disable the positioning of the cursor by clicking the text on the text area.
         * @param en        true: enable click positions; false: disable
         */
        inline void set_cursor_click_pos(bool en)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_cursor_click_pos(object, en);
          port::mutex_give();
        }

        /**
         * Enable/Disable password mode
         * @param en        true: enable, false: disable
         */
        inline void set_password_mode(bool en)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_password_mode(object, en);
          port::mutex_give();
        }

        /**
         * Set the replacement characters to show in password mode
         * @param bullet    pointer to the replacement text
         */
        inline void set_password_bullet(const char * bullet)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_password_bullet(object, bullet);
          port::mutex_give();
        }

        /**
         * Configure the text area to one line or back to normal
         * @param en        true: one line, false: normal
         */
        inline void set_one_line(bool en)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_one_line(object, en);
          port::mutex_give();
        }

        /**
         * Set a list of characters. Only these characters will be accepted by the text area
         * @param list      list of characters. Only the pointer is saved. E.g. "+-.,0123456789"
         */
        inline void set_accepted_chars(const char * list)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_accepted_chars(object, list);
          port::mutex_give();
        }

        /**
         * Set max length of a Text Area.
         * @param num       the maximal number of characters can be added (`lv_textarea_set_text` ignores it)
         */
        inline void set_max_length(uint32_t num)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_max_length(object, num);
          port::mutex_give();
        }

        /**
         * In `LV_EVENT_INSERT` the text which planned to be inserted can be replaced by an other text.
         * It can be used to add automatic formatting to the text area.
         * @param txt       pointer to a new string to insert. If `""` no text will be added.
         *                  The variable must be live after the `event_cb` exists. (Should be `global` or `static`)
         */
        inline void set_insert_replace(const char * txt)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_insert_replace(object, txt);
          port::mutex_give();
        }

        /**
         * Enable/disable selection mode.
         * @param en        true or false to enable/disable selection mode
         */
        inline void set_text_selection(bool en)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_text_selection(object, en);
          port::mutex_give();
        }

        /**
         * Set how long show the password before changing it to '*'
         * @param time      show time in milliseconds. 0: hide immediately.
         */
        inline void set_password_show_time(uint16_t time)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_password_show_time(object, time);
          port::mutex_give();
        }

        /**
         * Deprecated: use the normal text_align style property instead
         * Set the label's alignment.
         * It sets where the label is aligned (in one line mode it can be smaller than the text area)
         * and how the lines of the area align in case of multiline text area
         * @param align     the align mode from ::lv_text_align_t
         */
        inline void set_align(lv_text_align_t align)
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_set_align(object, align);
          port::mutex_give();
        }

        /*=====================
        * Getter functions
        *====================*/

        /**
         * Get the text of a text area. In password mode it gives the real text (not '*'s).
         * @return          pointer to the text
         */
        inline const char * get_text() const
        {
          if (object == nullptr)
            return nullptr;
          port::mutex_take();
          const char * text = lv_textarea_get_text(object);
          port::mutex_give();
          return text;
        }

        /**
         * Get the placeholder text of a text area
         * @return          pointer to the text
         */
        inline const char * get_placeholder_text() const
        {
          if (object == nullptr)
            return nullptr;
          port::mutex_take();
          const char * text = lv_textarea_get_placeholder_text(object);
          port::mutex_give();
          return text;
        }

        /**
         * Get the label of a text area
         * @return          pointer to the label object
         */
        inline lv_obj_t * get_label() const
        {
          if (object == nullptr)
            return nullptr;
          port::mutex_take();
          lv_obj_t * label = lv_textarea_get_label(object);
          port::mutex_give();
          return label;
        }

        /**
         * Get the current cursor position in character index
         * @return          the cursor position
         */
        inline uint32_t get_cursor_pos() const
        {
          if (object == nullptr)
            return 0;
          port::mutex_take();
          uint32_t pos = lv_textarea_get_cursor_pos(object);
          port::mutex_give();
          return pos;
        }

        /**
         * Get whether the cursor click positioning is enabled or not.
         * @return          true: enable click positions; false: disable
         */
        inline bool get_cursor_click_pos() const
        {
          if (object == nullptr)
            return false;
          port::mutex_take();
          bool enabled = lv_textarea_get_cursor_click_pos(object);
          port::mutex_give();
          return enabled;
        }

        /**
         * Get the password mode attribute
         * @return          true: password mode is enabled, false: disabled
         */
        inline bool get_password_mode() const
        {
          if (object == nullptr)
            return false;
          port::mutex_take();
          bool enabled = lv_textarea_get_password_mode(object);
          port::mutex_give();
          return enabled;
        }

        /**
         * Get the replacement characters to show in password mode
         * @return          pointer to the replacement text
         */
        inline const char * get_password_bullet() const
        {
          if (object == nullptr)
            return nullptr;
          port::mutex_take();
          const char * bullet = lv_textarea_get_password_bullet(object);
          port::mutex_give();
          return bullet;
        }

        /**
         * Get the one line configuration attribute
         * @return          true: one line configuration is enabled, false: disabled
         */
        inline bool get_one_line() const
        {
          port::mutex_take();
          bool enabled = lv_textarea_get_one_line(object);
          port::mutex_give();
          return enabled;
        }

        /**
         * Get a list of accepted characters.
         * @return          list of accented characters.
         */
        inline const char * get_accepted_chars() const
        {
          if (object == nullptr)
            return nullptr;
          port::mutex_take();
          const char * chars = lv_textarea_get_accepted_chars(object);
          port::mutex_give();
          return chars;
        }

        /**
         * Get max length of a Text Area.
         * @return          the maximal number of characters to be add
         */
        inline uint32_t get_max_length() const
        {
          if (object == nullptr)
            return 0;
          port::mutex_take();
          uint32_t num = lv_textarea_get_max_length(object);
          port::mutex_give();
          return num;
        }

        /**
         * Find whether text is selected or not.
         * @return          whether text is selected or not
         */
        inline bool text_is_selected() const
        {
          if (object == nullptr)
            return false;
          port::mutex_take();
          bool selected = lv_textarea_text_is_selected(object);
          port::mutex_give();
          return selected;
        }

        /**
         * Find whether selection mode is enabled.
         * @return          true: selection mode is enabled, false: disabled
         */
        inline bool get_text_selection() const
        {
          if (object == nullptr)
            return false;
          port::mutex_take();
          bool enabled = lv_textarea_get_text_selection(object);
          port::mutex_give();
          return enabled;
        }

        /**
         * Set how long show the password before changing it to '*'
         * @return          show time in milliseconds. 0: hide immediately.
         */
        inline uint16_t get_password_show_time() const
        {
          if (object == nullptr)
            return 0;
          port::mutex_take();
          uint16_t time = lv_textarea_get_password_show_time(object);
          port::mutex_give();
          return time;
        }

        /*=====================
        * Other functions
        *====================*/

        /**
         * Clear the selection on the text area.
         */
        inline void clear_selection()
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_clear_selection(object);
          port::mutex_give();
        }

        /**
         * Move the cursor one character right
         */
        inline void cursor_right()
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_cursor_right(object);
          port::mutex_give();
        }

        /**
         * Move the cursor one character left
         */
        inline void cursor_left()
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_cursor_left(object);
          port::mutex_give();
        }

        /**
         * Move the cursor one line down
         */
        inline void cursor_down()
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_cursor_down(object);
          port::mutex_give();
        }

        /**
         * Move the cursor one line up
         */
        inline void cursor_up()
        {
          if (object == nullptr)
            return;
          port::mutex_take();
          lv_textarea_cursor_up(object);
          port::mutex_give();
        }

    }; // class TextArea

} // namespace lvgl

#endif