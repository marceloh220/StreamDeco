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

#include "marcelino/hardware_esp.hpp"

#include "esp32-hal.h"
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_cpu.h"

#include "esp_mac.h"

#include "rom/spi_flash.h"
#include "esp_spi_flash.h"
#include <memory>
#include <soc/soc.h>

namespace esp
{

    typedef struct {
        uint8_t magic;
        uint8_t segment_count;
        uint8_t spi_mode;      /* flash read mode (esp_image_spi_mode_t as uint8_t) */
        uint8_t spi_speed: 4;  /* flash frequency (esp_image_spi_freq_t as uint8_t) */
        uint8_t spi_size: 4;   /* flash chip size (esp_image_flash_size_t as uint8_t) */
        uint32_t entry_addr;
        uint8_t encrypt_flag;    /* encrypt flag */
        uint8_t extra_header[15]; /* ESP32 additional header, unused by second bootloader */
    } esp_image_header_t;

    #define ESP_IMAGE_HEADER_MAGIC 0xE9

    FlashSize_t magicFlashChipSize(uint8_t byte);
    FlashSpeed_t magicFlashChipSpeed(uint8_t byte);
    FlashMode_t magicFlashChipMode(uint8_t byte);

    esp_err_t register_shutdown_handler(shutdown_handler_t handle) {
        return esp_register_shutdown_handler(handle);
    }

    esp_err_t unregister_shutdown_handler(shutdown_handler_t handle) {
        return esp_unregister_shutdown_handler(handle);
    }

    void reset() {
        esp_restart();
    }

    esp_reset_reason_t reset_reason() {
        return esp_reset_reason();
    }
    
    uint32_t freeHeap() {
        return esp_get_free_heap_size();
    }

    uint32_t free_internal_heap_size() {
        return esp_get_free_internal_heap_size();
    }

    uint32_t minimum_free_heap_size(void) {
        return esp_get_minimum_free_heap_size();
    }

    void system_abort(const char *details) {
        esp_system_abort(details);
    }

    esp_chip_info_t chipInfo() { 
        esp_chip_info_t info;
        esp_chip_info(&info);
        return info;
    }

    uint32_t chipFeatures() {
        esp_chip_info_t info;
        esp_chip_info(&info);
        return info.features;
    }

    esp_chip_model_t chipModel(){ 
        esp_chip_info_t info = chipInfo();
        return info.model;
    }

    uint8_t chipRevision(){ 
        esp_chip_info_t info = chipInfo();
        return info.revision;
    }

    uint8_t chipCores(){ 
        esp_chip_info_t info = chipInfo();
        return info.cores;
    }

    void cpu_stall(int core_id) {
        esp_cpu_stall(core_id);
    }

    void cpu_unstall(int core_id) {
        esp_cpu_unstall(core_id);
    }

    void cpu_reset(int core_id) {
        esp_cpu_reset(core_id);
    }

#ifndef ARDUINO_ARCH_ESP32
    void cpu_wait_for_intr() {
        esp_cpu_wait_for_intr();
    }

    int cpu_get_core_id(void) {
        esp_cpu_get_core_id();
    }
#endif

    uint32_t getCycleCount() {
        uint32_t ccount;
        __asm__ __volatile__("esync; rsr %0,ccount":"=a" (ccount));
        return ccount;
    }

    const char * getSdkVersion() {
        return esp_get_idf_version();
    }

    void deepSleep(uint32_t time_us) {
        esp_deep_sleep(time_us);
    }

    FlashSize_t getFlashChipSize() {
        esp_image_header_t fhdr;
        if(flashRead(0x1000, (uint32_t*)&fhdr, sizeof(esp_image_header_t)) && fhdr.magic != ESP_IMAGE_HEADER_MAGIC) {
            return FS_SIZE_FAIL;
        }
        return magicFlashChipSize(fhdr.spi_size);
    }

    uint32_t getFlashChipSpeed() {
        esp_image_header_t fhdr;
        if(flashRead(0x1000, (uint32_t*)&fhdr, sizeof(esp_image_header_t)) && fhdr.magic != ESP_IMAGE_HEADER_MAGIC) {
            return 0;
        }
        return magicFlashChipSpeed(fhdr.spi_speed);
    }

    FlashMode_t getFlashChipMode() {
        esp_image_header_t fhdr;
        if(flashRead(0x1000, (uint32_t*)&fhdr, sizeof(esp_image_header_t)) && fhdr.magic != ESP_IMAGE_HEADER_MAGIC) {
            return FM_UNKNOWN;
        }
        return magicFlashChipMode(fhdr.spi_mode);
    }

    FlashSize_t magicFlashChipSize(uint8_t byte) {
        switch(byte & 0x0F) {
            case 0x0: // 8 MBit (1MB)
                return (FS_1_MB);
            case 0x1: // 16 MBit (2MB)
                return (FS_2_MB);
            case 0x2: // 32 MBit (4MB)
                return (FS_4_MB);
            case 0x3: // 64 MBit (8MB)
                return (FS_8_MB);
            case 0x4: // 128 MBit (16MB)
                return (FS_16_MB);
            default: // fail?
                return FS_SIZE_FAIL;
        }
    }

    FlashSpeed_t magicFlashChipSpeed(uint8_t byte) {
        switch(byte & 0x0F) {
            case 0x0: // 40 MHz
                return (FS_40_MHz);
            case 0x1: // 26 MHz
                return (FS_26_MHz);
            case 0x2: // 20 MHz
                return (FS_20_MHz);
            case 0xf: // 80 MHz
                return (FS_80_MHz);
            default: // fail?
                return FS_SPEED_FAIL;
        }
    }

    FlashMode_t magicFlashChipMode(uint8_t byte) {
        FlashMode_t mode = (FlashMode_t)byte;
        if(mode > FM_SLOW_READ) {
            mode = FM_UNKNOWN;
        }
        return mode;
    }

    bool flashEraseSector(uint32_t sector) {
        return spi_flash_erase_sector(sector) == ESP_OK;
    }

    bool flashWrite(uint32_t offset, uint32_t *data, size_t size) {
        return spi_flash_write(offset, (uint32_t*) data, size) == ESP_OK;
    }

    bool flashRead(uint32_t offset, uint32_t *data, size_t size) {
        return spi_flash_read(offset, (uint32_t*) data, size) == ESP_OK;
    }

    uint64_t getEfuseMac() {
        uint64_t _chipmacid = 0;
        //esp_efuse_read_mac((uint8_t*) (&_chipmacid));
        return _chipmacid;
    }

} // namespace chip
