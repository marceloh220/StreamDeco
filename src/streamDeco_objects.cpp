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
 * @file    streamDeco_objects.cpp
 * @brief   Declare StreamDeco's objects
 * @details Icons and text can be change in this file
 */

#include "streamDeco_objects.hpp"
#include "streamDeco_monitor.hpp"

namespace streamDeco
{

  namespace settings
  {
    /**
     * @var     flash
     * @brief   Use NVS to keep settings safe into flash
     * @details Non-Volatile Storage (NVS) library is designed to store data in flash.
     * @details The data stored is not lost when SoC is reseted or power off.
     * @code
     * #define dataType int
     * marcelino::File<dataType>file_to_keep_data_into_flash("File Name");
     **/
    marcelino::File<settings_t> flash("Settings file");

    /**
     * @var     cache
     * @brief   Memory cache for settings
     **/
    settings_t cache;

  } // namespace settings

  /**
   * @namespace  streamDecoTasks
   * @brief      Tasks of streamDeco
   **/
  namespace streamDecoTasks
  {
    rtos::TaskStatic<streamDecoTask_buttons_stackSize> buttons("Task Buttons", 5);
    rtos::TaskStatic<streamDecoTask_uiReset_stackSize> uiReset("Task reset canvas", 3);
    rtos::TaskStatic<streamDecoTask_monitor_stackSize> monitor("Task Monitor", 5);
    rtos::TaskStatic<streamDecoTask_clock_stackSize> clock("Task Clock", 5);
  } // namespace streamDecoTask

  /**
   * @namespace  timer_ui
   * @brief      Timers
   * @details    Time to reset ui canvas or backlight sleep can be changed herer
   **/
  namespace timer_ui
  {
    rtos::TimerStatic uiReset("UI reset timer", 10s);
    rtos::TimerStatic backlight("Backlight timer", 30s);
  } // namespace timer_ui

  /**
   * @namespace  streamDecoCanvas
   * @brief Canvas to hold additional buttons
   */
  namespace streamDecoCanvas
  {
    /**
     * @var     applications
     * @brief   Applications Canvas
     * @details Canvas to hold addtional apps shortcuts
     **/
    lvgl::Canvas applications;

    /**
     * @var     multimedia
     * @brief   Multimedia Canvas
     * @details Canvas to hold multimedia apps shortcuts
     **/
    lvgl::Canvas multimedia;

    /**
     * @var     Configutations
     * @brief   Configurations Canvas
     * @details Canvas to hold configurations shortcuts
     **/
    lvgl::Canvas configurations;

    /**
     * @var     monitor
     * @brief   Monitor Canvas
     * @details Canvas to monitor computer stats
     **/
    lvgl::Canvas monitor;

    /**
     * @var     style_landscape
     * @brief   Style Canvas in landscape
     * @details Style for streamDecoCanvas in landscape */
    lvgl::Style style_landscape;

    /**
     * @var     style_portrait
     * @brief   Style Canvas in portrait
     * @details Style for streamDecoCanvas in portrait */
    lvgl::Style style_portrait;

    void init(lvgl::screen::rotation_t rotation)
    {
      /* configure style of streamDecoCanvas */
      style_landscape.set_pad_all(0);
      style_landscape.set_bg_color(lvgl::color::make(41, 45, 50));
      style_landscape.align(lvgl::alignment::CENTER, -74, 0);
      style_landscape.set_size(582, 470);

      style_portrait.set_pad_all(0);
      style_portrait.set_bg_color(lvgl::color::make(41, 45, 50));
      style_portrait.align(lvgl::alignment::CENTER, 0, -74);
      style_portrait.set_size(470, 582);

      /* configure Applications streamDecoCanvas */
      applications.create();
      applications.hidden();

      /* configure Multimedia streamDecoCanvas */
      multimedia.create();
      multimedia.hidden();

      /* Configurations streamDecoCanvas */
      configurations.create();
      configurations.hidden();

      /* Monitor streamDecoCanvas */
      monitor.create();
      monitor.hidden();

      rotation == lvgl::screen::LANDSCAPE ? landscape() : portrait();
    }

    void portrait()
    {
      lvgl::port::mutex_take();
      applications.add_style(style_portrait, lvgl::part::MAIN);
      multimedia.add_style(style_portrait, lvgl::part::MAIN);
      configurations.add_style(style_portrait, lvgl::part::MAIN);
      monitor.add_style(style_portrait, lvgl::part::MAIN);
      applications.update_layout();
      multimedia.update_layout();
      configurations.update_layout();
      monitor.update_layout();
      lvgl::port::mutex_give();
    }

    void landscape()
    {
      lvgl::port::mutex_take();
      applications.add_style(style_landscape, lvgl::part::MAIN);
      multimedia.add_style(style_landscape, lvgl::part::MAIN);
      configurations.add_style(style_landscape, lvgl::part::MAIN);
      monitor.add_style(style_landscape, lvgl::part::MAIN);
      applications.update_layout();
      multimedia.update_layout();
      configurations.update_layout();
      monitor.update_layout();
      lvgl::port::mutex_give();
    }
  } // namespace streamDecoCanvas

  /**
   * @namespace  streamDecoButtons
   * @brief      Buttons of StreamDeco
   * @details    Buttons's icons can be changed herer
   *
   * @note    The difference between mainIcon, CanvasButton and ConfigButton
   *          is position map
   * @code
   * streamDeco::MainButton(
   *          "Terminal",     // name, show name if icon not used
   *          &terminal_simp, // first icon
   *          NULL            // second icon, if used the icons can be swaped using swap_icon
   * ),
   **/
  namespace streamDecoButtons
  {

    /**
     * @brief   Callback registred in buttons
     * @details Send a notifications with event code to task buttons handler
     * @param   lvglEvent  Event received by the callback
     * @note    This callback is registred on buttons and streamDecoBrightSlider objects
     * @note    Each streamDecoButtons and streamDecoBrightSlider send a different event
     **/
    void buttons_callback(lvgl::event::event_t lvglEvent);

    /* ---   Main screen buttons   --- */

    /* Applications line, first line of buttons */
    streamDeco::MainButton terminal("Terminal", &terminal_simp, NULL);
    streamDeco::MainButton files("Files", &files_simp, NULL);
    streamDeco::MainButton web("Web", &web_simp, NULL);
    streamDeco::MainButton search("Search", &search_simp, NULL);
    streamDeco::MainButton applications_canvas("Application", &applications_simp, NULL);

    /* Multimedia line, second line of buttons */
    streamDeco::MainButton multimedia_prev("Prev", &backward_simp, NULL);
    streamDeco::MainButton multimedia_play("Play/Pause", &play_simp, &pause_simp);
    streamDeco::MainButton multimedia_next("Next", &forward_simp, NULL);
    streamDeco::MainButton multimedia_mic("Mic", &mic_on_simp, &mic_off_simp);
    streamDeco::MainButton multimedia_canvas("Multimedia", &multimedia_simp, NULL);

    /* Configurations line, third line of buttons */
    streamDeco::MainButton left_workspace("Left Workspace", &previous_workspace_simp, NULL);
    streamDeco::MainButton right_workspace("Right Workspace", &next_workspace_simp, NULL);
    streamDeco::MainButton pin("Pin", &pin_simp, &unpin_simp);
    streamDeco::MainButton desktop_mode("Desktop Mode", &mult_window_simp, &single_window_simp);
    streamDeco::MainButton configurations_canvas("Config", &config_simp, NULL);

    /* ---   Applications canvas buttons   --- */

    /* First line */
    CanvasButton app1("app1", &gogcom_simp, NULL);
    CanvasButton app2("app2", &discord_simp, NULL);
    CanvasButton app3("app3", &fps_simp, NULL);

    /* Second line */
    CanvasButton app4("app4", &code_simp, NULL);
    CanvasButton app5("app5", &texcompiler_simp, NULL);
    CanvasButton app6("app6", &calculator_simp, NULL);

    /* Third line */
    CanvasButton app7("app7", &build_simp, NULL);
    CanvasButton app8("app8", &download_simp, NULL);
    CanvasButton app9("app9", &serialport_simp, NULL);

    /* ---   Multimedia canvas buttons   --- */

    /* First line */
    CanvasButton mult1("mult 1", &video_stop_capt_simp, &video_start_capt_simp);
    CanvasButton mult2("mult 2", &mic_off_simp, &mic_on_simp);
    CanvasButton mult3("mult 3", &screen_capt_simp, NULL);

    /* Second line */
    CanvasButton mult4("mult 4", &add_clip_simp, NULL);
    CanvasButton mult5("mult 5", &ripple_simp, NULL);
    CanvasButton mult6("mult 6", &rolling_simp, NULL);

    /* Third line */
    CanvasButton mult7("mult 7", &seek_backward_simp, NULL);
    CanvasButton mult8("mult 8", &play_simp, NULL);
    CanvasButton mult9("mult 9", &seek_forward_simp, NULL);

    /* ---   Configurations canvas buttons   --- */

    /* First line */
    ConfigButton volmut("Vol Mute", &volume_mute_simp, NULL);
    ConfigButton voldown("Vol Down", &volume_low_simp, NULL);
    ConfigButton volup("Vol Up", &volume_high_simp, NULL);

    /* Second line */
    ConfigButton color_background("Color BG", &bgtheme_simp, NULL);
    ConfigButton color_button("Color Buttons", &btntheme_simp, NULL);
    ConfigButton rotation("Rotation", &rotation_simp, NULL);

    /* Third line */
    ConfigButton sysmonitor("System Monitor", &sysmon_simp, NULL);
    ConfigButton sysconfig("System Config", &config_simp, NULL);
    ConfigButton reboot("Reboot", &reboot_simp, NULL);

    /**
     * @brief  Create the main canva buttons
     * @param  settings  Settings configuration
     */
    void createMain(settings::settings_t settings)
    {
      /* the class MainButton deal with position buttons,
       * is only necessary pass the order */
      terminal.create(0, settings.color_buttons);
      files.create(1, settings.color_buttons);
      web.create(2, settings.color_buttons);
      search.create(3, settings.color_buttons);
      applications_canvas.create(4, settings.color_buttons);

      multimedia_prev.create(5, settings.color_buttons);
      multimedia_play.create(6, settings.color_buttons);
      multimedia_next.create(7, settings.color_buttons);
      multimedia_mic.create(8, settings.color_buttons);
      multimedia_canvas.create(9, settings.color_buttons);

      left_workspace.create(10, settings.color_buttons);
      right_workspace.create(11, settings.color_buttons);
      pin.create(12, settings.color_buttons);
      desktop_mode.create(13, settings.color_buttons);
      configurations_canvas.create(14, settings.color_buttons);

      /* register buttons event */
      terminal.callback(buttons_callback, lvgl::event::PRESSED, terminal_event);
      files.callback(buttons_callback, lvgl::event::PRESSED, files_event);
      web.callback(buttons_callback, lvgl::event::PRESSED, web_event);
      search.callback(buttons_callback, lvgl::event::PRESSED, search_event);
      applications_canvas.callback(buttons_callback, lvgl::event::SHORT_CLICKED, applications_canvas_event);
      applications_canvas.callback(buttons_callback, lvgl::event::LONG_PRESSED, applications_canvas_fix_event);

      multimedia_prev.callback(buttons_callback, lvgl::event::PRESSED, multimedia_prev_event);
      multimedia_play.callback(buttons_callback, lvgl::event::PRESSED, multimedia_play_event);
      multimedia_next.callback(buttons_callback, lvgl::event::PRESSED, multimedia_next_event);
      multimedia_mic.callback(buttons_callback, lvgl::event::PRESSED, multimedia_mic_event);
      multimedia_canvas.callback(buttons_callback, lvgl::event::SHORT_CLICKED, multimedia_canvas_event);
      multimedia_canvas.callback(buttons_callback, lvgl::event::LONG_PRESSED, multimedia_canvas_fix_event);

      left_workspace.callback(buttons_callback, lvgl::event::PRESSED, left_workspace_event);
      right_workspace.callback(buttons_callback, lvgl::event::PRESSED, right_workspace_event);
      pin.callback(buttons_callback, lvgl::event::PRESSED, pin_window_event);
      desktop_mode.callback(buttons_callback, lvgl::event::PRESSED, desktop_mode_event);
      configurations_canvas.callback(buttons_callback, lvgl::event::SHORT_CLICKED, configurations_canvas_event);
      configurations_canvas.callback(buttons_callback, lvgl::event::LONG_PRESSED, configurations_canvas_fix_event);
    }

    /**
     * @brief  Create the applications canva buttons
     * @param  parent    Object parent of the new slider
     * @param  settings  Settings configuration
     */
    void createApplication(lvgl::Object &parent, settings::settings_t settings)
    {
      app1.create(parent, 0, settings.color_buttons);
      app2.create(parent, 1, settings.color_buttons);
      app3.create(parent, 2, settings.color_buttons);

      app4.create(parent, 3, settings.color_buttons);
      app5.create(parent, 4, settings.color_buttons);
      app6.create(parent, 5, settings.color_buttons);

      app7.create(parent, 6, settings.color_buttons);
      app8.create(parent, 7, settings.color_buttons);
      app9.create(parent, 8, settings.color_buttons);

      /* register the events of buttons */
      app1.callback(buttons_callback, lvgl::event::PRESSED, applications_canvas_app1_event);
      app2.callback(buttons_callback, lvgl::event::PRESSED, applications_canvas_app2_event);
      app3.callback(buttons_callback, lvgl::event::PRESSED, applications_canvas_app3_event);

      app4.callback(buttons_callback, lvgl::event::PRESSED, applications_canvas_app4_event);
      app5.callback(buttons_callback, lvgl::event::PRESSED, applications_canvas_app5_event);
      app6.callback(buttons_callback, lvgl::event::PRESSED, applications_canvas_app6_event);

      app7.callback(buttons_callback, lvgl::event::PRESSED, applications_canvas_app7_event);
      app8.callback(buttons_callback, lvgl::event::PRESSED, applications_canvas_app8_event);
      app9.callback(buttons_callback, lvgl::event::PRESSED, applications_canvas_app9_event);
    }

    /**
     * @brief  Create the multimedia canva buttons
     * @param  parent    Object parent of the new slider
     * @param  settings  Settings configuration
     */
    void createMultimedia(lvgl::Object &parent, settings::settings_t settings)
    {
      mult1.create(parent, 0, settings.color_buttons);
      mult2.create(parent, 1, settings.color_buttons);
      mult3.create(parent, 2, settings.color_buttons);

      mult4.create(parent, 3, settings.color_buttons);
      mult5.create(parent, 4, settings.color_buttons);
      mult6.create(parent, 5, settings.color_buttons);

      mult7.create(parent, 6, settings.color_buttons);
      mult8.create(parent, 7, settings.color_buttons);
      mult9.create(parent, 8, settings.color_buttons);

      /* register the events of buttons */
      mult1.callback(buttons_callback, lvgl::event::PRESSED, multimedia_canvas_mult1_event);
      mult2.callback(buttons_callback, lvgl::event::PRESSED, multimedia_canvas_mult2_event);
      mult3.callback(buttons_callback, lvgl::event::PRESSED, multimedia_canvas_mult3_event);

      mult4.callback(buttons_callback, lvgl::event::PRESSED, multimedia_canvas_mult4_event);
      mult5.callback(buttons_callback, lvgl::event::PRESSED, multimedia_canvas_mult5_event);
      mult6.callback(buttons_callback, lvgl::event::PRESSED, multimedia_canvas_mult6_event);

      mult7.callback(buttons_callback, lvgl::event::PRESSED, multimedia_canvas_mult7_event);
      mult8.callback(buttons_callback, lvgl::event::PRESSED, multimedia_canvas_mult8_event);
      mult9.callback(buttons_callback, lvgl::event::PRESSED, multimedia_canvas_mult9_event);

      /* change pinned color of some buttons */
      mult1.iconPinnedColor(lvgl::color::make(255, 0, 0));
      mult1.buttonPinnedColor(lvgl::palette::CYAN);
      mult2.iconPinnedColor(lvgl::color::make(255, 0, 0));
      mult2.buttonPinnedColor(lvgl::palette::CYAN);
    }

    /**
     * @brief  Create the configuration canva buttons
     * @param  parent    Object parent of the new slider
     * @param  settings  Settings configuration
     */
    void createConfiguration(lvgl::Object &parent, settings::settings_t settings)
    {
      volmut.create(parent, 0, settings.color_buttons);
      voldown.create(parent, 1, settings.color_buttons);
      volup.create(parent, 2, settings.color_buttons);

      color_background.create(parent, 3, settings.color_buttons);
      color_button.create(parent, 4, settings.color_buttons);
      rotation.create(parent, 5, settings.color_buttons);

      sysmonitor.create(parent, 6, settings.color_buttons);
      sysconfig.create(parent, 7, settings.color_buttons);
      reboot.create(parent, 8, settings.color_buttons);

      /* register the events of buttons */
      volmut.callback(buttons_callback, lvgl::event::PRESSED, configuration_canvas_volmut_event);
      voldown.callback(buttons_callback, lvgl::event::PRESSING, configuration_canvas_voldown_event);
      volup.callback(buttons_callback, lvgl::event::PRESSING, configuration_canvas_volup_event);

      color_background.callback(buttons_callback, lvgl::event::PRESSED, configuration_canvas_colorbackground_event);
      color_button.callback(buttons_callback, lvgl::event::PRESSED, configuration_canvas_colorbutton_event);
      rotation.callback(buttons_callback, lvgl::event::PRESSED, configuration_canvas_rotate_screen_event);

      sysmonitor.callback(buttons_callback, lvgl::event::PRESSED, configuration_canvas_sysmonitor_event);
      sysconfig.callback(buttons_callback, lvgl::event::PRESSED, configuration_canvas_sysconfig_event);
      reboot.callback(buttons_callback, lvgl::event::PRESSED, configuration_canvas_reboot_event);
    }

    /**
     * @brief  Change color of Buttons
     * @param  color  New button color
     * @note   Called in color_button event
     **/
    void color(lvgl::palette::palette_t color)
    {
      terminal.buttonColor(color);
      files.buttonColor(color);
      web.buttonColor(color);
      search.buttonColor(color);
      applications_canvas.buttonColor(color);
      multimedia_prev.buttonColor(color);
      multimedia_play.buttonColor(color);
      multimedia_next.buttonColor(color);
      multimedia_mic.buttonColor(color);
      multimedia_canvas.buttonColor(color);
      left_workspace.buttonColor(color);
      right_workspace.buttonColor(color);
      pin.buttonColor(color);
      desktop_mode.buttonColor(color);
      configurations_canvas.buttonColor(color);
      app1.buttonColor(color);
      app2.buttonColor(color);
      app3.buttonColor(color);
      app4.buttonColor(color);
      app5.buttonColor(color);
      app6.buttonColor(color);
      app7.buttonColor(color);
      app8.buttonColor(color);
      app9.buttonColor(color);
      mult1.buttonColor(color);
      mult2.buttonColor(color);
      mult3.buttonColor(color);
      mult4.buttonColor(color);
      mult5.buttonColor(color);
      mult6.buttonColor(color);
      mult7.buttonColor(color);
      mult8.buttonColor(color);
      mult9.buttonColor(color);
      volmut.buttonColor(color);
      voldown.buttonColor(color);
      volup.buttonColor(color);
      color_background.buttonColor(color);
      color_button.buttonColor(color);
      rotation.buttonColor(color);
      sysmonitor.buttonColor(color);
      sysconfig.buttonColor(color);
      reboot.buttonColor(color);
    }

    /**
     * @brief  Sort buttons in portrait order
     **/
    void portrait()
    {
      terminal.position(0);
      files.position(3);
      web.position(6);
      search.position(9);
      applications_canvas.position(12);
      multimedia_prev.position(1);
      multimedia_play.position(4);
      multimedia_next.position(7);
      multimedia_mic.position(10);
      multimedia_canvas.position(13);
      left_workspace.position(2);
      right_workspace.position(5);
      pin.position(8);
      desktop_mode.position(11);
      configurations_canvas.position(14);
      volmut.position(0);
      voldown.position(1);
      volup.position(2);
      color_background.position(3);
      color_button.position(4);
      rotation.position(5);
      sysmonitor.position(6);
      sysconfig.position(7);
      reboot.position(8);
    }

    /**
     * @brief  Sort buttons in landscape order
     **/
    void landscape()
    {
      terminal.position(0);
      files.position(1);
      web.position(2);
      search.position(3);
      applications_canvas.position(4);
      multimedia_prev.position(5);
      multimedia_play.position(6);
      multimedia_next.position(7);
      multimedia_mic.position(8);
      multimedia_canvas.position(9);
      left_workspace.position(10);
      right_workspace.position(11);
      pin.position(12);
      desktop_mode.position(13);
      configurations_canvas.position(14);
      volmut.position(0);
      voldown.position(1);
      volup.position(2);
      color_background.position(3);
      color_button.position(4);
      rotation.position(5);
      sysmonitor.position(6);
      sysconfig.position(7);
      reboot.position(8);
    }

  };

  /**
   * @namespace  streamDecoBrightSlider
   * @brief      Bright control Slider
   * @details    Organize streamDecoBrightSlider bight backlight
   **/
  namespace streamDecoBrightSlider
  {
    /**
     * @var    slider
     * @brief  Backlight bright control streamDecoBrightSlider
     **/
    lvgl::Slider slider;

    /**
     * @var    styler
     * @brief  Backlight bright control streamDecoBrightSlider style
     **/
    lvgl::Style slider_style;

    /**
     * @var    icon
     * @brief  Backlight bright control icon
     **/
    lvgl::Image icon;

    /**
     * @var    icon
     * @brief  Backlight bright control icon style
     **/
    lvgl::Style icon_style;

    /**
     * @brief  Init backlight streamDecoBrightSlider
     * @param  parent    Object parent of the new slider
     * @param  callback  The new event function
     * @param  icon      Pointer to an lvgl::lv_img_dsc_t to be streamDecoBrightSlider icon
     * @param  settings  Settings configuration
     **/
    void init(lvgl::Object &parent, lvgl::icon_t _icon, settings::settings_t &settings)
    {
      /* configure streamDecoBrightSlider bright */
      slider.create(parent);
      slider.set_range(410, 4095);
      slider.set_ext_click_area(30);
      slider.set_value(settings.lcd_bright);
      slider_style.set_bg_color(settings.color_buttons);
      slider.add_style(slider_style, lvgl::part::INDICATOR);
      slider.add_style(slider_style, lvgl::part::KNOB);
      slider.add_event_cb(streamDecoButtons::buttons_callback, lvgl::event::VALUE_CHANGED, slider_backlight_bright_value_change_event);
      icon.create(parent);
      icon_style.set_img_recolor(settings.color_buttons);
      icon_style.set_img_recolor_opa(lvgl::opacity::OPA_COVER);
      icon.add_style(icon_style, lvgl::part::MAIN);
      icon.set_src(_icon);
      lvgl::port::backlight_setRaw(settings.lcd_bright);
      settings.rotation == lvgl::screen::LANDSCAPE ? landscape() : portrait();
    }

    /**
     * @brief  Put slider in landscape format
     */
    void landscape()
    {
      slider.set_pos(494 + 10, 92);
      slider.set_size(20, 240);
      icon.set_pos(484 + 10, 345);
      icon.update_layout();
    }

    /**
     * @brief  Put slider in portrait format
     */
    void portrait()
    {
      slider.set_pos(92, 494 + 10);
      slider.set_size(240, 20);
      icon.set_pos(345, 484 + 10);
      icon.update_layout();
    }

    /**
     * @brief  Return slider bright value
     * @note   Same as slider.get_value
     */
    int read()
    {
      return slider.get_value();
    }

    /**
     * @brief  Change color of slider
     * @param  color  New slider color
     * @note   Called in color_button event
     **/
    void color(lvgl::palette::palette_t color)
    {
      lvgl::port::mutex_take();
      slider_style.set_bg_color(color);
      icon_style.set_img_recolor(color);
      icon.update_layout();
      lvgl::port::mutex_give();
    }

  } // namespace streamDecoBrightSlider

  /**
   * @namespace  streamDecoMonitor
   * @brief      Monitor computer metrics
   * @details    Organize monitor metrics
   **/
  namespace streamDecoMonitor
  {
    /**
     * @var      cpu
     * @brief    Metric CPU
     * @details  Show CPU load, temperature and frequency metrics in complete metric
     **/
    metric::Complete cpu("CPU", &processor_22_simp);

    /**
     * @var      gpu
     * @brief    Metric GPU
     * @details  Show GPU load, temperature and frequency metrics in complete metric
     **/
    metric::Complete gpu("GPU", &gpu_22_simp);

    /**
     * @var      system
     * @brief    Metric system
     * @details  Show RAM and Disk usage metrics in basic metric
     **/
    metric::Basic system("MEM", &ram_22_simp);

    /**
     * @var      clock
     * @brief    Clock metric
     * @details  Show clock with time and data
     **/
    metric::Clock clock("Clock", &clock_22_simp);

    /**
     * @brief  Init system monitor applet
     * @param  parent  Object parent of the new slider
     * @param  color   Monitor applet color
     */
    void init(lvgl::Object &parent, lvgl::palette::palette_t color)
    {
      cpu.create(parent, color);
      cpu.set_size(280, 200);
      cpu.set_pos(14, 25);

      cpu.bar1_set_range(0, 100);
      cpu.bar2_set_range(0, 3600);

      gpu.create(parent, color);
      gpu.set_size(280, 200);
      gpu.set_pos(14, 25 + 200 + 20);

      gpu.bar1_set_range(0, 100);
      gpu.bar2_set_range(0, 3300);

      system.create(parent, color);
      system.set_size(250, 200);
      system.set_pos(14 + 280 + 14, 25);

      clock.create(parent, color);
      clock.set_size(250, 200);
      clock.set_pos(14 + 280 + 14, 25 + 200 + 20);
    } // end init

    /**
     * @brief  Change color of Monitor
     * @param  color  New monitor color
     * @note   Called in color_button event
     **/
    void color(lvgl::palette::palette_t color)
    {
      lvgl::port::mutex_take();
      cpu.color(color);
      gpu.color(color);
      system.color(color);
      clock.color(color);
      lvgl::port::mutex_give();
    }

  } // namespace streamDecoMonitor

  /**
   * @var     blekeyboard
   * @brief   BLE Bluetooth keyboard comunications
   * @details Edit Bluetooth device herer
   * @code
   * BleKeyboard bleKeyboard(
   *          "StreamDeco",    // device name
   *          "Marcelino Inc", // device manufactory
   *          100              // battery level
   * );
   **/
  BleKeyboard bleKeyboard("StreamDeco", "Marcelino Inc", 100);

  /**
   * @var  mutex_serial
   * @brief  Serial interface mutex
   * @note   Used to avoid Monitor and Clock task uses Serial interface in same instant
   */
  rtos::MutexRecursiveStatic mutex_serial;

} // namespace streamDeco
