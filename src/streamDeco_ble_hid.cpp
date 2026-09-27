#include "streamDeco_ble_hid.hpp"

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <NimBLEHIDDevice.h>

#include <HIDTypes.h>

namespace blekeyboard
{

#define KEYBOARD_ID 0x01
#define MEDIA_KEYS_ID 0x02

static const uint8_t hidReportDescriptor[] = {
    USAGE_PAGE(1), 0x01,
    USAGE(1), 0x06,
    COLLECTION(1), 0x01,
    REPORT_ID(1), KEYBOARD_ID,
    USAGE_PAGE(1), 0x07,
    USAGE_MINIMUM(1), 0xE0,
    USAGE_MAXIMUM(1), 0xE7,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01,
    REPORT_SIZE(1), 0x01,
    REPORT_COUNT(1), 0x08,
    HIDINPUT(1), 0x02,
    REPORT_COUNT(1), 0x01,
    REPORT_SIZE(1), 0x08,
    HIDINPUT(1), 0x01,
    REPORT_COUNT(1), 0x05,
    REPORT_SIZE(1), 0x01,
    USAGE_PAGE(1), 0x08,
    USAGE_MINIMUM(1), 0x01,
    USAGE_MAXIMUM(1), 0x05,
    HIDOUTPUT(1), 0x02,
    REPORT_COUNT(1), 0x01,
    REPORT_SIZE(1), 0x03,
    HIDOUTPUT(1), 0x01,
    REPORT_COUNT(1), 0x06,
    REPORT_SIZE(1), 0x08,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x65,
    USAGE_PAGE(1), 0x07,
    USAGE_MINIMUM(1), 0x00,
    USAGE_MAXIMUM(1), 0x65,
    HIDINPUT(1), 0x00,
    END_COLLECTION(0),

    USAGE_PAGE(1), 0x0C,
    USAGE(1), 0x01,
    COLLECTION(1), 0x01,
    REPORT_ID(1), MEDIA_KEYS_ID,
    USAGE_PAGE(1), 0x0C,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01,
    REPORT_SIZE(1), 0x01,
    REPORT_COUNT(1), 0x10,
    USAGE(1), 0xB5,
    USAGE(1), 0xB6,
    USAGE(1), 0xB7,
    USAGE(1), 0xCD,
    USAGE(1), 0xE2,
    USAGE(1), 0xE9,
    USAGE(1), 0xEA,
    USAGE(2), 0x23, 0x02,
    USAGE(2), 0x94, 0x01,
    USAGE(2), 0x92, 0x01,
    USAGE(2), 0x2A, 0x02,
    USAGE(2), 0x21, 0x02,
    USAGE(2), 0x26, 0x02,
    USAGE(2), 0x24, 0x02,
    USAGE(2), 0x83, 0x01,
    USAGE(2), 0x8A, 0x01,
    HIDINPUT(1), 0x02,
    END_COLLECTION(0)};

Device::Device(const char *deviceName, const char *manufacturer, uint8_t batteryLevel)
    : hid_(nullptr),
      inputKeyboard_(nullptr),
      outputKeyboard_(nullptr),
      inputMedia_(nullptr),
      advertising_(nullptr),
      keyReport_{0, 0, {0, 0, 0, 0, 0, 0}},
      mediaReport_{0, 0},
      deviceName_(deviceName),
      manufacturer_(manufacturer),
      batteryLevel_(batteryLevel),
      connected_(false) {}

void Device::begin() {
  NimBLEDevice::init(deviceName_.c_str());

  NimBLEServer *server = NimBLEDevice::createServer();
  server->setCallbacks(this);

  hid_ = new NimBLEHIDDevice(server);
  inputKeyboard_ = hid_->inputReport(KEYBOARD_ID);
  outputKeyboard_ = hid_->outputReport(KEYBOARD_ID);
  inputMedia_ = hid_->inputReport(MEDIA_KEYS_ID);

  outputKeyboard_->setCallbacks(this);

  hid_->manufacturer()->setValue(manufacturer_.c_str());
  hid_->pnp(0x02, 0x05AC, 0x820A, 0x0210);
  hid_->hidInfo(0x00, 0x01);
  NimBLEDevice::setSecurityAuth(true, true, true);
  hid_->reportMap((uint8_t *)hidReportDescriptor, sizeof(hidReportDescriptor));
  hid_->startServices();

  advertising_ = server->getAdvertising();
  advertising_->setAppearance(HID_KEYBOARD);
  advertising_->addServiceUUID(hid_->hidService()->getUUID());
  advertising_->setScanResponse(false);
  advertising_->start();

  hid_->setBatteryLevel(batteryLevel_);
}

bool Device::isConnected() const { return connected_; }

void Device::sendKeyboardReport() {
  if (!connected_ || inputKeyboard_ == nullptr) {
    return;
  }
  inputKeyboard_->setValue((uint8_t *)&keyReport_, sizeof(key::KeyReport));
  inputKeyboard_->notify();
  vTaskDelay(7 / portTICK_PERIOD_MS);
}

void Device::sendMediaReport() {
  if (!connected_ || inputMedia_ == nullptr) {
    return;
  }
  inputMedia_->setValue((uint8_t *)&mediaReport_, sizeof(key::MediaKeyReport));
  inputMedia_->notify();
  vTaskDelay(7 / portTICK_PERIOD_MS);
}

Device::HidCode Device::asciiToHid(uint8_t ascii) {
  if (ascii >= 'a' && ascii <= 'z') {
    return {(uint8_t)(0x04 + (ascii - 'a')), false, true};
  }
  if (ascii >= 'A' && ascii <= 'Z') {
    return {(uint8_t)(0x04 + (ascii - 'A')), true, true};
  }
  if (ascii >= '1' && ascii <= '9') {
    return {(uint8_t)(0x1E + (ascii - '1')), false, true};
  }
  if (ascii == '0') {
    return {0x27, false, true};
  }

  switch (ascii) {
  case ' ':
    return {0x2C, false, true};
  case '-':
    return {0x2D, false, true};
  case '_':
    return {0x2D, true, true};
  case '=':
    return {0x2E, false, true};
  case '+':
    return {0x2E, true, true};
  case ',':
    return {0x36, false, true};
  case '.':
    return {0x37, false, true};
  case '/':
    return {0x38, false, true};
  default:
    return {0x00, false, false};
  }
}

size_t Device::press(uint8_t key) {
  uint8_t hidKey = 0;
  bool useShift = false;

  if (key >= 136) {
    hidKey = key - 136;
  } else if (key >= 128) {
    keyReport_.modifiers |= (1 << (key - 128));
  } else {
    HidCode mapped = asciiToHid(key);
    if (!mapped.ok) {
      return 0;
    }
    hidKey = mapped.keycode;
    useShift = mapped.useShift;
  }

  if (useShift) {
    keyReport_.modifiers |= 0x02;
  }

  if (hidKey != 0) {
    for (uint8_t i = 0; i < 6; ++i) {
      if (keyReport_.keys[i] == hidKey) {
        sendKeyboardReport();
        return 1;
      }
    }

    for (uint8_t i = 0; i < 6; ++i) {
      if (keyReport_.keys[i] == 0) {
        keyReport_.keys[i] = hidKey;
        sendKeyboardReport();
        return 1;
      }
    }
    return 0;
  }

  sendKeyboardReport();
  return 1;
}

size_t Device::write(uint8_t key) {
  size_t wrote = press(key);
  releaseAll();
  return wrote;
}

size_t Device::press(const key::MediaKeyReport key) {
  uint16_t newKeys = (uint16_t)key[1] | ((uint16_t)key[0] << 8);
  uint16_t current = (uint16_t)mediaReport_[1] | ((uint16_t)mediaReport_[0] << 8);

  current |= newKeys;
  mediaReport_[0] = (uint8_t)((current & 0xFF00) >> 8);
  mediaReport_[1] = (uint8_t)(current & 0x00FF);

  sendMediaReport();
  return 1;
}

size_t Device::write(const key::MediaKeyReport key) {
  press(key);
  mediaReport_[0] = 0;
  mediaReport_[1] = 0;
  sendMediaReport();
  return 1;
}

void Device::releaseAll() {
  for (uint8_t i = 0; i < 6; ++i) {
    keyReport_.keys[i] = 0;
  }
  keyReport_.modifiers = 0;
  mediaReport_[0] = 0;
  mediaReport_[1] = 0;

  sendKeyboardReport();
  sendMediaReport();
}

void Device::onConnect(NimBLEServer *server) {
  (void)server;
  connected_ = true;
}

void Device::onDisconnect(NimBLEServer *server) {
  (void)server;
  connected_ = false;

  if (advertising_ != nullptr) {
    advertising_->start();
  }
}

void Device::onWrite(NimBLECharacteristic *characteristic) { (void)characteristic; }

} // namespace blekeyboard
