/**
 * Copyright © 2024 Marcelo H Moraes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "streamDeco_buttons.hpp"

namespace streamDeco
{

    static lv_position_t uiButtons_position_map_landscape[] = {
        {.x = -296, .y = -148},
        {.x = -148, .y = -148},
        {.x = 0, .y = -148},
        {.x = 148, .y = -148},
        {.x = 296, .y = -148},
        {.x = -296, .y = 0},
        {.x = -148, .y = 0},
        {.x = 0, .y = 0},
        {.x = 148, .y = 0},
        {.x = 296, .y = 0},
        {.x = -296, .y = 148},
        {.x = -148, .y = 148},
        {.x = 0, .y = 148},
        {.x = 148, .y = 148},
        {.x = 296, .y = 148},
    };

    static lv_position_t uiButtons_position_map_porttrait[] = {
        {.x = -148, .y = -296},
        {.x = 0, .y = -296},
        {.x = 148, .y = -296},
        {.x = -148, .y = -148},
        {.x = 0, .y = -148},
        {.x = 148, .y = -148},
        {.x = -148, .y = 0},
        {.x = 0, .y = 0},
        {.x = 148, .y = 0},
        {.x = -148, .y = 148},
        {.x = 0, .y = 146},
        {.x = 148, .y = 148},
        {.x = -148, .y = 296},
        {.x = 0, .y = 296},
        {.x = 148, .y = 296},
    };

    static lv_position_t layerButtons_position_map[]{
        {.x = 52, .y = 10},
        {.x = 200, .y = 10},
        {.x = 348, .y = 10},
        {.x = 52, .y = 150},
        {.x = 200, .y = 150},
        {.x = 348, .y = 150},
        {.x = 52, .y = 290},
        {.x = 200, .y = 290},
        {.x = 348, .y = 290},
    };

    static lv_position_t configButtons_position_map[]{
        {.x = 20, .y = 10},
        {.x = 168, .y = 10},
        {.x = 316, .y = 10},
        {.x = 20, .y = 150},
        {.x = 168, .y = 150},
        {.x = 316, .y = 150},
        {.x = 20, .y = 290},
        {.x = 168, .y = 290},
        {.x = 316, .y = 290},
    };

    void mainButton::create(uint8_t pos)
    {
        if (created)
            return;
        if (pos >= 15)
            return;
        lvgl::port::mutex_take();
        lv_color_t color = lv_color_make(41, 45, 50);
        style_buttonFixed.set_bg_color(color);
        style_buttonFixed.set_bg_grad_color(color);
        style_buttonFixed.set_outline_color(color);
        style_buttonFixed.set_border_color(lv_color_black());
        style_button.set_radius(6);
        style_button.set_bg_opa(LV_OPA_100);
        style_button.set_bg_color(LV_PALETTE_PURPLE);
        style_button.set_bg_grad_color(lv_palette_darken(LV_PALETTE_PURPLE, 2));
        style_button.set_bg_grad_dir(LV_GRAD_DIR_VER);
        style_button.set_border_opa(LV_OPA_40);
        style_button.set_border_width(2);
        style_button.set_border_color(lv_palette_main(LV_PALETTE_GREY));
        style_button.set_shadow_width(0);
        style_button.set_shadow_color(lv_palette_main(LV_PALETTE_GREY));
        style_button.set_shadow_ofs_y(0);
        style_button.set_outline_opa(LV_OPA_COVER);
        style_button.set_outline_color(lv_palette_main(LV_PALETTE_PURPLE));
        style_button.set_text_color(lv_color_white());
        style_button.set_pad_all(10);
        style_buttonPressed.set_outline_width(30);
        style_buttonPressed.set_outline_opa(LV_OPA_TRANSP);
        style_buttonPressed.set_translate_y(5);
        style_buttonPressed.set_shadow_ofs_y(3);
        style_buttonPressed.set_bg_color(lv_palette_darken(LV_PALETTE_PURPLE, 2));
        style_buttonPressed.set_bg_grad_color(
            lv_palette_darken(LV_PALETTE_PURPLE, 4));
        object = lv_btn_create(lv_scr_act());
        created = true;
        remove_style_all();
        add_style(style_button, LV_STATE_DEFAULT);
        add_style(style_buttonPressed, LV_STATE_PRESSED);
        set_size(128, 128);
        lv_disp_rot_t rotation = lvgl::screen::get_rotation();
        if (rotation == LV_DISP_ROT_NONE || rotation == LV_DISP_ROT_180)
        {
            align(LV_ALIGN_CENTER, uiButtons_position_map_landscape[pos].x,
                  uiButtons_position_map_landscape[pos].y);
        }
        else
        {
            align(LV_ALIGN_CENTER, uiButtons_position_map_porttrait[pos].x,
                  uiButtons_position_map_porttrait[pos].y);
        }
        if (icon1_scr != NULL)
        {
            icon.create(*this);
            icon.center();
            icon.set_src(icon1_scr);
            style_icon.set_img_recolor(lv_color_white());
            style_icon.set_img_recolor_opa(100);
        }
        else
        {
            label.create(*this);
            label.set_text(text_scr);
            label.center();
        }
        lvgl::port::mutex_give();
    } // mainButton::create

    void mainButton::position(uint8_t pos)
    {
        if (!created)
            return;
        if (pos >= 15)
            return;
        lvgl::port::mutex_take();
        lv_disp_rot_t rotation = lvgl::screen::get_rotation();
        if (rotation == LV_DISP_ROT_NONE || rotation == LV_DISP_ROT_180)
        {
            align(LV_ALIGN_CENTER, uiButtons_position_map_landscape[pos].x,
                  uiButtons_position_map_landscape[pos].y);
        }
        else
        {
            align(LV_ALIGN_CENTER, uiButtons_position_map_porttrait[pos].x,
                  uiButtons_position_map_porttrait[pos].y);
        }
        update_layout();
        lvgl::port::mutex_give();
    } // mainButton::position

    void mainButton::text(const char *text)
    {
        if (!created)
            return;
        lvgl::port::mutex_take();
        label.set_text(text);
        lvgl::port::mutex_give();
    } // mainButton::text

    void mainButton::color(lv_palette_t color)
    {
        if (!created)
            return;
        lvgl::port::mutex_take();
        style_button.set_bg_color(lv_palette_main(color));
        style_button.set_bg_grad_color(lv_palette_darken(color, 2));
        style_button.set_outline_color(lv_palette_main(color));
        style_buttonPressed.set_bg_color(lv_palette_darken(color, 2));
        style_buttonPressed.set_bg_grad_color(lv_palette_darken(color, 4));
        lvgl::port::mutex_give();
    } // mainButton::color

    void mainButton::iconColor(lv_palette_t color)
    {
        if (!created)
            return;
        lvgl::port::mutex_take();
        style_icon.set_img_recolor(color);
        lvgl::port::mutex_give();
    } // mainButton::iconColor

    void mainButton::iconColor(lv_color_t color)
    {
        if (!created)
            return;
        lvgl::port::mutex_take();
        style_icon.set_img_recolor(color);
        lvgl::port::mutex_give();
    } // mainButton::iconColor

    void mainButton::callback(lvgl::event::callback_t callback, lv_event_code_t code, int user_data)
    {
        if (!created)
            return;
        lvgl::port::mutex_take();
        add_event_cb(callback, code, (void *)user_data);
        lvgl::port::mutex_give();
    } // mainButton::callback

    void mainButton::swapIcon()
    {
        if (!created)
            return;
        if (icon1_scr == NULL && icon2_scr == NULL)
            return;

        icon_now = !icon_now;
        icon_now == true ? icon1() : icon2();
    } // mainButton::swapIcon

    void mainButton::fixe()
    {
        add_style(style_buttonFixed, LV_STATE_DEFAULT);
        add_style(style_buttonFixed, LV_STATE_PRESSED);
        icon.add_style(style_icon, LV_PART_MAIN);
        fixed = true;
    } // mainButton::fixe

    void mainButton::unfixe()
    {
        remove_style(style_buttonFixed, LV_STATE_DEFAULT);
        remove_style(style_buttonFixed, LV_STATE_PRESSED);
        icon.remove_style(style_icon, LV_PART_MAIN);
        fixed = false;
    } // mainButton::unfixe

    bool mainButton::is_fixed() { return fixed; }

    void mainButton::icon1()
    {
        if (icon1_scr != NULL)
        {
            lvgl::port::mutex_take();
            icon.set_src(icon1_scr);
            lvgl::port::mutex_give();
        }
    } // mainButton::icon1

    void mainButton::icon2()
    {
        if (icon2_scr != NULL)
        {
            lvgl::port::mutex_take();
            icon.set_src(icon2_scr);
            lvgl::port::mutex_give();
        }
    } // mainButton::icon2

    void layerButton::create(Object &parent, uint8_t pos)
    {
        if (created)
            return;
        if (pos >= 9)
            return;
        lvgl::port::mutex_take();
        lv_color_t color = lv_color_make(166, 0, 0);
        style_buttonFixed.set_bg_color(color);
        style_buttonFixed.set_bg_grad_color(color);
        style_buttonFixed.set_outline_color(color);
        style_button.set_radius(6);
        style_button.set_bg_opa(LV_OPA_100);
        style_button.set_bg_color(LV_PALETTE_PURPLE);
        style_button.set_bg_grad_color(lv_palette_darken(LV_PALETTE_PURPLE, 2));
        style_button.set_bg_grad_dir(LV_GRAD_DIR_VER);
        style_button.set_border_opa(LV_OPA_40);
        style_button.set_border_width(2);
        style_button.set_border_color(lv_palette_main(LV_PALETTE_GREY));
        style_button.set_shadow_width(0);
        style_button.set_shadow_color(lv_palette_main(LV_PALETTE_GREY));
        style_button.set_shadow_ofs_y(0);
        style_button.set_outline_opa(LV_OPA_COVER);
        style_button.set_outline_color(lv_palette_main(LV_PALETTE_PURPLE));
        style_button.set_text_color(lv_color_white());
        style_button.set_pad_all(10);
        style_buttonPressed.set_outline_width(30);
        style_buttonPressed.set_outline_opa(LV_OPA_TRANSP);
        style_buttonPressed.set_translate_y(5);
        style_buttonPressed.set_shadow_ofs_y(3);
        style_buttonPressed.set_bg_color(lv_palette_darken(LV_PALETTE_PURPLE, 2));
        style_buttonPressed.set_bg_grad_color(
            lv_palette_darken(LV_PALETTE_PURPLE, 4));
        object = lv_btn_create(parent.get_object());
        created = true;
        remove_style_all();
        add_style(style_button, LV_STATE_DEFAULT);
        add_style(style_buttonPressed, LV_STATE_PRESSED);
        set_size(128, 128);
        set_pos(layerButtons_position_map[pos].x, layerButtons_position_map[pos].y);
        if (icon1_scr != NULL)
        {
            icon.create(*this);
            icon.center();
            icon.set_src(icon1_scr);
            style_icon.set_img_recolor(lv_color_white());
            style_icon.set_img_recolor_opa(100);
        }
        else
        {
            label.create(*this);
            label.set_text(text_scr);
            label.center();
        }
        lvgl::port::mutex_give();
    } // layerButton::create

    void layerButton::callback(lvgl::event::callback_t callback, lv_event_code_t code, int user_data)
    {
        if (!created)
            return;
        lvgl::port::mutex_take();
        add_event_cb(callback, code, (void *)user_data);
        lvgl::port::mutex_give();
    } // layerButton::callback

    void layerButton::position(uint8_t pos)
    {
        //__unused(pos);
    } // layerButton::position

    void configButton::create(Object &parent, uint8_t pos)
    {
        if (created)
            return;
        if (pos >= 9)
            return;
        lvgl::port::mutex_take();
        lv_color_t color = lv_color_make(0, 166, 0);
        style_buttonFixed.set_bg_color(color);
        style_buttonFixed.set_bg_grad_color(color);
        style_buttonFixed.set_outline_color(color);
        style_button.set_radius(6);
        style_button.set_bg_opa(LV_OPA_100);
        style_button.set_bg_color(LV_PALETTE_PURPLE);
        style_button.set_bg_grad_color(lv_palette_darken(LV_PALETTE_PURPLE, 2));
        style_button.set_bg_grad_dir(LV_GRAD_DIR_VER);
        style_button.set_border_opa(LV_OPA_40);
        style_button.set_border_width(2);
        style_button.set_border_color(lv_palette_main(LV_PALETTE_GREY));
        style_button.set_shadow_width(0);
        style_button.set_shadow_color(lv_palette_main(LV_PALETTE_GREY));
        style_button.set_shadow_ofs_y(0);
        style_button.set_outline_opa(LV_OPA_COVER);
        style_button.set_outline_color(lv_palette_main(LV_PALETTE_PURPLE));
        style_button.set_text_color(lv_color_white());
        style_button.set_pad_all(10);
        style_buttonPressed.set_outline_width(30);
        style_buttonPressed.set_outline_opa(LV_OPA_TRANSP);
        style_buttonPressed.set_translate_y(5);
        style_buttonPressed.set_shadow_ofs_y(3);
        style_buttonPressed.set_bg_color(lv_palette_darken(LV_PALETTE_PURPLE, 2));
        style_buttonPressed.set_bg_grad_color(
            lv_palette_darken(LV_PALETTE_PURPLE, 4));
        object = lv_btn_create(parent.get_object());
        created = true;
        remove_style_all();
        add_style(style_button, LV_STATE_DEFAULT);
        add_style(style_buttonPressed, LV_STATE_PRESSED);
        set_size(128, 128);
        set_pos(configButtons_position_map[pos].x,
                configButtons_position_map[pos].y);
        if (icon1_scr != NULL)
        {
            icon.create(*this);
            icon.center();
            icon.set_src(icon1_scr);
            style_icon.set_img_recolor(lv_color_white());
            style_icon.set_img_recolor_opa(100);
        }
        else
        {
            label.create(*this);
            label.set_text(text_scr);
            label.center();
        }
        lvgl::port::mutex_give();
    } // configButton::create

    void configButton::callback(lvgl::event::callback_t callback, lv_event_code_t code, int user_data)
    {
        if (!created)
            return;
        lvgl::port::mutex_take();
        add_event_cb(callback, code, (void *)user_data);
        lvgl::port::mutex_give();
    } // configButton::callback

    void configButton::position(uint8_t pos)
    {
        //__unused(pos);
    } // configButton::position

} // namespace streamDeco