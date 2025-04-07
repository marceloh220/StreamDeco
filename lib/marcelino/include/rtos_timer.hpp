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

#ifndef _RTOS_TIMER_HPP_
#define _RTOS_TIMER_HPP_

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "rtos_chrono.hpp"

namespace rtos
{

  /**
   * @brief  Creates a new software timer instance.
   *
   * Internally, within the FreeRTOS implementation, software timers use a block
   * of memory, in which the timer data structure is stored. If a software timer
   * is created using rtos::Timer then the required memory is automatically
   * dynamically allocated inside the class.
   *
   * Timers are created in the dormant state, a callback function to handle timer
   * event must be attached with rtos::Timer::attach() method.
   * The rtos::Timer::start(), rtos::Timer::reset(), rtos::Timer::startFromISR(),
   * rtos::Timer::resetFromISR(), rtos::Timer::period() and rtos::Timer::periodFromISR()
   * API functions can all be used to transition a timer into the active state.
   *
   * @param name A text name that is assigned to the timer. This is done
   * purely to assist debugging. The kernel itself only ever references a timer
   * by its handle, and never by its name.
   *
   * @param period The timer period specified in milliseconds, seconds, minutres or hours.
   *               For example, if the timer must expire after 100 milliseconds,
   *               then period should be set to 100ms.
   *               Alternatively, if the timer must expire after 5 seconds,
   *               then period can be set to 5s. Time timer period must be greater than 0.
   *
   * @code
   * rtos::Timer xTimer1("Timer one", 5s);
   *
   * void xTimerCallback(TimerHandle_t xTimerArg)
   * {
   *
   *    if(xTimer1.verifyID(xTimerArg))
   *    {
   *
   *      printf("%s: event occour!\n", xTimer1.name());
   *
   *    }
   *
   * }
   *
   * extern "C" int app_main()
   * {
   *
   *    xTimer1.attach(xTimerCallback);
   *
   *    xTimer1.start();
   *
   * }
   */
  class Timer
  {
  public:
    Timer(const char *name, milliseconds periode);

    ~Timer();

    /**
     * @brief    Attach the handler callback on timer
     * @details  Affter instanciate the timer still not running
     *           and must have an handler attached to enter in execution and be started
     * @param    callback  Function callback to handler the timer execution
     * @param autoReload If autoReload is set to true then the timer will
     * expire repeatedly with a frequency set by the period parameter.
     * If autoReload is set to false then the timer will be a one-shot timer and
     * enter the dormant state after it expires.
     * @note     The handler function must be type void and receive arg type TimerHandle_t
     * @code
     *           void timer_callback(TimerHandler_t arg)
     *           {
     *
     *             printf("Timer periode expire =)\n");
     *
     *           }
     */
    void attach(TimerCallbackFunction_t callback, UBaseType_t autoreload = true);

    /**
     * @brief  Starts a timer that was previously attached using the
     * rtos::Timer::attach method. If the timer had already been started and was
     * already in the active state, then has equivalent functionality
     * to the rtos::Timer::reset method.
     */
    void start();

    /**
     * @sa    rtos::Timer::start()
     * @brief A version of rtos::Timer::start that can be called from an interrupt
     * service routine.
     */
    void startFromISR();

    /**
     * @brief  Stops a timer that was previously started using either of the
     * rtos::Timer::start(), rtos::Timer::reset(), rtos::Timer::startFromISR(),
     * rtos::Timer::resetFromISR(), rtos::Timer::period() or rtos::Timer::periodFromISR()
     * methods.
     */
    void stop();

    /**
     * @sa     rtos::Timer::stop()
     * @brief  A version of rtos::Timer::stop() that can be called from an interrupt
     * service routine.
     */
    void stopFromISR();

    /**
     * @sa     rtos::Tomar::start()
     * @brief  Te-starts a timer that was previously created using the.
     * If the timer had already been started and was already in the active state,
     * then rtos::Timer::reset() will cause the timer to re-evaluate its expiry time
     * so that it is relative to when rtos::Timer::reset() was called.
     * @note   If the timer was in the dormant state then rtos::Timer::reset() has
     * equivalent functionality to the rtos::Timer::start() API function.
     */
    void reset();

    /**
     * @sa    rtos::Timer::reset()
     * @brief A version of rtos::Timer::reset() that can be called from an
     * interrupt service routine.
     */
    void resetFromISR();
    /**
     * @brief  Changes the period of a timer that was previously created.
     * @param period The timer period specified in milliseconds, seconds, minutres or hours.
     *               For example, if the timer must expire after 100 milliseconds,
     *               then period should be set to 100ms.
     *               Alternatively, if the timer must expire after 5 seconds,
     *               then period can be set to 5s. Time timer period must be greater than 0.
     * @note   rtos::Timer::period() can be called to change the period of an active or
     * dormant state timer.
     */
    void periode(milliseconds periode);

    /**
     * @sa     rtos::Timer::changePeriod()
     * @brief  A version of rtos::Timer::changePeriod() that can be called from an interrupt
     * service routine.
     */
    void changePeriodeFromISR(milliseconds periode);

    /**
     * @brief   Get timer periode
     * @return  Timer periode in milliseconds type
     * @note    Safe to use into ISR
     */
    milliseconds getPeriode();

    /**
     * @brief   Get timer periode
     * @return  Timer periode in int64_t type specified in milliseconds
     * @note    Safe to use into ISR
     */
    int64_t getPeriodeIntMS();

    /**
     * @brief    Verify the timer ID
     * @details  Each timer has a unic ID created by class during timer creation.
     *           This method verify if timer received by callbak parameter
     *           correspond to the timer.
     * @return   Return true if ID of argument correspond with timer ID
     *           Or false if don't
     * @code
     * rtos::Timer xTimer1("Timer one", 5s);
     *
     * void xTimerCallback(TimerHandle_t xTimerArg)
     * {
     *
     *    if(xTimer1.verifyID(xTimerArg))
     *    {
     *
     *      printf("%s: event occour!\n", xTimer1.name());
     *
     *    }
     *
     * }
     */
    bool verifyID(TimerHandle_t timer);

    /**
     * @brief Deletes a timer that was previously created.
     */
    void timerDelete();

    /**
     * @brief   Get timer name
     * @return  The text (human readable) name of the task
     * @note    Safe to be called in ISR, but not usable =P
     */
    const char *name();

  private:
    /**
     * @var    _handle
     * @brief  Keep task handle into object to easy access
     */
    TimerHandle_t _handler = NULL;

    /**
     * @var    _name
     * @brief  Pointer to keep task name into object to easy access
     */
    const char *_name;

    /**
     * @var    _periode
     * @brief  Keep timer periode into object to easy access
     */
    milliseconds _periode;

  }; // class Timer

} // namespace rtos

#endif
