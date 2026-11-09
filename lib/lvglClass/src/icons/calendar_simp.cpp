#include "lvgl_icons.h"
#include <array>

/** IA generated this icon I'm still trying figure out how it works */
constexpr std::array<uint8_t, 512> calendar_icon_pixels()
{
  std::array<uint8_t, 512> pixels = {};
  for (int row = 4; row < 60; ++row)
  {
    for (int column = 6; column < 58; ++column)
    {
      const bool frame = row >= 10 &&
        (row < 14 || row >= 56 || column < 10 || column >= 54 || (row >= 22 && row < 26));
      const bool binding = row < 18 &&
        ((column >= 18 && column < 22) || (column >= 42 && column < 46));
      const bool day = row >= 32 && row < 52 && column >= 16 && column < 48 &&
        (row - 32) % 8 < 4 && (column - 16) % 8 < 4;
      if (frame || binding || day)
        pixels[row * 8 + column / 8] |= 0x80 >> (column % 8);
    }
  }
  return pixels;
}

constexpr auto calendar_pixels = calendar_icon_pixels();
const lv_img_dsc_t calendar_icon = [] {
  lv_img_dsc_t icon = {};
  icon.header.cf = LV_IMG_CF_ALPHA_1BIT;
  icon.header.w = 64;
  icon.header.h = 64;
  icon.data_size = calendar_pixels.size();
  icon.data = calendar_pixels.data();
  return icon;
}();
