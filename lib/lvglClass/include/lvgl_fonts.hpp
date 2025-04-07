#ifndef __LVGL_FONTS_HPP__
#define __LVGL_FONTS_HPP__

#include <lvgl.h>

namespace lvgl
{

  namespace font
  {

    typedef const lv_font_t *font_t;

#if LV_FONT_MONTSERRAT_8
    constexpr font_t montserrat_8 = &lv_font_montserrat_8;
#endif

#if LV_FONT_MONTSERRAT_10
    constexpr font_t montserrat_10 = &lv_font_montserrat_10;
#endif

#if LV_FONT_MONTSERRAT_12
    constexpr font_t montserrat_12 = &lv_font_montserrat_12;
#endif

#if LV_FONT_MONTSERRAT_14
    constexpr font_t montserrat_14 = &lv_font_montserrat_14;
#endif

#if LV_FONT_MONTSERRAT_16
    constexpr font_t montserrat_16 = &lv_font_montserrat_16;
#endif

#if LV_FONT_MONTSERRAT_18
    constexpr font_t montserrat_18 = &lv_font_montserrat_18;
#endif

#if LV_FONT_MONTSERRAT_20
    constexpr font_t montserrat_20 = &lv_font_montserrat_20;
#endif

#if LV_FONT_MONTSERRAT_22
    constexpr font_t montserrat_22 = &lv_font_montserrat_22;
#endif

#if LV_FONT_MONTSERRAT_24
    constexpr font_t montserrat_24 = &lv_font_montserrat_24;
#endif

#if LV_FONT_MONTSERRAT_26
    constexpr font_t montserrat_26 = &lv_font_montserrat_26;
#endif

#if LV_FONT_MONTSERRAT_28
    constexpr font_t montserrat_28 = &lv_font_montserrat_28;
#endif

#if LV_FONT_MONTSERRAT_30
    constexpr font_t montserrat_30 = &lv_font_montserrat_30;
#endif

#if LV_FONT_MONTSERRAT_32
    constexpr font_t montserrat_32 = &lv_font_montserrat_32;
#endif

#if LV_FONT_MONTSERRAT_34
    constexpr font_t montserrat_34 = &lv_font_montserrat_34;
#endif

#if LV_FONT_MONTSERRAT_36
    constexpr font_t montserrat_36 = &lv_font_montserrat_36;
#endif

#if LV_FONT_MONTSERRAT_38
    constexpr font_t montserrat_38 = &lv_font_montserrat_38;
#endif

#if LV_FONT_MONTSERRAT_40
    constexpr font_t montserrat_40 = &lv_font_montserrat_40;
#endif

#if LV_FONT_MONTSERRAT_42
    constexpr font_t montserrat_42 = &lv_font_montserrat_42;
#endif

#if LV_FONT_MONTSERRAT_44
    constexpr font_t montserrat_44 = &lv_font_montserrat_44;
#endif

#if LV_FONT_MONTSERRAT_46
    constexpr font_t montserrat_46 = &lv_font_montserrat_46;
#endif

#if LV_FONT_MONTSERRAT_48
    constexpr font_t montserrat_48 = &lv_font_montserrat_48;
#endif

#if LV_FONT_MONTSERRAT_12_SUBPX
    constexpr font_t montserrat_12_subpx = &lv_font_montserrat_12_subpx;
#endif

#if LV_FONT_MONTSERRAT_28_COMPRESSED
    constexpr font_t montserrat_28_compressed = &lv_font_montserrat_28_compressed;
#endif

#if LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    constexpr font_t dejavu_16_persian_hebrew = &lv_font_dejavu_16_persian_hebrew;
#endif

#if LV_FONT_SIMSUN_16_CJK
    constexpr font_t simsun_16_cjk = &lv_font_simsun_16_cjk;
#endif

#if LV_FONT_UNSCII_8
    constexpr font_t unscii_8 = &lv_font_unscii_8;
#endif

#if LV_FONT_UNSCII_16
    constexpr font_t unscii_16 = &lv_font_unscii_16;
#endif

  } // namespace font

} // namespace lvgl

#endif
