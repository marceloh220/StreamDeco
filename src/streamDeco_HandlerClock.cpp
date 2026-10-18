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
#include <time.h>

namespace streamDeco
{

  namespace
  {
    constexpr time_t kEpochWrap = 2082758399;

    String nextFrameToken(const String &frame, int &start)
    {
      if (start < 0 || start >= frame.length())
      {
        return "";
      }

      int sep = frame.indexOf(',', start);
      String token;

      if (sep < 0)
      {
        token = frame.substring(start);
        start = frame.length();
      }
      else
      {
        token = frame.substring(start, sep);
        start = sep + 1;
      }

      token.trim();
      return token;
    }

    bool readClockFromFrame(const String &frame, struct tm &tm_date)
    {
      int start = 0;
      String fields[17];

      for (int i = 0; i < 17; ++i)
      {
        fields[i] = nextFrameToken(frame, start);
      }

      // Require at least full date payload (sec..year) for RTC sync.
      if (!fields[10].length() || !fields[11].length() || !fields[12].length() ||
          !fields[13].length() || !fields[14].length() || !fields[15].length() || !fields[16].length())
      {
        return false;
      }

      tm_date.tm_sec = fields[10].toInt();
      tm_date.tm_min = fields[11].toInt();
      tm_date.tm_hour = fields[12].toInt();
      tm_date.tm_wday = fields[13].toInt();
      tm_date.tm_mday = fields[14].toInt();
      tm_date.tm_mon = fields[15].toInt() - 1;
      tm_date.tm_year = fields[16].toInt() - 1900;

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
    struct tm tm_date = {0};

    while (true)
    {

      mutex_serial.take();

      if (Serial.available())
      {
        String frame = Serial.readStringUntil('/');
        frame.trim();

        if (frame.length() > 0 && readClockFromFrame(frame, tm_date))
        {
          updateRtcFromTm(tm_date);
        }

        mutex_serial.give();

        return;

      } // Serial.avaliable

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

    struct tm tm_date = {0};

    while (true)
    {

      getLocalTime(&tm_date);
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