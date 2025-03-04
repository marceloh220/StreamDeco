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
 *
 */

#ifndef _LVGL_OBJECT_HPP_
#define _LVGL_OBJECT_HPP_

#include "lvgl_types.hpp"
#include "lvgl_port.hpp"
#include "lvgl_style.hpp"

/**
 * @namespace lvgl.
 * @brief     Basic LVGL space.
 */
namespace lvgl
{

  /**
   * @brief   Basic object class to LVGL warp.
   * @details Any widget can inherit this class as base.
   */
  class Object
  {

  public:
    
    Object() {}

    /**
     * @brief Delete the object on descontruction 
     */
    ~Object() {del();}

    /**
     * @brief   Get the pointer to address of this object into LVGL memory pool
     * @return  Pointer address to his object into LVGL memory pool
     */
    inline object_t *get_object() { return object; }

    /**
     * @sa    Object::hidden()
     * @brief Delete the object
     * @note  This method frees up the object's memory space in the LVGL pool and make the object unusable.
     *        The object can be recreated later, but its characteristics will be lost and will need to be reconfigured.
     *        To stop showing the object for a while, use the Object::hidden method.
     */
    inline void del()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_del(object);
      object = NULL;
      port::mutex_give();
    }

    /**
     * @brief Move the object to the foreground.
     * @note  It will look like if it was created as the last child of its parent.
     *        It also means it can cover any of the siblings.
     */
    inline void move_foreground()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      object_t *parent = lv_obj_get_parent(object);
      lv_obj_move_to_index(object, lv_obj_get_child_cnt(parent) - 1);
      port::mutex_give();
    }

    /**
     * @brief  Move the object to the background.
     *@details It will look like if it was created as the first child of its parent.
     *         It also means any of the siblings can cover the object.
     */
    inline void move_background()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_move_to_index(object, 0);
      port::mutex_give();
    }

    /**
     * @brief Set one or more flags
     * @param f     R-ed values from `object_flag_t` to set.
     */
    inline void add_flag(object_flag_t f)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_add_flag(object, f);
      port::mutex_give();
    }

    /**
     * @brief Clear one or more flags
     * @param flag     OR-ed values from `object_flag_t` to set.
     */
    inline void clear_flag(object_flag_t flag)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_clear_flag(object, flag);
      port::mutex_give();
    }

    /**
     * @brief Add one or more states to the object. The other state bits will remain
     *        unchanged. If specified in the styles, transition animation will be started
     *        from the previous state to the current.
     * @param state     the states to add.
     *                  E.g `state::STATE_PRESSED |
     * state::STATE_FOCUSED`
     */
    inline void add_state(state_t state)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_add_state(object, state);
      port::mutex_give();
    }

    /**
     * @brief Remove one or more states to the object. The other state bits will remain
     *        unchanged. If specified in the styles, transition animation will be started
     *        from the previous state to the current.
     * @param state     the states to add.
     *                  E.g `state::STATE_PRESSED | state::STATE_FOCUSED`
     */
    inline void clear_state(state_t state)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_clear_state(object, state);
      port::mutex_give();
    }

/**
 * @brief Set the user_data field of the object
 * @param user_data   pointer to the new user_data.
 */
#if LV_USE_USER_DATA
    template <typename type>
    inline void lv_obj_set_user_data(type user_data)
    {
      object->user_data = (void *)user_data;
    }
#endif

    /**
     * @brief Check if a given flag or all the given flags are set on an object.
     * @param flag  the flag(s) to check (OR-ed values can be used)
     * @return      true: all flags are set; false: not all flags are set
     */
    inline bool has_flag(object_flag_t flag)
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_has_flag(object, flag);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Check if a given flag or any of the flags are set on an object.
     * @param flag  the flag(s) to check (OR-ed values can be used)
     * @return      true: at lest one flag flag is set;
     *              false: none of the flags are set
     */
    inline bool has_flag_any(object_flag_t flag)
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_has_flag_any(object, flag);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the state of an object
     * @return      the state (OR-ed values from `state_t`)
     */
    inline state_t get_state()
    {
      state_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_state(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Check if the object is in a given state or not.
     * @param state     a state or combination of states to check
     * @return          true: `obj` is in `state`;
     *                  false: `obj` is not in `state`
     */
    inline bool has_state(state_t state)
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_has_state(object, state);
      port::mutex_give();
      return ret;
    }

/**
 * @brief Get the user_data field of the object
 * @return      the pointer to the user_data of the object
 */
#if LV_USE_USER_DATA
    template <typename type>
    inline type get_user_data()
    {
      return (type)object->user_data;
    }
#endif

    /**
     * @brief Set the position of an object relative to the set alignment.
     * @param x         new x coordinate
     * @param y         new y coordinate
     * @note            With default alignment it's the distance from the top left corner
     * @note            E.g. lvgl::alignment::CENTER alignment it's the offset from the center of the parent
     * @note            The position is interpreted on the content area of the parent
     * @note            The values can be set in pixel or in percentage of parent size with `lv_pct(v)`
     */
    inline void set_pos(coordinates_t x, coordinates_t y)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_pos(object, x, y);
      port::mutex_give();
    }

    /**
     * @brief Set the x coordinate of an object
     * @param x         new x coordinate
     * @note            With default alignment it's the distance from the top left corner
     * @note            E.g. lvgl::alignment::CENTER alignment it's the offset from the center of the parent
     * @note            The position is interpreted on the content area of the parent
     * @note            The values can be set in pixel or in percentage of parent size with `lv_pct(v)`
     */
    inline void set_x(coordinates_t x)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_x(object, x);
      port::mutex_give();
    }

    /**
     * @brief Set the y coordinate of an object
     * @param y         new y coordinate
     * @note            With default alignment it's the distance from the top left corner
     * @note            E.g. lvgl::alignment::CENTER alignment it's the offset from the center of the parent
     * @note            The position is interpreted on the content area of the parent
     * @note            The values can be set in pixel or in percentage of parent size with `lv_pct(v)`
     */
    inline void set_y(coordinates_t y)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_y(object, y);
      port::mutex_give();
    }

    /**
     * @brief Set the size of an object.
     * @param w         the new width
     * @param h         the new height
     * @note            possible values are:
     *                  pixel               simple set the size accordingly
     *                  LV_SIZE_CONTENT     set the size to involve all children in the given direction
     *                  LV_SIZE_PCT(x)     to set size in percentage of the parent's content area size
     *                                     (the size without paddings) x should be in [0..1000]% range
     */
    inline void set_size(coordinates_t w, coordinates_t h)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_size(object, w, h);
      port::mutex_give();
    }

    /**
     * @brief Recalculate the size of the object
     * @return          true: the size has been changed
     */
    inline bool refr_size()
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_refr_size(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Set the width of an object
     * @param w         the new width
     * @note            possible values are:
     *                  pixel               simple set the size accordingly
     *                  LV_SIZE_CONTENT     set the size to involve all children in the given direction
     *                  lv_pct(x)           to set size in percentage of the parent's content area size
     *                                      (the size without paddings). x should be in [0..1000]% range
     */
    inline void set_width(coordinates_t w)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_width(object, w);
      port::mutex_give();
    }

    /**
     * @brief Set the height of an object
     * @param h         the new height
     * @note            possible values are:
     *                  pixel               simple set the size accordingly
     *                  LV_SIZE_CONTENT     set the size to involve all children in the given direction
     *                  lv_pct(x)           to set size in percentage of the parent's content area size
     *                                      (the size without paddings). x should be in [0..1000]% range
     */
    inline void set_height(coordinates_t h)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_height(object, h);
      port::mutex_give();
    }

    /**
     * @brief Set the width reduced by the left and right padding and the border width.
     * @param w         the width without paddings in pixels
     */
    inline void set_content_width(coordinates_t w)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_content_width(object, w);
      port::mutex_give();
    }

    /**
     * @brief Set the height reduced by the top and bottom padding and the border width.
     * @param h         the height without paddings in pixels
     */
    inline void set_content_height(coordinates_t h)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_content_height(object, h);
      port::mutex_give();
    }

    /**
     * @brief Set a layout for an object
     * @param layout    pointer to a layout descriptor to set
     */
    inline void set_layout(uint32_t layout)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_layout(object, layout);
      port::mutex_give();
    }

    /**
     * @brief Test whether the and object is positioned by a layout or not
     * @return true:    positioned by a layout; false: not positioned by a layout
     */
    inline bool is_layout_positioned()
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_is_layout_positioned(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Mark the object for layout update.
     */
    inline void mark_layout_as_dirty()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_mark_layout_as_dirty(object);
      port::mutex_give();
    }

    /**
     * @brief Update the layout of an object.
     */
    inline void update_layout()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_update_layout(object);
      port::mutex_give();
    }

    /**
     * @brief Change the alignment of an object.
     * @param align     type of alignment (see 'lvgl::alignment_t' enum) `lvgl::alignment::OUT_...` can't be used.
     */
    inline void set_align(alignment::alignment_t align)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_align(object, align);
      port::mutex_give();
    }

    /**
     * @brief Change the alignment of an object and set new coordinates.
     *        Equivalent to:
     *          lv_obj_set_align(obj, align);
     *          lv_obj_set_pos(obj, x_ofs, y_ofs);
     * @param align     type of alignment (see 'alignment_t' enum) `lvgl::alignment::OUT_...` can't be used.
     * @param x_ofs     x coordinate offset after alignment
     * @param y_ofs     y coordinate offset after alignment
     */
    inline void align(alignment::alignment_t align, coordinates_t x_ofs, coordinates_t y_ofs)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_align(object, align, x_ofs, y_ofs);
      port::mutex_give();
    }

    /**
     * @brief Align an object to an other object.
     * @param base      pointer to an other object (if NULL `obj`s parent is used). 'obj' will be aligned to it.
     * @param align     type of alignment (see 'alignment_t' enum)
     * @param x_ofs     x coordinate offset after alignment
     * @param y_ofs     y coordinate offset after alignment
     * @note            if the position or size of `base` changes `obj` needs to be aligned manually again
     */
    inline void align_to(Object &base, alignment::alignment_t align, coordinates_t x_ofs, coordinates_t y_ofs)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_align_to(object, base.get_object(), align, x_ofs, y_ofs);
      port::mutex_give();
    }

    /**
     * @brief Align an object to the center on its parent.
     * @note            if the parent size changes `object` needs to be aligned manually again
     */
    inline void center()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_align(object, lvgl::alignment::CENTER, 0, 0);
      port::mutex_give();
    }

    /**
     * @brief Hidden the oject.
     */
    inline void hidden()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      if (!lv_obj_has_flag(object, object::FLAG_HIDDEN))
        lv_obj_add_flag(object, object::FLAG_HIDDEN);
      port::mutex_give();
    }

    /**
     * @brief Verify if the oject is hidden.
     * @return  return TRUE if object is hidden
     */
    inline bool is_hidden()
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_has_flag(object, object::FLAG_HIDDEN);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Unhidden the oject.
     */
    inline void unhidden()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      if (lv_obj_has_flag(object, object::FLAG_HIDDEN))
      {
        lv_obj_clear_flag(object, object::FLAG_HIDDEN);
        move_foreground();
      }
      port::mutex_give();
    }

    /**
     * @brief Change object hidden state
     */
    void change_hidden() { is_hidden() ? unhidden() : hidden(); }

    /**
     * @brief Copy the coordinates of an object to an area
     * @param coords    pointer to an area to store the coordinates
     */
    inline void get_coords(area_t *coords)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_get_coords(object, coords);
      port::mutex_give();
    }

    /**
     * @brief Get the x coordinate of object.
     * @return          distance of `obj` from the left side of its parent plus the parent's left padding
     * @note            The position of the object is recalculated only on the next redraw.
     *                  To force coordinate recalculation call `lv_obj_update_layout(obj)`.
     * @note            Zero return value means the object is on the left padding of the parent,
     *                  and not on the left edge.
     * @note            Scrolling of the parent doesn't change the returned value.
     * @note            The returned value is always the distance from the parent even if `obj` is positioned by a layout.
     */
    inline coordinates_t get_x()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_x(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the x2 coordinate of object.
     * @return          distance of `obj` from the right side of its parent plus the parent's right padding
     * @note            The position of the object is recalculated only on the next redraw.
     *                  To force coordinate recalculation call `lv_obj_update_layout(obj)`.
     * @note            Zero return value means the object is on the right padding of the parent,
     *                  and not on the right edge.
     * @note            Scrolling of the parent doesn't change the returned value.
     * @note            The returned value is always the distance from the parent even if `obj` is positioned by a layout.
     */
    inline coordinates_t get_x2()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_x2(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the y coordinate of object.
     * @return          distance of `obj` from the top side of its parent plus the parent's top padding
     * @note            The position of the object is recalculated only on the next redraw.
     *                  To force coordinate recalculation call `lv_obj_update_layout(obj)`.
     * @note            Zero return value means the object is on the top padding of the parent,
     *                  and not on the top edge.
     * @note            Scrolling of the parent doesn't change the returned value.
     * @note            The returned value is always the distance from the parent even if `obj` is positioned by a layout.
     */
    inline coordinates_t get_y()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_y(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the y2 coordinate of object.
     * @return          distance of `obj` from the bottom side of its parent plus the parent's bottom padding
     * @note            The position of the object is recalculated only on the next redraw.
     *                  To force coordinate recalculation call `lv_obj_update_layout(obj)`.
     * @note            Zero return value means the object is on the bottom padding of the parent,
     *                  and not on the bottom edge.
     * @note            Scrolling of the parent doesn't change the returned value.
     * @note            The returned value is always the distance from the parent even if `obj` is positioned by a layout.
     */
    inline coordinates_t get_y2()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_y2(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the actually set x coordinate of object,
     *        i.e. the offset form the set alignment
     * @return          the set x coordinate
     */
    inline coordinates_t get_x_aligned()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_x_aligned(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the actually set y coordinate of object, i.e. the offset form the set alignment
     * @return          the set y coordinate
     */
    inline coordinates_t get_y_aligned()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_y_aligned(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the width of an object
     * @note            The position of the object is recalculated only on the next redraw.
     *                  To force coordinate recalculation call `lv_obj_update_layout(obj)`.
     * @return          the width in pixels
     */
    inline coordinates_t get_width()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_width(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the height of an object
     * @note            The position of the object is recalculated only on the next redraw.
     *                  To force coordinate recalculation call `lv_obj_update_layout(obj)`.
     * @return          the height in pixels
     */
    inline coordinates_t get_height()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_height(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the width reduced by the left and right padding and the border width.
     * @note            The position of the object is recalculated only on the next redraw.
     *                  To force coordinate recalculation call `lv_obj_update_layout(obj)`.
     * @return          the width which still fits into its parent without causing overflow
     *                  (making the parent scrollable)
     */
    inline coordinates_t get_content_width()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_content_width(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the height reduced by the top and bottom padding and the border width.
     * @note            The position of the object is recalculated only on the next redraw.
     *                  To force coordinate recalculation call `lv_obj_update_layout(obj)`.
     * @return          the height which still fits into the parent without causing overflow
     *                  (making the parent scrollable)
     */
    inline coordinates_t get_content_height()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_content_height(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Get the area reduced by the paddings and the border width.
     * @note            The position of the object is recalculated only on the next redraw.
     *                  To force coordinate recalculation call `lv_obj_update_layout(obj)`.
     * @param area      the area which still fits into the parent without causing overflow
     *                  (making the parent scrollable)
     */
    inline void get_content_coords(area_t *area)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_get_content_coords(object, area);
      port::mutex_give();
    }

    /**
     * @brief Get the width occupied by the "parts" of the widget. E.g. the width of all columns of a table.
     * @return          the width of the virtually drawn content
     * @note            This size independent from the real size of the widget.
     *                  It just tells how large the internal ("virtual") content is.
     */
    inline coordinates_t get_self_width() { return lv_obj_get_self_width(object); }

    /**
     * @brief Get the height occupied by the "parts" of the widget.
     *         E.g. the height of all rows of a table.
     * @return          the width of the virtually drawn content
     * @note            This size independent from the real size of the widget.
     *                  It just tells how large the internal ("virtual") content is.
     */
    inline coordinates_t get_self_height()
    {
      coordinates_t ret = 0;
      if (object == NULL)
        return ret;
      port::mutex_take();
      ret = lv_obj_get_self_height(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Handle if the size of the internal ("virtual") content of an object has changed.
     * @return          false: nothing happened;
     *                  true: refresh happened
     */
    inline bool refresh_self_size()
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_refresh_self_size(object);
      port::mutex_give();
      return ret;
    }

    /**
     * @brief Refresh the object position
     */
    inline void refr_pos()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_refr_pos(object);
      port::mutex_give();
    }

    /**
     * @brief Move the object to a position
     * @param x             X-position
     * @param y             Y-position
     */
    inline void move_to(coordinates_t x, coordinates_t y)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_move_to(object, x, y);
      port::mutex_give();
    }

    /**
     * @brief Move the object's children to relative position
     * @param x             X-position relative
     * @param y             Y-position relative
     */
    inline void move_children_by(coordinates_t x_diff, coordinates_t y_diff, bool ignore_floating)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_move_children_by(object, x_diff, y_diff, ignore_floating);
      port::mutex_give();
    }

    /**
     * @brief Transform a point using the angle and zoom style properties of an object
     * @param p             a point to transform, the result will be written back here too
     * @param recursive     consider the transformation properties of the parents too
     * @param inv           do the inverse of the transformation (-angle and 1/zoom)
     */
    inline void transform_point(point_t *p, bool recursive, bool inv)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_transform_point(object, p, recursive, inv);
      port::mutex_give();
    }

    /**
     * @brief Transform an area using the angle and zoom style properties of an object
     * @param area          an area to transform, the result will be written back here too
     * @param recursive     consider the transformation properties of the parents too
     * @param inv           do the inverse of the transformation (-angle and 1/zoom)
     */
    inline void get_transformed_area(lv_area_t *area, bool recursive, bool inv)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_get_transformed_area(object, area, recursive, inv);
      port::mutex_give();
    }

    /**
     * @brief Mark an area of an object as invalid.
     *        The area will be truncated to the object's area and marked for redraw.
     * @param area          the area to redraw
     */
    inline void invalidate_area(const area_t *area)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_invalidate_area(object, area);
      port::mutex_give();
    }

    /**
     * @brief Mark the object as invalid to redrawn its area
     */
    inline void invalidate()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_invalidate(object);
      port::mutex_give();
    }

    /**
     * Tell whether an area of an object is visible (even partially) now or not
     * @param area      the are to check. The visible part of the area will be
     * written back here.
     * @return true     visible; false not visible (hidden, out of parent, on
     * other screen, etc)
     */
    inline bool area_is_visible(area_t *area)
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_area_is_visible(object, area);
      port::mutex_give();
      return ret;
    }

    /**
     * Tell whether an object is visible (even partially) now or not
     * @return      true: visible; false not visible (hidden, out of parent, on
     * other screen, etc)
     */
    inline bool is_visible()
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_is_visible(object);
      port::mutex_give();
      return ret;
    }

    /**
     * Set the size of an extended clickable area
     * @param size      extended clickable area in all 4 directions [px]
     */
    inline void set_ext_click_area(coordinates_t size)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_set_ext_click_area(object, size);
      port::mutex_give();
    }

    /**
     * Get the an area where to object can be clicked.
     * It's the object's normal area plus the extended click area.
     * @param area      store the result area here
     */
    inline void get_click_area(area_t *area)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_get_click_area(object, area);
      port::mutex_give();
    }

    /**
     * Hit-test an object given a particular point in screen space.
     * @param point     screen-space point (absolute coordinate)
     * @return          true: if the object is considered under the point
     */
    inline bool hit_test(const point_t *point)
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_hit_test(object, point);
      port::mutex_give();
      return ret;
    }

    /**
     * Add a style to an object.
     * @param style     pointer to a style to add
     * @param selector  OR-ed value of parts and state to which the style should
     * be added
     * @example         lv_obj_add_style(btn, &style_btn, 0); //Default button
     * style
     * @example         lv_obj_add_style(btn, &btn_red, LV_STATE_PRESSED);
     * //Overwrite only some colors to red when pressed
     */
    void add_style(lv_style_t *style, style_selector_t selector)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_add_style(object, style, selector);
      port::mutex_give();
    }

    /**
     * Add a style to an object.
     * @param style     pointer to a style to add
     * @param selector  OR-ed value of parts and state to which the style should
     * be added
     * @example         lv_obj_add_style(btn, &style_btn, 0); //Default button
     * style
     * @example         lv_obj_add_style(btn, &btn_red, LV_STATE_PRESSED);
     * //Overwrite only some colors to red when pressed
     */
    void add_style(Style &style, style_selector_t selector)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_add_style(object, style.get_style(), selector);
      port::mutex_give();
    }

    /**
     * Add a style to an object.
     * @param style     pointer to a style to remove. Can be NULL to check only
     * the selector
     * @param selector  OR-ed values of states and a part to remove only styles
     * with matching selectors. LV_STATE_ANY and lvgl::part::ANY can be used
     * @example lv_obj_remove_style(obj, &style, lvgl::part::ANY | LV_STATE_ANY);
     * //Remove a specific style
     * @example lv_obj_remove_style(obj, NULL, lvgl::part::MAIN | LV_STATE_ANY);
     * //Remove all styles from the main part
     * @example lv_obj_remove_style(obj, NULL, lvgl::part::ANY | LV_STATE_ANY);
     * //Remove all styles
     */
    void remove_style(lv_style_t *style, style_selector_t selector)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_remove_style(object, style, selector);
      port::mutex_give();
    }

    /**
     * Add a style to an object.
     * @param style     pointer to a style to remove. Can be NULL to check only
     * the selector
     * @param selector  OR-ed values of states and a part to remove only styles
     * with matching selectors. LV_STATE_ANY and lvgl::part::ANY can be used
     * @example lv_obj_remove_style(&style, lvgl::part::ANY | LV_STATE_ANY);
     * //Remove a specific style
     * @example lv_obj_remove_style(NULL, lvgl::part::MAIN | LV_STATE_ANY);
     * //Remove all styles from the main part
     * @example lv_obj_remove_style(NULL, lvgl::part::ANY | LV_STATE_ANY);
     * //Remove all styles
     */
    void remove_style(Style &style, style_selector_t selector)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_remove_style(object, style.get_style(), selector);
      port::mutex_give();
    }

    /**
     * Remove all styles from an object
     */
    inline void remove_style_all()
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_remove_style(object, NULL,
                          (lv_style_selector_t)lvgl::part::ANY |
                              (lv_style_selector_t)LV_STATE_ANY);
      port::mutex_give();
    }

    /**
     * Notify all object if a style is modified
     * @param style     pointer to a style. Only the objects with this style will
     * be notified (NULL to notify all objects)
     */
    void report_style_change(style_t *style)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_report_style_change(style);
      port::mutex_give();
    }

    /**
     * Notify all object if a style is modified
     * @param style     pointer to a style. Only the objects with this style will
     * be notified (NULL to notify all objects)
     */
    void report_style_change(Style &style)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_report_style_change(style.get_style());
      port::mutex_give();
    }

    /**
     * Enable or disable automatic style refreshing when a new style is
     * added/removed to/from an object or any other style change happens.
     * @param en        true: enable refreshing; false: disable refreshing
     */
    void enable_style_refresh(bool en)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_enable_style_refresh(en);
      port::mutex_give();
    }

    /**
     * Add an event handler function for an object.
     * Used by the user to react on event which happens with the object.
     * An object can have multiple event handler. They will be called in the same
     * order as they were added.
     * @param callback  the new event function
     * @param filter    and event code (e.g. `LV_EVENT_CLICKED`) on which the
     * event should be called. `LV_EVENT_ALL` can be sued the receive all the
     * events.
     * @param user_data custom data data will be available in `event_cb`
     */
    template <typename type>
    void add_event_cb(lvgl::event::callback_t callback, lvgl::event::code_t filter, type user_data)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      lv_obj_add_event_cb(object, callback, (lv_event_code_t)filter, (void *)user_data);
      port::mutex_give();
    }

    /**
     * Remove an event handler function for an object.
     * @param callback  the event function to remove
     * @return          true if any event handlers were removed
     */
    bool remove_event_cb(lvgl::event::callback_t callback)
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret = lv_obj_remove_event_cb(object, callback);
      port::mutex_give();
      return ret;
    }

    /**
     * Remove an event handler function with a specific user_data from an object.
     * @param callback  the event function to remove, or `NULL` only `user_data`
     * matters.
     * @param user_data the user_data specified in ::lv_obj_add_event_cb
     * @return          true if any event handlers were removed
     */
    bool remove_event_cb_with_user_data(lvgl::event::callback_t callback,
                                        const void *user_data)
    {
      if (object == NULL)
        return false;
      port::mutex_take();
      bool ret =
          lv_obj_remove_event_cb_with_user_data(object, callback, user_data);
      port::mutex_give();
      return ret;
    }

    /**
     * The user data of an event object event callback. Always the first match
     * with `event_cb` will be returned.
     * @param callback  the event function
     * @return          the user_data
     */
    template <typename type>
    type user_data(lvgl::event::callback_t callback)
    {
      if (object == NULL)
        return;
      port::mutex_take();
      type ret = (type)lv_obj_get_event_user_data(object, callback);
      port::mutex_give();
      return ret;
    }

  protected:
    object_t *object = NULL;
  };

} // namespace lvgl

#endif
