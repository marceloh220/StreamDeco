/**
 * Copyright © 2025 Marcelo H Moraes
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

#include "hardware_i2c.hpp"

namespace hardware
{

    static uint8_t I2C_PERIPHERAL[I2C_NUM_MAX] = {0};

    namespace master
    {

        I2C::I2C(gpio_num_t sda, gpio_num_t scl, i2c_port_t port, uint32_t speed)
        {
            speed = speed > 400_kHz ? 400_kHz : speed;
            speed = speed < 100_kHz ? 100_kHz : speed;
            _port = port;
            _config.sda_io_num = sda;
            _config.scl_io_num = scl;
            _config.mode = I2C_MODE_MASTER;
            _config.master.clk_speed = speed;
            _config.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;
        }

        I2C::~I2C()
        {
            if ((--I2C_PERIPHERAL[_port]))
            {
                printf("[HARDWARE]  I2C[%d] interface still in use by %d unites", _port, I2C_PERIPHERAL[_port]);
                return;
            }
            esp_err_t error = i2c_driver_delete(_port);
            printf("[HARDWARE]  Deinit I2C[%d] (delete driver) state: #%s\n", _port, esp_err_to_name(error));
            error = gpio_reset_pin((gpio_num_t)_config.sda_io_num);
            printf("[HARDWARE]  Reset SDA (pin %d) state: #%s\n", _config.sda_io_num, esp_err_to_name(error));
            error = gpio_reset_pin((gpio_num_t)_config.scl_io_num);
            printf("[HARDWARE]  Reset SCL (pin %d) state: #%s\n", _config.scl_io_num, esp_err_to_name(error));
            i2c_cmd_link_delete(_cmd);
        } // descontructor

        void I2C::init()
        {
            if (_port > (I2C_NUM_MAX - 1))
            {
                printf("[HARDWARE]  I2C[%d] port number incorrect. This SoC have %d I2C peripherals, select an port between 0 to %d\n", _port, I2C_NUM_MAX, I2C_NUM_MAX - 1);
                return;
            }
            if (I2C_PERIPHERAL[_port] == 0)
            {
                esp_err_t error = i2c_driver_install(_port, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_LOWMED);
                printf("[HARDWARE]  Init I2C[%d] (install driver) state: #%s\n", _port, esp_err_to_name(error));
                error = i2c_param_config(_port, &_config);
                printf("[HARDWARE]  Init I2C[%d] (config driver)  state: #%s\n", _port, esp_err_to_name(error));
            }
            I2C_PERIPHERAL[_port]++;
            _cmd = i2c_cmd_link_create();
        }

        void I2C::write(uint8_t *data, size_t length, uint8_t addr, milliseconds timeout)
        {
            esp_err_t error = i2c_master_write_to_device(_port, addr, data, length, rtos::chronoToTick(timeout));
            printf("[HARDWARE]  Write I2C[%d] device #%02X state: %s\n", _port, addr, esp_err_to_name(error));
        }

        void I2C::write(uint8_t *data, size_t length, uint8_t addr)
        {
            esp_err_t error = i2c_master_write_to_device(_port, addr, data, length, portMAX_DELAY);
            printf("[HARDWARE]  Write I2C[%d] device #%02X state: %s\n", _port, addr, esp_err_to_name(error));
        }

        void I2C::write(uint8_t data, uint8_t addr, milliseconds timeout)
        {
            esp_err_t error = i2c_master_write_to_device(_port, addr, &data, 1, rtos::chronoToTick(timeout));
            printf("[HARDWARE]  Write I2C[%d] device #%02X state: %s\n", _port, addr, esp_err_to_name(error));
        }

        void I2C::write(uint8_t data, uint8_t addr)
        {
            esp_err_t error = i2c_master_write_to_device(_port, addr, &data, 1, portMAX_DELAY);
            printf("[HARDWARE]  Write I2C[%d] device #%02X state: %s\n", _port, addr, esp_err_to_name(error));
        }

        void I2C::read(uint8_t *data, size_t length, uint8_t addr, milliseconds timeout)
        {
            esp_err_t error = i2c_master_read_from_device(_port, addr, data, length, rtos::chronoToTick(timeout));
            printf("[HARDWARE]  Read I2C[%d] device #%02X state: %s\n", _port, addr, esp_err_to_name(error));
        }

        void I2C::read(uint8_t *data, size_t length, uint8_t addr)
        {
            esp_err_t error = i2c_master_read_from_device(_port, addr, data, length, portMAX_DELAY);
            printf("[HARDWARE]  Read I2C[%d] device #%02X state: %s\n", _port, addr, esp_err_to_name(error));
        }

        uint8_t I2C::read(uint8_t addr, milliseconds timeout)
        {
            uint8_t data = 0;
            esp_err_t error = i2c_master_read_from_device(_port, addr, &data, 1, rtos::chronoToTick(timeout));
            printf("[HARDWARE]  Read I2C[%d] device #%02X state: %s\n", _port, addr, esp_err_to_name(error));
            return data;
        }

        uint8_t I2C::read(uint8_t addr)
        {
            uint8_t data = 0;
            esp_err_t error = i2c_master_read_from_device(_port, addr, &data, 1, portMAX_DELAY);
            printf("[HARDWARE]  Read I2C[%d] device #%02X state: %s\n", _port, addr, esp_err_to_name(error));
            return data;
        }

        void I2C::cmdStart()
        {
            i2c_master_start(_cmd);
        }

        void I2C::cmdStop()
        {
            i2c_master_stop(_cmd);
        }

        void I2C::cmdWrite(uint8_t data, bool ack)
        {
            i2c_master_write_byte(_cmd, data, ack);
        }

        void I2C::cmdWrite(uint8_t *data, size_t length, bool ack)
        {
            i2c_master_write(_cmd, data, length, ack);
        }

        void I2C::cmdTransmit(milliseconds timeout)
        {
            i2c_master_cmd_begin(_port, _cmd, rtos::chronoToTick(timeout));
        }

        void I2C::cmdTransmit()
        {
            i2c_master_cmd_begin(_port, _cmd, portMAX_DELAY);
        }

    } // end namespace master

} // end namespace hardware
