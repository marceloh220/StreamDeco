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

#include <Arduino.h>
#include <BleKeyboard.h>

#include "marcelino.hpp"

#include "settings.hpp"
#include "uibutton.hpp"

/* Stack size of buttons task */
#define STACK_BUTTONS_SIZE byte_k(4)

/* Handle the buttons task, made buttons configurations and manager buttons
 * events. This events can generate a keyboard code to send or change StreamDeco
 * configurations. */
void handle_buttons(arg_t arg);

/* Handle the ui reset task, each 10 seconds a timer will pass a event to this
 * task to hidden additional layers and return to Main Layer. */
void handle_uiReset(arg_t arg);

/* Task button to manager buttons configurations and events */
Task taskButtons("Task Buttons", 5, STACK_BUTTONS_SIZE);

/* Task to hidde inactivities layers */
Task taskUiReset("Task reset screen", 3, 1024);

/* Interface to send keyboard codes to Bluetooth */
BleKeyboard bleKeyboard("StreamDeco", "Marcelino Inc", 100);

/* Timer to generate an event to to hidden additional layers and return to Main
 * Layer affter 10 secons of inactivities. */
Timer uiResetTimer("UI reset timer", 10s);

/* Timer reduce screen backlight affter 30 secons of inactivities. */
Timer backlightTimer("Backlight timer", 30s);

/* Layer to hold addtional apps shortcuts. */
lvgl::Layer layer_applications;
/* Layer to hold multimedi apps shortcuts. */
lvgl::Layer layer_multimedia;
/* Layer to hold configurations shortcuts. */
lvgl::Layer layer_configurations;

/* --- MAIN BUTTONS --- */

/** this button send CTRL+SHIFT+F1, is suppose to open terminal */
streamDeco::mainButton btn_terminal("Terminal", &terminal_simp);

/** this button send KEY_MEDIA_LOCAL_MACHINE_BROWSER, is suppose to open file
 * manager */
streamDeco::mainButton btn_files("Files", &files_simp);

/** this button send KEY_MEDIA_WWW_HOME, is suppose to open browser */
streamDeco::mainButton btn_web("Web", &web_simp);

/** this button send KEY_MEDIA_WWW_SEARCH, is suppose to open desktop search */
streamDeco::mainButton btn_search("Search", &search_simp);

/** this button open or fixe Application layer */
streamDeco::mainButton btn_applications_layer("Application",
                                              &applications_simp);

/** this button send KEY_MEDIA_PREVIOUS_TRACK */
streamDeco::mainButton btn_multimedia_prev("Prev", &backward_simp);

/** this button send KEY_MEDIA_PLAY_PAUSE */
streamDeco::mainButton btn_mutimedia_play("Play/Pause", &play_simp,
                                          &pause_simp);

/** this button send KEY_MEDIA_NEXT_TRACK */
streamDeco::mainButton btn_multimedia_next("Next", &forward_simp);

/** this button send CTRL+SHIFT+F2, is suppose to mute and unmute mic */
streamDeco::mainButton btn_multimedia_mic("Mic", &mic_on_simp, &mic_off_simp);

/** this button open or fixe Multimedia layer */
streamDeco::mainButton btn_multimedia_layer("Multimedia", &multimedia_simp);

/** this button send CTRL+SHIFT+F3, is suppose to change to left workspace */
streamDeco::mainButton btn_prevWork("Prev Work", &previous_workspace_simp);

/** this button send CTRL+SHIFT+F4, is suppose to change to right workspace */
streamDeco::mainButton btn_nextWork("Next Work", &next_workspace_simp);

/** this button send CTRL+SHIFT+F5, is suppose to show general view */
streamDeco::mainButton btn_view("View", &view_simp);

/** this button send CTRL+SHIFT+F6, is suppose to lock computer screen */
streamDeco::mainButton btn_lock("Lock", &locked_simp);

/** this button open or fixe Configuration layer */
streamDeco::mainButton btn_configurations_layer("Config", &config_simp);

/* --- APPLICATIONS BUTTONS --- */

/** this button send CTRL+SHIFT+9 */
streamDeco::layerButton btn_app1("app1", &steam_simp);

/** this button send CTRL+SHIFT+0 */
streamDeco::layerButton btn_app2("app2", &discord_simp);

/** this button send CTRL+SHIFT+d */
streamDeco::layerButton btn_app3("app3", &sysmon_simp);

/** this button send CTRL+SHIFT+f */
streamDeco::layerButton btn_app4("app4", &code_simp);

/** this button send CTRL+SHIFT+g */
streamDeco::layerButton btn_app5("app5", &texcompiler_simp);

/** this button send CTRL+SHIFT+h */
streamDeco::layerButton btn_app6("app6", &calculator_simp);

/** this button send CTRL+ALT+b, in vscode build application */
streamDeco::layerButton btn_app7("app7", &build_simp);

/** this button send CTRL+ALT+u, in vscode upload application to microcontroler */
streamDeco::layerButton btn_app8("app8", &download_simp);

/** this button send CTRL+ALT+s, in vscode open terminal monitor */
streamDeco::layerButton btn_app9("app9", &serialport_simp);

/* --- MULTIMEDIA BUTTONS --- */

/** this button send CTRL+SHIFT+q */
streamDeco::layerButton btn_multimedia_mult1("mult 1", &music_simp);

/** this button send CTRL+SHIFT+w */
streamDeco::layerButton btn_multimedia_mult2("mult 2", &video_simp);

/** this button send CTRL+SHIFT+e */
streamDeco::layerButton btn_multimedia_mult3("mult 3", &audio_wave_simp);

/** this button send CTRL+SHIFT+r */
streamDeco::layerButton btn_multimedia_mult4("mult 4", &add_clip_simp);

/** this button send CTRL+SHIFT+t */
streamDeco::layerButton btn_multimedia_mult5("mult 5", &ripple_simp);

/** this button send CTRL+SHIFT+k */
streamDeco::layerButton btn_multimedia_mult6("mult 6", &rolling_simp);

/** this button send CTRL+SHIFT+i */
streamDeco::layerButton btn_multimedia_mult7("mult 7", &seek_backward_simp);

/** this button send CTRL+SHIFT+o */
streamDeco::layerButton btn_multimedia_mult8("mult 8", &play_simp);

/** this button send CTRL+SHIFT+p */
streamDeco::layerButton btn_multimedia_mult9("mult 9", &seek_forward_simp);

/* --- CONFIGURATIONS BUTTONS --- */

/** this button send KEY_MEDIA_MUTE */
streamDeco::configButton btn_volmut("Vol Mute", &volume_mute_simp);

/** this button send KEY_MEDIA_VOLUME_DOWN */
streamDeco::configButton btn_voldown("Vol Down", &volume_low_simp);

/** this button send KEY_MEDIA_VOLUME_MUTE */
streamDeco::configButton btn_volup("Vol Up", &volume_high_simp);

/** this button change StreamDeco background color */
streamDeco::configButton btn_color_background("Color BG", &bgtheme_simp);

/** this button change StreamDeco button color */
streamDeco::configButton btn_color_button("Color Buttons", &btntheme_simp);

/** this button send KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION */
streamDeco::configButton btn_confSys("Conf sys", &process_simp);

/** this button send CTRL+SHIFT+1 */
streamDeco::configButton btn_logout("Logout", &logout_simp);

/** this button send CTRL+SHIFT+2 */
streamDeco::configButton btn_reboot("Reboot", &reboot_simp);

/** this button send CTRL+SHIFT+3 */
streamDeco::configButton btn_shutdown("Shutdown", &shutdown_simp);

/* Bright control Slider on Configuration Layer. */
lvgl::Slider slider_backlightbright;
lvgl::Style slider_backlightbright_style;
lvgl::Image slider_backlightbright_icon;
lvgl::Style slider_backlightbright_icon_style;

/* Events enumerations */
enum event_e {

  /* --- MAIN LAYER EVENTS --- */
  terminal_event,
  files_event,
  web_event,
  search_event,
  applications_layer_event,
  applications_layer_fix_event,

  multimedia_prev_event,
  multimedia_play_event,
  multimedia_next_event,
  multimedia_mic_event,
  multimedia_layer_event,
  multimedia_layer_fix_event,

  prev_workspace_event,
  next_workspace_event,
  general_view_event,
  lock_computer_event,
  configurations_layer_event,

  /* --- APPLICATIONS LAYER EVENTS --- */

  applications_layer_app1_event,
  applications_layer_app2_event,
  applications_layer_app3_event,

  applications_layer_app4_event,
  applications_layer_app5_event,
  applications_layer_app6_event,

  applications_layer_app7_event,
  applications_layer_app8_event,
  applications_layer_app9_event,

  /* --- MULTIMEDIA LAYER EVENTS --- */

  multimedia_layer_mult1_event,
  multimedia_layer_mult2_event,
  multimedia_layer_mult3_event,

  multimedia_layer_mult4_event,
  multimedia_layer_mult5_event,
  multimedia_layer_mult6_event,

  multimedia_layer_mult7_event,
  multimedia_layer_mult8_event,
  multimedia_layer_mult9_event,

  /* --- CONFIGURATIONS LAYER EVENTS --- */

  configuration_layer_volmut_event,
  configuration_layer_voldown_event,
  configuration_layer_volup_event,

  configuration_layer_colorbackground_event,
  configuration_layer_colorbutton_event,
  configuration_layer_confsys_event,

  configuration_layer_logout_event,
  configuration_layer_reboot_event,
  configuration_layer_shutdown_event,

  slider_backlight_bright_value_change_event,

  /* --- LAYERS EVENT --- */

  hidden_layers_event,

  /* --- EXPERIMENTAL --- */
  rotate_screen_event,

};

namespace streamDeco {
/* Called in main_app or setup to init StreamDeco */
void init() {
  taskButtons.attach(handle_buttons);
  taskUiReset.attach(handle_uiReset);
}
/* Called in main_app loop or loop to print memory usage of tasks */
void print_task_memory_usage() {
  printf("Task Buttons mem usage %d\n", taskButtons.memUsage());
  printf("Task UI Reset mem usage %d\n", taskUiReset.memUsage());
}
} // namespace streamDeco

/* Experimental, someday will roll. Gambare gambare senpai! */
void change_rotation();

/* ISR to handle backlight timer. Set backlight bright to 10%. */
void isr_backlightTimer(TimerHandle_t arg) {
  if (backlightTimer.verifyID(arg)) {
    lvgl::port::backlight_set(.1);
  }
}

/* ISR to UI reset, send a event to a task to hidden layers and show Main Layer
 * again. */
void isr_uiResetTimer(TimerHandle_t arg) {
  if (uiResetTimer.verifyID(arg)) {
    taskUiReset.sendNotify(hidden_layers_event);
  }
}

/* Callback registred in buttons, send a notifications with event code to task
 * buttons handler. */
void buttons_callback(lvgl::event::event_t e) {
  taskButtons.sendNotify(lvgl::event::get_user_data<int>(e));
}

/* Hnadler of UI reset task, hidden layers if they are not fixed. */
void handle_uiReset(arg_t arg) {
  while (1) {
    uint32_t e = taskUiReset.takeNotify();
    if (e == hidden_layers_event) {
      if (!btn_applications_layer.is_fixed()) {
        layer_applications.hidden();
      }
      if (!btn_multimedia_layer.is_fixed()) {
        layer_multimedia.hidden();
      }
      /* Configuration Layer don't*/
      layer_configurations.hidden();
    }
  }
}

/* Handle of task buttons, init buttons configurations and manager events. */
void handle_buttons(arg_t arg) {

  /* used to change color of StreamDeco background */
  lv_palette_t color_background;

  /* used to change color of StreamDeco buttons */
  lv_palette_t color_buttons;

  /* used to change StreamDeco backlight bright, start with 60% of bright
   * port configure PWM with 4095 max value, 12-bits resolution */
  int lcd_bright = 2457;

  /* set initial screen rotation and color */
  lvgl::screen::set_rotation(LV_DISP_ROT_NONE);
  lvgl::screen::set_bg_color(LV_PALETTE_DEEP_ORANGE);

  /*  used to temporarily show initiation screen
   *  the most part of this screens is a delay to show the icons and text
   *  actually useless but still nice to see */
  lvgl::Label startScreen_label;
  lvgl::Image startScreen_icon;

  /* --- START --- */

  startScreen_label.create();
  startScreen_label.set_long_mode(LV_LABEL_LONG_WRAP);
  startScreen_label.set_style_text_align(LV_TEXT_ALIGN_CENTER);
  startScreen_label.set_style_text_font(&lv_font_montserrat_22);
  startScreen_label.align(LV_ALIGN_CENTER, 0, -80);

  startScreen_icon.create();
  startScreen_icon.center();

  startScreen_label.set_text("StreamDeco");
  startScreen_icon.set_src(&keyboard_simp);
  lvgl::screen::refresh();

  delay(1s); /* see my icon =) */

  bleKeyboard.begin(); /* start bluetooth keyboard interface */

  startScreen_label.set_text("Connecting...");
  startScreen_icon.set_src(&bluetooth_simp);
  lvgl::screen::refresh();

  delay(500ms); /* see more icon =) */

  /* try connection, while you see icons and text =) */
  while (1) {
    if (bleKeyboard.isConnected()) {
      break;
    }
    delay(2s);
  }

  /* delete the icons and text =( */
  startScreen_icon.del();
  startScreen_label.del();
  lvgl::screen::refresh();

  /* --- MAIN --- */

  /* the class mainButton deal with position button on screen
   * is only necessary pass the order */
  btn_terminal.create(0);
  btn_files.create(1);
  btn_web.create(2);
  btn_search.create(3);
  btn_applications_layer.create(4);

  btn_multimedia_prev.create(5);
  btn_mutimedia_play.create(6);
  btn_multimedia_next.create(7);
  btn_multimedia_mic.create(8);
  btn_multimedia_layer.create(9);

  btn_prevWork.create(10);
  btn_nextWork.create(11);
  btn_view.create(12);
  btn_lock.create(13);
  btn_configurations_layer.create(14);

  /* register the buttons event */
  btn_terminal.callback(buttons_callback, LV_EVENT_PRESSED, terminal_event);
  btn_files.callback(buttons_callback, LV_EVENT_PRESSED, files_event);
  btn_web.callback(buttons_callback, LV_EVENT_PRESSED, web_event);
  btn_search.callback(buttons_callback, LV_EVENT_PRESSED, search_event);
  btn_applications_layer.callback(buttons_callback, LV_EVENT_SHORT_CLICKED,
                                  applications_layer_event);
  btn_applications_layer.callback(buttons_callback, LV_EVENT_LONG_PRESSED,
                                  applications_layer_fix_event);

  btn_multimedia_prev.callback(buttons_callback, LV_EVENT_PRESSED,
                               multimedia_prev_event);
  btn_mutimedia_play.callback(buttons_callback, LV_EVENT_PRESSED,
                              multimedia_play_event);
  btn_multimedia_next.callback(buttons_callback, LV_EVENT_PRESSED,
                               multimedia_next_event);
  btn_multimedia_mic.callback(buttons_callback, LV_EVENT_PRESSED,
                              multimedia_mic_event);
  btn_multimedia_layer.callback(buttons_callback, LV_EVENT_SHORT_CLICKED,
                                multimedia_layer_event);
  btn_multimedia_layer.callback(buttons_callback, LV_EVENT_LONG_PRESSED,
                                multimedia_layer_fix_event);

  btn_prevWork.callback(buttons_callback, LV_EVENT_PRESSED,
                        prev_workspace_event);
  btn_nextWork.callback(buttons_callback, LV_EVENT_PRESSED,
                        next_workspace_event);
  btn_view.callback(buttons_callback, LV_EVENT_PRESSED, general_view_event);
  btn_lock.callback(buttons_callback, LV_EVENT_PRESSED, lock_computer_event);
  btn_configurations_layer.callback(buttons_callback, LV_EVENT_PRESSED,
                                    configurations_layer_event);

  /* --- APLICATIONS --- */

  /* configure Applications Layer */
  layer_applications.create();
  layer_applications.align(LV_ALIGN_CENTER, -74, 0);
  layer_applications.set_size(572, 470);
  layer_applications.set_bg_color(lv_color_make(41, 45, 50));
  layer_applications.hidden();

  /* now the buttons is created in Applications Layer
   * the layerButton also deal with screen position */
  btn_app1.create(layer_applications, 0);
  btn_app2.create(layer_applications, 1);
  btn_app3.create(layer_applications, 2);

  btn_app4.create(layer_applications, 3);
  btn_app5.create(layer_applications, 4);
  btn_app6.create(layer_applications, 5);

  btn_app7.create(layer_applications, 6);
  btn_app8.create(layer_applications, 7);
  btn_app9.create(layer_applications, 8);

  btn_app1.callback(buttons_callback, LV_EVENT_PRESSED,
                    applications_layer_app1_event);
  btn_app2.callback(buttons_callback, LV_EVENT_PRESSED,
                    applications_layer_app2_event);
  btn_app3.callback(buttons_callback, LV_EVENT_PRESSED,
                    applications_layer_app3_event);

  btn_app4.callback(buttons_callback, LV_EVENT_PRESSED,
                    applications_layer_app4_event);
  btn_app5.callback(buttons_callback, LV_EVENT_PRESSED,
                    applications_layer_app5_event);
  btn_app6.callback(buttons_callback, LV_EVENT_PRESSED,
                    applications_layer_app6_event);

  btn_app7.callback(buttons_callback, LV_EVENT_PRESSED,
                    applications_layer_app7_event);
  btn_app8.callback(buttons_callback, LV_EVENT_PRESSED,
                    applications_layer_app8_event);
  btn_app9.callback(buttons_callback, LV_EVENT_PRESSED,
                    applications_layer_app9_event);

  /* --- MULTIMEDIA --- */

  /* Multimedia Layer work like the Applications Layer
   * creating the layer and buttons with same methods */
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

  /* register the events of buttons */
  btn_multimedia_mult1.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_layer_mult1_event);
  btn_multimedia_mult2.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_layer_mult2_event);
  btn_multimedia_mult3.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_layer_mult3_event);

  btn_multimedia_mult4.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_layer_mult4_event);
  btn_multimedia_mult5.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_layer_mult5_event);
  btn_multimedia_mult6.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_layer_mult6_event);

  btn_multimedia_mult7.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_layer_mult7_event);
  btn_multimedia_mult8.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_layer_mult8_event);
  btn_multimedia_mult9.callback(buttons_callback, LV_EVENT_PRESSED,
                                multimedia_layer_mult9_event);

  /* --- CONFIGURATIONS --- */

  /* Configuratio Layer only use diferent position map from other layers */
  layer_configurations.create();
  layer_configurations.align(LV_ALIGN_CENTER, -74, 0);
  layer_configurations.set_size(572, 470);
  layer_configurations.set_bg_color(lv_color_make(41, 45, 50));
  layer_configurations.hidden();

  btn_volmut.create(layer_configurations, 0);
  btn_voldown.create(layer_configurations, 1);
  btn_volup.create(layer_configurations, 2);

  btn_color_background.create(layer_configurations, 3);
  btn_color_button.create(layer_configurations, 4);
  btn_confSys.create(layer_configurations, 5);

  btn_logout.create(layer_configurations, 6);
  btn_reboot.create(layer_configurations, 7);
  btn_shutdown.create(layer_configurations, 8);

  btn_volmut.callback(buttons_callback, LV_EVENT_PRESSED,
                      configuration_layer_volmut_event);
  btn_voldown.callback(buttons_callback, LV_EVENT_PRESSING,
                       configuration_layer_voldown_event);
  btn_volup.callback(buttons_callback, LV_EVENT_PRESSING,
                     configuration_layer_volup_event);

  btn_color_background.callback(buttons_callback, LV_EVENT_PRESSED,
                       configuration_layer_colorbackground_event);
  btn_color_button.callback(buttons_callback, LV_EVENT_PRESSED,
                        configuration_layer_colorbutton_event);
  btn_confSys.callback(buttons_callback, LV_EVENT_PRESSED,
                       configuration_layer_confsys_event);

  btn_logout.callback(buttons_callback, LV_EVENT_PRESSED,
                      configuration_layer_logout_event);
  btn_reboot.callback(buttons_callback, LV_EVENT_PRESSED,
                      configuration_layer_reboot_event);
  btn_shutdown.callback(buttons_callback, LV_EVENT_PRESSED,
                        configuration_layer_shutdown_event);

  slider_backlightbright.create(layer_configurations);
  slider_backlightbright.set_pos(346 + 148, 20 + 32 - 5 + 13);
  slider_backlightbright.set_range(410, 4095);
  slider_backlightbright.add_event_cb(
      buttons_callback, LV_EVENT_VALUE_CHANGED,
      slider_backlight_bright_value_change_event);
  slider_backlightbright.set_ext_click_area(30);
  slider_backlightbright.set_size(20, 240);
  slider_backlightbright.set_value(lcd_bright);
  slider_backlightbright_style.set_bg_color(LV_PALETTE_PURPLE);
  slider_backlightbright.add_style(slider_backlightbright_style,
                                   LV_PART_INDICATOR);
  slider_backlightbright.add_style(slider_backlightbright_style, LV_PART_KNOB);

  slider_backlightbright_icon.create(layer_configurations);
  slider_backlightbright_icon.set_pos(336 + 138 + 10, 300 + 13);
  slider_backlightbright_icon_style.set_img_recolor(LV_PALETTE_PURPLE);
  slider_backlightbright_icon_style.set_img_recolor_opa(100);
  slider_backlightbright_icon.add_style(slider_backlightbright_icon_style,
                                        LV_PART_MAIN);

  slider_backlightbright_icon.set_src(&brightness_simp);
  slider_backlightbright_icon.update_layout();

  /* register ISR to handle with backlightTimer and uiResetTimer event */
  backlightTimer.attach(isr_backlightTimer);
  uiResetTimer.attach(isr_uiResetTimer);

  while (1) {

    /* wait for a notification
     * this hotification is sent by LVGL button with a event code */
    uint32_t button_event = taskButtons.takeNotify();

    /* each code do diferrent things on this switch case
     * the keyboard code sended by bleKeyborad is configured herer */
    switch (button_event) {

      /* --- MAIN APPLICATIONS --- */

    case terminal_event: /* terminal button is pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F1);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case files_event: /* files button is pressed */
      bleKeyboard.write(KEY_MEDIA_LOCAL_MACHINE_BROWSER);
      break;

    case web_event: /* web button is pressed */
      bleKeyboard.write(KEY_MEDIA_WWW_HOME);
      break;

    case search_event: /* search button is pressed */
      bleKeyboard.write(KEY_MEDIA_WWW_SEARCH);
      break;

    case applications_layer_event: /* applications button receive a short
                                     click */
      if (btn_multimedia_layer.is_fixed()) {
        /* if Multimedia layer is fixed does nothing */
        break;
      }
      btn_applications_layer.unfixe();
      layer_multimedia.hidden();
      layer_configurations.hidden();
      layer_applications.change_hidden();
      layer_applications.move_foreground();
      break;

    case applications_layer_fix_event: /* aplications button receive a long
                                         click */
      if (btn_multimedia_layer.is_fixed()) {
        /* if Multimedia layer is fixed does nothing */
        break;
      }
      btn_applications_layer.fixe();
      layer_applications.unhidden();
      layer_multimedia.hidden();
      layer_configurations.hidden();
      layer_applications.move_foreground();
      break;

    /* --- MAIN MULTIMEDIA --- */
    case multimedia_prev_event: /* multimedia_prev button is pressed */
      bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
      break;

    case multimedia_play_event: /* multimedia_play button is pressed */
      bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
      /* this button have two icons, play_simp and pause_simp this change
       * between this two icons */
      btn_mutimedia_play.swapIcon();
      break;

    case multimedia_next_event: /* multimedia_next button is pressed */
      bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
      break;

    case multimedia_mic_event: /* multimedia_mic button is pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F2);
      delay(10ms);
      bleKeyboard.releaseAll();
      /* mic button also have two icons, mic_on_simp and mic_off_simp */
      btn_multimedia_mic.swapIcon();
      break;

    case multimedia_layer_event: /* multimedia button receive a short
                                     click */
      if (btn_applications_layer.is_fixed()) {
        /* if Applications layer is fixed does nothing */
        break;
      }
      btn_multimedia_layer.unfixe();
      layer_applications.hidden();
      layer_configurations.hidden();
      layer_multimedia.change_hidden();
      layer_multimedia.move_foreground();
      break;

    case multimedia_layer_fix_event: /* multimedia button receive a long
                                     click */
      if (btn_applications_layer.is_fixed()) {
        /* if Applications layer is fixed does nothing */
        break;
      }
      btn_multimedia_layer.fixe();
      layer_applications.hidden();
      layer_configurations.hidden();
      layer_multimedia.unhidden();
      layer_multimedia.move_foreground();
      break;

    /* --- MAIN CONFIGURATIONS --- */
    case prev_workspace_event: /* prev_workspace button is pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F3);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case next_workspace_event: /* next_workspace button is pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F4);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case general_view_event: /* general_view button is pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F5);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case lock_computer_event: /* lock_computer button is pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(KEY_F6);
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case configurations_layer_event: /* configuration button is pressed */
      if (btn_applications_layer.is_fixed() || btn_multimedia_layer.is_fixed())
        break;
      layer_applications.hidden();
      layer_multimedia.hidden();
      layer_configurations.change_hidden();
      layer_configurations.move_foreground();
      break;

      /* --- LAYER APPLICATIONS --- */

    case applications_layer_app1_event: /* aplication_layer_app1 button is
                                           pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('9');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case applications_layer_app2_event: /* aplication_layer_app2 button is
                                           pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('0');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case applications_layer_app3_event: /* aplication_layer_app3 button is
                                           pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('d');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case applications_layer_app4_event: /* aplication_layer_app4 button is
                                           pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('f');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case applications_layer_app5_event: /* aplication_layer_app5 button is
                                           pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('g');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case applications_layer_app6_event: /* aplication_layer_app6 button is
                                           pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('h');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case applications_layer_app7_event: /* aplication_layer_app7 button is
                                           pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.press('b');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case applications_layer_app8_event: /* aplication_layer_app8 button is
                                           pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.press('u');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case applications_layer_app9_event: /* aplication_layer_app9 button is
                                           pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.press('s');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    /* --- LAYER MULTIMEDIA --- */
    case multimedia_layer_mult1_event: /* multimedia_layer_mult1 button is
                                          pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('q');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case multimedia_layer_mult2_event: /* multimedia_layer_mult2 button is
                                          pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('w');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case multimedia_layer_mult3_event: /* multimedia_layer_mult3 button is
                                          pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('e');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case multimedia_layer_mult4_event: /* multimedia_layer_mult4 button is
                                          pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('r');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case multimedia_layer_mult5_event: /* multimedia_layer_mult5 button is
                                          pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('t');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case multimedia_layer_mult6_event: /* multimedia_layer_mult6 button is
                                          pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('k');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case multimedia_layer_mult7_event: /* multimedia_layer_mult7 button is
                                          pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('i');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case multimedia_layer_mult8_event: /* multimedia_layer_mult8 button is
                                          pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('o');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case multimedia_layer_mult9_event: /* multimedia_layer_mult9 button is
                                          pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('p');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    /* --- LAYER CONFIGURATIONS --- */
    case configuration_layer_volmut_event: /* configuration_layer_volmut button
                                              is pressed */
      bleKeyboard.write(KEY_MEDIA_MUTE);
      break;

    case configuration_layer_voldown_event: /* configuration_layer_voldown
                                               button is pressed */
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
      taskButtons.takeNotify();
      break;
    case configuration_layer_volup_event: /* configuration_layer_volup button is
                                             pressed */
      bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
      taskButtons.takeNotify();
      break;

    case configuration_layer_colorbackground_event: /* configuration_layer_colorbackground
                                                       button is pressed */
      color_background = lvgl::settings::bgColor();
      lvgl::screen::set_bg_color(color_background);
      break;

    case configuration_layer_colorbutton_event: /* configuration_layer_colorbutton
                                                   button is pressed */
      color_buttons = lvgl::settings::buttonColor();
      /* all buttons share same style, only need change color of one */
      btn_terminal.color(color_buttons);
      /* style of backlight bright slider is change too */
      slider_backlightbright_style.set_bg_color(color_buttons);
      slider_backlightbright_icon_style.set_img_recolor(color_buttons);
      slider_backlightbright_icon.update_layout();
      lvgl::screen::refresh();
      break;

    case configuration_layer_confsys_event: /* configuration_layer_confsys
                                               button is pressed */
      bleKeyboard.write(KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION);
      break;

    case configuration_layer_logout_event: /* configuration_layer_logout button
                                              is pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('1');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case configuration_layer_reboot_event: /* configuration_layer_rebbo button
                                              is pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('2');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case configuration_layer_shutdown_event: /* configuration_layer_shutdown
                                                button is pressed */
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press('3');
      delay(10ms);
      bleKeyboard.releaseAll();
      break;

    case rotate_screen_event: /* this will be reality some day */
      // change_rotation();
      break;

    case slider_backlight_bright_value_change_event: /* bright control slider
                                                        value change */
      lcd_bright = slider_backlightbright.get_value();
      break;

    } // end switch case

    /**
     * if some event is received the UI is not inactive
     * backlight bright cahnge to setpoint value
     * and timers are manting reseted */
    lvgl::port::backlight_setRaw(lcd_bright);
    backlightTimer.reset();
    uiResetTimer.reset();

    delay(100ms);

  } // end handle_buttons's infinit loop

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
    btn_configurations_layer.position(14);
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
    btn_configurations_layer.position(14);
    break;
  } // end switch case
  lvgl::port::mutex_give();
} // end change_rotatio
#endif
