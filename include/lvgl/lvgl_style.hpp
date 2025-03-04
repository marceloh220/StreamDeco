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

#ifndef _LVGL_STYLE_HPP_
#define _LVGL_STYLE_HPP_

#include "lvgl_types.hpp"
#include "lvgl_color.hpp"

namespace lvgl {

class Style {
public:
  /**
   * Initialize a style
   * @note Do not call `lv_style_init` on styles that already have some
   * properties because this function won't free the used memory, just sets a
   * default state for the style. In other words be sure to initialize styles
   * only once!
   */
  Style() { lv_style_init(&_style); }

  /**
   * Clear all properties from a style and free all allocated memories.
   */
  ~Style() { lv_style_reset(&_style); }

  lv_style_t *get_style() { return &_style; }

  /**
   * Checks if a style is empty (has no properties)
   * @return true if the style is empty
   */
  bool is_empty() { return lv_style_is_empty(&_style); }

  inline void set_size(coordinates_t width, coordinates_t height) {
    lv_style_set_width(&_style, width);
    lv_style_set_height(&_style, height);
  }

  inline void set_pad_all(coordinates_t value) {
    lv_style_set_pad_left(&_style, value);
    lv_style_set_pad_right(&_style, value);
    lv_style_set_pad_top(&_style, value);
    lv_style_set_pad_bottom(&_style, value);
  }

  inline void set_pad_hor(coordinates_t value) {
    lv_style_set_pad_left(&_style, value);
    lv_style_set_pad_right(&_style, value);
  }

  inline void set_pad_ver(coordinates_t value) {
    lv_style_set_pad_top(&_style, value);
    lv_style_set_pad_bottom(&_style, value);
  }

  inline void set_pad_gap(coordinates_t value) {
    lv_style_set_pad_row(&_style, value);
    lv_style_set_pad_column(&_style, value);
  }

  void set_width(coordinates_t value) { lv_style_set_width(&_style, value); }

  void set_min_width(coordinates_t value) {
    lv_style_set_min_width(&_style, value);
  }

  void set_max_width(coordinates_t value) {
    lv_style_set_max_width(&_style, value);
  }

  void set_height(coordinates_t value) { lv_style_set_height(&_style, value); }
  void set_min_height(coordinates_t value) {
    lv_style_set_min_height(&_style, value);
  }

  void set_max_height(coordinates_t value) {
    lv_style_set_max_height(&_style, value);
  }

  void set_pos(coordinates_t x, coordinates_t y) { 
    lv_style_set_x(&_style, x);
    lv_style_set_y(&_style, y);
  }

  void set_x(coordinates_t value) { lv_style_set_x(&_style, value); }

  void set_y(coordinates_t value) { lv_style_set_y(&_style, value); }

  void set_align(alignment::alignment_t value) { lv_style_set_align(&_style, (lv_align_t)value); }

  void align(alignment::alignment_t align, coordinates_t x_ofs, coordinates_t y_ofs) {
    set_align(align);
    set_pos(x_ofs, y_ofs);
  }

  void set_transform_width(coordinates_t value) {
    lv_style_set_transform_width(&_style, value);
  }

  void set_transform_height(coordinates_t value) {
    lv_style_set_transform_height(&_style, value);
  }

  void set_translate_x(coordinates_t value) {
    lv_style_set_translate_x(&_style, value);
  }

  void set_translate_y(coordinates_t value) {
    lv_style_set_translate_y(&_style, value);
  }

  void set_transform_zoom(coordinates_t value) {
    lv_style_set_transform_zoom(&_style, value);
  }

  void set_transform_angle(coordinates_t value) {
    lv_style_set_transform_angle(&_style, value);
  }

  void set_transform_pivot_x(coordinates_t value) {
    lv_style_set_transform_pivot_x(&_style, value);
  }

  void set_transform_pivot_y(coordinates_t value) {
    lv_style_set_transform_pivot_y(&_style, value);
  }

  void set_pad_top(coordinates_t value) { lv_style_set_pad_top(&_style, value); }

  void set_pad_bottom(coordinates_t value) {
    lv_style_set_pad_bottom(&_style, value);
  }

  void set_pad_left(coordinates_t value) { lv_style_set_pad_left(&_style, value); }

  void set_pad_right(coordinates_t value) {
    lv_style_set_pad_right(&_style, value);
  }

  void set_pad_row(coordinates_t value) { lv_style_set_pad_row(&_style, value); }

  void set_pad_column(coordinates_t value) {
    lv_style_set_pad_column(&_style, value);
  }

  void set_bg_color(color_t value) { lv_style_set_bg_color(&_style, value); }

  void set_bg_color(palette::palette_t value) {
    lv_style_set_bg_color(&_style, lv_palette_main((lv_palette_t)value));
  }

  void set_bg_opa(opacity::opacity_t value) { lv_style_set_bg_opa(&_style, value); }

  void set_bg_grad_color(color_t value) {
    lv_style_set_bg_grad_color(&_style, value);
  }

  void set_bg_grad_dir(lv_grad_dir_t value) {
    lv_style_set_bg_grad_dir(&_style, value);
  }

  void set_bg_main_stop(coordinates_t value) {
    lv_style_set_bg_main_stop(&_style, value);
  }

  void set_bg_grad_stop(coordinates_t value) {
    lv_style_set_bg_grad_stop(&_style, value);
  }

  void set_bg_grad(const lv_grad_dsc_t *value) {
    lv_style_set_bg_grad(&_style, value);
  }

  void set_bg_dither_mode(lv_dither_mode_t value) {
    lv_style_set_bg_dither_mode(&_style, value);
  }

  void set_bg_img_src(const void *value) {
    lv_style_set_bg_img_src(&_style, value);
  }

  void set_bg_img_opa(opacity::opacity_t value) {
    lv_style_set_bg_img_opa(&_style, value);
  }

  void set_bg_img_recolor(color_t value) {
    lv_style_set_bg_img_recolor(&_style, value);
  }

  void set_bg_img_recolor(palette::palette_t value) {
    lv_style_set_bg_img_recolor(&_style, lv_palette_main((lv_palette_t)value));
  }

  void set_bg_img_recolor_opa(opacity::opacity_t value) {
    lv_style_set_bg_img_recolor_opa(&_style, value);
  }

  void set_bg_img_tiled(bool value) {
    lv_style_set_bg_img_tiled(&_style, value);
  }

  void set_border_color(color_t value) {
    lv_style_set_border_color(&_style, value);
  }

  void set_border_color(palette::palette_t value) {
    lv_style_set_border_color(&_style, lv_palette_main((lv_palette_t)value));
  }

  void set_border_opa(opacity::opacity_t value) {
    lv_style_set_border_opa(&_style, value);
  }

  void set_border_width(coordinates_t value) {
    lv_style_set_border_width(&_style, value);
  }

  void set_border_side(lv_border_side_t value) {
    lv_style_set_border_side(&_style, value);
  }

  void set_border_post(bool value) { lv_style_set_border_post(&_style, value); }

  void set_outline_width(coordinates_t value) {
    lv_style_set_outline_width(&_style, value);
  }

  void set_outline_color(color_t value) {
    lv_style_set_outline_color(&_style, value);
  }

  void set_outline_opa(opacity::opacity_t value) {
    lv_style_set_outline_opa(&_style, value);
  }

  void set_outline_pad(coordinates_t value) {
    lv_style_set_outline_pad(&_style, value);
  }

  void set_shadow_width(coordinates_t value) {
    lv_style_set_shadow_width(&_style, value);
  }

  void set_shadow_ofs_x(coordinates_t value) {
    lv_style_set_shadow_ofs_x(&_style, value);
  }

  void set_shadow_ofs_y(coordinates_t value) {
    lv_style_set_shadow_ofs_y(&_style, value);
  }

  void set_shadow_spread(coordinates_t value) {
    lv_style_set_shadow_spread(&_style, value);
  }

  void set_shadow_color(color_t value) {
    lv_style_set_shadow_color(&_style, value);
  }

  void set_shadow_color(palette::palette_t value) {
    lv_style_set_shadow_color(&_style, lv_palette_main((lv_palette_t)value));
  }

  void set_shadow_opa(opacity::opacity_t value) {
    lv_style_set_shadow_opa(&_style, value);
  }

  void set_img_opa(opacity::opacity_t value) { lv_style_set_img_opa(&_style, value); }

  void set_img_recolor(color_t value) {
    lv_style_set_img_recolor(&_style, value);
  }

  void set_img_recolor(palette::palette_t value) {
    lv_style_set_img_recolor(&_style, lv_palette_main((lv_palette_t)value));
  }

  void set_img_recolor_opa(opacity::opacity_t value) {
    lv_style_set_img_recolor_opa(&_style, value);
  }

  void set_line_width(coordinates_t value) {
    lv_style_set_line_width(&_style, value);
  }

  void set_line_dash_width(coordinates_t value) {
    lv_style_set_line_dash_width(&_style, value);
  }

  void set_line_dash_gap(coordinates_t value) {
    lv_style_set_line_dash_gap(&_style, value);
  }

  void set_line_rounded(bool value) {
    lv_style_set_line_rounded(&_style, value);
  }

  void set_line_color(color_t value) {
    lv_style_set_line_color(&_style, value);
  }

  void set_line_color(palette::palette_t value) {
    lv_style_set_line_color(&_style, lv_palette_main((lv_palette_t)value));
  }

  void set_line_opa(opacity::opacity_t value) { lv_style_set_line_opa(&_style, value); }

  void set_arc_width(coordinates_t value) {
    lv_style_set_arc_width(&_style, value);
  }

  void set_arc_rounded(bool value) { lv_style_set_arc_rounded(&_style, value); }

  void set_arc_color(color_t value) {
    lv_style_set_arc_color(&_style, value);
  }

  void set_arc_color(palette::palette_t value) {
    lv_style_set_arc_color(&_style, lv_palette_main((lv_palette_t)value));
  }

  void set_arc_opa(opacity::opacity_t value) { lv_style_set_arc_opa(&_style, value); }

  void set_arc_img_src(const void *value) {
    lv_style_set_arc_img_src(&_style, value);
  }

  void set_text_color(color_t value) {
    lv_style_set_text_color(&_style, value);
  }

  void set_text_color(palette::palette_t value) {
    lv_style_set_text_color(&_style, lv_palette_main((lv_palette_t)value));
  }

  void set_text_opa(opacity::opacity_t value) { lv_style_set_text_opa(&_style, value); }

  void set_text_font(font_t value) {
    lv_style_set_text_font(&_style, value);
  }

  void set_text_letter_space(coordinates_t value) {
    lv_style_set_text_letter_space(&_style, value);
  }

  void set_text_line_space(coordinates_t value) {
    lv_style_set_text_line_space(&_style, value);
  }

  void set_text_decor(text::decor_t value) {
    lv_style_set_text_decor(&_style, value);
  }

  void set_text_align(text::align_t value) {
    lv_style_set_text_align(&_style, value);
  }

  void set_radius(coordinates_t value) { lv_style_set_radius(&_style, value); }

  void set_clip_corner(bool value) { lv_style_set_clip_corner(&_style, value); }

  void set_opa(opacity::opacity_t value) { lv_style_set_opa(&_style, value); }

  void set_opa_layered(opacity::opacity_t value) {
    lv_style_set_opa_layered(&_style, value);
  }

  void set_color_filter_dsc(const lv_color_filter_dsc_t *value) {
    lv_style_set_color_filter_dsc(&_style, value);
  }

  void set_color_filter_opa(opacity::opacity_t value) {
    lv_style_set_color_filter_opa(&_style, value);
  }

  void set_anim(const lv_anim_t *value) { lv_style_set_anim(&_style, value); }

  void set_anim_time(uint32_t value) { lv_style_set_anim_time(&_style, value); }

  void set_anim_speed(uint32_t value) {
    lv_style_set_anim_speed(&_style, value);
  }

  void set_transition(const lv_style_transition_dsc_t *value) {
    lv_style_set_transition(&_style, value);
  }

  void set_blend_mode(lv_blend_mode_t value) {
    lv_style_set_blend_mode(&_style, value);
  }

  void set_layout(uint16_t value) { lv_style_set_layout(&_style, value); }

  void set_base_dir(lv_base_dir_t value) {
    lv_style_set_base_dir(&_style, value);
  }

private:
  lv_style_t _style;
};

} // namespace lvgl

#endif