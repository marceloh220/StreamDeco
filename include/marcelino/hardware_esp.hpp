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

#ifndef __HARDWARE_ESP_HPP__
#define __HARDWARE_ESP_HPP__

#include "esp32-hal.h"

namespace esp
{

    typedef enum {
        WDTO_0MS    = 0,   //!< WDTO_0MS
        WDTO_15MS   = 15,  //!< WDTO_15MS
        WDTO_30MS   = 30,  //!< WDTO_30MS
        WDTO_60MS   = 60,  //!< WDTO_60MS
        WDTO_120MS  = 120, //!< WDTO_120MS
        WDTO_250MS  = 250, //!< WDTO_250MS
        WDTO_500MS  = 500, //!< WDTO_500MS
        WDTO_1S     = 1000,//!< WDTO_1S
        WDTO_2S     = 2000,//!< WDTO_2S
        WDTO_4S     = 4000,//!< WDTO_4S
        WDTO_8S     = 8000 //!< WDTO_8S
    } WDTO_t;

    typedef enum {
        FM_QIO = 0x00,
        FM_QOUT = 0x01,
        FM_DIO = 0x02,
        FM_DOUT = 0x03,
        FM_FAST_READ = 0x04,
        FM_SLOW_READ = 0x05,
        FM_UNKNOWN = 0xff
    } FlashMode_t;

    typedef enum {
        FS_1_MB = 0x00,
        FS_2_MB,
        FS_4_MB,
        FS_8_MB,
        FS_16_MB,
        FS_SIZE_FAIL = -1,
    } FlashSize_t;

    typedef enum {
        FS_40_MHz = 0x00,
        FS_26_MHz,
        FS_20_MHz,
        FS_80_MHz,
        FS_SPEED_FAIL = -1,
    } FlashSpeed_t;

    /**
     * @brief   Register shutdown handler.
     * @details This function allows you to register a handler that gets invoked before 
     *          the application is restarted using esp_restart function.
     * @param   handle -- function to execute on restart
     * @return  ESP_OK on success  
     *          ESP_ERR_INVALID_STATE if the handler has already been registered  
     *          ESP_ERR_NO_MEM if no more shutdown handler slots are available
     */
    esp_err_t register_shutdown_handler(shutdown_handler_t handle);

    /**
     * @brief   Unregister shutdown handler.
     * @details This function allows you to unregister a handler which was previously registered using 
     *          register_shutdown_handler function.
     * @param   handle -- function to unregister
     * @return  ESP_OK on success
     *          ESP_ERR_INVALID_STATE if the given handler hasn't been registered before
     */
    esp_err_t unregister_shutdown_handler(shutdown_handler_t handle);

    /**
     * @brief   Restart PRO and APP CPUs
     * @details This function can be called both from PRO and APP CPUs. 
     *          After successful restart, CPU reset reason will be SW_CPU_RESET. 
     *          Peripherals, except for Wi-Fi, BT, UART0, SPI1, and legacy timers, are not reset. 
     */
    void reset();

    /**
     * @brief   Get reason of last reset.
     * @return  ESP_RST_UNKNOWN, the reset reason can not be determined
                ESP_RST_POWERON, the reset due to power-on event
                ESP_RST_EXT, the reset by external pin (not applicable for ESP32)
                ESP_RST_SW, software reset via esp_restart
                ESP_RST_PANIC, software reset due to exception/panic
                ESP_RST_INT_WDT, the reset (software or hardware) due to interrupt watchdog
                ESP_RST_TASK_WDT, the reset due to task watchdog
                ESP_RST_WDT, the reset due to other watchdogs
                ESP_RST_DEEPSLEEP, the reset after exiting deep sleep mode
                ESP_RST_BROWNOUT, brownout reset (software or hardware)
                ESP_RST_SDIO, the reset over SDIO
     */
    esp_reset_reason_t reset_reason();

    /**
     * @brief   Get the size of available heap.
     * @return  Available heap size, in bytes.
     * @note    Note that the returned value may be larger than the maximum contiguous 
     *          block which can be allocated.
     */
    uint32_t freeHeap();

    /**
     * @brief   Get the size of available internal heap.
     * @returns Available internal heap size, in bytes.
     * @note    Note that the returned value may be larger than the maximum contiguous 
     *          block which can be allocated.
     */
    uint32_t free_internal_heap_size();

    /**
     * @brief   Get the size of available internal heap.
     * @returns Minimum free heap ever available.
     */
    uint32_t minimum_free_heap_size();

    /**
     * @brief   Trigger a software abort.
     * @param   details -- Details that will be displayed during panic handling.
     */
    void system_abort(const char *details);

    const char * getSdkVersion();

    /**
     * @brief   Fill an esp_chip_info_t structure with information about the chip.
     * @return  esp_chip_info_t structure to filled with chip infos
     *          esp_chip_model_t model - chip model, one of esp_chip_model_t
     *          uint32_t features - bit mask of CHIP_FEATURE_x feature flags
     *          uint16_t revision - chip revision number (in format MXX; where M - wafer major version, XX - wafer minor version)
     *          uint8_t cores - number of CPU cores
     */
    esp_chip_info_t chipInfo();

    /**
     * @brief   Get a bit mask of CHIP_FEATURE_x feature flags
     * @return  Chip features:
     *          CHIP_FEATURE_EMB_FLASH  - Chip has embedded flash memory
     *          CHIP_FEATURE_WIFI_BGN   - Chip has 2.4GHz WiFi
     *          CHIP_FEATURE_BLE        - Chip has Bluetooth LE
     *          CHIP_FEATURE_BT         - Chip has Bluetooth Classic
     *          CHIP_FEATURE_IEEE802154 - Chip has IEEE 802.15.4
     *          CHIP_FEATURE_EMB_PSRAM  - Chip has embedded psram
     */
    uint32_t chipFeatures();

    /**
     * @brief   Get chip model
     * @return  Chip model:
     *          CHIP_ESP32   - ESP32
     *          CHIP_ESP32S2 - ESP32-S2
     *          CHIP_ESP32S3 - ESP32-S3
     *          CHIP_ESP32C3 - ESP32-C3
     *          CHIP_ESP32C2 - ESP32-C2
     *          CHIP_ESP32C6 - ESP32-C6
     *          CHIP_ESP32H2 - ESP32-H2
     *          CHIP_ESP32P4  - ESP32-P4
     *          CHIP_POSIX_LINUX - The code is running on POSIX/Linux simulator
     */
    esp_chip_model_t chipModel();

    /**
     * @brief   Get chip revision number
     * @return  Chip revision number (in format MXX; where M - wafer major version, XX - wafer minor version)
     */
    uint8_t chipRevision();

    /**
     * @brief   Get number of cores embedded in chip
     * @return  Chip number of CPU cores
     */
    uint8_t chipCores();

    /**
     * @brief   Stall a CPU core
     * @param   core_id -- The core's ID
     */
    void cpu_stall(int core_id);

    /**
     * @brief   Unstall a CPU core
     * @param   core_id -- The core's ID
     */
    void cpu_unstall(int core_id);

    /**
     * @brief   Reset a CPU core
     * @param   core_id -- The core's ID
     */
    void cpu_reset(int core_id);

#ifndef ARDUINO_ARCH_ESP32

    /**
     * @brief   Wait for Interrupt.
     * @details This function causes the current CPU core to execute its Wait For Interrupt (WFI or equivalent) instruction.  
     *          After executing this function, the CPU core will stop execution until an interrupt occurs.
     */
    void cpu_wait_for_intr();

    /**
     * @brief   Get the current core's ID.
     * @details This function will return the ID of the current CPU (i.e., the CPU that calls this function).
     * @returns The current core's ID [0..SOC_CPU_CORES_NUM - 1]
     */
    int cpu_get_core_id(void);

#endif

    uint32_t getCycleCount();
    
    void deepSleep(uint32_t time_us);
    FlashSize_t getFlashChipSize();
    uint32_t getFlashChipSpeed();
    FlashMode_t getFlashChipMode();
    bool flashEraseSector(uint32_t sector);
    bool flashWrite(uint32_t offset, uint32_t *data, size_t size);
    bool flashRead(uint32_t offset, uint32_t *data, size_t size);
    uint64_t getEfuseMac();

} // namespace chip


#endif