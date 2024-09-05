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
 * @brief Clock task handler
 */

#include "streamDeco_objects.hpp"
#include <time.h>

namespace streamDeco
{

  /* Synchronizes ESP32-RTC with Computer clock */
  void synchro_clock(struct tm &tm_date)
  {
    int attempts = 0;
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
        String day = Serial.readStringUntil(',');
        String month = Serial.readStringUntil(',');
        String year = Serial.readStringUntil('/');

        tm_date.tm_sec = sec.toInt();
        tm_date.tm_min = min.toInt();
        tm_date.tm_hour = hour.toInt();
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
        settimeofday(&time_epoch, NULL);
        break;
      } // Serial.avaliable
      attempts++;
      if(attempts > 20) break;
      rtos::sleep(1s);
    } // loop check time
    mutex_serial.give();
  }

  /* Handle the clock task,
   * update clock time on Monitor canvas */
  void handleClock(taskArg_t task_arg)
  {
    struct tm tm_date = {0};
    int count = 0;
    synchro_clock(tm_date);
    synchro_clock(tm_date);

    while (1)
    {

      count++;

      if(count == (10*60)) {
        count = 0;
        synchro_clock(tm_date);
        synchro_clock(tm_date);
      }

      getLocalTime(&tm_date);
      monitor.clock.set_time(tm_date);

      rtos::sleep(500ms);
    }
  }

} // namespace streamDeco