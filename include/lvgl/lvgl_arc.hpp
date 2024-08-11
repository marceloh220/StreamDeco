#ifndef _LVGL_ARC_HPP_
#define _LVGL_ARC_HPP_

#include "lvgl_object.hpp"

namespace lvgl
{

    class Arc : public Object
    {

    public:
        /**
         * Create a bar object
         * @param parent    pointer to an object, it will be the parent of the new bar
         * @return          pointer to the created bar
         */
        inline void create(lv_obj_t *parent)
        {
            if (object != NULL)
                return;
            lvgl::port::mutex_take();
            if (parent == NULL)
                parent = lv_scr_act();
            object = lv_arc_create(parent);
            port::mutex_give();
        }

        inline void create(Object &parent)
        {
            if (object != NULL)
                return;
            lvgl::port::mutex_take();
            object = lv_arc_create(parent.get_object());
            lvgl::port::mutex_give();
        }

        /**
         * Set the start angle of an arc. 0 deg: right, 90 bottom, etc.
         * @param start the start angle
         */
        void set_start_angle(uint16_t start)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_start_angle(object, start);
            port::mutex_give();
        }

        /**
         * Set the end angle of an arc. 0 deg: right, 90 bottom, etc.
         * @param end   the end angle
         */
        void set_end_angle(uint16_t end)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_end_angle(object, end);
            port::mutex_give();
        }

        /**
         * Set the start and end angles
         * @param start the start angle
         * @param end   the end angle
         */
        void set_angles(uint16_t start, uint16_t end)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_angles(object, start, end);
            port::mutex_give();
        }

        /**
         * Set the start angle of an arc background. 0 deg: right, 90 bottom, etc.
         * @param start the start angle
         */
        void set_bg_start_angle(uint16_t start)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_bg_start_angle(object, start);
            port::mutex_give();
        }

        /**
         * Set the start angle of an arc background. 0 deg: right, 90 bottom etc.
         * @param end   the end angle
         */
        void set_bg_end_angle(uint16_t end)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_bg_end_angle(object, end);
            port::mutex_give();
        }

        /**
         * Set the start and end angles of the arc background
         * @param start the start angle
         * @param end   the end angle
         */
        void set_bg_angles(uint16_t start, uint16_t end)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_bg_angles(object, start, end);
            port::mutex_give();
        }

        /**
         * Set the rotation for the whole arc
         * @param rotation  rotation angle
         */
        void set_rotation(uint16_t rotation)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_rotation(object, rotation);
            port::mutex_give();
        }

        /**
         * Set the type of arc.
         * @param mode  arc's mode
         */
        void set_mode(lv_arc_mode_t type)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_mode(object, type);
            port::mutex_give();
        }

        /**
         * Set a new value on the arc
         * @param value new value
         */
        void set_value(int16_t value)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_value(object, value);
            port::mutex_give();
        }

        /**
         * Set minimum and the maximum values of an arc
         * @param min   minimum value
         * @param max   maximum value
         */
        void set_range(int16_t min, int16_t max)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_range(object, min, max);
            port::mutex_give();
        }

        /**
         * Set a change rate to limit the speed how fast the arc should reach the pressed point.
         * @param rate      the change rate
         */
        void set_change_rate(uint16_t rate)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_set_change_rate(object, rate);
            port::mutex_give();
        }

        /**
         * Get the start angle of an arc.
         * @return      the start angle [0..360]
         */
        uint16_t get_angle_start()
        {
            uint16_t ret = 0;
            if (object == NULL)
                return ret;
            port::mutex_take();
            ret = lv_arc_get_angle_start(object);
            port::mutex_give();
            return ret;
        }

        /**
         * Get the end angle of an arc.
         * @return      the end angle [0..360]
         */
        uint16_t get_angle_end()
        {
            uint16_t ret = 0;
            if (object == NULL)
                return ret;
            port::mutex_take();
            ret = lv_arc_get_angle_end(object);
            port::mutex_give();
            return ret;
        }

        /**
         * Get the start angle of an arc background.

         * @return      the  start angle [0..360]
         */
        uint16_t get_bg_angle_start()
        {
            uint16_t ret = 0;
            if (object == NULL)
                return ret;
            port::mutex_take();
            ret = lv_arc_get_bg_angle_start(object);
            port::mutex_give();
            return ret;
        }

        /**
         * Get the end angle of an arc background.
         * @return      the end angle [0..360]
         */
        uint16_t get_bg_angle_end()
        {
            uint16_t ret;
            if (object == NULL)
                return ret;
            port::mutex_take();
            ret = lv_arc_get_bg_angle_end(object);
            port::mutex_give();
            return ret;
        }

        /**
         * Get the value of an arc
         * @return          the value of the arc
         */
        int16_t get_value()
        {
            int16_t ret;
            if (object == NULL)
                return ret;
            port::mutex_take();
            ret = lv_arc_get_value(object);
            port::mutex_give();
            return ret;
        }

        /**
         * Get the minimum value of an arc
         * @return      the minimum value of the arc
         */
        int16_t get_min_value()
        {
            int16_t ret = 0;
            if (object == NULL)
                return ret;
            port::mutex_take();
            return lv_arc_get_min_value(object);
            port::mutex_give();
            return ret;
        }

        /**
         * Get the maximum value of an arc
         * @return      the maximum value of the arc
         */
        int16_t get_max_value()
        {
            int16_t ret = 0;
            if (object == NULL)
                return ret;
            port::mutex_take();
            return lv_arc_get_max_value(object);
            port::mutex_give();
            return ret;
        }

        /**
         * Get whether the arc is type or not.
         * @return          arc's mode
         */
        lv_arc_mode_t get_mode()
        {
            lv_arc_mode_t ret = LV_ARC_MODE_NORMAL;
            if (object == NULL)
                return ret;
            port::mutex_take();
            return lv_arc_get_mode(object);
            port::mutex_give();
            return ret;
        }

        /*=====================
         * Other functions
         *====================*/

        /**
         * Align an object to the current position of the arc (knob)
         * @param obj_to_align  pointer to an object to align
         * @param r_offset      consider the radius larger with this value (< 0: for smaller radius)
         */
        void align_obj_to_angle(lv_obj_t *obj_to_align, lv_coord_t r_offset)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_align_obj_to_angle(object, obj_to_align, r_offset);
            port::mutex_give();
        }

        /**
         * Align an object to the current position of the arc (knob)
         * @param obj_to_align  pointer to an object to align
         * @param r_offset      consider the radius larger with this value (< 0: for smaller radius)
         */
        void align_obj_to_angle(Object &obj_to_align, lv_coord_t r_offset)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_align_obj_to_angle(object, obj_to_align.get_object(), r_offset);
            port::mutex_give();
        }

        /**
         * Rotate an object to the current position of the arc (knob)
         * @param obj_to_align  pointer to an object to rotate
         * @param r_offset      consider the radius larger with this value (< 0: for smaller radius)
         */
        void rotate_obj_to_angle(lv_obj_t *obj_to_rotate, lv_coord_t r_offset)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_rotate_obj_to_angle(object, obj_to_rotate, r_offset);
            port::mutex_give();
        }

        /**
         * Rotate an object to the current position of the arc (knob)
         * @param obj_to_align  pointer to an object to rotate
         * @param r_offset      consider the radius larger with this value (< 0: for smaller radius)
         */
        void rotate_obj_to_angle(Object &obj_to_rotate, lv_coord_t r_offset)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_arc_rotate_obj_to_angle(object, obj_to_rotate.get_object(), r_offset);
            port::mutex_give();
        }

    }; // class Arc

} // namespace lvgl

#endif
