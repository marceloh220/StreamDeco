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

#include "settings.hpp"
#include "uibutton.hpp"

#include "streamDeco_monitor.hpp"

/* Stack size of buttons task */
#define STACK_BUTTONS_SIZE byte_k(4)

namespace streamDeco
{

  typedef struct
  {
    /* Task button to manager buttons configurations and events */
    Task buttons;
    /* Task to hidde inactivities layers */
    Task uiReset;
    /* Task to show computer metrics on configurations fixed layers */
    Task monitor;
    /* Task to update clock */
    Task clock;
  } task_t;

  extern task_t task;

  /* Interface to send keyboard codes to Bluetooth */
  extern BleKeyboard bleKeyboard;

  typedef struct
  {
    /** @brief  Timer to generate an event to hidden additional layers 
     *          and return to Main layer affter 10 secons of inactivities
     *  @note   This value can be change in file streamDeco_objects.cpp */
    Timer uiReset;
    /** @brief  Timer reduce screen backlight affter 30 secons of inactivities
     *  @note   This value cann be change in file streamDeco_objects.cpp
    */
    Timer backlight;
  } timer_t;

  extern timer_t timer_ui;

  typedef struct
  {
    /* Layer to hold addtional apps shortcuts. */
    lvgl::Layer applications;
    /* Layer to hold multimedia apps shortcuts. */
    lvgl::Layer multimedia;
    /* Layer to hold configurations shortcuts. */
    lvgl::Layer configurations;
    /* Layer to monitor computer stats. */
    lvgl::Layer monitor;
    /* Style for layers */
    lvgl::Style style;
  } layer_t;

  extern layer_t layer;

  typedef struct
  {

    /* --- MAIN BUTTONS --- */

    streamDeco::mainButton terminal;
    streamDeco::mainButton files;
    streamDeco::mainButton web;
    streamDeco::mainButton search;
    streamDeco::mainButton applications_layer;
    streamDeco::mainButton multimedia_prev;
    streamDeco::mainButton multimedia_play;
    streamDeco::mainButton multimedia_next;
    streamDeco::mainButton multimedia_mic;
    streamDeco::mainButton multimedia_layer;
    streamDeco::mainButton left_workspace;
    streamDeco::mainButton right_workspace;
    streamDeco::mainButton pin;
    streamDeco::mainButton lock;
    streamDeco::mainButton configurations_layer;

    /* --- APPLICATIONS BUTTONS --- */

    streamDeco::layerButton app1;
    streamDeco::layerButton app2;
    streamDeco::layerButton app3;
    streamDeco::layerButton app4;
    streamDeco::layerButton app5;
    streamDeco::layerButton app6;
    streamDeco::layerButton app7;
    streamDeco::layerButton app8;
    streamDeco::layerButton app9;

    /* --- MULTIMEDIA BUTTONS --- */

    streamDeco::layerButton multimedia_mult1;
    streamDeco::layerButton multimedia_mult2;
    streamDeco::layerButton multimedia_mult3;
    streamDeco::layerButton multimedia_mult4;
    streamDeco::layerButton multimedia_mult5;
    streamDeco::layerButton multimedia_mult6;
    streamDeco::layerButton multimedia_mult7;
    streamDeco::layerButton multimedia_mult8;
    streamDeco::layerButton multimedia_mult9;

    /* --- CONFIGURATIONS BUTTONS --- */

    streamDeco::configButton volmut;
    streamDeco::configButton voldown;
    streamDeco::configButton volup;
    streamDeco::configButton color_background;
    streamDeco::configButton color_button;
    streamDeco::configButton rotation;
    streamDeco::configButton sysmonitor;
    streamDeco::configButton sysconfig;
    streamDeco::configButton shutdown;

  } button_t;

  extern button_t button;

  /* Bright control Slider on Configuration Layer. */
  typedef struct
  {
    lvgl::Slider backlightbright;
    lvgl::Style backlightbright_style;
    lvgl::Image backlightbright_icon;
    lvgl::Style backlightbright_icon_style;
  } slider_t;

  extern slider_t slider;

  typedef struct {
    metric::Complete cpu;
    metric::Complete gpu;
    metric::Basic system;
    metric::Clock clock;
  } monitor_t;

  extern monitor_t monitor;

  /* Events enumerations */
  enum event_e
  {

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

    left_workspace_event,
    right_workspace_event,
    pin_window_event,
    lock_computer_event,
    configurations_layer_event,
    configurations_layer_fix_event,

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
    configuration_layer_rotate_screen_event, // experimenntal

    configuration_layer_sysmonitor_event,
    configuration_layer_sysconfig_event,
    configuration_layer_logout_event,
    configuration_layer_reboot_event,
    configuration_layer_shutdown_event,

    slider_backlight_bright_value_change_event,

    /* --- LAYERS EVENT --- */

    hidden_layers_event,

  };

  typedef struct
  {
    /* used to change color of StreamDeco background */
    lv_color_t color_background;

    /* used to change color of StreamDeco buttons */
    lv_palette_t color_buttons;

    /* used to change StreamDeco backlight bright, start with 60% of bright
     * port configure PWM with 4095 max value, 12-bits resolution */
    int lcd_bright;
  } settings_t;

  void process_event(uint32_t button_event, settings_t *settings);
  void change_color_buttons(lv_palette_t color);
  void change_rotation();

} // namespace streamDeco

#endif