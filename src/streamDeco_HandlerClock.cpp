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
 * @file  streamDeco_HandlerClock.cpp
 * @brief Clock streamDecoTasks handler
 */

#include "streamDeco_objects.hpp"
#include "streamDeco_serial.hpp"
#include "streamDeco_string.hpp"
#include <time.h>

namespace streamDeco
{

  namespace
  {
    constexpr time_t kEpochWrap = 2082758399;

    bool readClockFromFrame(const monitor::string &frame, struct tm &tm_date)
    {
      monitor::parseMonitorFrame(frame);

      tm_date.tm_sec = monitor::getMonitorParam(monitor::SEC);
      tm_date.tm_min = monitor::getMonitorParam(monitor::MIN);
      tm_date.tm_hour = monitor::getMonitorParam(monitor::HOUR);
      tm_date.tm_wday = monitor::getMonitorParam(monitor::WEEK_DAY);
      tm_date.tm_mday = monitor::getMonitorParam(monitor::DAY);
      tm_date.tm_mon = monitor::getMonitorParam(monitor::MONTH) - 1;
      tm_date.tm_year = monitor::getMonitorParam(monitor::YEAR) - 1900;

      return true;
    }

    void updateRtcFromTm(struct tm &tm_date)
    {
      time_t time_local = mktime(&tm_date);

      if (time_local > kEpochWrap)
      {
        time_local -= kEpochWrap;
      }

      struct timeval time_epoch;
      time_epoch.tv_sec = time_local;
      time_epoch.tv_usec = 0;

      settimeofday(&time_epoch, nullptr);
    }
  }

  /**
   * @brief    Synchronizes ESP32-RTC with Computer clock
   * @details  Wait for StreamDeco StreamDecoMonitor application response to synchronizes ESP32 RTC clock
   * @param    max_attempts - number of attempts to synchronize clock with computer
   */
  void sync_clock(int max_attempts)
  {

    int attempts = 0;
    struct tm tm_date = {};

    while (true)
    {

      mutex_serial.take();

      monitor::string frame;
      if (serial::read_frame(frame))
      {
        if (readClockFromFrame(frame, tm_date))
        {
          updateRtcFromTm(tm_date);
        }

        mutex_serial.give();

        return;

      } // serial::read_frame

      mutex_serial.give();

      ++attempts;
      if (max_attempts != -1 && attempts > max_attempts)
      {
        return;
      }

      rtos::sleep(1s);

    } // loop check time

  }

  /* Handle the clock streamDecoTasks,
   * update clock time on Monitor streamDecoCanvas */
  void handleClock(taskArg_t task_arg)
  {

    struct tm tm_date = {};

    while (true)
    {

      time_t now = time(nullptr);
      localtime_r(&now, &tm_date);
      streamDecoMonitor::clock.set_time(tm_date);

      rtos::sleep(500ms);
    }
  }

  /* Handle the clock sync streamDecoTasks,
   * synchronize clock time with streamDeco monitor application */
  void handleClockSync(taskArg_t task_arg)
  {

    (void)task_arg;

    while (true)
    {

      sync_clock(40);
      rtos::sleep(5min);

    }

  }

} // namespace streamDeco