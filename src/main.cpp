/**
 * Copyright © 2024 Marcelo H Moraes
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the “Software”), to deal in 
 * the Software without restriction, including without limitation the rights to 
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "marcelino.hpp"
#include "streamDeco_init.hpp"
#include "streamDeco_objects.hpp"

rtos::TaskStatic<2048> task_receiver_or("Task Receiver OR");
rtos::TaskStatic<2048> task_receiver_and("Task Receiver AND");
rtos::TaskStatic<2048> task_Transmiter("Task Transmiter");
 
rtos::EventGroupStatic event_group; 

#define EVENT_0 (1 << 0)
#define EVENT_1 (1 << 1)
#define EVENT_2 (1 << 2)
#define EVENT_3 (1 << 3)

void task_receiver_or_handler(taskStaticArg_t arg) {

  while(true) {

    if(event_group.waitAnyFlags(EVENT_0|EVENT_1)) {

      printf("Event group take by Task receiver OR\n");

    }

    rtos::sleep(500ms);

  }

}

void task_receiver_and_handler(taskStaticArg_t arg) {

  while(true) {

    if(event_group.waitAllFlags(EVENT_2|EVENT_3)) {

      printf("Event group take by Task receiver AND\n");

    }

    rtos::sleep(500ms);

  }

}

void task_transmiter_handler(taskStaticArg_t arg) {

  while(true) {

    event_group.set(EVENT_0);

    rtos::sleep(1s);

  }

}

void setup()
{

  Serial.begin(115200);

  lvgl::port::init();
  streamDeco::init();

  task_receiver_or.attach(task_receiver_or_handler);
  task_receiver_and.attach(task_receiver_and_handler);
  task_Transmiter.attach(task_transmiter_handler);

}

void loop()
{

  static int test_count = 0;

  streamDeco::mutex_serial.take();
  lvgl::port::print_task_memory_usage();
  streamDeco::print_task_memory_usage();
  printf("Test Cycle: %d\n", test_count++);
  streamDeco::mutex_serial.give();
  event_group.set(EVENT_1);
  event_group.set(EVENT_2|EVENT_3);
  rtos::sleep(10s);

}
