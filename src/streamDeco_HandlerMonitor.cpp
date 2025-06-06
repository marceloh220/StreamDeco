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

  /* Handle the StreamDecoMonitor streamDecoTasks,
   * show computer metrics on configure pinned streamDecoCanvas */
  void handleMonitor(taskArg_t task_arg)
  {

    while (1)
    {

      mutex_serial.take();
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
      mutex_serial.give();

      rtos::sleep(1s);
    }
  }

} // namespace streamDeco