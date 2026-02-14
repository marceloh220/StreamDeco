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

#include "marcelino.hpp"
#include "streamDeco_init.hpp"
#include "streamDeco_objects.hpp"

#include <iostream>

#define STORAGE_TEST 0

#if STORAGE_TEST
#include "esp_vfs_fat.h"
#include "tinyusb.h"
#include "tusb_msc_storage.h"

#define PARTITION_LABEL "storage"
// USB MSC configuration
#define VENDOR_ID "ESP32S3"
#define PRODUCT_ID "MSC"
#define PRODUCT_REV "1.0"
#endif

void setup()
{

#if STORAGE_TEST
  storage_init();
#else
  Serial.begin(115200);
  while(!Serial) {
    rtos::sleep(100ms);
  }
#endif

  lvgl::port::init();
  streamDeco::init();
}

void loop()
{
#if 0
  static int test_count = 0;

  streamDeco::mutex_serial.take();
  lvgl::port::print_task_memory_usage();
  streamDeco::print_task_memory_usage();
  ESP_LOGI("Test Cycle", "%d", test_count++);
  streamDeco::mutex_serial.give();
#endif

  rtos::sleep(10s);
}

#if STORAGE_TEST
void storage_init()
{
  esp_err_t ret;
  wl_handle_t wl_handle = WL_INVALID_HANDLE;

  ESP_LOGI(TAG, "Mounting FATFS partition '%s'", PARTITION_LABEL);

  // Mount FATFS from flash partition
  const esp_vfs_fat_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files = 5,
      .allocation_unit_size = CONFIG_WL_SECTOR_SIZE};

  ret = esp_vfs_fat_spiflash_mount(PARTITION_LABEL, PARTITION_LABEL, &mount_config, &wl_handle);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(ret));
    return;
  }

  ESP_LOGI(TAG, "FATFS mounted successfully");

  // TinyUSB MSC configuration
  const tinyusb_config_t tusb_cfg = {
      .device_descriptor = NULL,
      .string_descriptor = NULL,
      .external_phy = false};
  ret = tinyusb_driver_install(&tusb_cfg);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "TinyUSB driver install failed: %s", esp_err_to_name(ret));
    return;
  }

  // Configure MSC
  const tinyusb_msc_storage_config_t msc_cfg = {
      .vendor_id = VENDOR_ID,
      .product_id = PRODUCT_ID,
      .product_rev = PRODUCT_REV,
      .wl_handle = wl_handle};

  ret = tinyusb_msc_storage_init(&msc_cfg);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "MSC storage init failed: %s", esp_err_to_name(ret));
    return;
  }

  ESP_LOGI(TAG, "USB Mass Storage ready. Connect to PC via USB.");
}
#endif
