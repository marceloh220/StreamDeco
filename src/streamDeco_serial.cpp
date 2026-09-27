#include "streamDeco_serial.hpp"

#include "driver/uart.h"

#include <cctype>

namespace streamDeco
{
  namespace serial
  {
    namespace
    {
      constexpr uart_port_t kUartPort = UART_NUM_0;
      constexpr int kRxBufferSize = 4096;
      constexpr int kTxBufferSize = 0;

      bool initialized = false;
      std::string rx_buffer;

      void trim_in_place(std::string &text)
      {
        size_t start = 0;
        while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start])))
        {
          ++start;
        }

        size_t end = text.size();
        while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1])))
        {
          --end;
        }

        text = text.substr(start, end - start);
      }
    } // namespace

    void init_console(uint32_t baudrate)
    {
      if (initialized)
      {
        return;
      }

      uart_config_t config = {
          .baud_rate = static_cast<int>(baudrate),
          .data_bits = UART_DATA_8_BITS,
          .parity = UART_PARITY_DISABLE,
          .stop_bits = UART_STOP_BITS_1,
          .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
          .source_clk = UART_SCLK_DEFAULT,
#endif
      };

      uart_param_config(kUartPort, &config);
      uart_set_pin(kUartPort, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
      uart_driver_install(kUartPort, kRxBufferSize, kTxBufferSize, 0, nullptr, 0);

      initialized = true;
    }

    bool read_frame(std::string &frame, char terminator, uint32_t timeout_ms)
    {
      if (!initialized)
      {
        return false;
      }

      uint8_t chunk[128];
      const int read = uart_read_bytes(kUartPort, chunk, sizeof(chunk), pdMS_TO_TICKS(timeout_ms));
      if (read <= 0)
      {
        return false;
      }

      rx_buffer.append(reinterpret_cast<const char *>(chunk), static_cast<size_t>(read));

      const size_t pos = rx_buffer.find(terminator);
      if (pos == std::string::npos)
      {
        return false;
      }

      frame = rx_buffer.substr(0, pos);
      rx_buffer.erase(0, pos + 1);
      trim_in_place(frame);
      return !frame.empty();
    }

  } // namespace serial
} // namespace streamDeco