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

#include "hardware_file.hpp"
#include "rtos_mutex_static.hpp"

namespace hardware
{

  namespace flash
  {

    namespace ll
    {

      const char *space = "SDA1";
      nvs_handle handle;
      uint32_t keys = 0;

      rtos::MutexRecursiveStatic mutex_nvs;

      void init()
      {
        mutex_nvs.take();
        esp_err_t err = nvs_flash_init();
        mutex_nvs.give();
        if (err != ESP_OK)
          printf("[NVFS]    Erro in initiate of space %s  NVS: %s\n", space, esp_err_to_name(err));
        else
          printf("[NVFS]    Space %s NVS inicialited\n", space);
      }

      esp_err_t deinit()
      {
        mutex_nvs.take();
        esp_err_t error = nvs_flash_deinit();
        mutex_nvs.give();
        return error;
      }

      esp_err_t open(open_mode_t mode)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_open(space, (nvs_open_mode_t)mode, &handle);
        mutex_nvs.give();
        return error;
      }

      void close() { nvs_close(handle); }

      esp_err_t commit()
      {
        mutex_nvs.take();
        esp_err_t error = nvs_commit(handle);
        mutex_nvs.give();
        return error;
      }

      void addKey()
      {
        if (keys < 0xFFFFFFFF)
          keys++;
      }

      void removeKey()
      {
        if (keys > 0)
          keys--;
      }

      esp_err_t eraseKey(const char *key)
      {
        if (keys > 0)
          keys--;
        mutex_nvs.take();
        esp_err_t error = nvs_erase_key(ll::handle, key);
        mutex_nvs.give();
        return error;
      }

      esp_err_t eraseAllKey()
      {
        keys = 0;
        mutex_nvs.take();
        esp_err_t error = nvs_erase_all(handle);
        mutex_nvs.give();
        return error;
      }

      uint32_t keysRegistred() { return keys; }

      esp_err_t readU8(const char *key, uint8_t *data, size_t *length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_get_u8(handle, key, data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t readI8(const char *key, int8_t *data, size_t *length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_get_i8(handle, key, data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t readU16(const char *key, uint16_t *data, size_t *length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_get_u16(handle, key, data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t readI16(const char *key, int16_t *data, size_t *length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_get_i16(handle, key, data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t readU32(const char *key, uint32_t *data, size_t *length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_get_u32(handle, key, data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t readI32(const char *key, int32_t *data, size_t *length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_get_i32(handle, key, data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t readU64(const char *key, uint64_t *data, size_t *length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_get_u64(handle, key, data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t readI64(const char *key, int64_t *data, size_t *length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_get_i64(handle, key, data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t readstr(const char *key, char *data, size_t *length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_get_str(handle, key, data, length);
        mutex_nvs.give();
        return error;
      }

      esp_err_t readBlob(const char *key, void *data, size_t *length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_get_blob(handle, key, data, length);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, uint8_t *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_u8(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, int8_t *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_i8(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, char *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_i8(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, uint16_t *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_u16(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, int16_t *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_i16(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, uint32_t *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_u32(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, int32_t *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_u32(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, uint64_t *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_u64(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, int64_t *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_i64(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, unsigned long *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_u32(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, long *data, size_t length)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_i32(handle, key, *data);
        mutex_nvs.give();
        return error;
      }

      esp_err_t write(const char *key, void *data, size_t lenght)
      {
        mutex_nvs.take();
        esp_err_t error = nvs_set_blob(handle, key, data, lenght);
        mutex_nvs.give();
        return error;
      }

    } // namespace ll

  } // namespace flash

} // namespace hardware
