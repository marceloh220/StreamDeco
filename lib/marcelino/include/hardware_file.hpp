#ifndef __HARDWARE_FILE_HPP__
#define __HARDWARE_FILE_HPP__

#include "hardware_nvs.hpp"

namespace marcelino
{

  /**
   * @sa      hardware::Button
   * @class   File
   * @brief   Class template for read and write data on flash memory
   * @code
   * #include <marcelino.hpp>
   *
   * marcelino::File<int> flash_int_data("Int into flash");
   * hardware::Button button(board::BUTTON_R, 100ms);
   *
   * extern "C" main() {
   *
   *  int counter = flash_int_data;
   *
   *  printf("Counter value: %d", counter);
   *
   *  while(1) {
   *
   *      if(button) {
   *          printf("Counter updated: %d", ++counter);
   *          if(counter == 10) counter = 0;
   *          flash_int_data = counter;
   *      }
   *
   *      rtos::sleep(10ms);
   *
   *  }
   *
   * }
   */
  template <typename T>
  class File
  {

  public:
    File(const char *_name) : _name(_name)
    {
      _lenght = sizeof(T);
    }

    ~File()
    {
      hardware::flash::ll::removeKey();
      if (hardware::flash::ll::keysRegistred() == 0)
        hardware::flash::ll::deinit();
    }

    /**
     * @brief   Get file name
     * @return  File name in const char*
     */
    const char *name() { return this->_name; }

    /**
     * @brief  Erase file data
     */
    void erase()
    {
      if (_name == nullptr)
        return;
      init();
      hardware::flash::ll::eraseKey(_name);
    }

    /**
     * @brief   Read data from file
     * @return  File's data
     */
    [[nodiscard]] T read()
    {
      if (_name == nullptr)
        return _data = {0};
      init();
      hardware::flash::ll::open(hardware::flash::ll::READONLY);
      esp_err_t error = hardware::flash::ll::readU8(_name, (uint8_t *)&_data, &_lenght);
      if (error == ESP_OK)
      {
        hardware::flash::ll::close();
        return _data;
      }
      error = hardware::flash::ll::readI8(_name, (int8_t *)&_data, &_lenght);
      if (error == ESP_OK)
      {
        hardware::flash::ll::close();
        return _data;
      }
      error = hardware::flash::ll::readU16(_name, (uint16_t *)&_data, &_lenght);
      if (error == ESP_OK)
      {
        hardware::flash::ll::close();
        return _data;
      }
      error = hardware::flash::ll::readI16(_name, (int16_t *)&_data, &_lenght);
      if (error == ESP_OK)
      {
        hardware::flash::ll::close();
        return _data;
      }
      error = hardware::flash::ll::readU32(_name, (uint32_t *)&_data, &_lenght);
      if (error == ESP_OK)
      {
        hardware::flash::ll::close();
        return _data;
      }
      error = hardware::flash::ll::readI32(_name, (int32_t *)&_data, &_lenght);
      if (error == ESP_OK)
      {
        hardware::flash::ll::close();
        return _data;
      }
      error = hardware::flash::ll::readU64(_name, (uint64_t *)&_data, &_lenght);
      if (error == ESP_OK)
      {
        hardware::flash::ll::close();
        return _data;
      }
      error = hardware::flash::ll::readI64(_name, (int64_t *)&_data, &_lenght);
      if (error == ESP_OK)
      {
        hardware::flash::ll::close();
        return _data;
      }
      error = hardware::flash::ll::readBlob(_name, (void *)&_data, &_lenght);
      if (error == ESP_OK)
      {
        hardware::flash::ll::close();
        return _data;
      }
      hardware::flash::ll::open(hardware::flash::ll::READWRITE);
      hardware::flash::ll::write(_name, &_data, _lenght);
      hardware::flash::ll::commit();
      hardware::flash::ll::close();
      return _data;
    }

    /**
     * @brief  Write data on file
     * @param  data   The data to write on file
     */
    void write(T data)
    {
      if (_name == nullptr)
        return;
      init();
      _data = data;
      hardware::flash::ll::open(hardware::flash::ll::READWRITE);
      hardware::flash::ll::write(_name, &_data, _lenght);
      hardware::flash::ll::commit();
      hardware::flash::ll::close();
    }

    /**
     * @brief   Get file size
     * @return  File size in size_t
     */
    [[nodiscard]] inline size_t size() { return _lenght; }

    File<T> &operator=(T data)
    {
      write(data);
      return *this;
    }

    /**
     * @sa     write
     * @brief  Write data on file
     */
    File<T> &operator=(File<T> data)
    {
      write(data.read());
      return *this;
    }

    /**
     * @sa     read
     * @brief  Read data from file
     */
    [[nodiscard]] inline operator File<T>()
    {
      return read();
    }

    /**
     * @sa     read
     * @brief  Read data from file
     */
    [[nodiscard]] inline operator T()
    {
      return read();
    }

  private:
    const char *_name;
    bool _initiated = false;
    T _data;
    size_t _lenght;
    bool _state_initialization = false;

    bool init()
    {
      if (_state_initialization)
        return false;
      _state_initialization = true;
      if (hardware::flash::ll::keysRegistred() == 0)
        hardware::flash::ll::init();
      hardware::flash::ll::addKey();
      return true;
    } // private bool init

  }; // class file

} // namespace marcelino

#endif
