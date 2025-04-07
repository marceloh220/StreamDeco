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

#ifndef __HARDWARE_INPUT_HPP__
#define __HARDWARE_INPUT_HPP__

#include "hardware_gpio.hpp"

#ifdef PULLUP
#undef PULLUP
#endif
#ifdef PULLDOWN
#undef PULLDOWN
#endif

namespace hardware
{

  /**
   * @class      Input
   * @brief      Uses GPIO pin as input
   * @code
   * hardware::Input button(GPIO_PIN_5, hardware::Input::PULLUP);
   *
   * extern "C" app_main() {
   *
   *    while(true) {
   *
   *      if(button) printf("Warning! This is not an simulçation!\n The button was pressed!\n");
   *
   *    }
   *
   * }
   */
  class Input : private GPIO
  {
  public:
    /**
     * @typedef   hardware::Input::mode_t
     * @brief     GPIO Input modes avaliable
     */
    typedef enum mode_e
    {
      FLOATING = GPIO::INPUT,
      PULLUP = GPIO::INPUT_PULLUP,
      PULLDOWN = GPIO::INPUT_PULLDOWN,
    } mode_t;

    Input(gpio_num_t pin, mode_t mode = PULLDOWN)
        : GPIO(pin, (GPIO::mode_t)mode) {}

    /**
     * @brief   Get pin name
     * @return  Return the gpio_num_t type pin
     */
    inline gpio_num_t pinName() { return GPIO::pinName(); }

    /**
     * @brief   Read pin state without intervention
     * @return  True if the pin is in HIGH level state
     *          False if pin is in LOW level state
     * @note    In PULLDOWN mode the logic is inverted,
     *          the button will return true if an push button is not pressed.
     */
    [[nodiscard]] inline bool readRaw() { return GPIO::read(); }

    /**
     * @brief   Read pin state inverting logic in PULLUP mode
     * @return  True if the pin is in HIGH level state
     *          False if pin is in LOW level state
     * @note    In PULLUP mode the logic are inverted,
     *          the mutton will return true if an push button is pressed.
     */
    [[nodiscard]] virtual bool read()
    {
      bool state = GPIO::read();
      return GPIO::mode() == INPUT_PULLUP ? !state : state;
    }

    /**
     * @sa      read()
     * @brief   Object read() method shortcuts.
     * @code
     * hardware::Input button(GPIO_NUM_5, hardware::Input::PULLDOWN);
     *
     * extern "C" app_main() {
     *
     *    while(true) {
     *
     *      if(button) //same as if(button.read())
     *        printf("The button is pressed!\n");
     *
     *      rtos::sleep(1ms);
     *
     *    }
     *
     * }
     */
    [[nodiscard]] inline operator int() { return read(); }

  }; // class Input

} // namespace hardware

#endif
