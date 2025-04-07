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

#ifndef __LVGL_TYPES_HPP__
#define __LVGL_TYPES_HPP__

#include <lvgl.h>

namespace lvgl
{

    typedef lv_obj_t object_t;

    typedef uint16_t state_t;

    typedef lv_style_t style_t;

    typedef uint32_t style_selector_t;

    typedef lv_coord_t coordinates_t;

    typedef lv_point_t point_t;

    typedef lv_area_t area_t;

    typedef uint32_t object_flag_t;

    typedef const lv_img_dsc_t *icon_t;

    namespace object
    {
        /**
         * On/Off features controlling the object's behavior.
         * OR-ed values are possible
         */
        enum
        {
            FLAG_HIDDEN = (1L << 0),           /**< Make the object hidden. (Like it wasn't there at all)*/
            FLAG_CLICKABLE = (1L << 1),        /**< Make the object clickable by the input devices*/
            FLAG_CLICK_FOCUSABLE = (1L << 2),  /**< Add focused state to the object when clicked*/
            FLAG_CHECKABLE = (1L << 3),        /**< Toggle checked state when the object is clicked*/
            FLAG_SCROLLABLE = (1L << 4),       /**< Make the object scrollable*/
            FLAG_SCROLL_ELASTIC = (1L << 5),   /**< Allow scrolling inside but with slower speed*/
            FLAG_SCROLL_MOMENTUM = (1L << 6),  /**< Make the object scroll further when "thrown"*/
            FLAG_SCROLL_ONE = (1L << 7),       /**< Allow scrolling only one snappable children*/
            FLAG_SCROLL_CHAIN_HOR = (1L << 8), /**< Allow propagating the horizontal scroll to a parent*/
            FLAG_SCROLL_CHAIN_VER = (1L << 9), /**< Allow propagating the vertical scroll to a parent*/
            FLAG_SCROLL_CHAIN = (FLAG_SCROLL_CHAIN_HOR | FLAG_SCROLL_CHAIN_VER),
            FLAG_SCROLL_ON_FOCUS = (1L << 10),   /**< Automatically scroll object to make it visible when focused*/
            FLAG_SCROLL_WITH_ARROW = (1L << 11), /**< Allow scrolling the focused object with arrow keys*/
            FLAG_SNAPPABLE = (1L << 12),         /**< If scroll snap is enabled on the parent it can snap to this object*/
            FLAG_PRESS_LOCK = (1L << 13),        /**< Keep the object pressed even if the press slid from the object*/
            FLAG_EVENT_BUBBLE = (1L << 14),      /**< Propagate the events to the parent too*/
            FLAG_GESTURE_BUBBLE = (1L << 15),    /**< Propagate the gestures to the parent*/
            FLAG_ADV_HITTEST = (1L << 16),       /**< Allow performing more accurate hit (click) test. E.g. consider rounded corners.*/
            FLAG_IGNORE_LAYOUT = (1L << 17),     /**< Make the object position-able by the layouts*/
            FLAG_FLOATING = (1L << 18),          /**< Do not scroll the object when the parent scrolls and ignore layout*/
            FLAG_OVERFLOW_VISIBLE = (1L << 19),  /**< Do not clip the children's content to the parent's boundary*/

            FLAG_LAYOUT_1 = (1L << 23), /**< Custom flag, free to use by layouts*/
            FLAG_LAYOUT_2 = (1L << 24), /**< Custom flag, free to use by layouts*/

            FLAG_WIDGET_1 = (1L << 25), /**< Custom flag, free to use by widget*/
            FLAG_WIDGET_2 = (1L << 26), /**< Custom flag, free to use by widget*/
            FLAG_USER_1 = (1L << 27),   /**< Custom flag, free to use by user*/
            FLAG_USER_2 = (1L << 28),   /**< Custom flag, free to use by user*/
            FLAG_USER_3 = (1L << 29),   /**< Custom flag, free to use by user*/
            FLAG_USER_4 = (1L << 30),   /**< Custom flag, free to use by user*/

        };

    } // namespace object

    namespace state
    {

        enum
        {
            STATE_DEFAULT = 0x0000,
            STATE_CHECKED = 0x0001,
            STATE_FOCUSED = 0x0002,
            STATE_FOCUS_KEY = 0x0004,
            STATE_EDITED = 0x0008,
            STATE_HOVERED = 0x0010,
            STATE_PRESSED = 0x0020,
            STATE_SCROLLED = 0x0040,
            STATE_DISABLED = 0x0080,

            STATE_USER_1 = 0x1000,
            STATE_USER_2 = 0x2000,
            STATE_USER_3 = 0x4000,
            STATE_USER_4 = 0x8000,

            STATE_ANY = 0xFFFF, /**< Special value can be used in some functions to target all states*/
        };

    } // namespace state

    namespace alignment
    {

        typedef enum
        {
            TOP_LEFT = 1,
            TOP_MID,
            TOP_RIGHT,
            BOTTOM_LEFT,
            BOTTOM_MID,
            BOTTOM_RIGHT,
            LEFT_MID,
            RIGHT_MID,
            CENTER,

            OUT_TOP_LEFT,
            OUT_TOP_MID,
            OUT_TOP_RIGHT,
            OUT_BOTTOM_LEFT,
            OUT_BOTTOM_MID,
            OUT_BOTTOM_RIGHT,
            OUT_LEFT_TOP,
            OUT_LEFT_MID,
            OUT_LEFT_BOTTOM,
            OUT_RIGHT_TOP,
            OUT_RIGHT_MID,
            OUT_RIGHT_BOTTOM,
        } alignment_t;

    } // namespace alignment

    namespace screen
    {

        typedef enum
        {
            LANDSCAPE = 0,
            PORTRAIT = 1,
            MIRROR_LANDSCAPE = 2,
            MIRROR_PORTRAIT = 3
        } rotation_t;

    } // namespace screen

    namespace event
    {

        typedef lv_event_t *event_t;

        /**
         * @brief Event callback.
         * Events are used to notify user of some action being taken on the object.
         */
        typedef void callback_t(event_t);

        /**
         * Type of event being sent to the object.
         */
        typedef enum
        {
            ALL = 0,

            /** Input device events*/
            PRESSED,             /**< The object has been pressed*/
            PRESSING,            /**< The object is being pressed (called continuously while pressing)*/
            PRESS_LOST,          /**< The object is still being pressed but slid cursor/finger off of the object */
            SHORT_CLICKED,       /**< The object was pressed for a short period of time, then released it. Not called if scrolled.*/
            LONG_PRESSED,        /**< Object has been pressed for at least `long_press_time`.  Not called if scrolled.*/
            LONG_PRESSED_REPEAT, /**< Called after `long_press_time` in every `long_press_repeat_time` ms.  Not called if scrolled.*/
            CLICKED,             /**< Called on release if not scrolled (regardless to long press)*/
            RELEASED,            /**< Called in every cases when the object has been released*/
            SCROLL_BEGIN,        /**< Scrolling begins. The event parameter is a pointer to the animation of the scroll. Can be modified*/
            SCROLL_END,          /**< Scrolling ends*/
            SCROLL,              /**< Scrolling*/
            GESTURE,             /**< A gesture is detected. Get the gesture with `lv_indev_get_gesture_dir(lv_indev_get_act());` */
            KEY,                 /**< A key is sent to the object. Get the key with `lv_indev_get_key(lv_indev_get_act());`*/
            FOCUSED,             /**< The object is focused*/
            DEFOCUSED,           /**< The object is defocused*/
            LEAVE,               /**< The object is defocused but still selected*/
            HIT_TEST,            /**< Perform advanced hit-testing*/

            /** Drawing events*/
            COVER_CHECK,        /**< Check if the object fully covers an area. The event parameter is `lv_cover_check_info_t *`.*/
            REFR_EXT_DRAW_SIZE, /**< Get the required extra draw area around the object (e.g. for shadow). The event parameter is `lv_coord_t *` to store the size.*/
            DRAW_MAIN_BEGIN,    /**< Starting the main drawing phase*/
            DRAW_MAIN,          /**< Perform the main drawing*/
            DRAW_MAIN_END,      /**< Finishing the main drawing phase*/
            DRAW_POST_BEGIN,    /**< Starting the post draw phase (when all children are drawn)*/
            DRAW_POST,          /**< Perform the post draw phase (when all children are drawn)*/
            DRAW_POST_END,      /**< Finishing the post draw phase (when all children are drawn)*/
            DRAW_PART_BEGIN,    /**< Starting to draw a part. The event parameter is `lv_obj_draw_dsc_t *`. */
            DRAW_PART_END,      /**< Finishing to draw a part. The event parameter is `lv_obj_draw_dsc_t *`. */

            /** Special events*/
            VALUE_CHANGED, /**< The object's value has changed (i.e. slider moved)*/
            INSERT,        /**< A text is inserted to the object. The event data is `char *` being inserted.*/
            REFRESH,       /**< Notify the object to refresh something on it (for the user)*/
            READY,         /**< A process has finished*/
            CANCEL,        /**< A process has been cancelled */

            /** Other events*/
            DELETE,              /**< Object is being deleted*/
            CHILD_CHANGED,       /**< Child was removed, added, or its size, position were changed */
            CHILD_CREATED,       /**< Child was created, always bubbles up to all parents*/
            CHILD_DELETED,       /**< Child was deleted, always bubbles up to all parents*/
            SCREEN_UNLOAD_START, /**< A screen unload started, fired immediately when scr_load is called*/
            SCREEN_LOAD_START,   /**< A screen load started, fired when the screen change delay is expired*/
            SCREEN_LOADED,       /**< A screen was loaded*/
            SCREEN_UNLOADED,     /**< A screen was unloaded*/
            SIZE_CHANGED,        /**< Object coordinates/size have changed*/
            STYLE_CHANGED,       /**< Object's style has changed*/
            LAYOUT_CHANGED,      /**< The children position has changed due to a layout recalculation*/
            GET_SELF_SIZE,       /**< Get the internal size of a widget*/

            _LAST, /** Number of default events*/

            PREPROCESS = 0x80, /** This is a flag that can be set with an event so it's processed
                                            before the class default event processing */
        } code_t;

    } // namespace event

    namespace part
    {

        typedef enum
        {
            MAIN = 0x000000,         /**< A background like rectangle*/
            SCROLLBAR = 0x010000,    /**< The scrollbar(s)*/
            INDICATOR = 0x020000,    /**< Indicator, e.g. for slider, bar, switch, or the tick box of the checkbox*/
            KNOB = 0x030000,         /**< Like handle to grab to adjust the value*/
            SELECTED = 0x040000,     /**< Indicate the currently selected option or section*/
            ITEMS = 0x050000,        /**< Used if the widget has multiple similar elements (e.g. table cells)*/
            TICKS = 0x060000,        /**< Ticks on scale e.g. for a chart or meter*/
            CURSOR = 0x070000,       /**< Mark a specific place e.g. for text area's cursor or on a chart*/
            CUSTOM_FIRST = 0x080000, /**< Extension point for custom widgets*/
            ANY = 0x0F0000,          /**< Special value can be used in some functions to target all parts*/
        } lv_part_t;

    } // namespace part

    namespace opacity
    {

        typedef enum
        {
            OPA_TRANSP = 0,
            OPA_0 = 0,
            OPA_10 = 25,
            OPA_20 = 51,
            OPA_30 = 76,
            OPA_40 = 102,
            OPA_50 = 127,
            OPA_60 = 153,
            OPA_70 = 178,
            OPA_80 = 204,
            OPA_90 = 229,
            OPA_100 = 255,
            OPA_COVER = 255,
        } opacity_t;

    } // namespace opacity

    namespace text
    {

        typedef enum
        {
            ALIGN_AUTO,   /**< Align text auto*/
            ALIGN_LEFT,   /**< Align text to left*/
            ALIGN_CENTER, /**< Align text to center*/
            ALIGN_RIGHT,  /**< Align text to right*/
        } align_t;

        typedef enum
        {
            DECOR_NONE = 0x00,
            DECOR_UNDERLINE = 0x01,
            DECOR_STRIKETHROUGH = 0x02,
        } decor_t;

        typedef enum
        {
            LONG_WRAP,            /**< Keep the object width, wrap the too long lines and expand the object height*/
            LONG_DOT,             /**< Keep the size and write dots at the end if the text is too long*/
            LONG_SCROLL,          /**< Keep the size and roll the text back and forth*/
            LONG_SCROLL_CIRCULAR, /**< Keep the size and roll the text circularly*/
            LONG_CLIP,            /**< Keep the size and clip the text out of it*/
        } long_mode_t;

    } // namespace text

    namespace animation
    {

        typedef enum
        {
            OFF,
            ON,
        } enable_t;

    } // namespace animation

    namespace arc
    {

        typedef enum
        {
            NORMAL,
            SYMMETRICAL,
            REVERSE
        } mode_t;

    } // namespace arc

    namespace image
    {
        /**
         * Image size mode, when image size and object size is different
         */
        typedef enum
        {
            /** Zoom doesn't affect the coordinates of the object,
             *  however if zoomed in the image is drawn out of the its coordinates.
             *  The layout's won't change on zoom */
            SIZE_MODE_VIRTUAL = 0,

            /** If the object size is set to SIZE_CONTENT, then object size equals zoomed image size.
             *  It causes layout recalculation.
             *  If the object size is set explicitly, the image will be cropped when zoomed in.*/
            SIZE_MODE_REAL,
        } size_mode_t;

    } // namespace image

    namespace slider
    {

        enum
        {
            MODE_NORMAL = LV_BAR_MODE_NORMAL,
            MODE_SYMMETRICAL = LV_BAR_MODE_SYMMETRICAL,
            MODE_RANGE = LV_BAR_MODE_RANGE
        };

        typedef uint8_t mode_t;

    } // namespace slider

} // namespace lvgl

#endif
