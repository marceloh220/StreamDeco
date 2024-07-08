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

#include "streamDeco_objects.hpp"

namespace streamDeco
{

  /* Handle the buttons task, made buttons configurations and manager buttons
   * events. This events can generate a keyboard code to send or change StreamDeco
   * configurations. */
  void handle_buttons(arg_t arg);

  /* Handle the ui reset task, each 10 seconds a timer will pass a event to this
   * task to hidden additional layers and return to Main layer. */
  void handle_uiReset(arg_t arg);

  /* Called in main_app or setup to init StreamDeco */
  void init()
  {
    task.buttons.attach(handle_buttons);
    task.uiReset.attach(handle_uiReset);
  }
  /* Called in main_app loop or loop to print memory usage of tasks */
  void print_task_memory_usage()
  {
    printf("Task Buttons mem usage %d\n", task.buttons.memUsage());
    printf("Task UI Reset mem usage %d\n", task.uiReset.memUsage());
  }

  /* Experimental, someday will roll. Gambare gambare senpai! */
  void change_rotation();

  /* ISR to handle backlight timer_ui. Set backlight bright to 10%. */
  void isr_timer_backlight(TimerHandle_t arg)
  {
    if (timer_ui.backlight.verifyID(arg))
    {
      lvgl::port::backlight_set(.1);
    }
  }

  /* ISR to UI reset, send a event to a task to hidden layers and show Main Layer
   * again. */
  void isr_timer_uiReset(TimerHandle_t arg)
  {
    if (timer_ui.uiReset.verifyID(arg))
    {
      task.uiReset.sendNotify(hidden_layers_event);
    }
  }

  /* Callback registred in buttons, send a notifications with event code to task
   * buttons handler. */
  void buttons_callback(lvgl::event::event_t e)
  {
    task.buttons.sendNotify(lvgl::event::get_user_data<int>(e));
  }

  /* Hnadler of UI reset task, hidden layers if they are not fixed. */
  void handle_uiReset(arg_t arg)
  {
    while (1)
    {
      uint32_t e = task.uiReset.takeNotify();
      if (e == hidden_layers_event)
      {
        if (!button.applications_layer.is_fixed())
        {
          layer.applications.hidden();
        }
        if (!button.multimedia_layer.is_fixed())
        {
          layer.multimedia.hidden();
        }
        /* Configuration Layer don't*/
        layer.configurations.hidden();
      }
    }
  }

  /* Handle of task buttons, init buttons configurations and manager events. */
  void handle_buttons(arg_t arg)
  {

    /* set initial screen rotation and color */
    lvgl::screen::set_rotation(LV_DISP_ROT_NONE);
    lvgl::screen::set_bg_color(LV_PALETTE_DEEP_ORANGE);

    /*  used to temporarily show initiation screen
     *  the most part of this screens is a delay to show the icons and text
     *  actually useless but still nice to see */
    lvgl::Label startScreen_label;
    lvgl::Image startScreen_icon;

    settings_t settings;

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
    while (1)
    {
      if (bleKeyboard.isConnected())
      {
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
    button.terminal.create(0);
    button.files.create(1);
    button.web.create(2);
    button.search.create(3);
    button.applications_layer.create(4);

    button.multimedia_prev.create(5);
    button.multimedia_play.create(6);
    button.multimedia_next.create(7);
    button.multimedia_mic.create(8);
    button.multimedia_layer.create(9);

    button.left_workspace.create(10);
    button.right_workspace.create(11);
    button.pin.create(12);
    button.lock.create(13);
    button.configurations_layer.create(14);

    /* register the buttons event */
    button.terminal.callback(buttons_callback, LV_EVENT_PRESSED, terminal_event);
    button.files.callback(buttons_callback, LV_EVENT_PRESSED, files_event);
    button.web.callback(buttons_callback, LV_EVENT_PRESSED, web_event);
    button.search.callback(buttons_callback, LV_EVENT_PRESSED, search_event);
    button.applications_layer.callback(buttons_callback, LV_EVENT_SHORT_CLICKED,
                                       applications_layer_event);
    button.applications_layer.callback(buttons_callback, LV_EVENT_LONG_PRESSED,
                                       applications_layer_fix_event);

    button.multimedia_prev.callback(buttons_callback, LV_EVENT_PRESSED,
                                    multimedia_prev_event);
    button.multimedia_play.callback(buttons_callback, LV_EVENT_PRESSED,
                                    multimedia_play_event);
    button.multimedia_next.callback(buttons_callback, LV_EVENT_PRESSED,
                                    multimedia_next_event);
    button.multimedia_mic.callback(buttons_callback, LV_EVENT_PRESSED,
                                   multimedia_mic_event);
    button.multimedia_layer.callback(buttons_callback, LV_EVENT_SHORT_CLICKED,
                                     multimedia_layer_event);
    button.multimedia_layer.callback(buttons_callback, LV_EVENT_LONG_PRESSED,
                                     multimedia_layer_fix_event);

    button.left_workspace.callback(buttons_callback, LV_EVENT_PRESSED,
                                   left_workspace_event);
    button.right_workspace.callback(buttons_callback, LV_EVENT_PRESSED,
                                    right_workspace_event);
    button.pin.callback(buttons_callback, LV_EVENT_PRESSED, pin_window_event);
    button.lock.callback(buttons_callback, LV_EVENT_PRESSED, lock_computer_event);
    button.configurations_layer.callback(buttons_callback, LV_EVENT_PRESSED,
                                         configurations_layer_event);

    /* --- APLICATIONS --- */

    /* configure Applications Layer */
    layer.applications.create();
    layer.applications.align(LV_ALIGN_CENTER, -74, 0);
    layer.applications.set_size(572, 470);
    layer.applications.set_bg_color(lv_color_make(41, 45, 50));
    layer.applications.hidden();

    /* now the buttons is created in Applications Layer
     * the layerButton also deal with screen position */
    button.app1.create(layer.applications, 0);
    button.app2.create(layer.applications, 1);
    button.app3.create(layer.applications, 2);

    button.app4.create(layer.applications, 3);
    button.app5.create(layer.applications, 4);
    button.app6.create(layer.applications, 5);

    button.app7.create(layer.applications, 6);
    button.app8.create(layer.applications, 7);
    button.app9.create(layer.applications, 8);

    button.app1.callback(buttons_callback, LV_EVENT_PRESSED,
                         applications_layer_app1_event);
    button.app2.callback(buttons_callback, LV_EVENT_PRESSED,
                         applications_layer_app2_event);
    button.app3.callback(buttons_callback, LV_EVENT_PRESSED,
                         applications_layer_app3_event);

    button.app4.callback(buttons_callback, LV_EVENT_PRESSED,
                         applications_layer_app4_event);
    button.app5.callback(buttons_callback, LV_EVENT_PRESSED,
                         applications_layer_app5_event);
    button.app6.callback(buttons_callback, LV_EVENT_PRESSED,
                         applications_layer_app6_event);

    button.app7.callback(buttons_callback, LV_EVENT_PRESSED,
                         applications_layer_app7_event);
    button.app8.callback(buttons_callback, LV_EVENT_PRESSED,
                         applications_layer_app8_event);
    button.app9.callback(buttons_callback, LV_EVENT_PRESSED,
                         applications_layer_app9_event);

    /* --- MULTIMEDIA --- */

    /* Multimedia Layer work like the Applications Layer
     * creating the layer and buttons with same methods */
    layer.multimedia.create();
    layer.multimedia.align(LV_ALIGN_CENTER, -74, 0);
    layer.multimedia.set_size(572, 470);
    layer.multimedia.set_bg_color(lv_color_make(41, 45, 50));
    layer.multimedia.hidden();

    button.multimedia_mult1.create(layer.multimedia, 0);
    button.multimedia_mult2.create(layer.multimedia, 1);
    button.multimedia_mult3.create(layer.multimedia, 2);

    button.multimedia_mult4.create(layer.multimedia, 3);
    button.multimedia_mult5.create(layer.multimedia, 4);
    button.multimedia_mult6.create(layer.multimedia, 5);

    button.multimedia_mult7.create(layer.multimedia, 6);
    button.multimedia_mult8.create(layer.multimedia, 7);
    button.multimedia_mult9.create(layer.multimedia, 8);

    /* register the events of buttons */
    button.multimedia_mult1.callback(buttons_callback, LV_EVENT_PRESSED,
                                     multimedia_layer_mult1_event);
    button.multimedia_mult2.callback(buttons_callback, LV_EVENT_PRESSED,
                                     multimedia_layer_mult2_event);
    button.multimedia_mult3.callback(buttons_callback, LV_EVENT_PRESSED,
                                     multimedia_layer_mult3_event);

    button.multimedia_mult4.callback(buttons_callback, LV_EVENT_PRESSED,
                                     multimedia_layer_mult4_event);
    button.multimedia_mult5.callback(buttons_callback, LV_EVENT_PRESSED,
                                     multimedia_layer_mult5_event);
    button.multimedia_mult6.callback(buttons_callback, LV_EVENT_PRESSED,
                                     multimedia_layer_mult6_event);

    button.multimedia_mult7.callback(buttons_callback, LV_EVENT_PRESSED,
                                     multimedia_layer_mult7_event);
    button.multimedia_mult8.callback(buttons_callback, LV_EVENT_PRESSED,
                                     multimedia_layer_mult8_event);
    button.multimedia_mult9.callback(buttons_callback, LV_EVENT_PRESSED,
                                     multimedia_layer_mult9_event);

    /* --- CONFIGURATIONS --- */

    /* Configuratio Layer only use diferent position map from other layers */
    layer.configurations.create();
    layer.configurations.align(LV_ALIGN_CENTER, -74, 0);
    layer.configurations.set_size(572, 470);
    layer.configurations.set_bg_color(lv_color_make(41, 45, 50));
    layer.configurations.hidden();

    button.volmut.create(layer.configurations, 0);
    button.voldown.create(layer.configurations, 1);
    button.volup.create(layer.configurations, 2);

    button.color_background.create(layer.configurations, 3);
    button.color_button.create(layer.configurations, 4);
    button.rotation.create(layer.configurations, 5);

    button.sysmonitor.create(layer.configurations, 6);
    button.sysconfig.create(layer.configurations, 7);
    button.shutdown.create(layer.configurations, 8);

    button.volmut.callback(buttons_callback, LV_EVENT_PRESSED,
                           configuration_layer_volmut_event);
    button.voldown.callback(buttons_callback, LV_EVENT_PRESSING,
                            configuration_layer_voldown_event);
    button.volup.callback(buttons_callback, LV_EVENT_PRESSING,
                          configuration_layer_volup_event);

    button.color_background.callback(buttons_callback, LV_EVENT_PRESSED,
                                     configuration_layer_colorbackground_event);
    button.color_button.callback(buttons_callback, LV_EVENT_PRESSED,
                                 configuration_layer_colorbutton_event);
    button.rotation.callback(buttons_callback, LV_EVENT_PRESSED,
                            configuration_layer_rotate_screen_event);

    button.sysmonitor.callback(buttons_callback, LV_EVENT_PRESSED,
                           configuration_layer_sysmonitor_event);
    button.sysconfig.callback(buttons_callback, LV_EVENT_PRESSED,
                           configuration_layer_sysconfig_event);
    button.shutdown.callback(buttons_callback, LV_EVENT_PRESSED,
                             configuration_layer_shutdown_event);

    slider.backlightbright.create(layer.configurations);
    slider.backlightbright.set_pos(346 + 148, 20 + 32 - 5 + 13);
    slider.backlightbright.set_range(410, 4095);
    slider.backlightbright.add_event_cb(
        buttons_callback, LV_EVENT_VALUE_CHANGED,
        slider_backlight_bright_value_change_event);
    slider.backlightbright.set_ext_click_area(30);
    slider.backlightbright.set_size(20, 240);
    slider.backlightbright.set_value(settings.lcd_bright = 2457);
    slider.backlightbright_style.set_bg_color(LV_PALETTE_PURPLE);
    slider.backlightbright.add_style(slider.backlightbright_style,
                                     LV_PART_INDICATOR);
    slider.backlightbright.add_style(slider.backlightbright_style, LV_PART_KNOB);

    slider.backlightbright_icon.create(layer.configurations);
    slider.backlightbright_icon.set_pos(336 + 138 + 10, 300 + 13);
    slider.backlightbright_icon_style.set_img_recolor(LV_PALETTE_PURPLE);
    slider.backlightbright_icon_style.set_img_recolor_opa(100);
    slider.backlightbright_icon.add_style(slider.backlightbright_icon_style,
                                          LV_PART_MAIN);

    slider.backlightbright_icon.set_src(&brightness_simp);
    slider.backlightbright_icon.update_layout();

    /* register ISR to handle with timer_ui.backlight and uiResetTimer event */
    timer_ui.backlight.attach(isr_timer_backlight);
    timer_ui.uiReset.attach(isr_timer_uiReset);

    while (1)
    {

      /* wait for a notification
       * this hotification is sent by LVGL button with a event code */
      uint32_t button_event = task.buttons.takeNotify();

      process_event(button_event, &settings);

      /**
       * if some event is received the UI is not inactive
       * backlight bright cahnge to setpoint value
       * and timers are manting reseted */
      lvgl::port::backlight_setRaw(settings.lcd_bright);
      timer_ui.backlight.reset();
      timer_ui.uiReset.reset();

      delay(100ms);

    } // end handle_buttons's infinit loop

  } // end handle_buttons

#ifdef DECO_ROTATION
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
      button.configurations_layer.position(14);
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
      button.configurations_layer.position(14);
      break;
    } // end switch case
    lvgl::port::mutex_give();
  } // end change_rotatio
#endif

} // namespace streamDeco
