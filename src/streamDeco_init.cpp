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

/**
 * @brief 0 Disable StreamDeco StreamDecoMonitor first syncro
 *        1 Enable  StreamDeco StreamDecoMonitor first syncro
 * @note  Wainting for computer application syncro is boring
 */
#define DEVOSO_TESTING 0

/**
 * @brief 0 Disable screen rotation function
 *        1 Enable  screen rotation function
 * @note  I dont have solution for canvas StreamDecoMonitor layout trouble =/
 */
#define DECO_ROTATION 1

namespace streamDeco
{

  /**
   * @brief    Synchronizes ESP32-RTC with Computer clock
   * @details  Wait for StreamDeco StreamDecoMonitor application response to synchronizes ESP32 RTC clock
   * @param    tryes - number of attemps to try sinchron clock with computer
   */
  void synchro_clock(int tryes);

  /**
   * @brief   Init StreamDeco
   * @details Attach StreamDeco's tasks and made buttons configurations, layers and timers
   * @details Called in function main_app, function handler task or Arduino setup to init StreamDeco
   *
   */
  void init()
  {

    /** init settings cache and update with flash */
    settings::initCache();
    
    /* set initial screen rotation and color */
    lvgl::screen::set_rotation(settings::cache.rotation);
    lvgl::screen::set_bg_color(settings::cache.color_background);

    /*  used to temporarily show apresentation screen
     *  the most part of this screens is a delay to show the icons and text
     *  actually useless but still nice to see */
    lvgl::Label startScreen_label;
    lvgl::Image startScreen_icon;

    /* --- START SCREEN --- */

    startScreen_label.create();
    startScreen_label.set_long_mode(lvgl::text::LONG_WRAP);
    startScreen_label.set_style_text_align(lvgl::text::ALIGN_CENTER);
    startScreen_label.set_style_text_font(lvgl::font::montserrat_22);
    startScreen_label.align(lvgl::alignment::CENTER, 0, -80);

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

    /* try connection, while you see icons and text =) */
    while (1)
    {
      if (bleKeyboard.isConnected())
      {
        break;
      }
      rtos::sleep(2s);
    }

    rtos::sleep(500ms); /* see more icon =) */

    /* change icon to waiting StreamDeco StreamDecoMonitor Synchronization */
    startScreen_label.set_text("Start StreamDeco monitor");
    startScreen_icon.set_src(&keyboard_simp);
    lvgl::screen::refresh();

#if DEVOSO_TESTING == 0
    /* make 40 tryes to synchro clock with StreamDeco StreamDecoMonitor application */
    synchro_clock(40);
#endif

    /** See more icon =) */
    rtos::sleep(1s);

    /* delete apresentation icons and text, sad =( */
    startScreen_icon.del();
    startScreen_label.del();

    lvgl::port::mutex_take();

    /* --- MAIN BUTTONS --- */
    streamDecoButtons::createMain(settings::cache);

    /* --- INIT CANVAS --- */

    streamDecoCanvas::init(settings::cache.rotation);

    /* --- APLICATIONS CANVAS BUTTONS --- */

    /* apps buttons is created on Applications canvas */
    streamDecoButtons::createApplication(streamDecoCanvas::applications, settings::cache);

    /* --- MULTIMEDIA CANVAS BUTTONS --- */

    /* multimedia buttons is created on Multimedia canvas */
    streamDecoButtons::createMultimedia(streamDecoCanvas::multimedia, settings::cache);

    /* --- CONFIGURATIONS --- */

    /* configurations buttons is created on Configured canvas */
    streamDecoButtons::createConfiguration(streamDecoCanvas::configurations, settings::cache);

    /* configure slider bright */
    streamDecoBrightSlider::init(streamDecoCanvas::configurations, settings::cache);

    /* --- MONITOR --- */

    streamDecoMonitor::init(streamDecoCanvas::monitor, settings::cache.color_buttons);

    if (settings::cache.rotation == lvgl::screen::LANDSCAPE)
    {
      streamDecoCanvas::landscape();
      streamDecoBrightSlider::landscape();
      streamDecoButtons::landscape();
    }
    else
    {
      streamDecoCanvas::portrait();
      streamDecoBrightSlider::portrait();
      streamDecoButtons::portrait();
    }

    lvgl::port::mutex_give();

    /* register ISR to handle with timer_ui::backlight and uiResetTimer event */
    timer_ui::backlight.attach(timer_callback);
    timer_ui::uiReset.attach(timer_callback);

    /* start timer_ui::backlight and uiResetTimer */
    timer_ui::backlight.start();
    timer_ui::uiReset.start();

    /* attach tasks handlers and start them */
    streamDecoTasks::buttons.attach(handleButtons);
    streamDecoTasks::uiReset.attach(handleUiReset);
    streamDecoTasks::monitor.attach(handleMonitor);
    streamDecoTasks::clock.attach(handleClock);
    streamDecoTasks::clockSynchro.attach(handleClockSynchro);
    streamDecoTasks::updateCache.attach(handleUpdateCache);

  } // function init end

  /**
   * @brief   Print tasks memory usage
   * @details Called in function main_app loop, function handler task loop or Arduino loop
   * @details to print on serial StreamDecoMonitor tasks memory usage
   */
  void print_task_memory_usage()
  {
    printf("Task Buttons mem usage %d kB\n", streamDecoTasks::buttons.memUsage());
    printf("Task UI Reset mem usage %d kB\n", streamDecoTasks::uiReset.memUsage());
    printf("Task Monitor mem usage %d kB\n", streamDecoTasks::monitor.memUsage());
    printf("Task Ckock mem usage %d kB\n", streamDecoTasks::clock.memUsage());
    printf("Task Ckock synchro mem usage %d kB\n", streamDecoTasks::clockSynchro.memUsage());
    printf("Task Cache update mem usage %d kB\n", streamDecoTasks::updateCache.memUsage());
  }

} // namespace streamDeco
