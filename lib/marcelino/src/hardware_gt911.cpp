#include "hardware_output.hpp"
#include "hardware_gt911.hpp"
#include "hardware_i2c.hpp"

#include "hardware_gt911_codes.hpp"

#include "const_user.hpp"

namespace hardware
{

  namespace gt911
  {

    GT911::GT911(master::I2C *_wire,
      gpio_num_t _rst, gpio_num_t _int, hardware::GPIO::interrupt_t mode,
      uint16_t _width, uint16_t _height,
      uint8_t _addr)
    : wire(_wire), 
    reset_pin(_rst), interrupt_pin(_int, hardware::GPIO::OUTPUT), width(_width), height(_height), addr(_addr)
    {
    }

    void GT911::init()
    {
      reset();
    }

    void GT911::reset()
    {
      interrupt_pin = 0;
      reset_pin = 0;
      rtos::sleep(10ms);
      interrupt_pin = addr == GT911_ADDR2;
      rtos::sleep(1ms);
      reset_pin = 1;
      rtos::sleep(5ms);
      interrupt_pin = 0;
      rtos::sleep(50ms);
      interrupt_pin.mode(GPIO::INPUT);
      rtos::sleep(50ms);
      readBlockData(configBuffer.reg(), configBuffer.buffer(), configBuffer.length());
      setResolution(width, height);
    }

    void GT911::reflashConfig()
    {
      writeByteData(GT911_CONFIG_CHKSUM, configBuffer.calcChecksum());
      writeByteData(GT911_CONFIG_FRESH, 1);
    }
    
    void GT911::setRotation(uint8_t rot)
    {
      rotation = rot;
    }
    
    void GT911::setResolution(uint16_t width, uint16_t height)
    {
      configBuffer.xResolution = width;
      configBuffer.yResolution = height;
      reflashConfig();
    }

    void GT911::read()
    {
      uint8_t data[7];

      uint8_t pointInfo = readByteData(GT911_POINT_INFO);
      uint8_t bufferStatus = pointInfo >> 7 & 1;
      isLargeDetect = pointInfo >> 6 & 1;
      touches = pointInfo & 0xF;
      isTouched = touches > 0;
      if (bufferStatus == 1 && isTouched)
      {
        for (uint8_t i = 0; i < touches; i++)
        {
          readBlockData(GT911_POINT_1 + i * 8, data, 7);
          points[i] = readPoint(data, sizeof(data));
        }
      }
      writeByteData(GT911_POINT_INFO, 0);
    }

    Point GT911::readPoint(uint8_t *data, uint8_t size)
    {
      uint16_t temp;
      uint8_t id = data[0];
      uint16_t x = data[1] + (data[2] << 8);
      uint16_t y = data[3] + (data[4] << 8);
      uint16_t length = data[5] + (data[6] << 8);
      switch (rotation)
      {
      case ROTATION_NORMAL:
        x = width - x;
        y = height - y;
        break;
      case ROTATION_LEFT:
        temp = x;
        x = width - y;
        y = temp;
        break;
      case ROTATION_INVERTED:
        x = x;
        y = y;
        break;
      case ROTATION_RIGHT:
        temp = x;
        x = y;
        y = height - temp;
        break;
      default:
        break;
      }
      return Point(id, x, y, length);
    }

    void GT911::writeByteData(uint16_t reg, uint8_t data)
    {
      uint8_t send[] = {bitwise::high(reg), bitwise::low(reg), data};
      wire->write(send, 3, addr);
    }

    uint8_t GT911::readByteData(uint16_t reg)
    {
      uint8_t x;
      uint8_t data[] = {bitwise::high(reg), bitwise::low(reg)};
      wire->write(data, 2, addr);
      wire->read(&x, 1, addr);
      return x;
    }

    void GT911::writeBlockData(uint16_t reg, uint8_t *data, uint8_t length)
    {
      uint8_t send[] = {bitwise::high(reg), bitwise::low(reg)};
      wire->write(send, 2, addr);  
      wire->cmdStart();
      wire->cmdWrite(addr<<1|I2C_MASTER_WRITE, true);
      wire->cmdWrite(bitwise::high(reg), true);
      wire->cmdWrite(bitwise::low(reg), true);
      wire->cmdWrite(data, length, true);
      wire->cmdStop();
      wire->cmdTransmit();
    }

    void GT911::readBlockData(uint16_t reg, uint8_t *buffer, uint8_t length)
    {
      wire->read(buffer, length, addr);
    }

    Point::Point(void)
    {
      this->id = this->x = this->y = this->length = 0;
    }

    Point::Point(uint8_t id, uint16_t x, uint16_t y, uint16_t length)
    {
      this->id = id;
      this->x = x;
      this->y = y;
      this->length = length;
    }

    bool Point::operator==(Point point)
    {
      return ((point.x == this->x) && (point.y == this->y) && (point.length == this->length));
    }

    bool Point::operator!=(Point point)
    {
      return ((point.x != this->x) || (point.y != this->y) || (point.length != this->length));
    }

  } // namespace gt911

} // namespace hardware
