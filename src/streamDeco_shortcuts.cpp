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

    void process_event(uint32_t button_event, settings_t *settings)
    {

        /** @brief  Each code do diferrent things on this switch case
         *          the keyboard code sended by bleKeyborad is configured herer
         **/
        switch (button_event)
        {

            /* --- MAIN APPLICATIONS --- */

            /**
             * @brief  terminal button is pressed
             *         Open a terminal
             * @note   Need configuration on application or system
             **/
        case terminal_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press(KEY_F1);
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  files button is pressed
         *          Open file manager
         *  @note   This media shortcut may work by default on Windows and Linux
         **/
        case files_event:
            bleKeyboard.press(KEY_LEFT_GUI);
            bleKeyboard.press('e');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  Web button is pressed
         *          Open web browser
         *  @note   This media shortcut may work by default on Windows
         *          On Linux need configuration on system
         **/
        case web_event:
            bleKeyboard.write(KEY_MEDIA_WWW_HOME);
            break;

        /** @brief  Search button is pressed
         *          Open search feature
         *  @note   This media shortcut may work by default on Windows 11 and Linux KDE Plasma
         **/
        case search_event:
            bleKeyboard.press(KEY_LEFT_ALT);
            bleKeyboard.press(' ');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  Applications button receive a short click
         *          This event will:
         *              Unfixe the Applications layer
         *              Hidden Multimedia and Configurations layer
         *              Change de hidden state of Applications layer
         *          The layer will be hidden automaticaly after time defined on timer uiReset
         *          or with other short click on Applications button
         *  @note   Do nothing if Multimedia layer is fixed
         **/
        case applications_layer_event:
            if (button.multimedia_layer.is_fixed())
            {
                break;
            }
            button.applications_layer.unfixe();
            layer.multimedia.hidden();
            layer.configurations.hidden();
            layer.applications.change_hidden();
            break;

        /** @brief  Applications button receive a long click
         *          This event will:
         *              Fixe and unhidden the Applications layer
         *              Hidden Multimedia and Configurations layer
         *          A fixed layer will not be hidden until a short click on Applications button
         *  @note   Do nothing if Multimedia layer is fixed
         **/
        case applications_layer_fix_event:
            if (button.multimedia_layer.is_fixed())
            {
                break;
            }
            button.applications_layer.fixe();
            layer.applications.unhidden();
            layer.multimedia.hidden();
            layer.configurations.hidden();
            break;

        /* --- MAIN MULTIMEDIA --- */

        /** @brief  multimedia_prev button is pressed
         *          Backward media on player
         *  @note   This media shortcut may work by default on Windows and Linux
         **/
        case multimedia_prev_event:
            bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
            break;

        /** @brief  multimedia_play button is pressed
         *          Play or pause media on player
         *  @note   This media shortcut may work by default on Windows and Linux
         *          This button have two icons, play_simp and pause_simp
         *          swapIcon method change between this two icons
         *          Also the button is destaqued when pressed,
         *          but have no feedback from computer music player to indentify if media is playing or paused
         */
        case multimedia_play_event: /*  */
            bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
            button.multimedia_play.swapIcon();
            button.multimedia_play.is_fixed() ? button.multimedia_play.unfixe() : button.multimedia_play.fixe();
            break;

        /** @brief  multimedia_next button is pressed
         *          Forward media on player
         *  @note   This media shortcut may work by default on Windows and Linux
         **/
        case multimedia_next_event:
            bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
            break;

        /** @brief  multimedia_mic button is pressed
         *          Mute and unmute the microphone
         *  @note   This media shortcut need be configured in application or system
         *          This button have two icons, mic_on_simp and mic_off_simp
         *          swapIcon method change between this two icons
         *          Also the button is destaqued when pressed,
         *          but have no feedback from computer to indentify the microphone state
         */
        case multimedia_mic_event: /*  */
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press(KEY_F2);
            delay(10ms);
            bleKeyboard.releaseAll();
            button.multimedia_mic.swapIcon();
            button.multimedia_mic.is_fixed() ? button.multimedia_mic.unfixe() : button.multimedia_mic.fixe();
            break;

        /** @brief  Multimedia button receive a short click
         *          This event will:
         *              Unfixe the Multimedia layer
         *              Hidden Applications and Configurations layer
         *              Change de hidden state of Multimedia layer
         *          The layer will be hidden automaticaly after time defined on timer uiReset
         *          or with other short click on Multimedia button
         *  @note   Do nothing if Applications layer is fixed
         **/
        case multimedia_layer_event: /* multimedia button receive a short click */
            if (button.applications_layer.is_fixed())
            {
                break;
            }
            button.multimedia_layer.unfixe();
            layer.applications.hidden();
            layer.configurations.hidden();
            layer.multimedia.change_hidden();
            break;

            /** @brief  Multimedia button receive a long click
             *          This event will:
             *              Fixe and unhidden the Multimedia layer
             *              Hidden Applications and Configurations layer
             *          A fixed layer will not be hidden until a short click on Multimedia button
             *  @note   Do nothing if Applications layer is fixed
             **/
        case multimedia_layer_fix_event:
            if (button.applications_layer.is_fixed())
            {
                break;
            }
            button.multimedia_layer.fixe();
            layer.applications.hidden();
            layer.configurations.hidden();
            layer.multimedia.unhidden();
            break;

        /* --- MAIN CONFIGURATIONS --- */

        /** @brief  prev_workspace button is pressed
         *          Go to left workspace if exsist
         *  @note   This shortcut may work by default on Windows 11
         *          On Linux need configuration on system
         **/
        case left_workspace_event:
            bleKeyboard.press(KEY_LEFT_GUI);
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_ARROW);
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

            /** @brief  next_workspace button is pressed
             *          Go to right workspace if exsist
             *  @note   This shortcut may work by default on Windows 11
             *          On Linux need configuration on system
             **/
        case right_workspace_event:
            bleKeyboard.press(KEY_LEFT_GUI);
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_RIGHT_ARROW);
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  pin button is pressed
         *          Pin the active window on screen
         *  @note   Need configuration on application or system
         *          The button is destaqued when pressed,
         *          but have no feedback from computer to indentify a pinnned window
         **/
        case pin_window_event:
            bleKeyboard.press(KEY_LEFT_GUI);
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press('t');
            delay(10ms);
            bleKeyboard.releaseAll();
            button.pin.swapIcon();
            button.pin.is_fixed() ? button.pin.unfixe() : button.pin.fixe();
            break;

        /** @brief  lock_computer button is pressed
         *          Lock the computer
         *  @note   This shortcut may work by default on Windows and Linux
         **/
        case lock_computer_event:
            bleKeyboard.press(KEY_LEFT_GUI);
            bleKeyboard.press('l');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief Configurations button receive a short click
         *          This event will:
         *              Hidden Applications and Multimedia layer
         *              Change de hidden state of Configurations layer
         *          The layer will be hidden automaticaly after time defined on timer uiReset
         *          or with other short click on Configurations button
         *  @note   Do nothing if Applications or Multimedia layer is fixed
         *          The Configurations layer have no fixe function
         **/
        case configurations_layer_event: /* configuration button is pressed */
            if (button.applications_layer.is_fixed() || button.multimedia_layer.is_fixed())
            {
                break;
            }
            layer.applications.hidden();
            layer.multimedia.hidden();
            layer.configurations.change_hidden();
            break;

            /* --- LAYER APPLICATIONS --- */

        /** @brief  app1 button is pressed
         *          GOG.com Icon to launch GOG Galaxy desktop application
         *  @note   Need configuration on application or system
         **/
        case applications_layer_app1_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('9');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  app2 button is pressed
         *          Discord Icon to launch Discord desktop application
         *  @note   Need configuration on application or system
         **/
        case applications_layer_app2_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('0');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  app3 button is pressed
         *          FPS Icon to show metrics
         *  @note   Need configuration on application or system
         **/
        case applications_layer_app3_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('8');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  app4 button is pressed
         *          VSCode Icon to launch VSCode desktop application
         *  @note   Need configuration on application or system
         **/
        case applications_layer_app4_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('f');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  app5 button is pressed
         *          Tex Icon to launch LaTeX IDE desktop application
         *  @note   Need configuration on application or system
         **/
        case applications_layer_app5_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('g');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  app6 button is pressed
         *          Calculator Icon to launch calculator desktop application
         *  @note   This shortcut may work by default on Windows and Linux
         **/
        case applications_layer_app6_event:
            bleKeyboard.write(KEY_MEDIA_CALCULATOR);
            break;

        /** @brief  app7 button is pressed
         *          In VScode build PlatformIO project
         *  @note   This shortcut may work by default on VSCode PlatformIO extension
         **/
        case applications_layer_app7_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_ALT);
            bleKeyboard.press('b');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  app8 button is pressed
         *          In VScode upload PlatformIO project to microcontroler or SoC
         *  @note   This shortcut may work by default on VSCode PlatformIO extension
         **/
        case applications_layer_app8_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_ALT);
            bleKeyboard.press('u');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  app9 button is pressed
         *          In VScode open serial monitor on PlatformIO
         *  @note   This shortcut may work by default on VSCode PlatformIO extension
         **/
        case applications_layer_app9_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_ALT);
            bleKeyboard.press('s');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /* --- LAYER MULTIMEDIA --- */

        /** @brief  mult1 button is pressed
         *          Start or pause a record screen
         *  @note   Need configuration on application or system
         **/
        case multimedia_layer_mult1_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('e');
            delay(10ms);
            bleKeyboard.releaseAll();
            button.multimedia_mult1.swapIcon();
            button.multimedia_mult1.is_fixed() ? button.multimedia_mult1.unfixe() : button.multimedia_mult1.fixe(); 
            break;

        /** @brief  mult2 button is pressed
         *          Mute mic on record
         *  @note   Need configuration on application or system
         **/
        case multimedia_layer_mult2_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('m');
            delay(10ms);
            bleKeyboard.releaseAll();
            button.multimedia_mult2.swapIcon();
            button.multimedia_mult2.is_fixed() ? button.multimedia_mult2.unfixe() : button.multimedia_mult2.fixe();
            break;

        /** @brief  mult3 button is pressed
         *          Enable or disable camera
         *  @note   Need configuration on application or system
         **/
        case multimedia_layer_mult3_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('c');
            delay(10ms);
            bleKeyboard.releaseAll();
            button.multimedia_mult3.swapIcon();
            button.multimedia_mult3.is_fixed() ? button.multimedia_mult3.unfixe() : button.multimedia_mult3.fixe();
            break;

        /** @brief  mult4 button is pressed
         *          Add clip
         *  @note   Need configuration on application or system
         **/
        case multimedia_layer_mult4_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('r');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  mult5 button is pressed
         *          Ripple track
         *  @note   Need configuration on application or system
         **/
        case multimedia_layer_mult5_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('t');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  mult6 button is pressed
         *          Roll track
         *  @note   Need configuration on application or system
         **/
        case multimedia_layer_mult6_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('k');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  mult7 button is pressed
         *          Backward track
         *  @note   Need configuration on application or system
         **/
        case multimedia_layer_mult7_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('i');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  mult8 button is pressed
         *          Play/Pause track
         *  @note   Need configuration on application or system
         **/
        case multimedia_layer_mult8_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('o');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  mult9 button is pressed
         *          Forward track
         *  @note   Need configuration on application or system
         **/
        case multimedia_layer_mult9_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('p');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /* --- LAYER CONFIGURATIONS --- */

        /** @brief  volmut button is pressed
         *          Mute sound volume
         *  @note   This media shortcut may work by default on Windows and Linux
         **/
        case configuration_layer_volmut_event:
            bleKeyboard.write(KEY_MEDIA_MUTE);
            break;

        /** @brief  voldown button is pressed
         *          Lower the sound volume
         *  @note   This media shortcut may work by default on Windows and Linux
         **/
        case configuration_layer_voldown_event:
            bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
            task.buttons.takeNotify();
            break;
        
        /** @brief  volup button is pressed
         *          Rise sound volume
         *  @note   This media shortcut may work by default on Windows and Linux
         **/
        case configuration_layer_volup_event:
            bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
            task.buttons.takeNotify();
            break;

        /** @brief  colorbackground button is pressed
         *          Change background color
         *  @note   Background colors are allocated in vector palette_bg in settings.cpp file
         *          You can add colors in this vector
         **/
        case configuration_layer_colorbackground_event:
            settings->color_background = lvgl::settings::bgColor();
            lvgl::screen::set_bg_color(settings->color_background);
            break;

        /** @brief  colorbutton button is pressed
         *          Change button color
         *  @note   Buttons colors are allocated in vector palette_button in settings.cpp file
         *          You can add colors in this vector
         **/
        case configuration_layer_colorbutton_event:
            settings->color_buttons = lvgl::settings::buttonColor();
            change_color_buttons(settings->color_buttons);
            slider.backlightbright_style.set_bg_color(settings->color_buttons);
            slider.backlightbright_icon_style.set_img_recolor(settings->color_buttons);
            slider.backlightbright_icon.update_layout();
            lvgl::screen::refresh();
            break;

        /** @brief  sysmon button is pressed
         *          Launch system monitor
         *  @note   This shortcut may work by default on Windows 10 or 11
         *          On Linux need configuration on system
         **/
        case configuration_layer_sysmonitor_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press(KEY_ESC);
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  sysconf button is pressed
         *          Launch system configuration panel
         *  @note   This shortcut may work by default on Windows 10 or 11
         *          On Linux need configuration on system
         **/
        case configuration_layer_sysconfig_event:
            bleKeyboard.press(KEY_LEFT_GUI);
            bleKeyboard.press('i');
            break;

        /** @brief  reboot button is pressed
         *          Reboot the computer
         *  @note   Need configuration on system or application
         **/
        case configuration_layer_reboot_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('2');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  shuntdown button is pressed
         *          Shuntdown the computer
         *  @note   Need configuration on system or application
         **/
        case configuration_layer_shutdown_event:
            bleKeyboard.press(KEY_LEFT_CTRL);
            bleKeyboard.press(KEY_LEFT_SHIFT);
            bleKeyboard.press('3');
            delay(10ms);
            bleKeyboard.releaseAll();
            break;

        /** @brief  rotate screen button is pressed
         *          Rotate streamDeco screen
         *  @note   Work in progress
         **/
        case configuration_layer_rotate_screen_event:
            // change_rotation();
            break;

        /** @brief  Backlight bright control slider change value
         *          Set new value to backlight bright
         **/
        case slider_backlight_bright_value_change_event:
            settings->lcd_bright = slider.backlightbright.get_value();
            break;

        } // end switch case

    } // end process event

} // namespace streamDeco