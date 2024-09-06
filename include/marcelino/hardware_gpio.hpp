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

#ifndef __HARDWARE_GPIO_HPP_
#define __HARDWARE_GPIO_HPP_

#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "esp_rom_gpio.h"
#include "hal/gpio_types.h"

#include "esp_attr.h"

#undef LOW
#undef HIGH
#undef INPUT
#undef OUTPUT
#undef INPUT_PULLUP
#undef INPUT_PULLDOWN

namespace hardware {

class GPIO {

public:

  typedef enum state_e {
    LOW,
    HIGH,
  } state_t;

  typedef enum mode_e {
    INPUT = GPIO_MODE_INPUT,
    OUTPUT = GPIO_MODE_OUTPUT,
    INPUT_OUTPUT = GPIO_MODE_INPUT_OUTPUT,
    INPUT_PULLUP,
    INPUT_PULLDOWN,
  } mode_t;

  typedef enum interrupt_e {
    DISABLE = GPIO_INTR_DISABLE,
    RISING_EDGE = GPIO_INTR_POSEDGE,
    FALLING_EDGE = GPIO_INTR_NEGEDGE,
    ANY_EDGE = GPIO_INTR_ANYEDGE,
    LOW_LEVEL = GPIO_INTR_LOW_LEVEL,
    HIGH_LEVEL = GPIO_INTR_HIGH_LEVEL,
  } interrupt_t;

  GPIO(gpio_num_t pin, mode_t mode);

  ~GPIO();

  gpio_num_t pinName();

  void mode(mode_t mode);

  mode_t mode();

  void interruptAttach(void function(void *), interrupt_t mode, void *arg);

  void interruptDettach();

  interrupt_t interruptType();

  void interruptDisable();

  void interruptEnable();

  bool read();

  void write(bool state);

  void toggle(int state = 1);

  void freeze();

  void unfreeze();

  GPIO &operator=(GPIO &v);

  GPIO &operator=(int v);

  GPIO &operator^=(GPIO &v);

  GPIO &operator^=(int v);

  operator int();

private:

  const gpio_num_t _pin;
  mode_t _mode;
  bool _state;
  interrupt_t _interrupt;

};

} // namespace hardware

#endif
