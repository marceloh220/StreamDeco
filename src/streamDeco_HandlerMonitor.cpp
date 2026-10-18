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
 * @file     streamDeco_HandlerMonitor.cpp
 * @brief    Handler of streamDecoTasks StreamDecoMonitor
 */

#include "streamDeco_objects.hpp"


namespace streamDeco
{

  static String cpu_load;
  static String cpu_temp;
  static String cpu_freq;

  static String gpu_load;
  static String gpu_temp;
  static String gpu_freq;

  static String mem_used;
  static String mem_max;
  static String disk_used;
  static String disk_max;

  static String sec;
  static String min;
  static String hour;
  static String day;
  static String month;
  static String year;

  namespace
  {
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

    bool parseMonitorFrame(const String &frame)
    {
      int start = 0;

      String token = nextFrameToken(frame, start);
      if (token.length())
        cpu_load = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        cpu_temp = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        cpu_freq = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        gpu_load = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        gpu_temp = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        gpu_freq = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        mem_used = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        mem_max = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        disk_used = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        disk_max = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        sec = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        min = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        hour = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        day = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        month = token;

      token = nextFrameToken(frame, start);
      if (token.length())
        year = token;

      return true;
    }
  }

  /* Handle the StreamDecoMonitor streamDecoTasks,
   * show computer metrics on configure pinned streamDecoCanvas */
  void handleMonitor(taskArg_t task_arg)
  {

    while (1)
    {

      mutex_serial.take();
      if (Serial.available())
      {
        String frame = Serial.readStringUntil('/');
        frame.trim();

        if (frame.length() > 0)
        {
          parseMonitorFrame(frame);

          streamDecoMonitor::cpu.arc_set_value(cpu_load.toInt());
          streamDecoMonitor::cpu.bar1_set_value(cpu_temp.toInt(), "", " °C");
          streamDecoMonitor::cpu.bar2_set_value(cpu_freq.toInt(), "", " MHz");

          streamDecoMonitor::gpu.arc_set_value(gpu_load.toInt());
          streamDecoMonitor::gpu.bar1_set_value(gpu_temp.toInt(), "", " °C");
          streamDecoMonitor::gpu.bar2_set_value(gpu_freq.toInt(), "", " MHz");

          streamDecoMonitor::system.bar1_set_range(0, mem_max.toInt());
          streamDecoMonitor::system.bar2_set_range(0, disk_max.toInt());

          streamDecoMonitor::system.bar1_set_value(mem_used.toInt(), "RAM: ", " MB");
          streamDecoMonitor::system.bar2_set_value(disk_used.toInt(), disk_max.toInt(), "C: ", " GB");
        }
      }
      mutex_serial.give();

      rtos::sleep(1s);
    }
  }

} // namespace streamDeco