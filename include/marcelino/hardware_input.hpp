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

#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include "hardware_gpio.hpp"

#undef PULLUP
#undef PULLDOWN

namespace hardware {

class Input : private GPIO {
public:
  typedef enum mode_e {
    FLOATING = GPIO::INPUT,
    PULLUP = GPIO::INPUT_PULLUP,
    PULLDOWN = GPIO::INPUT_PULLDOWN,
  } mode_t;

  Input(gpio_num_t pin, mode_t mode = PULLDOWN)
      : GPIO(pin, (GPIO::mode_t)mode) {}

  inline gpio_num_t pinName() { return GPIO::pinName(); }

  [[nodiscard]] inline bool readRaw() { return GPIO::read(); }

  [[nodiscard]] virtual bool read() {
    bool state = GPIO::read();
    return GPIO::mode() == INPUT_PULLUP ? !state : state;
  }

  [[nodiscard]] inline operator int() { return read(); }

}; // class Input

} // namespace hardware

#endif
