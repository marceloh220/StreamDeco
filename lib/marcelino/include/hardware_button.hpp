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

#ifndef __HARDWARE_BUTTOM_HPP__
#define __HARDWARE_BUTTOM_HPP__

#include "freertos/projdefs.h"
#include "hardware_input.hpp"

#include "rtos_chrono.hpp"

namespace hardware
{

  class Button : public Input
  {

  public:
    Button(gpio_num_t pin, milliseconds debounce, Input::mode_t mode = Input::PULLDOWN)
        : Input(pin, mode), _debounce(debounce) {}

    [[nodiscard]] bool read()
    {
      bool state = Input::read();
      if (state && !_lock)
      {
        _lock = true;
        return true;
      }
      else if (!state && _lock)
      {
        rtos::sleep(_debounce);
        if (!Input::read())
        {
          _lock = false;
        }
      }
      return false;
    }

    inline void resetTiming() { _timing = rtos::time<milliseconds>(); }

    milliseconds getTiming() { return rtos::time<milliseconds>() - _timing; }

  private:
    bool _lock = false;
    milliseconds _debounce;
    milliseconds _timing;
  };

} // namespace hardware

#endif
