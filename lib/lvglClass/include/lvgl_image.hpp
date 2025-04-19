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

#ifndef _LVGL_IMAGE_HPP_
#define _LVGL_IMAGE_HPP_

#include "lvgl_object.hpp"

namespace lvgl
{

  class Image : public Object
  {

  public:
    Image() {}

    /**
     * @brief  Create a new image
     * @param  parent object parent of the new image
     * @note   The new image will be created into parent
     * @note   If any parent is passed the image will be created into main screen
     */
    inline void create(object_t *parent = nullptr)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      if (parent == nullptr)
        parent = lv_scr_act();
      object = lv_img_create(parent);
      port::mutex_give();
    }

    /**
     * @brief  Create a new image
     * @param  parent object parent of the new image
     * @note   The new image will be created into parent
     */
    inline void create(Object &parent)
    {
      if (object != nullptr)
        return;
      port::mutex_take();
      object = lv_img_create(parent.get_object());
      port::mutex_give();
    }

    /**
     * Set the image data to display on the object
     * @param src_img   1) pointer to an ::lv_img_dsc_t descriptor (converted by LVGL's image converter) (e.g. &my_img) or
     *                  2) path to an image file (e.g. "S:/dir/img.bin") or
     *                  3) a SYMBOL (e.g. LV_SYMBOL_OK)
     */
    void set_src(icon_t src)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_img_set_src(object, src);
      port::mutex_give();
    }

    /**
     * Set an offset for the source of an image so the image will be displayed
     * from the new origin.
     * @param x         the new offset along x axis.
     */
    void set_offset_x(coordinates_t x)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_img_set_offset_x(object, x);
      port::mutex_give();
    }

    /**
     * Set an offset for the source of an image.
     * so the image will be displayed from the new origin.
     * @param y         the new offset along y axis.
     */
    void set_offset_y(coordinates_t y)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_img_set_offset_y(object, y);
      port::mutex_give();
    }

    /**
     * Set the rotation angle of the image.
     * The image will be rotated around the set pivot set by `lv_img_set_pivot()`
     * Note that indexed and alpha only images can't be transformed.
     * @param angle     rotation angle in degree with 0.1 degree resolution
     * (0..3600: clock wise)
     */
    void set_angle(int16_t angle)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_img_set_angle(object, angle);
      port::mutex_give();
    }

    /**
     * Set the rotation center of the image.
     * The image will be rotated around this point.
     * @param x         rotation center x of the image
     * @param y         rotation center y of the image
     */
    void set_pivot(coordinates_t x, coordinates_t y)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_img_set_pivot(object, x, y);
      port::mutex_give();
    }

    /**
     * Set the zoom factor of the image.
     * Note that indexed and alpha only images can't be transformed.
     * @param scale      the zoom factor.
     * @example 256 or LV_ZOOM_IMG_NONE for no zoom
     * @example <256: scale down
     * @example >256 scale up
     * @example 128 half size
     * @example 512 double size
     */
    void set_scale(float scale)
    {
      if (object == nullptr)
        return;
      if (scale < 0.0f)
        scale = 0.0f;
      uint16_t zoom = (uint16_t)(256 * scale);
      port::mutex_take();
      lv_img_set_zoom(object, zoom);
      port::mutex_give();
    }

    /**
     * Enable/disable anti-aliasing for the transformations (rotate, zoom) or not.
     * The quality is better with anti-aliasing looks better but slower.
     * @param antialias true: anti-aliased; false: not anti-aliased
     */
    void set_antialias(bool antialias)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_img_set_antialias(object, antialias);
      port::mutex_give();
    }

    /**
     * Set the image object size mode.
     *
     * @param mode      the new size mode.
     */
    void set_size_mode(lvgl::image::size_mode_t mode)
    {
      if (object == nullptr)
        return;
      port::mutex_take();
      lv_img_set_size_mode(object, (lv_img_size_mode_t)mode);
      port::mutex_give();
    }

    /**
     * Get the source of the image
     * @return          the image source (symbol, file name or ::lv-img_dsc_t for
     * C arrays)
     */
    const void *get_src()
    {
      if (object == nullptr)
        return nullptr;
      port::mutex_take();
      const void *ret = lv_img_get_src(object);
      port::mutex_give();
      return ret;
    }

    /**
     * Get the offset's x attribute of the image object.
     * @return          offset X value.
     */
    coordinates_t get_offset_x()
    {
      coordinates_t ret = 0;
      if (object == nullptr)
        return ret;
      port::mutex_take();
      ret = lv_img_get_offset_x(object);
      port::mutex_give();
      return ret;
    }

    /**
     * Get the offset's y attribute of the image object.
     * @return          offset Y value.
     */
    coordinates_t get_offset_y()
    {
      coordinates_t ret = 0;
      if (object == nullptr)
        return ret;
      port::mutex_take();
      ret = lv_img_get_offset_y(object);
      port::mutex_give();
      return ret;
    }

    /**
     * Get the rotation angle of the image.
     * @return      rotation angle in 0.1 degrees (0..3600)
     */
    uint16_t get_angle()
    {
      uint16_t ret = 0;
      if (object == nullptr)
        return ret;
      port::mutex_take();
      ret = lv_img_get_angle(object);
      port::mutex_give();
      return ret;
    }

    /**
     * Get the pivot (rotation center) of the image.
     * @param pivot     store the rotation center here
     */
    void get_pivot(point_t *pivot) { lv_img_get_pivot(object, pivot); }

    /**
     * Get the zoom factor of the image.
     * @return          zoom factor (256: no zoom)
     */
    float get_scale()
    {
      uint16_t ret = 0;
      if (object == nullptr)
        return 0.0;
      port::mutex_take();
      ret = lv_img_get_zoom(object);
      port::mutex_give();
      return ret / 256.0f;
    }

    /**
     * Get whether the transformations (rotate, zoom) are anti-aliased or not
     * @return          true: anti-aliased; false: not anti-aliased
     */
    bool get_antialias()
    {
      bool ret = false;
      if (object == nullptr)
        return ret;
      port::mutex_take();
      ret = lv_img_get_antialias(object);
      port::mutex_give();
      return ret;
    }

    /**
     * Get the size mode of the image
     * @return          element of @ref image::size_mode_t
     */
    image::size_mode_t get_size_mode()
    {
      image::size_mode_t ret = image::SIZE_MODE_VIRTUAL;
      if (object == nullptr)
        return ret;
      port::mutex_take();
      ret = (image::size_mode_t)lv_img_get_size_mode(object);
      port::mutex_give();
      return ret;
    }
  };

} // namespace lvgl

#endif