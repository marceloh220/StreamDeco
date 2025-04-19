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

/**
 * @namespace esp
 * @brief     Miscellaneous System APIs functions
 */
namespace esp
{

	/**
	 * @namespace system
	 * @brief     System functions
	 */
	namespace system
	{

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
		 *          After successful restart, CPU reset reason will be SW_RESET.
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

		/**
		 * @brief   Return full IDF version string, same as 'git describe' output.
		 * @note    If you are printing the ESP-IDF version in a log file or other information,
		 * this function provides more information than using the numerical version macros.
		 * For example, numerical version macros don't differentiate between development,
		 * pre-release and release versions, but the output of this function does.
		 * @return  constant string from IDF_VER
		 *          Major version number (X.x.x)
		 *          Minor version number (x.X.x)
		 *          Patch version number (x.x.X)
		 */
		const char *get_idf_version();

	} // namespace system

	/**
	 * @namespace chip
	 * @brief     Chip informations functions
	 */
	namespace chip
	{

		/**
		 * @brief   Fill an esp_chip_info_t structure with information about the chip.
		 * @return  esp_chip_info_t structure to filled with chip infos
		 *          esp_chip_model_t model - chip model, one of esp_chip_model_t
		 *          uint32_t features - bit mask of CHIP_FEATURE_x feature flags
		 *          uint16_t revision - chip revision number (in format MXX; where M - wafer major version, XX - wafer minor version)
		 *          uint8_t cores - number of CPU cores
		 */
		esp_chip_info_t info();

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
		uint32_t features();

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
		esp_chip_model_t model();

		/**
		 * @brief   Get chip revision number
		 * @return  Chip revision number (in format MXX; where M - wafer major version, XX - wafer minor version)
		 */
		uint8_t revision();

		/**
		 * @brief   Get number of cores embedded in chip
		 * @return  Chip number of CPU cores
		 */
		uint8_t cores();

		/**
		 * @brief   Set base MAC address with the MAC address which is stored in BLK3 of EFUSE or external storage e.g. flash and EEPROM.
		 * @details Base MAC address is used to generate the MAC addresses used by network interfaces.
		 * If using a custom base MAC address, call this API before initializing any network interfaces.
		 * Refer to the ESP-IDF Programming Guide for details about how the Base MAC is used.
		 * @note    Base MAC must be a unicast MAC (least significant bit of first byte must be zero).
		 * If not using a valid OUI, set the "locally administered" bit (bit value 0x02 in the first byte) to avoid collisions.
		 * @param   mac -- base MAC address, length: 6 bytes. length: 6 bytes for MAC-48
		 * @return  ESP_OK on success ESP_ERR_INVALID_ARG If mac is nullptr or is not a unicast MAC
		 */
		esp_err_t base_mac_addr_set(const uint8_t *mac);

		/**
		 * @brief   Return base MAC address which is set using esp_base_mac_addr_set.
		 * @note    If no custom Base MAC has been set, this returns the pre-programmed Espressif base MAC address.
		 * @param   mac -- base MAC address, length: 6 bytes. length: 6 bytes for MAC-48
		 * @return  ESP_OK               on success
		 *          ESP_ERR_INVALID_ARG  mac is nullptr
		 *          ESP_ERR_INVALID_MAC  base MAC address has not been set
		 */
		esp_err_t base_mac_addr_get(uint8_t *mac);

		/**
		 * @brief   Return base MAC address which is factory-programmed by Espressif in EFUSE.
		 * @param   mac -- base MAC address, length: 6 bytes/8 bytes.
		 *          length: 6 bytes for MAC-48 8 bytes for EUI-64(used for IEEE 802.15.4, if CONFIG_SOC_IEEE802154_SUPPORTED=y)
		 * @return  ESP_OK on success
		 *          ESP_ERR_INVALID_ARG mac is nullptr
		 */
		esp_err_t efuse_mac_get_default(uint8_t *mac);

		/**
		 * @brief   Read base MAC address and set MAC address of the interface.
		 * @details This function first get base MAC address using esp_base_mac_addr_get().
		 * Then calculates the MAC address of the specific interface requested, refer to ESP-IDF Programming Guide for the algorithm.
		 * The MAC address set by the esp_iface_mac_addr_set() function will not depend on the base MAC address.
		 * @param   mac -- base MAC address, length: 6 bytes/8 bytes.
		 *          length: 6 bytes for MAC-48 8 bytes for EUI-64(used for IEEE 802.15.4, if CONFIG_SOC_IEEE802154_SUPPORTED=y)
		 * @param   type -- Type of MAC address to return
		 *          ESP_MAC_WIFI_STA      - MAC for WiFi Station (6 bytes)
		 *          ESP_MAC_WIFI_SOFTAP   - MAC for WiFi Soft-AP (6 bytes)
		 *          ESP_MAC_BT            - MAC for Bluetooth (6 bytes)
		 *          ESP_MAC_ETH           - MAC for Ethernet (6 bytes)
		 *          ESP_MAC_IEEE802154    - if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC for IEEE802154 (8 bytes)
		 *          ESP_MAC_BASE          - Base MAC for that used for other MAC types (6 bytes)
		 *          ESP_MAC_EFUSE_FACTORY - MAC_FACTORY eFuse which was burned by Espressif in production (6 bytes)
		 *          ESP_MAC_EFUSE_CUSTOM  - MAC_CUSTOM eFuse which was can be burned by customer (6 bytes)
		 *          ESP_MAC_EFUSE_EXT     - if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC_EXT eFuse which is used as an extender for IEEE802154 MAC (2 bytes)
		 * @return  ESP_OK on success
		 */
		esp_err_t read_mac(uint8_t *mac, esp_mac_type_t type);

		/**
		 * @brief   Derive local MAC address from universal MAC address.
		 * @details This function copies a universal MAC address and then sets the "locally
		 * administered" bit (bit 0x2) in the first octet, creating a locally administered MAC address.
		 * If the universal MAC address argument is already a locally administered MAC address,
		 * then the first octet is XORed with 0x4 in order to create a different locally administered MAC address.
		 * @param   local_mac -- base MAC address, length: 6 bytes. length: 6 bytes for MAC-48
		 * @param   universal_mac -- Source universal MAC address, length: 6 bytes.
		 * @return  ESP_OK on success
		 */
		esp_err_t derive_local_mac(uint8_t *local_mac, const uint8_t *universal_mac);

#ifndef ARDUINO_ARCH_ESP32

		/**
		 * @brief   Set custom MAC address of the interface.
		 * @details This function allows you to overwrite the MAC addresses of the interfaces set by the base MAC address.
		 * @param   mac -- MAC address, length: 6 bytes/8 bytes.
		 *          length: 6 bytes for MAC-48 8 bytes for EUI-64(used for ESP_MAC_IEEE802154 type, if CONFIG_SOC_IEEE802154_SUPPORTED=y)
		 * @param   type -- Type of MAC address
		 *          ESP_MAC_WIFI_STA      - MAC for WiFi Station (6 bytes)
		 *          ESP_MAC_WIFI_SOFTAP   - MAC for WiFi Soft-AP (6 bytes)
		 *          ESP_MAC_BT            - MAC for Bluetooth (6 bytes)
		 *          ESP_MAC_ETH           - MAC for Ethernet (6 bytes)
		 *          ESP_MAC_IEEE802154    - if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC for IEEE802154 (8 bytes)
		 *          ESP_MAC_BASE          - Base MAC for that used for other MAC types (6 bytes)
		 *          ESP_MAC_EFUSE_FACTORY - MAC_FACTORY eFuse which was burned by Espressif in production (6 bytes)
		 *          ESP_MAC_EFUSE_CUSTOM  - MAC_CUSTOM eFuse which was can be burned by customer (6 bytes)
		 *          ESP_MAC_EFUSE_EXT     - if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC_EXT eFuse which is used as an extender for IEEE802154 MAC (2 bytes)
		 * @return  ESP_OK on success
		 */
		esp_err_t iface_mac_addr_set(const uint8_t *mac, esp_mac_type_t type);

		/**
		 * @brief   Return the size of the MAC type in bytes.
		 * @details If CONFIG_SOC_IEEE802154_SUPPORTED is set then for these types:
		 *          ESP_MAC_IEEE802154 is 8 bytes.
		 *          ESP_MAC_BASE, ESP_MAC_EFUSE_FACTORY and ESP_MAC_EFUSE_CUSTOM the MAC size is 6 bytes.
		 *          ESP_MAC_EFUSE_EXT is 2 bytes. If CONFIG_SOC_IEEE802154_SUPPORTED is not set then for all types it returns 6 bytes.
		 * @param   type -- Type of MAC address
		 *          ESP_MAC_WIFI_STA      - MAC for WiFi Station (6 bytes)
		 *          ESP_MAC_WIFI_SOFTAP   - MAC for WiFi Soft-AP (6 bytes)
		 *          ESP_MAC_BT            - MAC for Bluetooth (6 bytes)
		 *          ESP_MAC_ETH           - MAC for Ethernet (6 bytes)
		 *          ESP_MAC_IEEE802154    - if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC for IEEE802154 (8 bytes)
		 *          ESP_MAC_BASE          - Base MAC for that used for other MAC types (6 bytes)
		 *          ESP_MAC_EFUSE_FACTORY - MAC_FACTORY eFuse which was burned by Espressif in production (6 bytes)
		 *          ESP_MAC_EFUSE_CUSTOM  - MAC_CUSTOM eFuse which was can be burned by customer (6 bytes)
		 *          ESP_MAC_EFUSE_EXT     - if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC_EXT eFuse which is used as an extender for IEEE802154 MAC (2 bytes)
		 * @return  0 MAC type not found (not supported)
		 *          6 bytes for MAC-48
		 *          8 bytes for EUI-64
		 */
		size_t mac_addr_len_get(esp_mac_type_t type);

#endif

	} // namespace chip

	/**
	 * @namespace cpu
	 * @brief     CPU specific functions
	 */
	namespace cpu
	{

		/**
		 * @brief   Stall a CPU core
		 * @param   core_id -- The core's ID
		 */
		void stall(int core_id);

		/**
		 * @brief   Unstall a CPU core
		 * @param   core_id -- The core's ID
		 */
		void unstall(int core_id);

		/**
		 * @brief   Reset a CPU core
		 * @param   core_id -- The core's ID
		 */
		void reset(int core_id);

#ifndef ARDUINO_ARCH_ESP32

		/**
		 * @brief   Wait for Interrupt.
		 * @details This function causes the current CPU core to execute its Wait For Interrupt (WFI or equivalent) instruction.
		 *          After executing this function, the CPU core will stop execution until an interrupt occurs.
		 */
		void wait_for_intr();

		/**
		 * @brief   Get the current core's ID.
		 * @details This function will return the ID of the current CPU (i.e., the CPU that calls this function).
		 * @return  The current core's ID [0..SOC_CORES_NUM - 1]
		 */
		int get_core_id(void);

		/**
		 * @brief   Get the current CPU core's cycle count.
		 * @details Each CPU core maintains an internal counter (i.e., cycle count)
		 *          that increments every CPU clock cycle.
		 * @return  Current CPU's cycle count, 0 if not supported.
		 */
		esp_cycle_count_t get_cycle_count(void);

		/**
		 * @brief   Set the current CPU core's cycle count.
		 * @details Set the given value into the internal counter that increments every CPU clock cycle.
		 * @param   cycle_count -- CPU cycle count
		 */
		void set_cycle_count(esp_cycle_count_t cycle_count);

		/**
		 * @brief   Convert a program counter (PC) value to address.
		 * @details If the architecture does not store the true virtual address in the CPU's PC or return addresses,
		 *          this function will convert the PC value to a virtual address. Otherwise, the PC is just returned
		 * @param   pc -- PC value
		 * @return  Virtual address
		 */
		void *pc_to_addr(uint32_t pc);

		/**
		 * @brief   Get a CPU interrupt's descriptor.
		 * @details Each CPU interrupt has a descriptor describing the interrupt's capabilities and restrictions.
		 *          This function gets the descriptor of a particular interrupt on a particular CPU.
		 * @param   core_id -- [in] The core's ID
		 * @param   intr_num -- [in] Interrupt number
		 * @param   intr_desc_ret -- [out] The interrupt's descriptor
		 */
		void intr_get_desc(int core_id, int intr_num, esp_intr_desc_t *intr_desc_ret);

		/**
		 * @brief   Set the base address of the current CPU's Interrupt Vector Table (IVT)
		 * @param   ivt_addr -- Interrupt Vector Table's base address
		 */
		void intr_set_ivt_addr(const void *ivt_addr);

		/**
		 * @brief   Check if a particular interrupt already has a handler function.
		 * @details Check if a particular interrupt on the current CPU already has a handler function assigned.
		 * @note    This function simply checks if the IVT of the current CPU already has a handler assigned.
		 * @param   intr_num -- Interrupt number (from 0 to 31)
		 * @return  True if the interrupt has a handler function, false otherwise.
		 */
		bool intr_has_handler(int intr_num);

		/**
		 * @brief   Set the handler function of a particular interrupt.
		 * @details Assign a handler function (i.e., ISR) to a particular interrupt on the current CPU.
		 * @note    This function simply sets the handler function (in the IVT) and does not actually enable the interrupt.
		 * @param   intr_num -- Interrupt number (from 0 to 31)
		 * @param   handler -- Handler function
		 * @param   handler_arg -- Argument passed to the handler function
		 */
		void intr_set_handler(int intr_num, esp_intr_handler_t handler, void *handler_arg);

		/**
		 * @brief   Get a handler function's argument of.
		 * @details Get the argument of a previously assigned handler function on the current CPU.
		 * @param   intr_num -- Interrupt number (from 0 to 31)
		 * @return  The the argument passed to the handler function
		 */
		void *intr_get_handler_arg(int intr_num);

		/**
		 * @brief   Enable particular interrupts on the current CPU.
		 * @param   intr_mask -- Bit mask of the interrupts to enable
		 */
		void intr_enable(uint32_t intr_mask);

		/**
		 * @brief   Disable particular interrupts on the current CPU.
		 * @param   intr_mask -- Bit mask of the interrupts to disable
		 */
		void intr_disable(uint32_t intr_mask);

		/**
		 * @brief   Get the enabled interrupts on the current CPU.
		 * @return  Bit mask of the enabled interrupts
		 */
		uint32_t intr_get_enabled_mask();

		/**
		 * @brief   Acknowledge an edge interrupt.
		 * @param   intr_num -- Interrupt number (from 0 to 31)
		 */
		void intr_edge_ack(int intr_num);

		/**
		 * @brief   Configure the CPU to disable access to invalid memory regions.
		 */
		void configure_region_protection();

		/**
		 * @brief   Set and enable a hardware breakpoint on the current CPU.
		 * @details This function is meant to be called by the panic handler to set a breakpoint
		 *          for an attached debugger during a panic.
		 * @note    Overwrites previously set breakpoint with same breakpoint number.
		 * @param   bp_num -- Hardware breakpoint number [0..SOC_BREAKPOINTS_NUM - 1]
		 * @param   bp_addr -- Address to set a breakpoint on
		 * @return  ESP_OK if breakpoint is set. Failure otherwise
		 */
		esp_err_t set_breakpoint(int bp_num, const void *bp_addr);

		/**
		 * @brief   Clear a hardware breakpoint on the current CPU.
		 * @note    Clears a breakpoint regardless of whether it was previously set
		 * @param   bp_num -- Hardware breakpoint number [0..SOC_BREAKPOINTS_NUM - 1]
		 * @return  ESP_OK if breakpoint is cleared. Failure otherwise
		 */
		esp_err_t clear_breakpoint(int bp_num);

		/**
		 * @brief   Set and enable a hardware watchpoint on the current CPU.
		 * @details Set and enable a hardware watchpoint on the current CPU,
		 * specifying the memory range and trigger operation.
		 * Watchpoints will break/panic the CPU when the CPU accesses (according to the trigger type) on a certain memory range.
		 * @note    Overwrites previously set watchpoint with same watchpoint number.
		 * On RISC-V chips, this API uses method0(Exact matching) and method1(NAPOT matching) according to the riscv-debug-spec-0.13 specification for address matching.
		 * If the watch region size is 1byte, it uses exact matching (method 0). If the watch region size is larger than 1byte, it uses NAPOT matching (method 1).
		 * This mode requires the watching region start address to be aligned to the watching region size.
		 * @param   wp_num -- Hardware watchpoint number [0..SOC_WATCHPOINTS_NUM - 1]
		 * @param   wp_addr -- Watchpoint's base address, must be naturally aligned to the size of the region
		 * @param   size -- Size of the region to watch. Must be one of 2^n and in the range of [1 ... SOC_WATCHPOINT_MAX_REGION_SIZE]
		 * @param   trigger -- Trigger type
		 * @return  ESP_ERR_INVALID_ARG on invalid arg, ESP_OK otherwise
		 */
		esp_err_t set_watchpoint(int wp_num, const void *wp_addr, size_t size, esp_watchpoint_trigger_t trigger);

		/**
		 * @brief   Clear a hardware watchpoint on the current CPU.
		 * @note    Clears a watchpoint regardless of whether it was previously set
		 * @param   wp_num -- Hardware watchpoint number [0..SOC_WATCHPOINTS_NUM - 1]
		 * @return  ESP_OK if watchpoint was cleared. Failure otherwise.
		 */
		esp_err_t clear_watchpoint(int wp_num);

		/**
		 * @brief   Check if the current CPU has a debugger attached.
		 * @return  True if debugger is attached, false otherwise
		 */
		bool dbgr_is_attached();

		/**
		 * @brief   Trigger a call to the current CPU's attached debugger.
		 */
		void dbgr_break(void);

		/**
		 * @brief   Given the return address, calculate the address of the preceding call instruction.
		 *          This is typically used to answer the question "where was the function called from?".
		 * @param   return_address -- The value of the return address register. Typically set to the value of __builtin_return_address(0).
		 * @return  Address of the call instruction preceding the return address.
		 */
		intptr_t get_call_addr(intptr_t return_address);

		/**
		 * @brief   Atomic compare-and-set operation.
		 * @param   addr -- Address of atomic variable
		 * @param   compare_value -- Value to compare the atomic variable to
		 * @param   new_value -- New value to set the atomic variable to
		 * @return  Whether the atomic variable was set or not
		 */
		bool compare_and_set(volatile uint32_t *addr, uint32_t compare_value, uint32_t new_value);
#endif

		/**
		 * @brief   Read the current stack pointer address.
		 * @return  Stack pointer address
		 */
		void *get_sp();

	} // namespace cpu

} // namespace chip

#endif