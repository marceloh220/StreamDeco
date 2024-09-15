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

#ifdef LOW
  #undef LOW
#endif
#ifdef HIGH
  #undef HIGH
#endif
#ifdef INPUT
  #undef INPUT
#endif
#ifdef OUTPUT
  #undef OUTPUT
#endif
#ifdef INPUT_PULLUP
  #undef INPUT_PULLUP
#endif
#ifdef INPUT_PULLDOWN
  #undef INPUT_PULLDOWN
#endif

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
   * rtos::Task task_to_do_more_complicated_things("Button Pressed!");
   * 
   * hardware::GPIO button(GPIO_NUM_5, hardware::GPIO::INPUT);
   * 
   * hardware::GPIO led(GPIO_NUM_6, hardware::GPIO::OUTPUT);
   * 
   * void IRAM_ATTR isr_button(void *arg) {
   * 
   *    // do some urgents and simples things. Don't print herer!
   * 
   *    led ^= 1;
   *    
   *    // left the rest to do  by some task
   * 
   *    task_to_do_more_complicated_things.sendNotifyFromISR(42);
   * 
   * }
   * 
   * void task_to_do_more_complicated_things_callback(taskArg_t arg) {
   * 
   *    while(true) {
   * 
   *      uint32_t notification = task_to_do_more_complicated_things.takeNotify();
   * 
   *      if(notification == 42) {
   * 
   *        printf("This is not a simulation! The button was pressed!")
   * 
   *      }
   * 
   *    }
   * 
   * }
   * 
   * extern "C" void app_main() {
   * 
   *    button.attach(isr_button, hardware::GPIO::RISING_EDGE);
   *    
   *    task_to_do_more_complicated_things.attach(task_to_do_more_complicated_things_callback);
   * 
   *    while(true) {
   * 
   *      printf("Hour off!\n");
   * 
   *      rtos::sleep(1h);
   * 
   *    }
   * 
   * }
   */
  void interruptAttach(void function(void *), interrupt_t mode, void *arg = NULL);

  /**
   * @brief   Detach the interrupt from GPIO pin
   */
  void interruptDettach();

  /**
   * @sa      interrupt_t
   * @brief   Get the interrupt type
   * @return  interrupt_t  Interrupt type
   */
  interrupt_t interruptType();

  /**
   * @brief   Disable the interrupt
   * @note    This method don't remove the interrupt ISR service,
   *          interruptEnable method can be used to enable interrupt again.
   */
  void interruptDisable();

  /**
   * @brief    Enable an disabled interrupt
   */
  void interruptEnable();

  /**
   * @brief   Read the pin state
   * @return  true if pin state are in high level 
   *          false if pin state are in low level 
   */
  bool read();

  /**
   * @brief   Write an state in pin
   * @param   state  Pin state, this can be true, 1 or 
   *          hardware::GPIO::HIGH to set pin to high level.
   *          Or false, or 0 or hardware::GPIO::LOW to clear 
   *          the state of pin.
   */
  void write(bool state);

  /**
   * @brief   Toggle the state of GPIO pin.
   * @details If pin is in LOW level it's become HIGH level.
   * @details If pin is in HIGH level it's become LOW level.
   * @note    Useful to make Hello World programs
   */
  void toggle(int state = 1);

  /**
   * @brief   Trying discover what the fluid it does
   */
  void freeze();

  /**
   * @brief   Trying discover what the fluid it does
   */
  void unfreeze();

  /**
   * @sa      write()
   * @sa      read()
   * @sa      operator int
   * @brief   Object write method shortcuts
   * @code
   * hardware::GPIO led(GPIO_NUM_5, hardware::GPIO::OUTPUT);
   * 
   * hardware::GPIO button(GPIO_NUM_6, hardware::GPIO::INPUT_PULLDOWN);
   * 
   * extern "C" app_main() {
   * 
   *    while(true) {
   * 
   *      led = button; //same as led.write(button.read())
   *  
   *      rtos::sleep(50ms);
   * 
   *    }
   * 
   * }
   */
  GPIO &operator=(GPIO &v);

  /**
   * @sa      write()
   * @brief   Object write method shortcuts
   * @code
   * hardware::GPIO led(GPIO_NUM_5, hardware::GPIO::OUTPUT);
   * 
   * extern "C" app_main() {
   * 
   *    while(true) {
   * 
   *      led = 1; //same as led.write(1)
   *  
   *      rtos::sleep(500ms);
   *  
   *      led = 0; //same as led.write(0)
   * 
   *      rtos::sleep(500ms);
   * 
   *    }
   * 
   * }
   */
  GPIO &operator=(int v);

  /**
   * @sa      toogle()
   * @sa      read()
   * @sa      operator int
   * @brief   Object toogle method shortcuts.
   * @details Toggle state level of GPIO pin if pin passed as parameter is in HIGH level state.
   * @code
   * hardware::GPIO led(GPIO_NUM_5, hardware::GPIO::OUTPUT);
   * 
   * hardware::GPIO button(GPIO_NUM_6, hardware::GPIO::INPUT_PULLDOWN);
   * 
   * extern "C" app_main() {
   * 
   *    while(true) {
   * 
   *      led ^= button; //same as led.toggle(button.read())
   *  
   *      rtos::sleep(50ms);
   * 
   *    }
   * 
   * }
   */
  GPIO &operator^=(GPIO &v);

  /**
   * @sa      toggle()
   * @brief   Object toogle method shortcuts.
   * @details Toggle state level of GPIO pin if true are passed as parameter.
   * @code
   * hardware::GPIO led(GPIO_NUM_5, hardware::GPIO::OUTPUT);
   * 
   * extern "C" app_main() {
   * 
   *    while(true) {
   * 
   *      led ^= 1; //same as led.toggle(1)
   *  
   *      rtos::sleep(50ms);
   * 
   *    }
   * 
   * }
   */
  GPIO &operator^=(int v);

  /**
   * @sa      read()
   * @brief   Object read method shortcuts.
   * @details Read state level of GPIO pin.
   * @code
   * hardware::GPIO button(GPIO_NUM_6, hardware::GPIO::INPUT_PULLDOWN);
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
  operator int();

private:

  const gpio_num_t _pin;
  mode_t _mode;
  bool _state;
  interrupt_t _interrupt;

};

} // namespace hardware

#endif
