#ifndef __HARDWARE_NVS__
#define __HARDWARE_NVS__
#include "nvs_flash.h"

namespace hardware
{

  namespace flash
  {

    namespace ll
    {

      /**
       * @brief Mode of opening the non-volatile storage
       */
      typedef enum
      {
        READONLY, /*!< Read only */
        READWRITE /*!< Read and write */
      } open_mode_t;

      /**
       * @brief Initialize the default NVS partition.
       *
       * This API initialises the default NVS partition. The default NVS partition
       * is the one that is labeled "nvs" in the partition table.
       *
       * When "NVS_ENCRYPTION" is enabled in the menuconfig, this API enables
       * the NVS encryption for the default NVS partition as follows
       *      1. Read security configurations from the first NVS key
       *         partition listed in the partition table. (NVS key partition is
       *         any "data" type partition which has the subtype value set to "nvs_keys")
       *      2. If the NVS key partiton obtained in the previous step is empty,
       *         generate and store new keys in that NVS key partiton.
       *      3. Internally call "nvs_flash_secure_init()" with
       *         the security configurations obtained/generated in the previous steps.
       *
       * Post initialization NVS read/write APIs
       * remain the same irrespective of NVS encryption.
       *
       * @return
       *      - ESP_OK if storage was successfully initialized.
       *      - ESP_ERR_NVS_NO_FREE_PAGES if the NVS storage contains no empty pages
       *        (which may happen if NVS partition was truncated)
       *      - ESP_ERR_NOT_FOUND if no partition with label "nvs" is found in the partition table
       *      - ESP_ERR_NO_MEM in case memory could not be allocated for the internal structures
       *      - one of the error codes from the underlying flash storage driver
       *      - error codes from nvs_flash_read_security_cfg API (when "NVS_ENCRYPTION" is enabled).
       *      - error codes from nvs_flash_generate_keys API (when "NVS_ENCRYPTION" is enabled).
       *      - error codes from nvs_flash_secure_init_partition API (when "NVS_ENCRYPTION" is enabled) .
       */
      void init();

      /**
       * @brief Deinitialize NVS storage for the default NVS partition
       *
       * Default NVS partition is the partition with "nvs" label in the partition table.
       *
       * @return
       *      - ESP_OK on success (storage was deinitialized)
       *      - ESP_ERR_NVS_NOT_INITIALIZED if the storage was not initialized prior to this call
       */
      esp_err_t deinit();

      /**
       * @brief      Open non-volatile storage with a given namespace from the default NVS partition
       *
       * Multiple internal ESP-IDF and third party application modules can store
       * their key-value pairs in the NVS module. In order to reduce possible
       * conflicts on key names, each module can use its own namespace.
       * The default NVS partition is the one that is labelled "nvs" in the partition
       * table.
       *
       * @param[in]  open_mode   NVS_READWRITE or NVS_READONLY. If NVS_READONLY, will
       *                         open a handle for reading only. All write requests will
       *                         be rejected for this handle.
       *
       * @return
       *             - ESP_OK if storage handle was opened successfully
       *             - ESP_FAIL if there is an internal error; most likely due to corrupted
       *               NVS partition (only if NVS assertion checks are disabled)
       *             - ESP_ERR_NVS_NOT_INITIALIZED if the storage driver is not initialized
       *             - ESP_ERR_NVS_PART_NOT_FOUND if the partition with label "nvs" is not found
       *             - ESP_ERR_NVS_NOT_FOUND id namespace doesn't exist yet and
       *               mode is NVS_READONLY
       *             - ESP_ERR_NVS_INVALID_NAME if namespace name doesn't satisfy constraints
       *             - ESP_ERR_NO_MEM in case memory could not be allocated for the internal structures
       *             - ESP_ERR_NVS_NOT_ENOUGH_SPACE if there is no space for a new entry or there are too many different
       *                                  namespaces (maximum allowed different namespaces: 254)
       *             - other error codes from the underlying storage driver
       */
      esp_err_t open(open_mode_t mode);

      /**
       * @brief      Close the storage handle and free any allocated resources
       *
       * This function should be called for each handle opened with nvs_open once
       * the handle is not in use any more. Closing the handle may not automatically
       * write the changes to nonvolatile storage. This has to be done explicitly using
       * nvs_commit function.
       * Once this function is called on a handle, the handle should no longer be used.
       */
      void close();

      /**
       * @brief      Write any pending changes to non-volatile storage
       *
       * After setting any values, nvs_commit() must be called to ensure changes are written
       * to non-volatile storage. Individual implementations may write to storage at other times,
       * but this is not guaranteed.
       *
       * @return
       *             - ESP_OK if the changes have been written successfully
       *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is nullptr
       *             - other error codes from the underlying storage driver
       */
      esp_err_t commit();

      void addKey();
      void removeKey();

      /**
       * @brief      Erase key-value pair with given key name.
       *
       * Note that actual storage may not be updated until nvs_commit function is called.
       *
       * @param[in]  key     Key name. Maximal length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
       *
       * @return
       *              - ESP_OK if erase operation was successful
       *              - ESP_FAIL if there is an internal error; most likely due to corrupted
       *                NVS partition (only if NVS assertion checks are disabled)
       *              - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is nullptr
       *              - ESP_ERR_NVS_READ_ONLY if handle was opened as read only
       *              - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
       *              - other error codes from the underlying storage driver
       */
      esp_err_t eraseKey(const char *key);

      /**
       * @brief      Erase all key-value pairs in a namespace
       *
       * Note that actual storage may not be updated until nvs_commit function is called.
       *
       * @param[in]  handle  Storage handle obtained with nvs_open.
       *                     Handles that were opened read only cannot be used.
       *
       * @return
       *              - ESP_OK if erase operation was successful
       *              - ESP_FAIL if there is an internal error; most likely due to corrupted
       *                NVS partition (only if NVS assertion checks are disabled)
       *              - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is nullptr
       *              - ESP_ERR_NVS_READ_ONLY if handle was opened as read only
       *              - other error codes from the underlying storage driver
       */
      esp_err_t eraseAllKey();

      uint32_t keysRegistred();

      esp_err_t readU8(const char *key, uint8_t *data, size_t *length);
      esp_err_t readI8(const char *key, int8_t *data, size_t *length);
      esp_err_t readU16(const char *key, uint16_t *data, size_t *length);
      esp_err_t readI16(const char *key, int16_t *data, size_t *length);
      esp_err_t readU32(const char *key, uint32_t *data, size_t *length);
      esp_err_t readI32(const char *key, int32_t *data, size_t *length);
      esp_err_t readU64(const char *key, uint64_t *data, size_t *length);
      esp_err_t readI64(const char *key, int64_t *data, size_t *length);
      esp_err_t readSTR(const char *key, char *data, size_t *length);
      esp_err_t readBlob(const char *key, void *data, size_t *length);

      /**
       * @brief      set int8_t value for given key
       *
       * Set value for the key, given its name. Note that the actual storage will not be updated
       * until \c commit is called.
       *
       * @param[in]  key     Key name. Maximal length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
       * @param[in]  value   The value to set.
       *
       * @return
       *             - ESP_OK if value was set successfully
       *             - ESP_FAIL if there is an internal error; most likely due to corrupted
       *               NVS partition (only if NVS assertion checks are disabled)
       *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is nullptr
       *             - ESP_ERR_NVS_READ_ONLY if storage handle was opened as read only
       *             - ESP_ERR_NVS_INVALID_NAME if key name doesn't satisfy constraints
       *             - ESP_ERR_NVS_NOT_ENOUGH_SPACE if there is not enough space in the
       *               underlying storage to save the value
       *             - ESP_ERR_NVS_REMOVE_FAILED if the value wasn't updated because flash
       *               write operation has failed. The value was written however, and
       *               update will be finished after re-initialization of nvs, provided that
       *               flash operation doesn't fail again.
       */
      esp_err_t write(const char *key, uint8_t *data, size_t length);
      esp_err_t write(const char *key, int8_t *data, size_t length);
      esp_err_t write(const char *key, char *data, size_t length);
      esp_err_t write(const char *key, uint16_t *data, size_t length);
      esp_err_t write(const char *key, int16_t *data, size_t length);
      esp_err_t write(const char *key, uint32_t *data, size_t length);
      esp_err_t write(const char *key, int32_t *data, size_t length);
      esp_err_t write(const char *key, uint64_t *data, size_t length);
      esp_err_t write(const char *key, int64_t *data, size_t length);
      esp_err_t write(const char *key, unsigned long *data, size_t length);
      esp_err_t write(const char *key, long *data, size_t length);
      esp_err_t write(const char *key, void *data, size_t lenght);

    } // namespace ll

  } // namespace flash

} // namespace hardware

#endif