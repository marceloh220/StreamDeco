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

#include <Arduino.h>
#include <BleKeyboard.h>

#include "marcelino.hpp"

#include "settings.hpp"
#include "uibutton.hpp"

#define STACK_BUTTONS_SIZE byte_k(4)

void handle_buttons(arg_t arg);
void handle_uiReset(arg_t arg);

Task taskButtons("Task Buttons", 5, STACK_BUTTONS_SIZE);
Task taskUiReset("Task reset screen", 3, 1024);

BleKeyboard bleKeyboard("StreamDeco", "Marcelino Inc", 100);

Timer uiResetTimer("Ui rest timer", 10s);
Timer backlightTimer("Backlight timer", 30s);

lvgl::Layer layer_applications;
lvgl::Layer layer_multimedia;
lvgl::Layer layer_configurations;

/* --- MAIN BUTTONS --- */
streamDeco::uiButton btn_terminal("Terminal", &terminal_simp);
streamDeco::uiButton btn_files("Files", &files_simp);
streamDeco::uiButton btn_web("Web", &web_simp);
streamDeco::uiButton btn_search("Search", &search_simp);
streamDeco::uiButton btn_application("Application", &applications_simp);

streamDeco::uiButton btn_multimedia_prev("Prev", &backward_simp);
streamDeco::uiButton btn_mutimedia_play("Play/Pause", &play_simp, &pause_simp);
streamDeco::uiButton btn_multimedia_next("Next", &forward_simp);
streamDeco::uiButton btn_multimedia_microphone("Mic", &mic_on_simp,
                                               &mic_off_simp);
streamDeco::uiButton btn_multimedia("Multimedia", &multimedia_simp);

streamDeco::uiButton btn_prevWork("Prev Work", &previous_workspace_simp);
streamDeco::uiButton btn_nextWork("Next Work", &next_workspace_simp);
streamDeco::uiButton btn_view("View", &view_simp);
streamDeco::uiButton btn_lock("Lock", &locked_simp);
streamDeco::uiButton btn_configurations("Config", &config_simp);

/* --- APPLICATIONS BUTTONS --- */
streamDeco::layerButton btn_app1("app1", &steam_simp);
streamDeco::layerButton btn_app2("app2", &discord_simp);
streamDeco::layerButton btn_app3("app3", &sysmon_simp);

streamDeco::layerButton btn_app4("app4", &code_simp);
streamDeco::layerButton btn_app5("app5", &texcompiler_simp);
streamDeco::layerButton btn_app6("app6", &calculator_simp);

streamDeco::layerButton btn_app7("app7", &build_simp);
streamDeco::layerButton btn_app8("app8", &download_simp);
streamDeco::layerButton btn_app9("app9", &serialport_simp);

/* --- MULTIMEDIA BUTTONS --- */
streamDeco::layerButton btn_multimedia_mult1("mult 1", &music_simp);
streamDeco::layerButton btn_multimedia_mult2("mult 2", &video_simp);
streamDeco::layerButton btn_multimedia_mult3("mult 3", &audio_wave_simp);

streamDeco::layerButton btn_multimedia_mult4("mult 4", &add_clip_simp);
streamDeco::layerButton btn_multimedia_mult5("mult 5", &ripple_simp);
streamDeco::layerButton btn_multimedia_mult6("mult 6", &rolling_simp);

streamDeco::layerButton btn_multimedia_mult7("mult 7", &seek_backward_simp);
streamDeco::layerButton btn_multimedia_mult8("mult 8", &play_simp);
streamDeco::layerButton btn_multimedia_mult9("mult 9", &seek_forward_simp);

/* --- CONFIGURATIONS BUTTONS --- */
streamDeco::configButton btn_volmut("Vol Mut", &volume_mute_simp);
streamDeco::configButton btn_voldown("Vol Down", &volume_low_simp);
streamDeco::configButton btn_volup("Vol Up", &volume_high_simp);

streamDeco::configButton btn_colorBg("Color BG", &bgtheme_simp);
streamDeco::configButton btn_colorBtn("Color Buttons", &btntheme_simp);
streamDeco::configButton btn_confSys("Conf sys", &process_simp);

streamDeco::configButton btn_userSwitch("User switch", &switch_user_simp);
streamDeco::configButton btn_reboot("Reboot", &reboot_simp);
streamDeco::configButton btn_shutdown("Shutdown", &shutdown_simp);

lvgl::Slider slider_bright;
lvgl::Style slider_style;
lvgl::Image slider_img;
lvgl::Style slider_style_img;

enum {
  terminal_event,
  files_event,
  web_event,
  search_event,
  applications_event,
  applications_fix_event,

  app1_event,
  app2_event,
  app3_event,

  app4_event,
  app5_event,
  app6_event,

  app7_event,
  app8_event,
  app9_event,

  multimedi_prev_event,
  multimedia_play_event,
  multimedia_next_event,
  multimedia_microphone_event,
  multimedia_event,
  multimedia_fix_event,

  multimedia_mult1_event,
  multimedia_mult2_event,
  multimedia_mult3_event,

  multimedia_mult4_event,
  multimedia_mult5_event,
  multimedia_mult6_event,

  multimedia_mult7_event,
  multimedia_mult8_event,
  multimedia_mult9_event,

  multimedia_volmut_event,
  multimedia_voldown_event,
  multimedia_volup_event,

  prevwork_event,
  nextwork_event,
  view_event,
  lock_event,
  configuration_event,

  colorBg_event,
  colorBtn_event,
  confSys_event,

  userSwitch_event,
  reboot_event,
  shutdown_event,

  rotation_event,

  slider_value,

  hiddenlayers_event,
};

namespace streamDeco {
void init() {
  taskButtons.attach(handle_buttons);
  taskUiReset.attach(handle_uiReset);
}
void print_task_memory_usage() {
  printf("Task Buttons mem usage %d\n", taskButtons.memUsage());
}
} // namespace streamDeco

void change_rotation();

void isr_backlightTimer(TimerHandle_t arg) {
  if (backlightTimer.verifyID(arg)) {
    lvgl::port::backlight_set(.1);
  }
}

void isr_uiResetTimer(TimerHandle_t arg) {
  if (uiResetTimer.verifyID(arg)) {
    taskUiReset.sendNotify(hiddenlayers_event);
  }
}

void buttons_callback(lvgl::event::event_t e) {
  taskButtons.sendNotify(lvgl::event::get_user_data<int>(e));
}

void handle_uiReset(arg_t arg) {
  while (1) {
    uint32_t e = taskUiReset.takeNotify();
    if (e == hiddenlayers_event) {
      if (!btn_application.is_fix())
        layer_applications.hidden();
      if (!btn_multimedia.is_fix())
        layer_multimedia.hidden();
      layer_configurations.hidden();
    }
  }
}

void handle_buttons(arg_t arg) {

  lv_palette_t color_bg;
  lv_palette_t color_buttons;
  int lcd_bright = 2457;

  lvgl::screen::set_rotation(LV_DISP_ROT_NONE);
  lvgl::screen::set_bg_color(LV_PALETTE_DEEP_ORANGE);

  lvgl::Label label;
  lvgl::Image icon;

  /* --- START --- */

  label.create();
  label.set_long_mode(LV_LABEL_LONG_WRAP);
  label.set_style_text_align(LV_TEXT_ALIGN_CENTER);
  label.set_style_text_font(&lv_font_montserrat_22);
  label.align(LV_ALIGN_CENTER, 0, -64 - 15);

  icon.create();
  icon.center();

  label.set_text("StreamDeco");
  icon.set_src(&keyboard_simp);
  lvgl::screen::refresh();

  delay(1s);

  bleKeyboard.begin();

  label.set_text("Connecting...");
  icon.set_src(&bluetooth_simp);
  lvgl::screen::refresh();

  delay(500ms);

  while (1) {
    if (bleKeyboard.isConnected()) {
      break;
    }
    delay(2s);
  }

  icon.del();
  label.del();
  lvgl::screen::refresh();

  /* --- MAIN --- */

  btn_terminal.create(0);
  btn_files.create(1);
  btn_web.create(2);
  btn_search.create(3);
  btn_application.create(4);

  btn_multimedia_prev.create(5);
  btn_mutimedia_play.create(6);
  btn_multimedia_next.create(7);
  btn_multimedia_microphone.create(8);
  btn_multimedia.create(9);

  btn_prevWork.create(10);
  btn_nextWork.create(11);
  btn_view.create(12);
  btn_lock.create(13);
  btn_configurations.create(14);

  btn_terminal.callback(buttons_callback, LV_EVENT_PRESSED, terminal_event);
  btn_files.callback(buttons_callback, LV_EVENT_PRESSED, files_event);
  btn_web.callback(buttons_callback, LV_EVENT_PRESSED, web_event);
  btn_search.callback(buttons_callback, LV_EVENT_PRESSED, search_event);
  btn_application.callback(buttons_callback, LV_EVENT_SHORT_CLICKED,
                           applications_event);
  btn_application.callback(buttons_callback, LV_EVENT_LONG_PRESSED,
                           applications_fix_event);

  btn_multimedia_prev.callback(buttons_callback, LV_EVENT_PRESSED,
                               multimedi_prev_event);
  btn_mutimedia_play.callback(buttons_callback, LV_EVENT_PRESSED,
                              multimedia_play_event);
  btn_multimedia_next.callback(buttons_callback, LV_EVENT_PRESSED,
                               multimedia_next_event);
  btn_multimedia_microphone.callback(buttons_callback, LV_EVENT_PRESSED,
                                     multimedia_microphone_event);
  btn_multimedia.callback(buttons_callback, LV_EVENT_SHORT_CLICKED,
                          multimedia_event);
  btn_multimedia.callback(buttons_callback, LV_EVENT_LONG_PRESSED,
                          multimedia_fix_event);

  btn_prevWork.callback(buttons_callback, LV_EVENT_PRESSED, prevwork_event);
  btn_nextWork.callback(buttons_callback, LV_EVENT_PRESSED, nextwork_event);
  btn_view.callback(buttons_callback, LV_EVENT_PRESSED, view_event);
  btn_lock.callback(buttons_callback, LV_EVENT_PRESSED, lock_event);
  btn_configurations.callback(buttons_callback, LV_EVENT_PRESSED,
                              configuration_event);

  /* --- APLICATIONS --- */

  layer_applications.create();
  layer_applications.align(LV_ALIGN_CENTER, -74, 0);
  layer_applications.set_size(572, 470);
  layer_applications.set_bg_color(lv_color_make(41, 45, 50));
  layer_applications.hidden();

  btn_app1.create(layer_applications, 0);
  btn_app2.create(layer_applications, 1);
  btn_app3.create(layer_applications, 2);

  btn_app4.create(layer_applications, 3);
  btn_app5.create(layer_applications, 4);
  btn_app6.create(layer_applications, 5);

  btn_app7.create(layer_applications, 6);
  btn_app8.create(layer_applications, 7);
  btn_app9.create(layer_applications, 8);

  btn_app1.callback(buttons_callback, LV_EVENT_PRESSED, app1_event);
  btn_app2.callback(buttons_callback, LV_EVENT_PRESSED, app2_event);
  btn_app3.callback(buttons_callback, LV_EVENT_PRESSED, app3_event);

  btn_app4.callback(buttons_callback, LV_EVENT_PRESSED, app4_event);
  btn_app5.callback(buttons_callback, LV_EVENT_PRESSED, app5_event);
  btn_app6.callback(buttons_callback, LV_EVENT_PRESSED, app6_event);

  btn_app7.callback(buttons_callback, LV_EVENT_PRESSED, app7_event);
  btn_app8.callback(buttons_callback, LV_EVENT_PRESSED, app8_event);
  btn_app9.callback(buttons_callback, LV_EVENT_PRESSED, app9_event);

  /* --- MULTIMEDIA --- */

  layer_multimedia.create();
  layer_multimedia.align(LV_ALIGN_CENTER, -74, 0);
  layer_multimedia.set_size(572, 470);
  layer_multimedia.set_bg_color(lv_color_make(41, 45, 50));
  layer_multimedia.hidden();

  btn_multimedia_mult1.create(layer_multimedia, 0);
  btn_multimedia_mult2.create(layer_multimedia, 1);
  btn_multimedia_mult3.create(layer_multimedia, 2);

  btn_multimedia_mult4.create(layer_multimedia, 3);
  btn_multimedia_mult5.create(layer_multimedia, 4);
  btn_multimedia_mult6.create(layer_multimedia, 5);

  btn_multimedia_mult7.create(layer_multimedia, 6);
  btn_multimedia_mult8.create(layer_multimedia, 7);
  btn_multimedia_mult9.create(layer_multimedia, 8);

  btn_multimedia_mult1.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_mult1_event);
  btn_multimedia_mult2.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_mult2_event);
  btn_multimedia_mult3.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_mult3_event);

  btn_multimedia_mult4.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_mult4_event);
  btn_multimedia_mult5.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_mult5_event);
  btn_multimedia_mult6.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_mult6_event);

  btn_multimedia_mult7.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_mult7_event);
  btn_multimedia_mult8.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_mult8_event);
  btn_multimedia_mult9.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_mult9_event);

  /* --- CONFIGURATIONS --- */

  layer_configurations.create();
  layer_configurations.align(LV_ALIGN_CENTER, -74, 0);
  layer_configurations.set_size(572, 470);
  layer_configurations.set_bg_color(lv_color_make(41, 45, 50));
  layer_configurations.hidden();

  btn_volmut.create(layer_configurations, 0);
  btn_voldown.create(layer_configurations, 1);
  btn_volup.create(layer_configurations, 2);

  btn_colorBg.create(layer_configurations, 3);
  btn_colorBtn.create(layer_configurations, 4);
  btn_confSys.create(layer_configurations, 5);

  btn_userSwitch.create(layer_configurations, 6);
  btn_reboot.create(layer_configurations, 7);
  btn_shutdown.create(layer_configurations, 8);

  btn_volmut.callback(buttons_callback, LV_EVENT_PRESSED,
                      multimedia_volmut_event);
  btn_voldown.callback(buttons_callback, LV_EVENT_PRESSING,
                       multimedia_voldown_event);
  btn_volup.callback(buttons_callback, LV_EVENT_PRESSING,
                     multimedia_volup_event);

  btn_colorBg.callback(buttons_callback, LV_EVENT_PRESSED, colorBg_event);
  btn_colorBtn.callback(buttons_callback, LV_EVENT_PRESSED, colorBtn_event);
  btn_confSys.callback(buttons_callback, LV_EVENT_PRESSED, confSys_event);

  btn_userSwitch.callback(buttons_callback, LV_EVENT_PRESSED, userSwitch_event);
  btn_reboot.callback(buttons_callback, LV_EVENT_PRESSED, reboot_event);
  btn_shutdown.callback(buttons_callback, LV_EVENT_PRESSED, shutdown_event);

  slider_bright.create(layer_configurations);
  slider_bright.set_pos(346 + 148, 20 + 32 - 5 + 13);
  slider_bright.set_range(410, 4095);
  slider_bright.add_event_cb(buttons_callback, LV_EVENT_VALUE_CHANGED,
                             slider_value);
  slider_bright.set_ext_click_area(30);
  slider_bright.set_size(20, 240);
  slider_bright.set_value(lcd_bright);
  slider_style.set_bg_color(LV_PALETTE_PURPLE);
  slider_bright.add_style(slider_style, LV_PART_INDICATOR);
  slider_bright.add_style(slider_style, LV_PART_KNOB);

  slider_img.create(layer_configurations);
  slider_img.set_pos(336 + 138 + 10, 300 + 13);
  slider_style_img.set_img_recolor(LV_PALETTE_PURPLE);
  slider_style_img.set_img_recolor_opa(100);
  slider_img.add_style(slider_style_img, LV_PART_MAIN);

  slider_img.set_src(&brightness_simp);
  slider_img.update_layout();

  delay(100ms);

  backlightTimer.attach(isr_backlightTimer);
  uiResetTimer.attach(isr_uiResetTimer);

  while (1) {

    uint32_t e = taskButtons.takeNotify();

    switch (e) {

    /* --- MAIN APPLICATIONS --- */
    case terminal_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F1);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case files_event:
      bleKeyboard.write(KEY_MEDIA_LOCAL_MACHINE_BROWSER);
      break;
    case web_event:
      bleKeyboard.write(KEY_MEDIA_WWW_HOME);
      break;
    case search_event:
      bleKeyboard.write(KEY_MEDIA_WWW_SEARCH);
      break;
    case applications_event:
      if (btn_multimedia.is_fix())
        break;
      btn_application.unfix();
      layer_multimedia.hidden();
      layer_configurations.hidden();
      layer_applications.change_hidden();
      layer_applications.move_foreground();
      break;
    case applications_fix_event:
      if (btn_multimedia.is_fix())
        break;
      btn_application.fix();
      layer_multimedia.hidden();
      layer_configurations.hidden();
      layer_applications.unhidden();
      layer_applications.move_foreground();
      break;

    /* --- MAIN MULTIMEDIA --- */
    case multimedi_prev_event:
      bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
      break;
    case multimedia_play_event:
      bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
      btn_mutimedia_play.swapIcon();
      break;
    case multimedia_next_event:
      bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
      break;
    case multimedia_microphone_event:
      btn_multimedia_microphone.swapIcon();
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F2);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case multimedia_event:
      if (btn_application.is_fix())
        break;
      btn_multimedia.unfix();
      layer_applications.hidden();
      layer_configurations.hidden();
      layer_multimedia.change_hidden();
      layer_multimedia.move_foreground();
      break;
    case multimedia_fix_event:
      if (btn_application.is_fix())
        break;
      btn_multimedia.fix();
      layer_applications.hidden();
      layer_configurations.hidden();
      layer_multimedia.unhidden();
      layer_multimedia.move_foreground();
      break;

    /* --- MAIN CONFIGURATIONS --- */
    case prevwork_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F3);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case nextwork_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F4);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case view_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F5);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case lock_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F6);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case configuration_event:
      if (btn_application.is_fix() || btn_multimedia.is_fix())
        break;
      layer_applications.hidden();
      layer_multimedia.hidden();
      layer_configurations.change_hidden();
      layer_configurations.move_foreground();
      break;

    /* --- LAYER APPLICATIONS --- */
    case app1_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('9');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case app2_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('0');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case app3_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('d');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case app4_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('f');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case app5_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('g');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case app6_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('h');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case app7_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.press('b');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case app8_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.press('u');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case app9_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.press('s');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    /* --- LAYER MULTIMEDIA --- */
    case multimedia_mult1_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('q');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case multimedia_mult2_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('w');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case multimedia_mult3_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('e');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case multimedia_mult4_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('r');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case multimedia_mult5_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('t');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case multimedia_mult6_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('k');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case multimedia_mult7_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('i');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case multimedia_mult8_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('o');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case multimedia_mult9_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('p');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    /* --- LAYER CONFIGURATIONS --- */
    case multimedia_volmut_event:
      bleKeyboard.write(KEY_MEDIA_MUTE);
      break;
    case multimedia_voldown_event:
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
      break;
    case multimedia_volup_event:
      bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
      break;

    case colorBg_event:
      color_bg = lvgl::settings::bgColor();
      lvgl::screen::set_bg_color(color_bg);
      break;
    case colorBtn_event:
      color_buttons = lvgl::settings::buttonColor();
      btn_terminal.color(color_buttons);
      slider_style.set_bg_color(color_buttons);
      slider_style_img.set_img_recolor(color_buttons);
      slider_img.update_layout();
      lvgl::screen::refresh();
      break;
    case confSys_event:
      bleKeyboard.write(KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION);
      break;

    case userSwitch_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('1');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case reboot_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('2');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;
    case shutdown_event:
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('3');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case rotation_event:
      // change_rotation();
      break;

    case slider_value:
      lcd_bright = slider_bright.get_value();
    } // end switch case

    lvgl::port::backlight_setRaw(lcd_bright);
    backlightTimer.reset();
    uiResetTimer.reset();
    delay(100ms);

  } // end infinit loop

} // end handle_buttons

#ifdef DECO_ROTATION
void change_rotation() {
  lvgl::port::mutex_take();
  lv_disp_rot_t rotation = lvgl::screen::get_rotation();
  switch (rotation) {
  case LV_DISP_ROT_NONE:
  case LV_DISP_ROT_180:
    lvgl::screen::set_rotation(LV_DISP_ROT_90);
    btn_terminal.position(0);
    btn_files.position(1);
    btn_web.position(2);
    btn_multimedia_prev.position(3);
    btn_mutimedia_play.position(4);
    btn_multimedia_next.position(5);
    btn_search.position(6);
    btn_voldown.position(7);
    btn_volup.position(8);
    btn_prevWork.position(9);
    btn_nextWork.position(10);
    btn_view.position(11);
    // btn_calc.position(12);
    btn_lock.position(13);
    btn_configurations.position(14);
    break;
  case LV_DISP_ROT_90:
  case LV_DISP_ROT_270:
    lvgl::screen::set_rotation(LV_DISP_ROT_NONE);
    btn_terminal.position(0);
    btn_files.position(1);
    btn_web.position(2);
    btn_search.position(3);
    // btn_calc.position(4);
    btn_multimedia_prev.position(5);
    btn_mutimedia_play.position(6);
    btn_multimedia_next.position(7);
    btn_voldown.position(8);
    btn_volup.position(9);
    btn_prevWork.position(10);
    btn_nextWork.position(11);
    btn_view.position(12);
    btn_lock.position(13);
    btn_configurations.position(14);
    break;
  } // end switch case
  lvgl::port::mutex_give();
} // end change_rotatio
#endif
