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

/**
 * @def      STACK_BUTTONS_SIZE
 * @brief    Stack size of buttons task
 * @details  Buttons task have stak of 4KBytes (4096 bytes)
 **/
#define STACK_BUTTONS_SIZE byte_k(4)

namespace streamDeco
{

  /**
   * @struct   task_s
   * @typedef  task_t
   * @brief    Typedef of tasks struct
   * @details  Organize tasks into one struct
   */
  typedef struct task_s
  {
    /**
     * @var      buttons
     * @brief    Task buttons
     * @details  Task to manager buttons configurations and events
     **/
    rtos::Task buttons;
    
    /**
     * @var      uiReset
     * @brief    Task uiReset
     * @details  Task to hidde inactivities canvas
     **/
    rtos::Task uiReset;
    
    /** 
     * @var      monitor
     * @brief    Task monitor
     * @details  Task to show computer metrics on monitor canvas
     **/
    rtos::Task monitor;
    
    /**
     * @var      clock
     * @brief    Task clock
     * @details  Task to update clock
     **/
    rtos::Task clock;
  } task_t;

  /**
   * @var      task
   * @brief    Reference to task struct
   * @details  Organize tasks into one struct
   * @details  buttons task configure buttons and handle with buttons events
   * @details  uiReset task hidde inactivities canvas
   * @details  monitor task show computer metrics on monitor canvas
   * @details  clock task upate clock on monitor canvas
   */
  extern task_t task;

  /**
   * @var    bleKeyboard
   * @brief  Reference to blekeyboard object
   * @brief  Interface to send keyboard codes to Bluetooth
   */
  extern BleKeyboard bleKeyboard;

  /**
   * @struct   timer_s
   * @typedef  timer_t
   * @brief    Typedef of timer struct
   * @details  Organize tasks into one struct
   */
  typedef struct timer_s
  {
    /** 
     * @var      uiReset
     * @brief    uiReset timer
     * @details  Timer to generate an event to hidden additional canvas 
     * @details  and return to Main screen in inactivitie
     * @note     The timer value can be change in file streamDeco_objects.cpp
     **/
    rtos::Timer uiReset;
    
    /** 
     * @var      backlight
     * @brief    Backlight timer
     * @details  Timer to reduce screen backlight in inactivitie
     * @note     The timer value can be change in file streamDeco_objects.cpp
    */
    rtos::Timer backlight;
  } timer_t;

  /**
   * @var      timer_ui
   * @brief    Organize timers into one struct
   * @details  uiReset timer to hidden canvas
   * @details  backlight reduce screen bright
   * */
  extern timer_t timer_ui;

  /**
   * @struct   canvas_s
   * @typedef  canvas_t
   * @brief    Typedef of canvas struct
   * @details  Organize canvas into one struct
   **/
  typedef struct canvas_s
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
     * @var     style
     * @brief   Style Canvas
     * @details Style for canvas */
    lvgl::Style style;
  } canvas_t;

  /**
   * @var      Canvas
   * @brief    Organize canvas into one struct
   * @details  applications canvas 9 applications buttons
   * @details  multimedia canvas 9 multimedia buttons
   * @details  configurations canvas 9 configurations buttons and bright slider 
   * @details  monitor canvas with computer metrics and clock
   * */
  extern canvas_t canvas;

  /**
   * @struct   button_s
   * @typedef  button_t
   * @brief    Typedef of button struct
   * @details  Organize buttons into one struct
   */
  typedef struct button_s
  {

    /* --- MAIN BUTTONS --- */

    /**
     * @var      terminal
     * @brief    terminal button
     * @details  This button is supposed to open terminal on computer
     **/
    mainButton terminal;
    
    /**
     * @var      files
     * @brief    files button
     * @details  This button is supposed to open file explorer on computer
     **/
    mainButton files;
    
    /**
     * @var      web
     * @brief    web button
     * @details  This button is supposed to open web browser on computer
     **/
    mainButton web;
    
    /**
     * @var      search
     * @brief    search button
     * @details  This button is supposed to open search mechanism on computer
     **/
    mainButton search;
    
    /**
     * @var      applications_canvas
     * @brief    applications_canvas button
     * @details  This button open, close or pin Applications canvas
     **/
    mainButton applications_canvas;

    /**
     * @var      multimedia_prev
     * @brief    Multimedia prev button
     * @details  This button pass to previous media on music player computer
     **/
    mainButton multimedia_prev;
    
    /**
     * @var      multimedia_play
     * @brief    Multimedia play button
     * @details  This button play/pause media on music player computer
     **/
    mainButton multimedia_play;

    /**
     * @var      multimedia_next
     * @brief    Multimedia next button
     * @details  This button pass to next media on music player computer
     **/
    mainButton multimedia_next;

    /**
     * @var      multimedia_mic
     * @brief    Multimedia mic button
     * @details  This button is supposed to mute/unmute microphone computer
     **/
    mainButton multimedia_mic;

    /**
     * @var      multimedia_canvas
     * @brief    multimedia_canvas button
     * @details  This button open, close or pin Multimedia canvas
     **/
    mainButton multimedia_canvas;

    /**
     * @var      left_workspace
     * @brief    Left workspace mic button
     * @details  This button is supposed to change to left workspace on computer
     **/
    mainButton left_workspace;

    /**
     * @var      right_workspace
     * @brief    Right workspace mic button
     * @details  This button is supposed to change to right workspace on computer
     **/
    mainButton right_workspace;

    /**
     * @var      pin
     * @brief    Pin button
     * @details  This button is supposed to pin/unpin window computer
     **/
    mainButton pin;

    /**
     * @var      lock
     * @brief    Lock button
     * @details  This button is supposed to lock computer
     **/
    mainButton lock;

    /**
     * @var      configurations_canvas
     * @brief    configurations_canvas button
     * @details  This button open, close or pin Configurations canvas
     **/
    mainButton configurations_canvas;

    /* --- APPLICATIONS BUTTONS --- */

    /**
     * @var      app1
     * @brief    Application 1 button
     * @details  This button open app 1 on computer
     **/
    canvasButton app1;

    /**
     * @var      app2
     * @brief    Application 2 button
     * @details  This button open app 2 on computer
     **/
    canvasButton app2;

    /**
     * @var      app3
     * @brief    Application 3 button
     * @details  This button open app 3 on computer
     **/
    canvasButton app3;

    /**
     * @var      app4
     * @brief    Application 4 button
     * @details  This button open app 4 on computer
     **/
    canvasButton app4;

    /**
     * @var      app5
     * @brief    Application 5 button
     * @details  This button open app 5 on computer
     **/
    canvasButton app5;

    /**
     * @var      app6
     * @brief    Application 6 button
     * @details  This button open app 6 on computer
     **/
    canvasButton app6;

    /**
     * @var      app7
     * @brief    Application 7 button
     * @details  This button open app 7 on computer
     **/
    canvasButton app7;

    /**
     * @var      app8
     * @brief    Application 8 button
     * @details  This button open app 8 on computer
     **/
    canvasButton app8;

    /**
     * @var      app9
     * @brief    Application 9 button
     * @details  This button open app 9 on computer
     **/
    canvasButton app9;

    /* --- MULTIMEDIA BUTTONS --- */

    /**
     * @var      mult1
     * @brief    Multimedia 1 button
     * @details  This button execute multimedia action 1 on computer
     **/
    canvasButton mult1;

    /**
     * @var      mult2
     * @brief    Multimedia 2 button
     * @details  This button execute multimedia action 2 on computer
     **/
    canvasButton mult2;

    /**
     * @var      mult3
     * @brief    Multimedia 3 button
     * @details  This button execute multimedia action 3 on computer
     **/
    canvasButton mult3;

    /**
     * @var      mult4
     * @brief    Multimedia 4 button
     * @details  This button execute multimedia action 4 on computer
     **/
    canvasButton mult4;

    /**
     * @var      mult5
     * @brief    Multimedia 5 button
     * @details  This button execute multimedia action 5 on computer
     **/
    canvasButton mult5;

    /**
     * @var      mult6
     * @brief    Multimedia 6 button
     * @details  This button execute multimedia action 6 on computer
     **/
    canvasButton mult6;

    /**
     * @var      mult7
     * @brief    Multimedia 7 button
     * @details  This button execute multimedia action 7 on computer
     **/
    canvasButton mult7;

    /**
     * @var      mult8
     * @brief    Multimedia 8 button
     * @details  This button execute multimedia action 8 on computer
     **/
    canvasButton mult8;

    /**
     * @var      mult9
     * @brief    Multimedia 9 button
     * @details  This button execute multimedia action 9 on computer
     **/
    canvasButton mult9;

    /* --- CONFIGURATIONS BUTTONS --- */

    /**
     * @var      volmult
     * @brief    Volume mute button
     * @details  This button mute/unmute audio output on computer
     **/
    configButton volmut;

    /**
     * @var      voldown
     * @brief    Volume down button
     * @details  This button lower volume of audio output on computer
     **/
    configButton voldown;

    /**
     * @var      volup
     * @brief    Volume up button
     * @details  This button riser volume of audio output on computer
     **/
    configButton volup;

    /**
     * @var      color_background
     * @brief    Color backgound button
     * @details  Change background color of StreamDeco
     **/
    configButton color_background;

    /**
     * @var      color_button
     * @brief    Color button button
     * @details  Change button color of StreamDeco
     **/
    configButton color_button;
    
    /**
     * @brief in work
     */
    configButton rotation;

    /**
     * @var      sysmonitor
     * @brief    System Monitor button
     * @details  This button is supposed to open system monitor on computer
     **/
    configButton sysmonitor;

    /**
     * @var      sysconfig
     * @brief    System Configurations button
     * @details  This button is supposed to open configurations panel on computer
     **/
    configButton sysconfig;

    /**
     * @var      shutdown
     * @brief    Shutdown button
     * @details  This button is supposed to shutdown on computer
     **/
    configButton shutdown;

  } button_t;

  /**
   * @var      button
   * @brief    Reference to streamDeco buttons
   * @details  Organize streamDeco buttons into one struct
   **/
  extern button_t button;

  /**
   * @struct   slider_s
   * @typedef  slider_t
   * @brief    Bright control Slider
   * @details  Organize slider bight backlight
   **/
  typedef struct slider_s
  {
    /**
     * @var    backlightbright_slider
     * @brief  Backlight bright control slider
     **/
    lvgl::Slider backlightbright_slider;

    /**
     * @var    backlightbright_styler
     * @brief  Backlight bright control slider style
     **/
    lvgl::Style backlightbright_slider_style;
    
    /**
     * @var    backlightbright_icon
     * @brief  Backlight bright control icon
     **/
    lvgl::Image backlightbright_icon;

    /**
     * @var    backlightbright_icon
     * @brief  Backlight bright control icon style
     **/
    lvgl::Style backlightbright_icon_style;
  } slider_t;

  /**
   * @var      slider
   * @brief    Reference to backlight bright control slider
   * @details  Organize backlight bright control slider into one struct
   **/
  extern slider_t slider;

  /**
   * @struct   monitor_s
   * @typedef  monitor_t
   * @brief    Typedef of monitor metrics struct
   * @details  Organize monbitor metrics into one struct
   **/
  typedef struct monitor_s {
    /**
     * @var      cpu
     * @brief    Metric CPU
     * @details  Show CPU load, temperature and frequency metrics in complete metric
     **/
    metric::Complete cpu;

    /**
     * @var      gpu
     * @brief    Metric GPU
     * @details  Show GPU load, temperature and frequency metrics in complete metric
     **/
    metric::Complete gpu;

    /**
     * @var      system
     * @brief    Metric system
     * @details  Show RAM and Disk usage metrics in basic metric
     **/
    metric::Basic system;

    /**
     * @var      clock
     * @brief    Clock metric
     * @details  Show clock with time and data
     **/
    metric::Clock clock;
  } monitor_t;

  /**
   * @var      monitor
   * @brief    Reference to monitor computer metrics
   * @details  Organize monitor computer metrics into one struct
   **/
  extern monitor_t monitor;

  /**
   * @var    mutex_serial
   * @brief  Reference to serial interface mutex
   * @note   Used to avoid Monitor and Clock task uses Serial interface in same time
   */
  extern rtos::Mutex mutex_serial;

  /**
   * @enum     event_e
   * @brief    Events enumerations
   * @details  events registred on StreamDeco buttons
   **/
  enum event_e
  {

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

  };

  /**
   * @struct   settings_s
   * @typedef  settings_t
   * @brief    Typedef of StreamDeco settings
   * @details  Organize StreamDeco settings into one struct
   **/
  typedef struct settings_s
  {
    /**
     * @var      color_background 
     * @brief    StreamDeco background color
     * @details  Used to change color of StreamDeco background
     **/
    lv_color_t color_background;

    /**
     * @var      color_buttons
     * @brief    StreamDeco buttons color
     * @details  Used to change color of StreamDeco buttons
     **/
    lv_palette_t color_buttons;

    /**
     * @var lcd_bright
     * @brief    Backlight bright value
     * @details  Used to change StreamDeco backlight bright
     * @details  port configure PWM with 4095 max value and 12-bits resolution */
    int lcd_bright;
  } settings_t;

  /**
   * @brief    Process buttons event
   * @param    button_event  Event generated by button 
   * @param    settings      Reference to StreamDeco settings
   * @details  Called by task button to send shortcuts to computer via BLE Bluetooth interface
   * @note     Each button send a event code registred during button configuration
   * 
   */
  void process_event(uint32_t button_event, settings_t *settings);

  /**
   * @brief  Change buttons color
   * @param  color  color of button
   **/
  void change_color_buttons(lv_palette_t color);

  /**
   * @brief    Change scrren rotation
   * @details  Rotate scrren and reorganize buttons
   * @note     Implementation on work
   */
  void change_rotation();

} // namespace streamDeco

#endif