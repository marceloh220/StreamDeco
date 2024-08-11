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

#ifndef _GPIO_HPP_
#define _GPIO_HPP_

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

  GPIO(gpio_num_t pin, mode_t mode) : _pin(pin), _mode(mode) {
    esp_rom_gpio_pad_select_gpio(pin);
    this->mode(mode);
  }

  ~GPIO() {
    gpio_isr_handler_remove(_pin);
    gpio_reset_pin(_pin);
  }

  inline gpio_num_t pinName() { return _pin; }

  void mode(mode_t mode) {
    _mode = mode;
    switch (mode) {
    case INPUT:
    case OUTPUT:
    case INPUT_OUTPUT:
      gpio_set_direction(_pin, (gpio_mode_t)mode);
      break;
    case INPUT_PULLUP:
      gpio_set_direction(_pin, GPIO_MODE_INPUT);
      gpio_pulldown_dis(_pin);
      gpio_pullup_en(_pin);
      break;
    case INPUT_PULLDOWN:
      gpio_set_direction(_pin, GPIO_MODE_INPUT);
      gpio_pullup_dis(_pin);
      gpio_pulldown_en(_pin);
      break;
    }
  }

  inline mode_t mode() { return _mode; }

  void attach(void function(void *), interrupt_t mode, void *arg) {
    _interrupt = mode;
    gpio_set_intr_type(_pin, (gpio_int_type_t)mode);
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_isr_handler_add(_pin, function, arg);
  }

  inline void dettach() { gpio_isr_handler_remove(_pin); }

  inline interrupt_t interruptType() { return _interrupt; }

  inline void interruptDisable() { gpio_intr_disable(_pin); }

  inline void interruptEnable() { gpio_intr_enable(_pin); }

  inline bool read() { return gpio_get_level(_pin); }

  inline void write(bool state) {
    _state = state;
    gpio_set_level(_pin, state);
  }

  inline void toggle(int state = 1) {
    if (state) {
      _state ^= 1;
      write(_state);
    }
  }

  void freeze() {
    gpio_hold_en(_pin);
    gpio_deep_sleep_hold_en();
  }

  void unfreeze() {
    gpio_hold_dis(_pin);
    gpio_deep_sleep_hold_dis();
  }

  GPIO &operator=(GPIO &v) {
    this->write(v.read());
    return *this;
  }

  GPIO &operator=(int v) {
    this->write(v);
    return *this;
  }

  GPIO &operator^=(int v) {
    this->toggle(v);
    return *this;
  }

  inline operator int() { return read(); }

private:
  const gpio_num_t _pin;
  mode_t _mode;
  bool _state;
  interrupt_t _interrupt;
};

} // namespace hardware

#endif
