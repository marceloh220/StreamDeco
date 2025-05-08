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

#ifndef __HARDWARE_I2C_HP__
#define __HARDWARE_I2C_HP__

#include <driver/i2c.h>
#include <hal/i2c_types.h>
#include "rtos_chrono.hpp"
#include "const_user.hpp"

namespace hardware
{

    namespace master
    {

        class I2C
        {

        public:
            I2C(gpio_num_t sda, gpio_num_t scl, i2c_port_t port, uint32_t speed = 400_kHz);
            ~I2C();
            void init();
            void write(uint8_t *data, size_t length, uint8_t addr, milliseconds timeout);
            void write(uint8_t *data, size_t length, uint8_t addr);
            void write(uint8_t data, uint8_t addr, milliseconds timeout);
            void write(uint8_t data, uint8_t addr);
            void read(uint8_t *data, size_t length, uint8_t addr, milliseconds timeout);
            void read(uint8_t *data, size_t length, uint8_t addr);
            uint8_t read(uint8_t addr, milliseconds timeout);
            uint8_t read(uint8_t addr);
            void cmdStart();
            void cmdStop();
            void cmdWrite(uint8_t data, bool ack = true);
            void cmdWrite(uint8_t *data, size_t length, bool ack);
            void cmdTransmit(milliseconds timeout);
            void cmdTransmit();

        private:
            i2c_port_t _port;
            i2c_config_t _config;
            i2c_cmd_handle_t _cmd;

        }; // end class I2C

    } // end namespace master

} // end namespace hardware

#endif
