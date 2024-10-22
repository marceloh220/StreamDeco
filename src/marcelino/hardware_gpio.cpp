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

#include "marcelino/hardware_gpio.hpp"

namespace hardware
{

  GPIO::GPIO(gpio_num_t pin, mode_t mode)
      : _pin(pin), _mode(mode)
  {
    esp_rom_gpio_pad_select_gpio(pin);
    this->mode(mode);
  }

  GPIO::~GPIO()
  {
    gpio_isr_handler_remove(_pin);
    gpio_reset_pin(_pin);
  }

  gpio_num_t GPIO::pinName()
  {
    return _pin;
  }

  void GPIO::mode(mode_t mode)
  {
    _mode = mode;
    switch (mode)
    {
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

  GPIO::mode_t GPIO::mode()
  {
    return _mode;
  }

  void GPIO::interruptAttach(void function(void *), interrupt_t mode, void *arg)
  {
    _interrupt = mode;
    gpio_set_intr_type(_pin, (gpio_int_type_t)mode);
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_isr_handler_add(_pin, function, arg);
  }

  void GPIO::interruptDettach()
  {
    gpio_isr_handler_remove(_pin);
  }

  GPIO::interrupt_t GPIO::interruptType()
  {
    return _interrupt;
  }

  void GPIO::interruptDisable()
  {
    gpio_intr_disable(_pin);
  }

  void GPIO::interruptEnable()
  {
    gpio_intr_enable(_pin);
  }

  bool GPIO::read()
  {
    return gpio_get_level(_pin);
  }

  void GPIO::write(bool state)
  {
    _state = state;
    gpio_set_level(_pin, state);
  }

  void GPIO::toggle(int state)
  {
    if (state)
    {
      _state ^= 1;
      write(_state);
    }
  }

  void GPIO::freeze()
  {
    gpio_hold_en(_pin);
    gpio_deep_sleep_hold_en();
  }

  void GPIO::unfreeze()
  {
    gpio_hold_dis(_pin);
    gpio_deep_sleep_hold_dis();
  }

  GPIO &GPIO::operator=(GPIO &v)
  {
    this->write(v.read());
    return *this;
  }

  GPIO &GPIO::operator=(int v)
  {
    this->write(v);
    return *this;
  }
  
  GPIO &GPIO::operator^=(GPIO &v)
  {
    this->toggle(v.read());
    return *this;
  }

  GPIO &GPIO::operator^=(int v)
  {
    this->toggle(v);
    return *this;
  }

  GPIO::operator int()
  {
    return read();
  }

} // namespace hardware
