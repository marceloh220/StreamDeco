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
        rtos::TimerStatic("UI reset timer", 10s),
        rtos::TimerStatic("Backlight timer", 30s),
    };

    /**
     * @var     button
     * @brief   Buttons of StreamDeco
     * @details Buttons's icons can be changed herer
     * 
     * @note    The difference between mainIcon, CanvasButton and ConfigButton
     *          is position map
     * @code
     * MainButton(
     *          "Terminal",     // name, show name if icon not used
     *          &terminal_simp, // first icon
     *          NULL            // second icon, if used the icons can be swaped using swap_icon
     * ),
     **/
    button_t button = {

        /* ---   Main screen buttons   --- */

        /* Applications line, first line of buttons */
        MainButton("Terminal", &terminal_simp, NULL),
        MainButton("Files", &files_simp, NULL),
        MainButton("Web", &web_simp, NULL),
        MainButton("Search", &search_simp, NULL),
        MainButton("Application", &applications_simp, NULL),

        /* Multimedia line, second line of buttons */
        MainButton("Prev", &backward_simp, NULL),
        MainButton("Play/Pause", &play_simp, &pause_simp),
        MainButton("Next", &forward_simp, NULL),
        MainButton("Mic", &mic_on_simp, &mic_off_simp),
        MainButton("Multimedia", &multimedia_simp, NULL),

        /* Configurations line, third line of buttons */
        MainButton("Left Workspace", &previous_workspace_simp, NULL),
        MainButton("Right Workspace", &next_workspace_simp, NULL),
        MainButton("Pin", &pin_simp, &unpin_simp),
        MainButton("Desktop Mode", &mult_window_simp, &single_window_simp),
        MainButton("Config", &config_simp, NULL),

        /* ---   Applications canvas buttons   --- */

        /* First line */
        CanvasButton("app1", &gogcom_simp, NULL),
        CanvasButton("app2", &discord_simp, NULL),
        CanvasButton("app3", &fps_simp, NULL),

        /* Second line */
        CanvasButton("app4", &code_simp, NULL),
        CanvasButton("app5", &texcompiler_simp, NULL),
        CanvasButton("app6", &calculator_simp, NULL),

        /* Third line */
        CanvasButton("app7", &build_simp, NULL),
        CanvasButton("app8", &download_simp, NULL),
        CanvasButton("app9", &serialport_simp, NULL),

        /* ---   Multimedia canvas buttons   --- */

        /* First line */
        CanvasButton("mult 1", &video_stop_capt_simp, &video_start_capt_simp),
        CanvasButton("mult 2", &mic_off_simp, &mic_on_simp),
        CanvasButton("mult 3", &screen_capt_simp, NULL),

        /* Second line */
        CanvasButton("mult 4", &add_clip_simp, NULL),
        CanvasButton("mult 5", &ripple_simp, NULL),
        CanvasButton("mult 6", &rolling_simp, NULL),

        /* Third line */
        CanvasButton("mult 7", &seek_backward_simp, NULL),
        CanvasButton("mult 8", &play_simp, NULL),
        CanvasButton("mult 9", &seek_forward_simp, NULL),

        /* ---   Configurations canvas buttons   --- */

        /* First line */
        ConfigButton("Vol Mute", &volume_mute_simp, NULL),
        ConfigButton("Vol Down", &volume_low_simp, NULL),
        ConfigButton("Vol Up", &volume_high_simp, NULL),

        /* Second line */
        ConfigButton("Color BG", &bgtheme_simp, NULL),
        ConfigButton("Color Buttons", &btntheme_simp, NULL),
        ConfigButton("Rotation", &rotation_simp, NULL),

        /* Third line */
        ConfigButton("System Monitor", &sysmon_simp, NULL),
        ConfigButton("System Config", &config_simp, NULL),
        ConfigButton("Reboot", &reboot_simp, NULL),
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
        rtos::TaskStatic<STACK_BUTTONS_SIZE>("Task Buttons", 5),
        rtos::TaskStatic<1024>("Task reset canvas", 3),
        rtos::TaskStatic<3*1024>("Task Monitor", 5),
        rtos::TaskStatic<3*1024>("Task Clock", 5),
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
    rtos::MutexRecursiveStatic mutex_serial;

    /**
     * @brief  Change color of Buttons, Metrics and bright Slider
     * @param  color  New button color
     * @note   Called in color_button event
     **/
    void change_color_buttons(lv_palette_t color)
    {
        button.terminal.colorButton(color);
        button.files.colorButton(color);
        button.web.colorButton(color);
        button.search.colorButton(color);
        button.applications_canvas.colorButton(color);

        button.multimedia_prev.colorButton(color);
        button.multimedia_play.colorButton(color);
        button.multimedia_next.colorButton(color);
        button.multimedia_mic.colorButton(color);
        button.multimedia_canvas.colorButton(color);

        button.left_workspace.colorButton(color);
        button.right_workspace.colorButton(color);
        button.pin.colorButton(color);
        button.desktop_mode.colorButton(color);
        button.configurations_canvas.colorButton(color);

        button.app1.colorButton(color);
        button.app2.colorButton(color);
        button.app3.colorButton(color);
        button.app4.colorButton(color);
        button.app5.colorButton(color);
        button.app6.colorButton(color);
        button.app7.colorButton(color);
        button.app8.colorButton(color);
        button.app9.colorButton(color);

        button.mult1.colorButton(color);
        button.mult2.colorButton(color);
        button.mult3.colorButton(color);
        button.mult4.colorButton(color);
        button.mult5.colorButton(color);
        button.mult6.colorButton(color);
        button.mult7.colorButton(color);
        button.mult8.colorButton(color);
        button.mult9.colorButton(color);

        button.volmut.colorButton(color);
        button.voldown.colorButton(color);
        button.volup.colorButton(color);
        button.color_background.colorButton(color);
        button.color_button.colorButton(color);
        button.rotation.colorButton(color);
        button.sysmonitor.colorButton(color);
        button.sysconfig.colorButton(color);
        button.reboot.colorButton(color);

        monitor.cpu.color(color);
        monitor.gpu.color(color);
        monitor.system.color(color);
        monitor.clock.color(color);

        slider.backlightbright_slider_style.set_bg_color(color);
        slider.backlightbright_icon_style.set_img_recolor(color);
        slider.backlightbright_icon.update_layout();
    } // function end change_color_buttons

} // namespace streamDeco
