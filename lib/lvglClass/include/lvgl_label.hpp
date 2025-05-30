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

#ifndef _LVGL_LABEL_HPP_
#define _LVGL_LABEL_HPP_

#include "lvgl_types.hpp"
#include "lvgl_fonts.hpp"
#include "lvgl_object.hpp"
#include "lvgl_color.hpp"
#include "esp_heap_caps.h"
#include "stdio.h"

namespace lvgl
{

  class Label : public Object
  {

  public:
    Label() {}

    ~Label()
    {
      port::mutex_take();
      del();
      port::mutex_give();
    }

    /**
     * @brief  Create a new label
     * @param  parent object parent of the new label
     * @note   The new label will be created into parent
     * @note   If any parent is passed the slider will be created into main screen
     */
    inline void create(object_t *parent = nullptr)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      if (parent == nullptr)
        parent = lv_scr_act();
      object = lv_label_create(parent);
      port::mutex_give();
    }

    /**
     * @brief  Create a new label
     * @param  parent object parent of the new label
     * @note   The new label will be created into parent
     */
    inline void create(Object &parent)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      object = lv_label_create(parent.get_object());
      port::mutex_give();
    }

    /**
     * Set a new text for a label. Memory will be allocated to store the text by
     * the label.
     * @param text          '\0' terminated character string. nullptr to refresh with
     * the current text.
     */
    void set_text(const char *text)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_label_set_text(object, text);
      port::mutex_give();
    }

    /**
     * Set a new formatted text for a label. Memory will be allocated to store the
     * text by the label.
     * @param fmt           `printf`-like format
     * @example lv_label_set_text_fmt(label1, "%d user", user_num);
     */
    void set_text_fmt(const char *fmt, ...)
    {
      if (object == nullptr)
        return;
      va_list args;
      va_start(args, fmt);
      int size = vsnprintf(nullptr, 0, fmt, args);
      va_end(args);
      char *buffer = (char *)heap_caps_malloc(size + 1, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
      va_start(args, fmt);
      vsnprintf(buffer, size + 1, fmt, args);
      va_end(args);
      port::mutex_take();
      lv_label_set_text(object, buffer);
      heap_caps_free(buffer);
      port::mutex_give();
    }

    /**
     * Set a static text. It will not be saved by the label so the 'text' variable
     * has to be 'alive' while the label exists.
     * @param text          pointer to a text. nullptr to refresh with the current
     * text.
     */
    void set_text_static(const char *text)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_label_set_text_static(object, text);
      port::mutex_give();
    }

    /**
     * Set the behavior of the label with longer text then the object size
     * @param long_mode     the new mode from 'lv_label_long_mode' enum.
     *                      In LV_LONG_WRAP/DOT/SCROLL/SCROLL_CIRC the size of the
     * label should be set AFTER this function
     */
    void set_long_mode(text::long_mode_t long_mode)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_label_set_long_mode(object, (text::long_mode_t)long_mode);
      port::mutex_give();
    }

    /**
     * Enable the recoloring by in-line commands
     * @param en            true: enable recoloring, false: disable
     * @example "This is a #ff0000 red# word"
     */
    void set_recolor(bool en)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_label_set_recolor(object, en);
      port::mutex_give();
    }

    /**
     * Set where text selection should start
     * @param index     character index from where selection should start.
     * `LV_LABEL_TEXT_SELECTION_OFF` for no selection
     */
    void set_text_sel_start(uint32_t index)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_label_set_text_sel_start(object, index);
      port::mutex_give();
    }

    /**
     * Set where text selection should end
     * @param index     character index where selection should end.
     * `LV_LABEL_TEXT_SELECTION_OFF` for no selection
     */
    void set_text_sel_end(uint32_t index)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_label_set_text_sel_end(object, index);
      port::mutex_give();
    }

    /**
     * Get the text of a label
     * @return          the text of the label
     */
    char *get_text()
    {
      if (object == nullptr)
        return nullptr;
      port::mutex_take();
      char *ret = lv_label_get_text(object);
      port::mutex_give();
      return ret;
    }

    /**
     * Get the long mode of a label
     * @return          the current long mode
     */
    text::long_mode_t get_long_mode()
    {
      lv_label_long_mode_t ret = 0;
      if (object == nullptr)
        return (text::long_mode_t)ret;
      port::mutex_take();
      ret = lv_label_get_long_mode(object);
      port::mutex_give();
      return (text::long_mode_t)ret;
    }

    /**
     * Get the recoloring attribute
     * @return          true: recoloring is enabled, false: disable
     */
    bool get_recolor()
    {
      bool ret = false;
      if (object == nullptr)
        return ret;
      port::mutex_take();
      ret = lv_label_get_recolor(object);
      port::mutex_give();
      return ret;
    }

    /**
     * Get the relative x and y coordinates of a letter
     * @param index     index of the character [0 ... text length - 1].
     *                  Expressed in character index, not byte index (different in
     * UTF-8)
     * @param pos       store the result here (E.g. index = 0 gives 0;0
     * coordinates if the text if aligned to the left)
     */
    void get_letter_pos(uint32_t char_id, point_t *pos)
    {
      lv_label_get_letter_pos(object, char_id, (lv_point_t *)pos);
    }

    /**
     * Get the index of letter on a relative point of a label.
     * @param pos       pointer to point with coordinates on a the label
     * @return          The index of the letter on the 'pos_p' point (E.g. on 0;0
     * is the 0. letter if aligned to the left) Expressed in character index and
     * not byte index (different in UTF-8)
     */
    uint32_t get_letter_on(const object_t *object, point_t *pos_in)
    {
      uint32_t ret = 0;
      if (object == nullptr)
        return ret;
      port::mutex_take();
      ret = lv_label_get_letter_on(object, (lv_point_t *)pos_in);
      port::mutex_give();
      return ret;
    }

    /**
     * Check if a character is drawn under a point.
     * @param pos       Point to check for character under
     * @return          whether a character is drawn under the point
     */
    bool is_char_under_pos(point_t *pos)
    {
      bool ret = false;
      if (object == nullptr)
        return ret;
      port::mutex_take();
      ret = lv_label_is_char_under_pos(object, (lv_point_t *)pos);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the selection start index.
     * @return          selection start index. `LV_LABEL_TEXT_SELECTION_OFF` if
     * nothing is selected.
     */
    uint32_t get_text_selection_start()
    {
      return lv_label_get_text_selection_start(object);
    }

    /**
     * @brief Get the selection end index.
     * @return          selection end index. `LV_LABEL_TXT_SEL_OFF` if nothing is
     * selected.
     */
    uint32_t get_text_selection_end()
    {
      return lv_label_get_text_selection_end(object);
    }

    /**
     * Insert a text to a label. The label text can not be static.
     * @param pos       character index to insert. Expressed in character index
     * and not byte index. 0: before first char. LV_LABEL_POS_LAST: after last
     * char.
     * @param txt       pointer to the text to insert
     */
    void ins_text(uint32_t pos, const char *txt)
    {
      port::mutex_take();
      lv_label_ins_text(object, pos, txt);
      port::mutex_give();
    }

    /**
     * Delete characters from a label. The label text can not be static.
     * @param pos       character index from where to cut. Expressed in character
     * index and not byte index. 0: start in from of the first character
     * @param cnt       number of characters to cut
     */
    void cut_text(uint32_t pos, uint32_t cnt)
    {
      port::mutex_take();
      lv_label_cut_text(object, pos, cnt);
      port::mutex_give();
    }

    void set_style_text_color(lvgl::color_t value)
    {
      port::mutex_take();
      lv_style_value_t v = {.color = value};
      lv_obj_set_local_style_prop(object, LV_STYLE_TEXT_COLOR, v, 0);
      port::mutex_give();
    }

    void set_style_text_opa(opacity::opacity_t value)
    {
      port::mutex_take();
      lv_style_value_t v = {.num = (int32_t)value};
      lv_obj_set_local_style_prop(object, LV_STYLE_TEXT_OPA, v, 0);
      port::mutex_give();
    }

    void set_style_text_font(font::font_t value)
    {
      port::mutex_take();
      lv_style_value_t v = {.ptr = value};
      lv_obj_set_local_style_prop(object, LV_STYLE_TEXT_FONT, v, 0);
      port::mutex_give();
    }

    void set_style_text_letter_space(coordinates_t value)
    {
      port::mutex_take();
      lv_style_value_t v = {.num = (int32_t)value};
      lv_obj_set_local_style_prop(object, LV_STYLE_TEXT_LETTER_SPACE, v, 0);
      port::mutex_give();
    }

    void set_style_text_line_space(coordinates_t value)
    {
      port::mutex_take();
      lv_style_value_t v = {.num = (int32_t)value};
      lv_obj_set_local_style_prop(object, LV_STYLE_TEXT_LINE_SPACE, v, 0);
      port::mutex_give();
    }

    void set_style_text_decor(text::decor_t value)
    {
      port::mutex_take();
      lv_style_value_t v = {.num = (int32_t)value};
      lv_obj_set_local_style_prop(object, LV_STYLE_TEXT_DECOR, v, 0);
      port::mutex_give();
    }

    void set_style_text_align(text::align_t value)
    {
      port::mutex_take();
      lv_style_value_t v = {.num = (int32_t)value};
      lv_obj_set_local_style_prop(object, LV_STYLE_TEXT_ALIGN, v, 0);
      port::mutex_give();
    }
  };

} // namespace lvgl

#endif