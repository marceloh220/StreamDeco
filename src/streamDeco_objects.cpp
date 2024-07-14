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
#include "streamDeco_monitor.hpp"
#include "icons.h"

namespace streamDeco
{

    /**
     * @brief  Edit bluetooth device herer
     * @code
     * BleKeyboard bleKeyboard(
     *          "StreamDeco",    // device name
     *          "Marcelino Inc", // device manufactory
     *          100              // battery level
     * );
     **/
    BleKeyboard bleKeyboard("StreamDeco", "Marcelino Inc", 100);

    /**
     * @brief  Time to reset ui layers or backlight sleep can be changed herer
     * @code
     * Timer(
     *      "UI reset timer", // timer name, only for debug
     *      10s               // time to timer occour, ui layer will be reseted affter 10 seconds
     * ),
     **/
    timer_t timer_ui = {
        Timer("UI reset timer", 10s),
        Timer("Backlight timer", 30s),
    };

    /**
     * @brief  Edit icons herer
     * @code
     * mainButton(
     *          "Terminal",     // name, show name if icon not used
     *          &terminal_simp, // first icon
     *          NULL            // second icon, if used the icons can be swaped using swap_icon
     * ),
     **/
    button_t button = {

        /* ---   Main Layer buttons   --- */

        /* Applications line */
        mainButton("Terminal", &terminal_simp, NULL),
        mainButton("Files", &files_simp, NULL),
        mainButton("Web", &web_simp, NULL),
        mainButton("Search", &search_simp, NULL),
        mainButton("Application", &applications_simp, NULL),

        /* Multimedia line */
        mainButton("Prev", &backward_simp, NULL),
        mainButton("Play/Pause", &play_simp, &pause_simp),
        mainButton("Next", &forward_simp, NULL),
        mainButton("Mic", &mic_on_simp, &mic_off_simp),
        mainButton("Multimedia", &multimedia_simp, NULL),

        /* Configurations line */
        mainButton("Left Workspace", &previous_workspace_simp, NULL),
        mainButton("Right Workspace", &next_workspace_simp, NULL),
        mainButton("Pin", &pin_simp, &unpin_simp),
        mainButton("Lock", &locked_simp, NULL),
        mainButton("Config", &config_simp, NULL),

        /* ---   Applications Layer buttons   --- */

        /* First line */
        layerButton("app1", &gogcom_simp, NULL),
        layerButton("app2", &discord_simp, NULL),
        layerButton("app3", &fps_simp, NULL),

        /* Second line */
        layerButton("app4", &code_simp, NULL),
        layerButton("app5", &texcompiler_simp, NULL),
        layerButton("app6", &calculator_simp, NULL),

        /* Third line */
        layerButton("app7", &build_simp, NULL),
        layerButton("app8", &download_simp, NULL),
        layerButton("app9", &serialport_simp, NULL),

        /* ---   Multimedia Layer buttons   --- */

        /* First line */
        layerButton("mult 1", &video_start_capt_simp, &video_stop_capt_simp),
        layerButton("mult 2", &mic_off_simp, &mic_on_simp),
        layerButton("mult 3", &cam_off_simp, &cam_on_simp),

        /* Second line */
        layerButton("mult 4", &add_clip_simp, NULL),
        layerButton("mult 5", &ripple_simp, NULL),
        layerButton("mult 6", &rolling_simp, NULL),

        /* Third line */
        layerButton("mult 7", &seek_backward_simp, NULL),
        layerButton("mult 8", &play_simp, NULL),
        layerButton("mult 9", &seek_forward_simp, NULL),

        /* ---   Configurations Layer buttons   --- */

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

    task_t task = {
        Task("Task Buttons", 5, STACK_BUTTONS_SIZE),
        Task("Task reset screen", 3, 1024),
        Task("Task Monitor", 5, 3 * 1024),
        Task("Task Clock", 5, 3 * 1024),
    };

    layer_t layer;
    slider_t slider;

    monitor_t monitor = {
        metric::Complete("CPU", &processor_22_simp),
        metric::Complete("GPU", &gpu_22_simp),
        metric::Basic("CPU", &ram_22_simp),
        metric::Clock()
    };

    lvgl::Style layer_monitor_style;

    void change_color_buttons(lv_palette_t color)
    {

        button.terminal.color(color);
        button.files.color(color);
        button.web.color(color);
        button.search.color(color);
        button.applications_layer.color(color);

        button.multimedia_prev.color(color);
        button.multimedia_play.color(color);
        button.multimedia_next.color(color);
        button.multimedia_mic.color(color);
        button.multimedia_layer.color(color);

        button.left_workspace.color(color);
        button.right_workspace.color(color);
        button.pin.color(color);
        button.lock.color(color);
        button.configurations_layer.color(color);

        button.app1.color(color);
        button.app2.color(color);
        button.app3.color(color);
        button.app4.color(color);
        button.app5.color(color);
        button.app6.color(color);
        button.app7.color(color);
        button.app8.color(color);
        button.app9.color(color);

        button.multimedia_mult1.color(color);
        button.multimedia_mult2.color(color);
        button.multimedia_mult3.color(color);
        button.multimedia_mult4.color(color);
        button.multimedia_mult5.color(color);
        button.multimedia_mult6.color(color);
        button.multimedia_mult7.color(color);
        button.multimedia_mult8.color(color);
        button.multimedia_mult9.color(color);

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

        slider.backlightbright_style.set_bg_color(color);
        slider.backlightbright_icon_style.set_img_recolor(color);
        slider.backlightbright_icon.update_layout();
    }

} // namespace streamDeco
