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
 * @file  streamDeco_HandlerButtons.cpp
 * @brief Button task handler
 */

#include "streamDeco_objects.hpp"

namespace streamDeco
{

  namespace streamDecoCanvas
  {
    namespace
    {
      void calendar_touch_event(lv_event_t *event)
      {
        auto *target = lv_event_get_target(event);
        for (auto *parent = target; parent != nullptr; parent = lv_obj_get_parent(parent))
        {
          if (parent == calendar.get_object())
          {
            streamDecoTasks::buttons.sendNotify(nothing_event);
            return;
          }
          if (parent == streamDecoButtons::calendar_canvas.get_object())
            return;
          if (!calendar.is_hidden() &&
              (parent == streamDecoButtons::applications_canvas.get_object() ||
               parent == streamDecoButtons::multimedia_canvas.get_object() ||
               parent == streamDecoButtons::configurations_canvas.get_object()))
          {
            lv_indev_wait_release(lv_indev_get_act());
            break;
          }
        }
        calendar.hidden();
      }

      void register_calendar_touch(lv_obj_t *object)
      {
        lv_obj_add_flag(object, LV_OBJ_FLAG_EVENT_BUBBLE);
        for (uint32_t index = 0; index < lv_obj_get_child_cnt(object); ++index)
          register_calendar_touch(lv_obj_get_child(object, index));
      }
    }

    void register_calendar_events()
    {
      lv_obj_add_event_cb(lv_scr_act(), calendar_touch_event, LV_EVENT_PRESSED, nullptr);
      register_calendar_touch(lv_scr_act());
    }
  }

  namespace
  {
    constexpr lvgl::screen::rotation_t rotation_sequence[] = {
        lvgl::screen::LANDSCAPE, lvgl::screen::PORTRAIT,
        lvgl::screen::MIRROR_LANDSCAPE, lvgl::screen::MIRROR_PORTRAIT
    };

    void process_button_event(uint32_t button_event)
    {
      lvgl::screen::rotation_t rotation;

      switch (button_event)
      {
      case nothing_event:
      case configuration_canvas_logout_event:
        break;

      case applications_canvas_event:
        if (!streamDecoCanvas::calendar.is_hidden())
          break;
        if (streamDecoButtons::multimedia_canvas.pinned())
          break;
        if (streamDecoButtons::configurations_canvas.pinned())
          break;
        lvgl::port::mutex_take();
        streamDecoButtons::applications_canvas.unpin();
        streamDecoCanvas::multimedia.hidden();
        streamDecoCanvas::configurations.hidden();
        streamDecoCanvas::monitor.hidden();
        streamDecoCanvas::applications.change_hidden();
        lvgl::port::mutex_give();
        break;

      case applications_canvas_fix_event:
        if (!streamDecoCanvas::calendar.is_hidden())
          break;
        if (streamDecoButtons::multimedia_canvas.pinned())
          break;
        if (streamDecoButtons::configurations_canvas.pinned())
          break;
        lvgl::port::mutex_take();
        streamDecoButtons::applications_canvas.pin();
        streamDecoCanvas::multimedia.hidden();
        streamDecoCanvas::monitor.hidden();
        streamDecoCanvas::configurations.hidden();
        streamDecoCanvas::applications.unhidden();
        lvgl::port::mutex_give();
        break;

      case multimedia_canvas_event:
        if (!streamDecoCanvas::calendar.is_hidden())
          break;
        if (streamDecoButtons::applications_canvas.pinned())
          break;
        if (streamDecoButtons::configurations_canvas.pinned())
          break;
        lvgl::port::mutex_take();
        streamDecoButtons::multimedia_canvas.unpin();
        streamDecoCanvas::applications.hidden();
        streamDecoCanvas::configurations.hidden();
        streamDecoCanvas::monitor.hidden();
        streamDecoCanvas::multimedia.change_hidden();
        lvgl::port::mutex_give();
        break;

      case multimedia_canvas_fix_event:
        if (!streamDecoCanvas::calendar.is_hidden())
          break;
        if (streamDecoButtons::applications_canvas.pinned())
          break;
        if (streamDecoButtons::configurations_canvas.pinned())
          break;
        lvgl::port::mutex_take();
        streamDecoButtons::multimedia_canvas.pin();
        streamDecoCanvas::applications.hidden();
        streamDecoCanvas::configurations.hidden();
        streamDecoCanvas::monitor.hidden();
        streamDecoCanvas::multimedia.unhidden();
        lvgl::port::mutex_give();
        break;

      case calendar_canvas_event:
        if (streamDecoButtons::applications_canvas.pinned() ||
            streamDecoButtons::multimedia_canvas.pinned() ||
            streamDecoButtons::configurations_canvas.pinned())
          break;
        lvgl::port::mutex_take();
        streamDecoCanvas::applications.hidden();
        streamDecoCanvas::multimedia.hidden();
        streamDecoCanvas::configurations.hidden();
        streamDecoCanvas::monitor.hidden();
        if (streamDecoCanvas::calendar.is_hidden())
        {
          streamDecoCanvas::calendar_sheet.refresh_date(true);
          streamDecoCanvas::calendar.move_foreground();
          streamDecoCanvas::calendar.unhidden();
        }
        else
        {
          streamDecoCanvas::calendar.hidden();
        }
        lvgl::port::mutex_give();
        break;

      case configurations_canvas_event:
        if (!streamDecoCanvas::calendar.is_hidden())
          break;
        if (streamDecoButtons::applications_canvas.pinned())
          break;
        if (streamDecoButtons::multimedia_canvas.pinned())
          break;
        lvgl::port::mutex_take();
        streamDecoButtons::configurations_canvas.unpin();
        streamDecoCanvas::applications.hidden();
        streamDecoCanvas::multimedia.hidden();
        if (streamDecoCanvas::monitor.is_hidden())
        {
          streamDecoCanvas::configurations.change_hidden();
        }
        else
        {
          streamDecoCanvas::configurations.hidden();
        }
        streamDecoCanvas::monitor.hidden();
        lvgl::port::mutex_give();
        break;

      case configurations_canvas_fix_event:
        if (!streamDecoCanvas::calendar.is_hidden())
          break;
        if (streamDecoButtons::applications_canvas.pinned())
          break;
        if (streamDecoButtons::multimedia_canvas.pinned())
          break;
        lvgl::port::mutex_take();
        streamDecoButtons::configurations_canvas.pin();
        streamDecoCanvas::applications.hidden();
        streamDecoCanvas::multimedia.hidden();
        streamDecoCanvas::configurations.hidden();
        streamDecoCanvas::monitor.unhidden();
        lvgl::port::mutex_give();
        break;

      case configuration_canvas_colorbackground_event:
        settings::cache.color_background = settings::nextBackgroundColor(settings::cache.color_background_index);
        lvgl::screen::set_bg_color(settings::cache.color_background);
        streamDecoCanvas::calendar_sheet.set_today_color(settings::cache.color_background);
        break;

      case configuration_canvas_colorbutton_event:
        settings::cache.color_buttons = settings::nextButtonColor(settings::cache.color_buttons_index);
        lvgl::port::mutex_take();
        streamDecoButtons::color(settings::cache.color_buttons);
        streamDecoBrightSlider::color(settings::cache.color_buttons);
        streamDecoMonitor::color(settings::cache.color_buttons);
        streamDecoCanvas::calendar_sheet.set_button_color(settings::cache.color_buttons);
        lvgl::port::mutex_give();
        lvgl::screen::refresh();
        break;

      case configuration_canvas_rotate_screen_event:
        lvgl::port::mutex_take();
        rotation = lvgl::screen::get_rotation();
        rotation = rotation_sequence[
            (std::find(std::begin(rotation_sequence), std::end(rotation_sequence), rotation)
            - std::begin(rotation_sequence) + 1)
            % std::size(rotation_sequence)];
        lvgl::screen::set_rotation(rotation);
        settings::cache.rotation = rotation;
        if (rotation == lvgl::screen::LANDSCAPE || rotation == lvgl::screen::MIRROR_LANDSCAPE)
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
        break;

      case configuration_canvas_reboot_event:
        streamDecoTasks::updateCache.sendNotify(update_settings_cache_with_reset_event);
        break;

      case slider_backlight_bright_value_change_event:
        settings::cache.lcd_bright = streamDecoBrightSlider::read();
        break;

      default:
        streamDeco::mutex_serial.take();
        process_shortcuts(button_event);
        streamDeco::mutex_serial.give();
        break;
      }
    }
  }

  /**
   * @brief   Handle the buttons task to manager buttons events
    * @details The events can generate a keyboard code to send to computer through BLE Bluetooth
   *          or change StreamDeco configurations
   **/
  void handleButtons(taskArg_t task_arg)
  {

    while (1)
    {

      /* wait for a notification
       * this notification is sent by LVGL streamDecoButtons with a event code */
      uint32_t button_event = streamDecoTasks::buttons.takeNotify();

      process_button_event(button_event);

      /**
       * if some event is received the UI is not inactive
       * backlight bright change to setpoint value
        * and timers are kept reset */
      lvgl::port::backlight_setRaw(settings::cache.lcd_bright);
      timers_idle::backlight_idle.reset();
      timers_idle::canvas_idle.reset();

      rtos::sleep(100ms);

    } // end handleButtons's infinit loop

  } // end handleButtons

} // namespace streamDeco
