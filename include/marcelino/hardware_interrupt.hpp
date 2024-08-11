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

#ifndef __INTERRUPT_HPP__
#define __INTERRUPT_HPP__

#include "hardware_input.hpp"

namespace hardware {

class Interrupt : private GPIO {

public:
  typedef enum interrupt_e {
    DISABLE = GPIO_INTR_DISABLE,
    RISING_EDGE = GPIO_INTR_POSEDGE,
    FALLING_EDGE = GPIO_INTR_NEGEDGE,
    ANY_EDGE = GPIO_INTR_ANYEDGE,
    LOW_LEVEL = GPIO_INTR_LOW_LEVEL,
    HIGH_LEVEL = GPIO_INTR_HIGH_LEVEL,
  } interrupt_t;

  Interrupt(gpio_num_t pin, Input::mode_t mode)
      : GPIO(pin, (GPIO::mode_t)mode) {}

  Interrupt(gpio_num_t pin, Input::mode_t mode, void function(void *),
            interrupt_t interrupt, void *arg)
      : GPIO(pin, (GPIO::mode_t)mode) {
    GPIO::attach(function, (GPIO::interrupt_t)interrupt, arg);
  }

  [[nodiscard]] inline gpio_num_t pinName() { return GPIO::pinName(); }

  inline void attach(void function(void *), interrupt_t interrupt, void *arg) {
    GPIO::attach(function, (GPIO::interrupt_t)interrupt, arg);
  }

  inline void dettach() { GPIO::dettach(); }

  inline void disable() { GPIO::interruptDisable(); }

  inline void enable() { GPIO::interruptEnable(); }

  [[nodiscard]] inline interrupt_t interrupt() {
    return (interrupt_t)GPIO::interruptType();
  }

  [[nodiscard]] inline bool interrupt(interrupt_t mode) {
    return GPIO::interruptType() == (GPIO::interrupt_t)mode;
  }

  [[nodiscard]] inline virtual bool read() { return GPIO::read(); }

  [[nodiscard]] inline operator int() { return read(); }

}; // class Interrupt

} // namespace hardware

#endif
