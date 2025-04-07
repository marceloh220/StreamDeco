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

#ifndef _STREAMDECO_OBJECTS_HPP_
#define _STREAMDECO_OBJECTS_HPP_

#include <Arduino.h>
#include <BleKeyboard.h>

#include "marcelino.hpp"

#include "streamDeco_settings.hpp"
#include "streamDeco_buttons.hpp"
#include "streamDeco_monitor.hpp"

namespace streamDeco
{

  constexpr long streamDecoTask_buttons_stackSize = 3_kB;
  constexpr long streamDecoTask_uiReset_stackSize = 1_kB;
  constexpr long streamDecoTask_monitor_stackSize = 3_kB;
  constexpr long streamDecoTask_clock_stackSize = 3_kB;

  /**
   * @enum     event_e
   * @brief    Events enumerations
   * @details  events registred on StreamDeco buttons
   **/
  enum event_e
  {

    nothing_event,

    /* --- MAIN CANVAS EVENTS --- */
    terminal_event,
    files_event,
    web_event,
    search_event,
    applications_canvas_event,
    applications_canvas_fix_event,

    multimedia_prev_event,
    multimedia_play_event,
    multimedia_next_event,
    multimedia_mic_event,
    multimedia_canvas_event,
    multimedia_canvas_fix_event,

    left_workspace_event,
    right_workspace_event,
    pin_window_event,
    lock_computer_event,
    desktop_mode_event,
    configurations_canvas_event,
    configurations_canvas_fix_event,

    /* --- APPLICATIONS CANVAS EVENTS --- */

    applications_canvas_app1_event,
    applications_canvas_app2_event,
    applications_canvas_app3_event,

    applications_canvas_app4_event,
    applications_canvas_app5_event,
    applications_canvas_app6_event,

    applications_canvas_app7_event,
    applications_canvas_app8_event,
    applications_canvas_app9_event,

    /* --- MULTIMEDIA CANVAS EVENTS --- */

    multimedia_canvas_mult1_event,
    multimedia_canvas_mult2_event,
    multimedia_canvas_mult3_event,

    multimedia_canvas_mult4_event,
    multimedia_canvas_mult5_event,
    multimedia_canvas_mult6_event,

    multimedia_canvas_mult7_event,
    multimedia_canvas_mult8_event,
    multimedia_canvas_mult9_event,

    /* --- CONFIGURATIONS CANVAS EVENTS --- */

    configuration_canvas_volmut_event,
    configuration_canvas_voldown_event,
    configuration_canvas_volup_event,

    configuration_canvas_colorbackground_event,
    configuration_canvas_colorbutton_event,
    configuration_canvas_rotate_screen_event, // experimenntal

    configuration_canvas_sysmonitor_event,
    configuration_canvas_sysconfig_event,
    configuration_canvas_logout_event,
    configuration_canvas_reboot_event,
    configuration_canvas_shutdown_event,

    slider_backlight_bright_value_change_event,

    /* --- CANVAS EVENT --- */

    hidden_canvas_event,

    /* --- Reduce backlight bright event --- */

    rest_backlight_event,

  };

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
    extern marcelino::File<settings_t> flash;

    /**
     * @var     cache
     * @brief   Memory cache for settings
     **/
    extern settings_t cache;

  } // namespace settings

  /**
   * @namespace  streamDecoTasks
   * @brief      Tasks of streamDeco
   **/
  namespace streamDecoTasks
  {
    /**
     * @var      buttons
     * @brief    Task buttons
     * @details  Task to manager buttons configurations and events
     **/
    extern rtos::TaskStatic<streamDecoTask_buttons_stackSize> buttons;

    /**
     * @var      uiReset
     * @brief    Task uiReset
     * @details  Task to hidde inactivities streamDecoCanvas
     **/
    extern rtos::TaskStatic<streamDecoTask_uiReset_stackSize> uiReset;

    /**
     * @var      monitor
     * @brief    Task monitor
     * @details  Task to show computer metrics on monitor streamDecoCanvas
     **/
    extern rtos::TaskStatic<streamDecoTask_monitor_stackSize> monitor;

    /**
     * @var      clock
     * @brief    Task clock
     * @details  Task to update clock
     **/
    extern rtos::TaskStatic<streamDecoTask_clock_stackSize> clock;
  } // namespace streamDecoTasks

  /**
   * @namespace  timer_ui
   * @brief      Timers
   **/
  namespace timer_ui
  {
    /**
     * @var      uiReset
     * @brief    uiReset timer
     * @details  Timer to generate an event to hidden additional streamDecoCanvas
     * @details  and return to Main screen in inactivitie
     * @note     The timer value can be change in file streamDeco_objects.cpp
     **/
    extern rtos::TimerStatic uiReset;

    /**
     * @var      backlight
     * @brief    Backlight timer
     * @details  Timer to reduce screen backlight in inactivitie
     * @note     The timer value can be change in file streamDeco_objects.cpp
     */
    extern rtos::TimerStatic backlight;
  } // namespace timer_ui

  /**
   * @namespace  streamDecoCanvas
   * @brief      Canvas to hold additional buttons
   * @details  applications streamDecoCanvas 9 applications buttons
   * @details  multimedia streamDecoCanvas 9 multimedia buttons
   * @details  configurations streamDecoCanvas 9 configurations buttons and bright streamDecoBrightSlider
   * @details  monitor streamDecoCanvas with computer metrics and clock
   */
  namespace streamDecoCanvas
  {
    /**
     * @var     applications
     * @brief   Applications Canvas
     * @details Canvas to hold addtional apps shortcuts
     **/
    extern lvgl::Canvas applications;

    /**
     * @var     multimedia
     * @brief   Multimedia Canvas
     * @details Canvas to hold multimedia apps shortcuts
     **/
    extern lvgl::Canvas multimedia;

    /**
     * @var     Configutations
     * @brief   Configurations Canvas
     * @details Canvas to hold configurations shortcuts
     **/
    extern lvgl::Canvas configurations;

    /**
     * @var     monitor
     * @brief   Monitor Canvas
     * @details Canvas to monitor computer stats
     **/
    extern lvgl::Canvas monitor;

    /**
     * @var     style_landscape
     * @brief   Style Canvas in landscape
     * @details Style for streamDecoCanvas in landscape */
    extern lvgl::Style style_landscape;

    /**
     * @var     style_portrait
     * @brief   Style Canvas in portrait
     * @details Style for streamDecoCanvas in portrait */
    extern lvgl::Style style_portrait;

    void init(lvgl::screen::rotation_t rotation);
    void portrait();
    void landscape();
  } // namespace streamDecoCanvas

  /**
   * @namespace  streamDecoButtons
   * @brief      Buttons of StreamDeco
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

    /* --- MAIN BUTTONS --- */

    /**
     * @var      terminal
     * @brief    terminal streamDecoButtons
     * @details  This streamDecoButtons is supposed to open terminal on computer
     **/
    extern MainButton terminal;

    /**
     * @var      files
     * @brief    files streamDecoButtons
     * @details  This streamDecoButtons is supposed to open file explorer on computer
     **/
    extern MainButton files;

    /**
     * @var      web
     * @brief    web streamDecoButtons
     * @details  This streamDecoButtons is supposed to open web browser on computer
     **/
    extern MainButton web;

    /**
     * @var      search
     * @brief    search streamDecoButtons
     * @details  This streamDecoButtons is supposed to open search mechanism on computer
     **/
    extern MainButton search;

    /**
     * @var      applications_canvas
     * @brief    applications_canvas streamDecoButtons
     * @details  This streamDecoButtons open, close or pin Applications streamDecoCanvas
     **/
    extern MainButton applications_canvas;

    /**
     * @var      multimedia_prev
     * @brief    Multimedia prev streamDecoButtons
     * @details  This streamDecoButtons pass to previous media on music player computer
     **/
    extern MainButton multimedia_prev;

    /**
     * @var      multimedia_play
     * @brief    Multimedia play streamDecoButtons
     * @details  This streamDecoButtons play/pause media on music player computer
     **/
    extern MainButton multimedia_play;

    /**
     * @var      multimedia_next
     * @brief    Multimedia next streamDecoButtons
     * @details  This streamDecoButtons pass to next media on music player computer
     **/
    extern MainButton multimedia_next;

    /**
     * @var      multimedia_mic
     * @brief    Multimedia mic streamDecoButtons
     * @details  This streamDecoButtons is supposed to mute/unmute microphone computer
     **/
    extern MainButton multimedia_mic;

    /**
     * @var      multimedia_canvas
     * @brief    multimedia_canvas streamDecoButtons
     * @details  This streamDecoButtons open, close or pin Multimedia streamDecoCanvas
     **/
    extern MainButton multimedia_canvas;

    /**
     * @var      left_workspace
     * @brief    Left workspace mic streamDecoButtons
     * @details  This streamDecoButtons is supposed to change to left workspace on computer
     **/
    extern MainButton left_workspace;

    /**
     * @var      right_workspace
     * @brief    Right workspace mic streamDecoButtons
     * @details  This streamDecoButtons is supposed to change to right workspace on computer
     **/
    extern MainButton right_workspace;

    /**
     * @var      pin
     * @brief    Pin streamDecoButtons
     * @details  This streamDecoButtons is supposed to pin/unpin window computer
     **/
    extern MainButton pin;

    /**
     * @var      desktop_mode
     * @brief    Lock streamDecoButtons
     * @details  This streamDecoButtons is supposed to desktop_mode computer
     **/
    extern MainButton desktop_mode;

    /**
     * @var      configurations_canvas
     * @brief    configurations_canvas streamDecoButtons
     * @details  This streamDecoButtons open, close or pin Configurations streamDecoCanvas
     **/
    extern MainButton configurations_canvas;

    /* --- APPLICATIONS BUTTONS --- */

    /**
     * @var      app1
     * @brief    Application 1 streamDecoButtons
     * @details  This streamDecoButtons open app 1 on computer
     **/
    extern CanvasButton app1;

    /**
     * @var      app2
     * @brief    Application 2 streamDecoButtons
     * @details  This streamDecoButtons open app 2 on computer
     **/
    extern CanvasButton app2;

    /**
     * @var      app3
     * @brief    Application 3 streamDecoButtons
     * @details  This streamDecoButtons open app 3 on computer
     **/
    extern CanvasButton app3;

    /**
     * @var      app4
     * @brief    Application 4 streamDecoButtons
     * @details  This streamDecoButtons open app 4 on computer
     **/
    extern CanvasButton app4;

    /**
     * @var      app5
     * @brief    Application 5 streamDecoButtons
     * @details  This streamDecoButtons open app 5 on computer
     **/
    extern CanvasButton app5;

    /**
     * @var      app6
     * @brief    Application 6 streamDecoButtons
     * @details  This streamDecoButtons open app 6 on computer
     **/
    extern CanvasButton app6;

    /**
     * @var      app7
     * @brief    Application 7 streamDecoButtons
     * @details  This streamDecoButtons open app 7 on computer
     **/
    extern CanvasButton app7;

    /**
     * @var      app8
     * @brief    Application 8 streamDecoButtons
     * @details  This streamDecoButtons open app 8 on computer
     **/
    extern CanvasButton app8;

    /**
     * @var      app9
     * @brief    Application 9 streamDecoButtons
     * @details  This streamDecoButtons open app 9 on computer
     **/
    extern CanvasButton app9;

    /* --- MULTIMEDIA BUTTONS --- */

    /**
     * @var      mult1
     * @brief    Multimedia 1 streamDecoButtons
     * @details  This streamDecoButtons execute multimedia action 1 on computer
     **/
    extern CanvasButton mult1;

    /**
     * @var      mult2
     * @brief    Multimedia 2 streamDecoButtons
     * @details  This streamDecoButtons execute multimedia action 2 on computer
     **/
    extern CanvasButton mult2;

    /**
     * @var      mult3
     * @brief    Multimedia 3 streamDecoButtons
     * @details  This streamDecoButtons execute multimedia action 3 on computer
     **/
    extern CanvasButton mult3;

    /**
     * @var      mult4
     * @brief    Multimedia 4 streamDecoButtons
     * @details  This streamDecoButtons execute multimedia action 4 on computer
     **/
    extern CanvasButton mult4;

    /**
     * @var      mult5
     * @brief    Multimedia 5 streamDecoButtons
     * @details  This streamDecoButtons execute multimedia action 5 on computer
     **/
    extern CanvasButton mult5;

    /**
     * @var      mult6
     * @brief    Multimedia 6 streamDecoButtons
     * @details  This streamDecoButtons execute multimedia action 6 on computer
     **/
    extern CanvasButton mult6;

    /**
     * @var      mult7
     * @brief    Multimedia 7 streamDecoButtons
     * @details  This streamDecoButtons execute multimedia action 7 on computer
     **/
    extern CanvasButton mult7;

    /**
     * @var      mult8
     * @brief    Multimedia 8 streamDecoButtons
     * @details  This streamDecoButtons execute multimedia action 8 on computer
     **/
    extern CanvasButton mult8;

    /**
     * @var      mult9
     * @brief    Multimedia 9 streamDecoButtons
     * @details  This streamDecoButtons execute multimedia action 9 on computer
     **/
    extern CanvasButton mult9;

    /* --- CONFIGURATIONS BUTTONS --- */

    /**
     * @var      volmult
     * @brief    Volume mute streamDecoButtons
     * @details  This streamDecoButtons mute/unmute audio output on computer
     **/
    extern ConfigButton volmut;

    /**
     * @var      voldown
     * @brief    Volume down streamDecoButtons
     * @details  This streamDecoButtons lower volume of audio output on computer
     **/
    extern ConfigButton voldown;

    /**
     * @var      volup
     * @brief    Volume up streamDecoButtons
     * @details  This streamDecoButtons riser volume of audio output on computer
     **/
    extern ConfigButton volup;

    /**
     * @var      color_background
     * @brief    Color backgound streamDecoButtons
     * @details  Change background color of StreamDeco
     **/
    extern ConfigButton color_background;

    /**
     * @var      color_button
     * @brief    Color streamDecoButtons streamDecoButtons
     * @details  Change streamDecoButtons color of StreamDeco
     **/
    extern ConfigButton color_button;

    /**
     * @brief in work
     */
    extern ConfigButton rotation;

    /**
     * @var      sysmonitor
     * @brief    System Monitor streamDecoButtons
     * @details  This streamDecoButtons is supposed to open system monitor on computer
     **/
    extern ConfigButton sysmonitor;

    /**
     * @var      sysconfig
     * @brief    System Configurations streamDecoButtons
     * @details  This streamDecoButtons is supposed to open configurations panel on computer
     **/
    extern ConfigButton sysconfig;

    /**
     * @var      reboot
     * @brief    Reboot streamDecoButtons
     * @details  This streamDecoButtons is supposed to restart StreamDeco
     **/
    extern ConfigButton reboot;

    /**
     * @brief  Create the main canva buttons
     * @param  settings  Settings configuration
     */
    void createMain(settings::settings_t settings);

    /**
     * @brief  Create the applications canva buttons
     * @param  parent    Object parent of the new slider
     * @param  settings  Settings configuration
     */
    void createApplication(lvgl::Object &parent, settings::settings_t settings);

    /**
     * @brief  Create the multimedia canva buttons
     * @param  parent    Object parent of the new slider
     * @param  settings  Settings configuration
     */
    void createMultimedia(lvgl::Object &parent, settings::settings_t settings);

    /**
     * @brief  Create the configuration canva buttons
     * @param  parent    Object parent of the new slider
     * @param  settings  Settings configuration
     */
    void createConfiguration(lvgl::Object &parent, settings::settings_t settings);

    /**
     * @brief  Change color of Buttons
     * @param  color  New button color
     * @note   Called in color_button event
     **/
    void color(lvgl::palette::palette_t color);

    /**
     * @brief  Sort buttons in portrait order
     **/
    void portrait();

    /**
     * @brief  Sort buttons in landscape order
     **/
    void landscape();

  } // namespace streamDecoButtons

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
    extern lvgl::Slider slider;

    /**
     * @var    styler
     * @brief  Backlight bright control streamDecoBrightSlider style
     **/
    extern lvgl::Style slider_style;

    /**
     * @var    icon
     * @brief  Backlight bright control icon
     **/
    extern lvgl::Image icon;

    /**
     * @var    icon
     * @brief  Backlight bright control icon style
     **/
    extern lvgl::Style icon_style;

    /**
     * @brief  Init backlight streamDecoBrightSlider
     * @param  parent    Object parent of the new slider
     * @param  callback  The new event function
     * @param  icon      Pointer to an lvgl::lv_img_dsc_t to be streamDecoBrightSlider icon
     * @param  settings  Settings configuration
     **/
    void init(lvgl::Object &parent, lvgl::icon_t _icon, settings::settings_t &settings);

    /**
     * @brief  Put slider in landscape format
     */
    void landscape();

    /**
     * @brief  Put slider in portrait format
     */
    void portrait();

    /**
     * @brief  Return slider bright value
     * @note   Same as slider.get_value
     */
    int read();

    /**
     * @brief  Change color of slider
     * @param  color  New slider color
     * @note   Called in color_button event
     **/
    void color(lvgl::palette::palette_t color);

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
    extern metric::Complete cpu;

    /**
     * @var      gpu
     * @brief    Metric GPU
     * @details  Show GPU load, temperature and frequency metrics in complete metric
     **/
    extern metric::Complete gpu;

    /**
     * @var      system
     * @brief    Metric system
     * @details  Show RAM and Disk usage metrics in basic metric
     **/
    extern metric::Basic system;

    /**
     * @var      clock
     * @brief    Clock metric
     * @details  Show clock with time and data
     **/
    extern metric::Clock clock;

    /**
     * @brief  Init system monitor applet
     * @param  parent  Object parent of the new slider
     * @param  color   Monitor applet color
     */
    void init(lvgl::Object &parent, lvgl::palette::palette_t color);

    /**
     * @brief  Change color of Monitor
     * @param  color  New monitor color
     * @note   Called in color_button event
     **/
    void color(lvgl::palette::palette_t color);

  } // namespace streamDecoMonitor

  /**
   * @var    bleKeyboard
   * @brief  Reference to blekeyboard object
   * @brief  Interface to send keyboard codes to Bluetooth
   */
  extern BleKeyboard bleKeyboard;

  /**
   * @var    mutex_serial
   * @brief  Reference to serial interface mutex
   * @note   Used to avoid Monitor and Clock streamDecoTasks uses Serial interface in same time
   */
  extern rtos::MutexRecursiveStatic mutex_serial;

  /**
   * @brief    Process buttons event
   * @param    button_event  Event generated by streamDecoButtons
   * @param    settings      Reference to StreamDeco settings
   * @details  Called by streamDecoTasks streamDecoButtons to send shortcuts to computer via BLE Bluetooth interface
   * @note     Each streamDecoButtons send a event code registred during streamDecoButtons configuration
   *
   */
  void process_event(uint32_t button_event);

} // namespace streamDeco

#endif