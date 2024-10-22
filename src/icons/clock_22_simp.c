/**
 * Vectors and icons by Vectopus
 * https://github.com/Vectopus/Atlas-icons-font?ref=svgrepo.com
 * in MIT License via SVG Repo
 * https://www.svgrepo.com/
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


#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_CLOCK_22_SIMP
#define LV_ATTRIBUTE_IMG_CLOCK_22_SIMP
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_CLOCK_22_SIMP uint8_t clock_22_simp_map[] = {
  0x00, 0x78, 0x00, 
  0x03, 0xff, 0x00, 
  0x07, 0xff, 0x80, 
  0x1e, 0x01, 0xe0, 
  0x18, 0x30, 0x60, 
  0x30, 0x30, 0x30, 
  0x70, 0x30, 0x38, 
  0x60, 0x30, 0x18, 
  0x60, 0x30, 0x18, 
  0xe0, 0x30, 0x1c, 
  0xc0, 0x78, 0x1c, 
  0xe0, 0x78, 0x1c, 
  0xe0, 0x3c, 0x1c, 
  0x60, 0x0e, 0x18, 
  0x60, 0x07, 0x18, 
  0x70, 0x02, 0x38, 
  0x30, 0x00, 0x30, 
  0x18, 0x00, 0x60, 
  0x1e, 0x01, 0xe0, 
  0x07, 0xff, 0x80, 
  0x03, 0xff, 0x00, 
  0x00, 0x78, 0x00, 
};

const lv_img_dsc_t clock_22_simp = {
  .header.cf = LV_IMG_CF_ALPHA_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 22,
  .header.h = 22,
  .data_size = 66,
  .data = clock_22_simp_map,
};
