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

#ifndef __HARDWARE_OUTPUT_HPP__
#define __HARDWARE_OUTPUT_HPP__

#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "hal/gpio_types.h"

#include "hardware_gpio.hpp"

namespace hardware
{

  namespace State
  {
    typedef enum state_e
    {
      LOW,
      HIGH,
    } state_t;
  }

  class Output : private GPIO
  {
  public:
    Output(gpio_num_t pin, bool sink = false)
        : GPIO(pin, GPIO::OUTPUT), _sink(sink) {}

    inline gpio_num_t pinName() { return GPIO::pinName(); }

    [[nodiscard]] inline bool read() { return GPIO::read(); }

    inline void write(bool level)
    {
      level = _sink ? !level : level;
      GPIO::write(level);
    }

    inline void toggle(int level = 1) { GPIO::toggle(level); }

    inline Output &operator=(Output &v)
    {
      this->write(v.read());
      return *this;
    }

    inline Output &operator=(int v)
    {
      this->write(v);
      return *this;
    }

    inline Output &operator^=(int v)
    {
      this->toggle(v);
      return *this;
    }

    [[nodiscard]] inline operator int() { return read(); }

  protected:
    const bool _sink;
  }; // class output

} // namespace hardware

#endif
