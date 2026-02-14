#ifndef MARCELINO_HARDWARE_PWM_HPP
#define MARCELINO_HARDWARE_PWM_HPP

#include "driver/ledc.h"
#include "const_user.hpp"
#include "esp_log.h"

namespace hardware
{

  /**
   * @brief   PWM usage tracker
   * @details To avoid using same timer and channel for multiple PWM instances
   * @note    ESP32 has 4 timers and 8 channels for LEDC peripheral
   * @note    ESP32-C3 has 2 timers and 4 channels for LEDC peripheral
   * @note    ESP32-S2 has 4 timers and 8 channels for LEDC peripheral
   * @note    ESP32-S3 has 4 timers and 8 channels for LEDC peripheral
   * @note    ESP32-C6 has 2 timers and 4 channels for LEDC peripheral
   * @note    ESP32-H2 has 2 timers and 4 channels for LEDC peripheral
   * @note    ESP32-C2 has 2 timers and 4 channels for LEDC peripheral
   * @note    ESP32-C5 has 2 timers and 4 channels for LEDC peripheral
   */
  static bool pwm_used[LEDC_TIMER_MAX][LEDC_CHANNEL_MAX] = {{false}};

  /**
   * @brief   PWM logger tag
   * @details Used to identify PWM logs in console
   */
  static const char *pwm_log_tag = "PWM logger";

  /**
   * @class   PWM
   * @brief   Abstraction class to PWM peripheral using LEDC driver
   * @details Create PWM on specified pin with given resolution and frequency
   *          Set and get brightness as float (0.0 to 1.0) or raw value (0 to max based on resolution)
   *         Avoid using same timer and channel for multiple PWM instances
   *        Example:
   *     @code
   * hardware::PWM backlight(GPIO_NUM_21, hardware::PWM::TIMER_8_BIT, 1_kHz, 0.8f);
   * // Create a PWM on GPIO21 with 8 bits resolution, 1 kHz frequency and 80% initial brightness
   *     @endcode
   */
  class PWM
  {

  public:
    /**
     * @brief   PWM resolution type
     * @details Resolution from 1 to 20 bits based on LEDC driver support
     * @note    Setting numeric values to match ledc_timer_bit_t values are a hack to avoid collision with
     *          LEDC_TIMER_x_BIT in the driver, as these enums have very similar names and user may pass
     *          one of these by mistake.
     */
    typedef enum
    {
      TIMER_1_BIT = 1, /*!< PWM duty resolution of  1 bits */
      TIMER_2_BIT,     /*!< PWM duty resolution of  2 bits */
      TIMER_3_BIT,     /*!< PWM duty resolution of  3 bits */
      TIMER_4_BIT,     /*!< PWM duty resolution of  4 bits */
      TIMER_5_BIT,     /*!< PWM duty resolution of  5 bits */
      TIMER_6_BIT,     /*!< PWM duty resolution of  6 bits */
      TIMER_7_BIT,     /*!< PWM duty resolution of  7 bits */
      TIMER_8_BIT,     /*!< PWM duty resolution of  8 bits */
      TIMER_9_BIT,     /*!< PWM duty resolution of  9 bits */
      TIMER_10_BIT,    /*!< PWM duty resolution of 10 bits */
      TIMER_11_BIT,    /*!< PWM duty resolution of 11 bits */
      TIMER_12_BIT,    /*!< PWM duty resolution of 12 bits */
      TIMER_13_BIT,    /*!< PWM duty resolution of 13 bits */
      TIMER_14_BIT,    /*!< PWM duty resolution of 14 bits */
#if SOC_LEDC_TIMER_BIT_WIDE_NUM > 14
      TIMER_15_BIT, /*!< PWM duty resolution of 15 bits */
      TIMER_16_BIT, /*!< PWM duty resolution of 16 bits */
      TIMER_17_BIT, /*!< PWM duty resolution of 17 bits */
      TIMER_18_BIT, /*!< PWM duty resolution of 18 bits */
      TIMER_19_BIT, /*!< PWM duty resolution of 19 bits */
      TIMER_20_BIT, /*!< PWM duty resolution of 20 bits */
#endif
      TIMER_BIT_MAX,
    } resolution_t;

    /**
     * @brief   PWM class constructor
     * @param   pin GPIO pin to be used as PWM output
     * @param   resolution PWM resolution, default is 8 bits
     * @param   frequency PWM frequency, default is 1 kHz
     * @param   initial_bright Initial brightness from 0.0 to 1.0, default is 0.5
     * @details Create a PWM on specified pin with given resolution and frequency
     *         Set initial brightness as well
     * @code
     * hardware::PWM backlight(GPIO_NUM_21, hardware::PWM::TIMER_8_BIT, 1_kHz, 0.8f);
     * // Create a PWM on GPIO21 with 8 bits resolution, 1 kHz frequency and 80% initial brightness
     * @endcode
     */
    PWM(gpio_num_t pin, resolution_t resolution = TIMER_8_BIT, uint32_t frequency = 1_kHz, float initial_bright = 0.5f)
    {
      int timer_num = -1;
      int channel_num = -1;
      math::clamp<float>(initial_bright, 0.0f, 1.0f);
      for (int t = 0; t < LEDC_TIMER_MAX; t++)
      {
        for (int c = 0; c < LEDC_CHANNEL_MAX; c++)
        {
          if (!pwm_used[t][c])
          {
            timer_num = t;
            channel_num = c;
            pwm_used[t][c] = true;
            break;
          }
        }
        if (timer_num != -1 && channel_num != -1)
        {
          break;
        }
      }
      if (timer_num == -1 && channel_num == -1)
      {
        // No available PWM timer and channel
        ESP_LOGE(pwm_log_tag, "No available PWM timer and channel!");
        this->state = false;
      }
      else
      {
        ESP_LOGI(pwm_log_tag, "Using PWM timer %d and channel %d", timer_num, channel_num);
        this->timer.speed_mode = frequency > 40_kHz ? LEDC_SPEED_MODE_MAX : LEDC_LOW_SPEED_MODE;
        this->timer.duty_resolution = static_cast<ledc_timer_bit_t>(resolution);
        this->timer.timer_num = static_cast<ledc_timer_t>(timer_num);
        this->timer.freq_hz = frequency;
        this->timer.clk_cfg = LEDC_AUTO_CLK;
        ESP_ERROR_CHECK(ledc_timer_config(&this->timer));
        this->channel.gpio_num = pin;
        this->channel.speed_mode = this->timer.speed_mode;
        this->channel.channel = static_cast<ledc_channel_t>(channel_num);
        this->channel.intr_type = LEDC_INTR_DISABLE;
        this->channel.timer_sel = this->timer.timer_num;
        this->channel.duty = this->max() * initial_bright;
        this->channel.hpoint = 0;
        ESP_ERROR_CHECK(ledc_channel_config(&this->channel));
        this->state = true;
      } // end else
    } // end constructor

    /**
     * @brief   Get maximun PWM value based on PWM resolution
     * @return  Maximun PWM value
     */
    uint32_t max()
    {
      if (this->state == false)
      {
        return 0;
      }
      return (((uint32_t)1 << (uint32_t)this->timer.duty_resolution) - (uint32_t)1);
    }

    /**
     * @brief   Set backlight bright
     * @details Set backlight bright for 0.0 to 1.0
     * @param   bright 0.0 to zero bright 1.0 to maximum bright
     *                 0.5 mean halfh bright
     */
    void set(float bright)
    {
      if (this->state == false)
      {
        return;
      }
      math::clamp<float>(bright, 0.0f, 1.0f);
      uint32_t pwm_value = this->max() * bright;
      ledc_set_duty(this->channel.speed_mode, this->channel.channel, pwm_value);
      ledc_update_duty(this->channel.speed_mode, this->channel.channel);
    }

    /**
     * @sa      max
     * @brief   Set backlight bright
     * @details Set backlight bright in raw number from 0 to max()
     * @param   bright 0 to zero bright and maximun to maximun bright
     *                 half bright can be achived with max()/2
     */
    void set(int bright)
    {
      if (this->state == false)
      {
        return;
      }
      math::clamp<int>(bright, 0, this->max());
      ledc_set_duty(this->channel.speed_mode, this->channel.channel, bright);
      ledc_update_duty(this->channel.speed_mode, this->channel.channel);
    }

    uint32_t read()
    {
      if (this->state == false)
      {
        return 0;
      }
      return ledc_get_duty(this->channel.speed_mode, this->channel.channel);
    }

    /**
     * @brief   Get current PWM value as raw number
     * @return  Current PWM value from 0 to max()
     */
    operator uint32_t()
    {
      if (this->state == false)
      {
        return 0;
      }
      return this->read();
    }

    /**
     * @brief   Get current PWM value as float
     * @return  Current PWM value from 0.0 to 1.0
     */
    operator float()
    {
      if (this->state == false)
      {
        return 0.0f;
      }
      return ((float)this->read() / (float)this->max());
    }

    /**
     * @brief   Set PWM brightness using assignment operator with float
     * @param   bright Brightness from 0.0 to 1.0
     * @return  Reference to this PWM instance
     */
    PWM &operator=(float bright)
    {
      if (this->state == true)
        this->set(bright);
      return *this;
    }

    /**
     * @brief   Set PWM brightness using assignment operator with int
     * @param   bright Brightness from 0 to max()
     * @return  Reference to this PWM instance
     */
    PWM &operator=(int bright)
    {
      if (this->state == true)
        this->set(bright);
      return *this;
    }

    PWM &operator=(const PWM &other) {
      if (this != &other) {
        this->set(other.read());
      }
      return *this;
    }
    
    private:
    ledc_timer_config_t timer;
    ledc_channel_config_t channel;
    bool state = true;
  }; // class PWM

} // namespace hardware

#endif // MARCELINO_HARDWARE_PWM_HPP