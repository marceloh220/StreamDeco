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
#include "esp_idf_version.h"
#include "esp_chip_info.h"
#include "esp_cpu.h"

#include "esp_mac.h"

#include "rom/spi_flash.h"
#include "esp_spi_flash.h"
#include <memory>
#include <soc/soc.h>

namespace esp {

namespace system {

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

    const char *get_idf_version() {
        return esp_get_idf_version();
    }

} // namespace system

namespace chip {

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

    esp_err_t base_mac_addr_set(const uint8_t *mac) {
        return esp_base_mac_addr_set(mac);
    }

    esp_err_t base_mac_addr_get(uint8_t *mac) {
        return esp_base_mac_addr_get(mac);
    }

    esp_err_t efuse_mac_get_default(uint8_t *mac) {
        return esp_efuse_mac_get_default(mac);
    }

    esp_err_t read_mac(uint8_t *mac, esp_mac_type_t type) {
        return esp_read_mac(mac, type);
    }

    esp_err_t derive_local_mac(uint8_t *local_mac, const uint8_t *universal_mac) {
        return esp_derive_local_mac(local_mac, universal_mac);
    }

#ifndef ARDUINO_ARCH_ESP32

    esp_err_t iface_mac_addr_set(const uint8_t *mac, esp_mac_type_t type) {
        return esp_iface_mac_addr_set(mac, type);
    }

    size_t mac_addr_len_get(esp_mac_type_t type) {
       return esp_mac_addr_len_get(type); 
    }

#endif

} // namespace chip

namespace cpu {
    
    void stall(int core_id) {
        esp_cpu_stall(core_id);
    }

    void unstall(int core_id) {
        esp_cpu_unstall(core_id);
    }

    void reset(int core_id) {
        esp_cpu_reset(core_id);
    }

#ifndef ARDUINO_ARCH_ESP32
    void wait_for_intr() {
        esp_cpu_wait_for_intr();
    }

    int get_core_id(void) {
        esp_cpu_get_core_id();
    }

    esp_cpu_cycle_count_t get_cycle_count(void) {
        return esp_cpu_get_cycle_count();
    }

    void set_cycle_count(esp_cpu_cycle_count_t cycle_count) {
        esp_cpu_set_cycle_count(cycle_count);
    }

    void *pc_to_addr(uint32_t pc) {
        return esp_cpu_pc_to_addr(pc);
    }

    void intr_get_desc(int core_id, int intr_num, esp_cpu_intr_desc_t *intr_desc_ret) {
        esp_cpu_intr_get_desc(core_id, intr_num, intr_desc_ret);
    }

    void intr_set_ivt_addr(const void *ivt_addr) {
        esp_cpu_intr_set_ivt_addr(ivt_addr);
    }

    bool intr_has_handler(int intr_num) {
        return esp_cpu_intr_has_handler(intr_num);
    }

    void intr_set_handler(int intr_num, esp_cpu_intr_handler_t handler, void *handler_arg) {
        esp_cpu_intr_set_handler(intr_num, handler, handler_arg);
    }

    void *intr_get_handler_arg(int intr_num) {
        return esp_cpu_intr_get_handler_arg(int intr_num);
    }

    void intr_enable(uint32_t intr_mask) {
        void esp_cpu_intr_enable(intr_mask);
    }

    void intr_disable(uint32_t intr_mask) {
        void esp_cpu_intr_disable(intr_mask);
    }

    uint32_t intr_get_enabled_mask() {
        return uint32_t esp_cpu_intr_get_enabled_mask();
    }

    void intr_edge_ack(int intr_num) {
        esp_cpu_intr_edge_ack(intr_num);
    }

    void configure_region_protection(void) {
        esp_cpu_configure_region_protection();
    }

    esp_err_t set_breakpoint(int bp_num, const void *bp_addr) {
        return esp_cpu_set_breakpoint(bp_num, bp_addr);
    }

    esp_err_t clear_breakpoint(int bp_num) { 
        return esp_cpu_clear_breakpoint(bp_num);
    }
    
    esp_err_t set_watchpoint(int wp_num, const void *wp_addr, size_t size, esp_cpu_watchpoint_trigger_t trigger) {
        return esp_cpu_set_watchpoint(wp_num, wp_addr, size, trigger);
    }

    esp_err_tclear_watchpoint(int wp_num) {
        return esp_cpu_clear_watchpoint(wp_num);
    }

    bool dbgr_is_attached() {
        return esp_cpu_dbgr_is_attached();
    }

    void dbgr_break() {
        esp_cpu_dbgr_break();
    }

    intptr_t get_call_addr(intptr_t return_address) {
        return esp_cpu_get_call_addr(return_address);
    }

    bool compare_and_set(volatile uint32_t *addr, uint32_t compare_value, uint32_t new_value) {
        return esp_cpu_compare_and_set(addr, compare_value, new_value);
    }
#endif

    void *get_sp(void) {
        return esp_cpu_get_sp();
    }

} // namespace cpu

} // namespace chip
