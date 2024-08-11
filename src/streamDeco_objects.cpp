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
#include "streamDeco_icons.h"

namespace streamDeco
{

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
     * @var     timer_ui
     * @brief   Timers
     * @details Time to reset ui canvas or backlight sleep can be changed herer
     * @code
     * Timer(
     *      "UI reset timer", // timer name, only for debug
     *      10s               // time to timer occour, ui canvas will be reseted affter 10 seconds
     * ),
     * @note    uiReset time and backlight time can be changed herer
     * @note    10 seconds to hidden canvas
     * @note    30 seconds to change backlight bright to 10% of bright
     **/
    timer_t timer_ui = {
        rtos::Timer("UI reset timer", 10s),
        rtos::Timer("Backlight timer", 30s),
    };

    /**
     * @var     button
     * @brief   Buttons of StreamDeco
     * @details Buttons's icons can be changed herer
     * 
     * @note    The difference between mainIcon, canvasButton and configButton
     *          is position map
     * @code
     * mainButton(
     *          "Terminal",     // name, show name if icon not used
     *          &terminal_simp, // first icon
     *          NULL            // second icon, if used the icons can be swaped using swap_icon
     * ),
     **/
    button_t button = {

        /* ---   Main screen buttons   --- */

        /* Applications line, first line of buttons */
        mainButton("Terminal", &terminal_simp, NULL),
        mainButton("Files", &files_simp, NULL),
        mainButton("Web", &web_simp, NULL),
        mainButton("Search", &search_simp, NULL),
        mainButton("Application", &applications_simp, NULL),

        /* Multimedia line, second line of buttons */
        mainButton("Prev", &backward_simp, NULL),
        mainButton("Play/Pause", &play_simp, &pause_simp),
        mainButton("Next", &forward_simp, NULL),
        mainButton("Mic", &mic_on_simp, &mic_off_simp),
        mainButton("Multimedia", &multimedia_simp, NULL),

        /* Configurations line, third line of buttons */
        mainButton("Left Workspace", &previous_workspace_simp, NULL),
        mainButton("Right Workspace", &next_workspace_simp, NULL),
        mainButton("Pin", &pin_simp, &unpin_simp),
        mainButton("Lock", &locked_simp, NULL),
        mainButton("Config", &config_simp, NULL),

        /* ---   Applications canvas buttons   --- */

        /* First line */
        canvasButton("app1", &gogcom_simp, NULL),
        canvasButton("app2", &discord_simp, NULL),
        canvasButton("app3", &fps_simp, NULL),

        /* Second line */
        canvasButton("app4", &code_simp, NULL),
        canvasButton("app5", &texcompiler_simp, NULL),
        canvasButton("app6", &calculator_simp, NULL),

        /* Third line */
        canvasButton("app7", &build_simp, NULL),
        canvasButton("app8", &download_simp, NULL),
        canvasButton("app9", &serialport_simp, NULL),

        /* ---   Multimedia canvas buttons   --- */

        /* First line */
        canvasButton("mult 1", &video_start_capt_simp, &video_stop_capt_simp),
        canvasButton("mult 2", &mic_off_simp, &mic_on_simp),
        canvasButton("mult 3", &cam_off_simp, &cam_on_simp),

        /* Second line */
        canvasButton("mult 4", &add_clip_simp, NULL),
        canvasButton("mult 5", &ripple_simp, NULL),
        canvasButton("mult 6", &rolling_simp, NULL),

        /* Third line */
        canvasButton("mult 7", &seek_backward_simp, NULL),
        canvasButton("mult 8", &play_simp, NULL),
        canvasButton("mult 9", &seek_forward_simp, NULL),

        /* ---   Configurations canvas buttons   --- */

        /* First line */
        configButton("Vol Mute", &volume_mute_simp, NULL),
        configButton("Vol Down", &volume_low_simp, NULL),
        configButton("Vol Up", &volume_high_simp, NULL),

        /* Second line */
        configButton("Color BG", &bgtheme_simp, NULL),
        configButton("Color Buttons", &btntheme_simp, NULL),
        configButton("Rotation", &rotation_simp, NULL),

        /* Third line */
        configButton("System Monitor", &sysmon_simp, NULL),
        configButton("System Config", &config_simp, NULL),
        configButton("Shutdown", &shutdown_simp, NULL),
    };

    /**
     * @var    task
     * @brief  Tasks of streamDeco
     * @code
     * Task(
     *          "Terminal",  // Task name, used for debug
     *          Priority,    // Higher numbers mean higher task priority
     *          StakSize     // Can start with a higher value and change affter see memory usage with memUsage method
     * ),
     * @note   The function handler is set with attach method
     *         Other parameters can be see in file marcelino/task.hpp
     **/
    task_t task = {
        rtos::Task("Task Buttons", 5, STACK_BUTTONS_SIZE),
        rtos::Task("Task reset canvas", 3, 1024),
        rtos::Task("Task Monitor", 5, 3 * 1024),
        rtos::Task("Task Clock", 5, 3 * 1024),
    };

    /**
     * @var   canvas
     * @brief Canvas to hold additional buttons
     */
    canvas_t canvas;

    /**
     * @var   slider
     * @brief Bright control slider
     */
    slider_t slider;

    /**
     * @var    monitor
     * @brief  Metrics of Monitor canvas
     * @code
     * metric::Complete(
     *          "CPU",          // name show on metric space
     *          &terminal_simp, // icon show in metric space, if NULL only name is show
     * ),
     * @note   Complete metric show an arc for load and two bars
     *         Basic only show two bars
     *         Clock show a clock formated in Hour:Minute:Second // Day/Month/Year
     **/
    monitor_t monitor = {
        metric::Complete("CPU", &processor_22_simp),
        metric::Complete("GPU", &gpu_22_simp),
        metric::Basic("MEM", &ram_22_simp),
        metric::Clock("Clock", &clock_22_simp)
    };

    /**
     * @var  mutex_serial
   * @brief  Serial interface mutex
   * @note   Used to avoid Monitor and Clock task uses Serial interface in same instant
   */
    rtos::Mutex mutex_serial;

    /**
     * @brief  Change color of Buttons, Metrics and bright Slider
     * @param  color  New button color
     * @note   Called in color_button event
     **/
    void change_color_buttons(lv_palette_t color)
    {
        button.terminal.color(color);
        button.files.color(color);
        button.web.color(color);
        button.search.color(color);
        button.applications_canvas.color(color);

        button.multimedia_prev.color(color);
        button.multimedia_play.color(color);
        button.multimedia_next.color(color);
        button.multimedia_mic.color(color);
        button.multimedia_canvas.color(color);

        button.left_workspace.color(color);
        button.right_workspace.color(color);
        button.pin.color(color);
        button.lock.color(color);
        button.configurations_canvas.color(color);

        button.app1.color(color);
        button.app2.color(color);
        button.app3.color(color);
        button.app4.color(color);
        button.app5.color(color);
        button.app6.color(color);
        button.app7.color(color);
        button.app8.color(color);
        button.app9.color(color);

        button.mult1.color(color);
        button.mult2.color(color);
        button.mult3.color(color);
        button.mult4.color(color);
        button.mult5.color(color);
        button.mult6.color(color);
        button.mult7.color(color);
        button.mult8.color(color);
        button.mult9.color(color);

        button.volmut.color(color);
        button.voldown.color(color);
        button.volup.color(color);
        button.color_background.color(color);
        button.color_button.color(color);
        button.rotation.color(color);
        button.sysmonitor.color(color);
        button.sysconfig.color(color);
        button.shutdown.color(color);

        monitor.cpu.color(color);
        monitor.gpu.color(color);
        monitor.system.color(color);
        monitor.clock.color(color);

        slider.backlightbright_slider_style.set_bg_color(color);
        slider.backlightbright_icon_style.set_img_recolor(color);
        slider.backlightbright_icon.update_layout();
    } // function end change_color_buttons

} // namespace streamDeco
