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

rtos::Queue<int, 2> queue_intData;
rtos::Task task_receiver("Task Receiver");

void task_receiver_handler(taskStaticArg_t arg) {

  int counter = 0;

  while(true) {

    if(queue_intData.receive(counter, 500ms)) {

      printf("Message received: %d\n", counter);

    } else {

      printf("Message not received\n");

    }

  }
  
}

void setup()
{

  Serial.begin(115200);

  lvgl::port::init();
  streamDeco::init();
  task_receiver.attach(task_receiver_handler);

}

void loop()
{

  static int counter = 0;

  lvgl::port::print_task_memory_usage();
  streamDeco::print_task_memory_usage();
  queue_intData.send(counter);
  counter++;

  rtos::sleep(10s);

}
