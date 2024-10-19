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
 * 
 * @file  streamDeco_init.cpp
 * @brief Init StreamDeco
 */

#include "streamDeco_objects.hpp"
#include "streamDeco_handlers.hpp"
#include "streamDeco_timerCallback.hpp"
#include "streamDeco_icons.h"

namespace streamDeco
{

  /**
   * @brief   Init StreamDeco
   * @details Attach StreamDeco's tasks and made buttons configurations, layers and timers
   * @details Called in function main_app, function handler task or Arduino setup to init StreamDeco
   *
   */
  void init()
  {

    /**
     * @var      settings_t *settings
     * @brief    Wallpaper color, buttons theme and screen bright
     * @details  lv_color_t color_background
     *           lv_palette_t color_buttons
     *           int lcd_bright
     * @note     This settings is passed to handleButtons task
     *           so need be allocate into heap
     **/
    settings_t *settings = new settings_t;

    /* set initial screen rotation and color */
    lvgl::screen::set_rotation(LV_DISP_ROT_NONE);
    lvgl::screen::set_bg_color(LV_PALETTE_DEEP_ORANGE);

    /*  used to temporarily show apresentation screen
     *  the most part of this screens is a delay to show the icons and text
     *  actually useless but still nice to see */
    lvgl::Label startScreen_label;
    lvgl::Image startScreen_icon;

    /* --- START SCREEN --- */

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

    rtos::sleep(1s); /* see my icon =) */

    /* start bluetooth keyboard interface */
    bleKeyboard.begin();

    /* change icon to show connecting */
    startScreen_label.set_text("Connecting...");
    startScreen_icon.set_src(&bluetooth_simp);
    lvgl::screen::refresh();

    rtos::sleep(500ms); /* see more icon =) */

    /* try connection, while you see icons and text =) */
    while (1)
    {
      if (bleKeyboard.isConnected())
      {
        break;
      }
      rtos::sleep(2s);
    }

    /* change icon to waiting StreamDeco monitor Synchronization */
    startScreen_label.set_text("Start StreamDeco monitor");
    startScreen_icon.set_src(&keyboard_simp);
    lvgl::screen::refresh();

    /** This task wait for streamDeco monitor application to synchro clock
     *  Affter 40 tryes without success in synchronization the StreamDeco 
     *  start without correct date.
    */
    task.clock.attach(handleClock);

    /** Give time to handleClock task start, and see more icon =) */
    rtos::sleep(1s);

    /** This mutex is take by synchronization task and make initialization wait by syncrho tryes */
    mutex_serial.take();

    /* delete apresentation icons and text =( */
    startScreen_icon.del();
    startScreen_label.del();
    lvgl::screen::refresh();

    /* --- MAIN BUTTONS --- */

    /* the class mainButton deal with position buttons,
     * is only necessary pass the order */
    button.terminal.create(0);
    button.files.create(1);
    button.web.create(2);
    button.search.create(3);
    button.applications_canvas.create(4);

    button.multimedia_prev.create(5);
    button.multimedia_play.create(6);
    button.multimedia_next.create(7);
    button.multimedia_mic.create(8);
    button.multimedia_canvas.create(9);

    button.left_workspace.create(10);
    button.right_workspace.create(11);
    button.pin.create(12);
    button.lock.create(13);
    button.configurations_canvas.create(14);

    /* register buttons event */
    button.terminal.callback(buttons_callback, LV_EVENT_PRESSED, terminal_event);
    button.files.callback(buttons_callback, LV_EVENT_PRESSED, files_event);
    button.web.callback(buttons_callback, LV_EVENT_PRESSED, web_event);
    button.search.callback(buttons_callback, LV_EVENT_PRESSED, search_event);
    button.applications_canvas.callback(buttons_callback, LV_EVENT_SHORT_CLICKED, applications_canvas_event);
    button.applications_canvas.callback(buttons_callback, LV_EVENT_LONG_PRESSED, applications_canvas_fix_event);

    button.multimedia_prev.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_prev_event);
    button.multimedia_play.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_play_event);
    button.multimedia_next.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_next_event);
    button.multimedia_mic.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_mic_event);
    button.multimedia_canvas.callback(buttons_callback, LV_EVENT_SHORT_CLICKED, multimedia_canvas_event);
    button.multimedia_canvas.callback(buttons_callback, LV_EVENT_LONG_PRESSED, multimedia_canvas_fix_event);

    button.left_workspace.callback(buttons_callback, LV_EVENT_PRESSED, left_workspace_event);
    button.right_workspace.callback(buttons_callback, LV_EVENT_PRESSED, right_workspace_event);
    button.pin.callback(buttons_callback, LV_EVENT_PRESSED, pin_window_event);
    button.lock.callback(buttons_callback, LV_EVENT_PRESSED, lock_computer_event);
    button.configurations_canvas.callback(buttons_callback, LV_EVENT_SHORT_CLICKED, configurations_canvas_event);
    button.configurations_canvas.callback(buttons_callback, LV_EVENT_LONG_PRESSED, configurations_canvas_fix_event);

    /* configure style of canvas */
    canvas.style.set_pad_all(0);
    canvas.style.set_bg_color(lv_color_make(41, 45, 50));
    canvas.style.align(LV_ALIGN_CENTER, -74, 0);
    canvas.style.set_size(572, 470);

    /* --- APLICATIONS CANVAS BUTTONS --- */

    /* configure Applications canvas */
    canvas.applications.create();
    canvas.applications.add_style(canvas.style, LV_PART_MAIN);
    canvas.applications.hidden();

    /* apps buttons is created on Applications canvas */
    button.app1.create(canvas.applications, 0);
    button.app2.create(canvas.applications, 1);
    button.app3.create(canvas.applications, 2);

    button.app4.create(canvas.applications, 3);
    button.app5.create(canvas.applications, 4);
    button.app6.create(canvas.applications, 5);

    button.app7.create(canvas.applications, 6);
    button.app8.create(canvas.applications, 7);
    button.app9.create(canvas.applications, 8);

    /* register the events of buttons */
    button.app1.callback(buttons_callback, LV_EVENT_PRESSED, applications_canvas_app1_event);
    button.app2.callback(buttons_callback, LV_EVENT_PRESSED, applications_canvas_app2_event);
    button.app3.callback(buttons_callback, LV_EVENT_PRESSED, applications_canvas_app3_event);

    button.app4.callback(buttons_callback, LV_EVENT_PRESSED, applications_canvas_app4_event);
    button.app5.callback(buttons_callback, LV_EVENT_PRESSED, applications_canvas_app5_event);
    button.app6.callback(buttons_callback, LV_EVENT_PRESSED, applications_canvas_app6_event);

    button.app7.callback(buttons_callback, LV_EVENT_PRESSED, applications_canvas_app7_event);
    button.app8.callback(buttons_callback, LV_EVENT_PRESSED, applications_canvas_app8_event);
    button.app9.callback(buttons_callback, LV_EVENT_PRESSED, applications_canvas_app9_event);

    /* --- MULTIMEDIA CANVAS BUTTONS --- */

    /* configure Multimedia canvas */
    canvas.multimedia.create();
    canvas.multimedia.add_style(canvas.style, LV_PART_MAIN);
    canvas.multimedia.hidden();

    /* multimedia buttons is created on Multimedia canvas */
    button.mult1.create(canvas.multimedia, 0);
    button.mult2.create(canvas.multimedia, 1);
    button.mult3.create(canvas.multimedia, 2);

    button.mult4.create(canvas.multimedia, 3);
    button.mult5.create(canvas.multimedia, 4);
    button.mult6.create(canvas.multimedia, 5);

    button.mult7.create(canvas.multimedia, 6);
    button.mult8.create(canvas.multimedia, 7);
    button.mult9.create(canvas.multimedia, 8);

    /* register the events of buttons */
    button.mult1.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_canvas_mult1_event);
    button.mult2.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_canvas_mult2_event);
    button.mult3.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_canvas_mult3_event);

    button.mult4.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_canvas_mult4_event);
    button.mult5.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_canvas_mult5_event);
    button.mult6.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_canvas_mult6_event);

    button.mult7.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_canvas_mult7_event);
    button.mult8.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_canvas_mult8_event);
    button.mult9.callback(buttons_callback, LV_EVENT_PRESSED, multimedia_canvas_mult9_event);

    /* --- CONFIGURATIONS --- */

    /* Configurations canvas */
    canvas.configurations.create();
    canvas.configurations.add_style(canvas.style, LV_PART_MAIN);
    canvas.configurations.hidden();

    /* configurations buttons is created on Configured canvas */
    button.volmut.create(canvas.configurations, 0);
    button.voldown.create(canvas.configurations, 1);
    button.volup.create(canvas.configurations, 2);

    button.color_background.create(canvas.configurations, 3);
    button.color_button.create(canvas.configurations, 4);
    button.rotation.create(canvas.configurations, 5);

    button.sysmonitor.create(canvas.configurations, 6);
    button.sysconfig.create(canvas.configurations, 7);
    button.reboot.create(canvas.configurations, 8);

    /* register the events of buttons */
    button.volmut.callback(buttons_callback, LV_EVENT_PRESSED, configuration_canvas_volmut_event);
    button.voldown.callback(buttons_callback, LV_EVENT_PRESSING, configuration_canvas_voldown_event);
    button.volup.callback(buttons_callback, LV_EVENT_PRESSING, configuration_canvas_volup_event);

    button.color_background.callback(buttons_callback, LV_EVENT_PRESSED,configuration_canvas_colorbackground_event);
    button.color_button.callback(buttons_callback, LV_EVENT_PRESSED, configuration_canvas_colorbutton_event);
    button.rotation.callback(buttons_callback, LV_EVENT_PRESSED, configuration_canvas_rotate_screen_event);

    button.sysmonitor.callback(buttons_callback, LV_EVENT_PRESSED, configuration_canvas_sysmonitor_event);
    button.sysconfig.callback(buttons_callback, LV_EVENT_PRESSED, configuration_canvas_sysconfig_event);
    button.reboot.callback(buttons_callback, LV_EVENT_PRESSED, configuration_canvas_reboot_event);

    /* configure slider bright */
    slider.backlightbright_slider.create(canvas.configurations);
    slider.backlightbright_slider.set_pos(346 + 148, 20 + 32 - 5 + 13);
    slider.backlightbright_slider.set_range(410, 4095);
    slider.backlightbright_slider.set_ext_click_area(30);
    slider.backlightbright_slider.set_size(20, 240);
    slider.backlightbright_slider.set_value(settings->lcd_bright = 2457);
    slider.backlightbright_slider_style.set_bg_color(LV_PALETTE_PURPLE);
    slider.backlightbright_slider.add_style(slider.backlightbright_slider_style, LV_PART_INDICATOR);
    slider.backlightbright_slider.add_style(slider.backlightbright_slider_style, LV_PART_KNOB);
    slider.backlightbright_slider.add_event_cb(buttons_callback, LV_EVENT_VALUE_CHANGED, slider_backlight_bright_value_change_event);

    slider.backlightbright_icon.create(canvas.configurations);
    slider.backlightbright_icon_style.set_pos(336 + 138 + 10, 300 + 13);
    slider.backlightbright_icon_style.set_img_recolor(LV_PALETTE_PURPLE);
    slider.backlightbright_icon_style.set_img_recolor_opa(100);
    slider.backlightbright_icon.add_style(slider.backlightbright_icon_style, LV_PART_MAIN);
    slider.backlightbright_icon.set_src(&brightness_simp);
    slider.backlightbright_icon.update_layout();

    /* --- MONITOR --- */

    canvas.monitor.create();
    canvas.monitor.add_style(canvas.style, LV_PART_MAIN);
    canvas.monitor.hidden();

    monitor.cpu.create(canvas.monitor);
    monitor.cpu.set_size(280, 200);
    monitor.cpu.set_pos(14, 25);

    monitor.cpu.bar1_set_range(0, 100);
    monitor.cpu.bar2_set_range(0, 3600);

    monitor.gpu.create(canvas.monitor);
    monitor.gpu.set_size(280, 200);
    monitor.gpu.set_pos(14, 25 + 200 + 20);

    monitor.gpu.bar1_set_range(0, 100);
    monitor.gpu.bar2_set_range(0, 3300);

    monitor.system.create(canvas.monitor);
    monitor.system.set_size(250, 200);
    monitor.system.set_pos(14 + 280 + 14, 25);

    monitor.clock.create(canvas.monitor);
    monitor.clock.set_size(250, 200);
    monitor.clock.set_pos(14 + 280 + 14, 25 + 200 + 20);

    /* register ISR to handle with timer_ui.backlight and uiResetTimer event */
    timer_ui.backlight.attach(timer_callback);
    timer_ui.uiReset.attach(timer_callback);

    /* start timer_ui.backlight and uiResetTimer */
    timer_ui.backlight.start();
    timer_ui.uiReset.start();

    mutex_serial.give();

    task.buttons.attach(handleButtons, settings);
    task.uiReset.attach(handleUiReset);
    task.monitor.attach(handleMonitor);

  } // function init end

  /**
   * @brief   Print tasks memory usage
   * @details Called in function main_app loop, function handler task loop or Arduino loop
   * @details to print on serial monitor tasks memory usage
   */
  void print_task_memory_usage()
  {
    printf("Task Buttons mem usage %d\n",  task.buttons.memUsage());
    printf("Task UI Reset mem usage %d\n", task.uiReset.memUsage());
    printf("Task Monitor mem usage %d\n",  task.monitor.memUsage());
    printf("Task Ckock mem usage %d\n",    task.clock.memUsage());
  }

#ifdef DECO_ROTATION
  /* Experimental, someday will roll. Gambare gambare senpai! */
  void change_rotation();
  
  void change_rotation()
  {
    lvgl::port::mutex_take();
    lv_disp_rot_t rotation = lvgl::screen::get_rotation();
    switch (rotation)
    {
    case LV_DISP_ROT_NONE:
    case LV_DISP_ROT_180:
      lvgl::screen::set_rotation(LV_DISP_ROT_90);
      button.terminal.position(0);
      button.files.position(1);
      button.web.position(2);
      button.multimedia_prev.position(3);
      button.multimedia_play.position(4);
      button.multimedia_next.position(5);
      button.search.position(6);
      button.voldown.position(7);
      button.volup.position(8);
      button.left_workspace.position(9);
      button.right_workspace.position(10);
      button.pin.position(11);
      // button.calc.position(12);
      button.lock.position(13);
      button.configurations_screen.position(14);
      break;
    case LV_DISP_ROT_90:
    case LV_DISP_ROT_270:
      lvgl::screen::set_rotation(LV_DISP_ROT_NONE);
      button.terminal.position(0);
      button.files.position(1);
      button.web.position(2);
      button.search.position(3);
      // button.calc.position(4);
      button.multimedia_prev.position(5);
      button.multimedia_play.position(6);
      button.multimedia_next.position(7);
      button.voldown.position(8);
      button.volup.position(9);
      button.left_workspace.position(10);
      button.right_workspace.position(11);
      button.pin.position(12);
      button.lock.position(13);
      button.configurations_screen.position(14);
      break;
    } // end switch case
    lvgl::port::mutex_give();
  } // end change_rotatio
#endif

} // namespace streamDeco
