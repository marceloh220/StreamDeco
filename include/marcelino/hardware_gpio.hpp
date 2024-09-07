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

/**
 * @namespace  hardware
 * @brief      Low level hardware abstraction peripherals
 * @details    This abstraction layer uses ESP-IDF API to use peripherals
 */
namespace hardware {

/**
 * @sa       rtos::sleep()
 * @sa       rtos::Task
 * @class    GPIO
 * @brief    Abstraction class to GPIO PINS
 * @details  Can configure pin as input, output or interrupt
 * @code
 * hardware::GPIO led_blink(GPIO_NUM_5, hardware::GPIO::OUTPUT);
 * 
 * hardware::GPIO led_controlledByButton(GPIO_NUM_6, hardware::GPIO::OUTPUT);
 * 
 * hardware::GPIO button(GPIO_NUM_7, hardware::GPIO::OUTPUT);
 * 
 * void task_blink_callback(taskArg_t arg) {
 * 
 *    while(true) {
 * 
 *      led_controlledByButton = button;
 * 
 *      rtos::sleep(100ms);
 * 
 *    }
 * 
 * }
 * 
 * extern "C" void app_main() {
 * 
 *    while(true) {
 * 
 *      led_blink = 1;
 * 
 *      rtos::sleep(500ms);
 * 
 *      led_blink = 0;
 * 
 *      rtos::sleep(500ms);
 * 
 *    }
 * 
 * 
 * 
 * }
 */
class GPIO {

public:

  /**
   * @typedef  state_t
   * @brief    GPIO states
   * @details  Simples definitions of HIGH and LOW for logic level high and low
   */
  typedef enum state_e {
    LOW,
    HIGH,
  } state_t;

  /**
   * @typedef  mode_t
   * @brief    GPIO modes supported by API
   */
  typedef enum mode_e {
    INPUT = GPIO_MODE_INPUT,
    OUTPUT = GPIO_MODE_OUTPUT,
    INPUT_OUTPUT = GPIO_MODE_INPUT_OUTPUT,
    INPUT_PULLUP,
    INPUT_PULLDOWN,
  } mode_t;

  /**
   * @typedef  interrupt_t
   * @brief    GPIO interrupt supported by API
   */
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

  /**
   * @brief   Return the GPIO pin name
   * @return  GPIO pin name
   */
  gpio_num_t pinName();

  /**
   * @sa      mode_t
   * @brief   Change GPIO pin mode
   * @param   mode  GPIO pin mode
   */
  void mode(mode_t mode);

  /**
   * @brief   Return the GPIO pin mode
   * @return  GPIO pin mode
   */
  mode_t mode();

  /**
   * @brief   Attach an interrupt on the pin and associate this interrupt 
   *          with an handler callback function
   * @param   function  The function callback to handle interrupt
   * @param   mode      The interrupt mode, must be type mode_t
   * @param   arg       Some argument to be passed by API to function callback
   * @note    The callback function must return and receive an void pointer argument, 
   *          also the IRAM_ATTR attribute must be used to copy the function code onto
   *          the instruction RAM section while SoC initialization. Since this space is not large,
   *          interrupts functions must be brief and execute only urgent actions.
   *          Complicated things must be left be made by some task.
   * @code
   * void IRAM_ATTR isr_botao(void *arg) {  
   * 
   *    // do some urgents and simples things
   * 
   *    ...
   *    
   *    // left the rest to do  by some task
   * 
   *    task_to_do_more_complicated_things.sendNotifyFromISR(42);
   * 
   * }
   */
  void interruptAttach(void function(void *), interrupt_t mode, void *arg = NULL);

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
