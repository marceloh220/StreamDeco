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

  /**
   * @brief    Synchronizes ESP32-RTC with Computer clock
   * @details  Wait for StreamDeco StreamDecoMonitor application response to synchronizes ESP32 RTC clock
   * @param    tryes - number oa ttemps to try sinchron clock with computer
   */
  void synchro_clock(int tryes)
  {

    int attempts = 0;
    struct tm tm_date = {0};

    mutex_serial.take();

    while (1)
    {
      if (Serial.available())
      {
        String cpu_load = Serial.readStringUntil(',');
        String cpu_temp = Serial.readStringUntil(',');
        String cpu_freq = Serial.readStringUntil(',');

        String gpu_load = Serial.readStringUntil(',');
        String gpu_temp = Serial.readStringUntil(',');
        String gpu_freq = Serial.readStringUntil(',');

        String mem_used = Serial.readStringUntil(',');
        String mem_max = Serial.readStringUntil(',');
        String disk_used = Serial.readStringUntil(',');
        String disk_max = Serial.readStringUntil(',');

        String sec = Serial.readStringUntil(',');
        String min = Serial.readStringUntil(',');
        String hour = Serial.readStringUntil(',');
        String week = Serial.readStringUntil(','); 
        String day = Serial.readStringUntil(',');
        String month = Serial.readStringUntil(',');
        String year = Serial.readStringUntil('/');

        tm_date.tm_sec = sec.toInt();
        tm_date.tm_min = min.toInt();
        tm_date.tm_hour = hour.toInt();
        tm_date.tm_wday = week.toInt();
        tm_date.tm_mday = day.toInt();
        tm_date.tm_mon = month.toInt() - 1;
        tm_date.tm_year = year.toInt() - 1900;

        time_t time_local = mktime(&tm_date);

        struct timeval time_epoch;
        if (time_local > 2082758399)
        {
          time_local = time_local - 2082758399;
        }
        time_epoch.tv_sec = time_local;
        time_epoch.tv_usec = 0;
        settimeofday(&time_epoch, nullptr);

        break;

      } // Serial.avaliable

      attempts++;

      if (attempts > tryes && tryes != -1)
        break;
      rtos::sleep(1s);

    } // loop check time

    mutex_serial.give();
  }

  /* Handle the clock streamDecoTasks,
   * update clock time on Monitor streamDecoCanvas */
  void handleClock(taskArg_t task_arg)
  {

    struct tm tm_date = {0};

    while (1)
    {

      getLocalTime(&tm_date);
      streamDecoMonitor::clock.set_time(tm_date);

      rtos::sleep(500ms);
    }
  }

  /* Handle the clock synchro streamDecoTasks,
   * synchro clock time with streamDeco monitor application */
  void handleClockSynchro(taskArg_t task_arg) {

    while(1) {
  
      synchro_clock(40);
      rtos::sleep(5min);
  
    }
  
  }

} // namespace streamDeco