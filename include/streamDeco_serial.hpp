#ifndef _STREAMDECO_SERIAL_HPP_
#define _STREAMDECO_SERIAL_HPP_

#include <cstdint>
#include <string>

namespace streamDeco
{
  namespace serial
  {
    void init_console(uint32_t baudrate = 115200);
    bool read_frame(std::string &frame, char terminator = '/', uint32_t timeout_ms = 10);
  } // namespace serial
} // namespace streamDeco

#endif // _STREAMDECO_SERIAL_HPP_