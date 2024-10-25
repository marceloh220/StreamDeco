#ifndef _LVGL_BAR_HPP_
#define _LVGL_BAR_HPP_

#include "lvgl_object.hpp"

namespace lvgl
{

    class Bar : public Object
    {

    public:
        /**
         * Create a bar object
         * @param parent    pointer to an object, it will be the parent of the new bar
         */
        inline void create(lv_obj_t *parent = NULL)
        {
            if (object != NULL)
                return;
            port::mutex_take();
            if (parent == NULL)
                parent = lv_scr_act();
            object = lv_bar_create(parent);
            port::mutex_give();
        }

        inline void create(Object &parent)
        {
            if (object != NULL)
                return;
            port::mutex_take();
            object = lv_bar_create(parent.get_object());
            port::mutex_give();
        }

        /**
         * Set a new value on the bar
         * @param value     new value
         * @param anim      LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
         */
        void set_value(int32_t value, lv_anim_enable_t anim)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_bar_set_value(object, value, anim);
            port::mutex_give();
        }

        /**
         * Set a new start value on the bar
         * @param value     new start value
         * @param anim      LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
         */
        void set_start_value(int32_t start_value, lv_anim_enable_t anim)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_bar_set_start_value(object, start_value, anim);
            port::mutex_give();
        }

        /**
         * Set minimum and the maximum values of a bar
         * @param min       minimum value
         * @param max       maximum value
         */
        void set_range(int32_t min, int32_t max)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_bar_set_range(object, min, max);
            port::mutex_give();
        }

        /**
         * Set the type of bar.
         * @param mode      bar type from ::lv_bar_mode_t
         */
        void set_mode(lv_bar_mode_t mode)
        {
            if (object == NULL)
                return;
            port::mutex_take();
            lv_bar_set_mode(object, mode);
            port::mutex_give();
        }

        /**
         * Get the value of a bar
         * @return          the value of the bar
         */
        int32_t get_value()
        {
            int32_t ret = 0;
            if (object == NULL)
                return ret;
            port::mutex_take();
            ret = lv_bar_get_value(object);
            port::mutex_give();
            return ret;
        }

        /**
         * Get the start value of a bar
         * @return          the start value of the bar
         */
        int32_t get_start_value()
        {
            int32_t ret = 0;
            if (object == NULL)
                return ret;
            port::mutex_take();
            ret = lv_bar_get_start_value(object);
            port::mutex_give();
            return ret;
        }

        /**
         * Get the minimum value of a bar
         * @return          the minimum value of the bar
         */
        int32_t get_min_value()
        {
            int32_t ret = 0;
            if (object == NULL)
                return ret;
            port::mutex_take();
            ret = lv_bar_get_min_value(object);
            port::mutex_give();
            return ret;
        }

        /**
         * Get the maximum value of a bar
         * @return          the maximum value of the bar
         */
        int32_t get_max_value()
        {
            int32_t ret = 0;
            if (object == NULL)
                return ret;
            port::mutex_take();
            ret = lv_bar_get_max_value(object);
            port::mutex_give();
            return ret;
        }
        
        /**
         * Get the type of bar.
         * @param obj       pointer to bar object
         * @return          bar type from ::lv_bar_mode_t
         */
        lv_bar_mode_t get_mode()
        {
            int32_t ret = 0;
            if (object == NULL)
                return ret;
            port::mutex_take();
            ret = lv_bar_get_mode(object);
            port::mutex_give();
            return ret;
        }

    }; // class Bar

} // namespace lvgl

#endif
