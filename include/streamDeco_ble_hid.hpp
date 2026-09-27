#ifndef _STREAMDECO_BLE_HID_HPP_
#define _STREAMDECO_BLE_HID_HPP_

#include <cstdint>
#include <string>


#include <NimBLECharacteristic.h>
#include <NimBLEHIDDevice.h>
#include <NimBLEServer.h>

namespace blekeyboard
{

    namespace key {
        const uint8_t LEFT_CTRL = 0x80;
        const uint8_t LEFT_SHIFT = 0x81;
        const uint8_t LEFT_ALT = 0x82;
        const uint8_t LEFT_GUI = 0x83;
        const uint8_t RIGHT_CTRL = 0x84;
        const uint8_t RIGHT_SHIFT = 0x85;
        const uint8_t RIGHT_ALT = 0x86;
        const uint8_t RIGHT_GUI = 0x87;

        const uint8_t LEFT_ARROW = 0xD8;
        const uint8_t RIGHT_ARROW = 0xD7;
        const uint8_t RETURN = 0xB0;
        const uint8_t ESC_KEY = 0xB1;
        const uint8_t PRTSC = 0xCE;
        const uint8_t F2 = 0xC3;

        typedef uint8_t MediaKeyReport[2];

        const MediaKeyReport MEDIA_NEXT_TRACK = {1, 0};
        const MediaKeyReport MEDIA_PREVIOUS_TRACK = {2, 0};
        const MediaKeyReport MEDIA_PLAY_PAUSE = {8, 0};
        const MediaKeyReport MEDIA_MUTE = {16, 0};
        const MediaKeyReport MEDIA_VOLUME_UP = {32, 0};
        const MediaKeyReport MEDIA_VOLUME_DOWN = {64, 0};
        const MediaKeyReport MEDIA_CALCULATOR = {0, 2};

        typedef struct {
            uint8_t modifiers;
            uint8_t reserved;
            uint8_t keys[6];
        } KeyReport;

    } // namespace key


class Device : public NimBLEServerCallbacks, public NimBLECharacteristicCallbacks {
public:
  Device(const char *deviceName = "StreamDeco", const char *manufacturer = "Marcelino Inc", uint8_t batteryLevel = 100);

  void begin();
  bool isConnected() const;

  size_t press(uint8_t key);
  size_t write(uint8_t key);
  size_t press(const key::MediaKeyReport key);
  size_t write(const key::MediaKeyReport key);
  void releaseAll();

protected:
  void onConnect(NimBLEServer *server) override;
  void onDisconnect(NimBLEServer *server) override;
  void onWrite(NimBLECharacteristic *characteristic) override;

private:
  struct HidCode {
    uint8_t keycode;
    bool useShift;
    bool ok;
  };

  static HidCode asciiToHid(uint8_t ascii);
  void sendKeyboardReport();
  void sendMediaReport();

  NimBLEHIDDevice *hid_;
  NimBLECharacteristic *inputKeyboard_;
  NimBLECharacteristic *outputKeyboard_;
  NimBLECharacteristic *inputMedia_;
  NimBLEAdvertising *advertising_;

  key::KeyReport keyReport_;
  key::MediaKeyReport mediaReport_;

  std::string deviceName_;
  std::string manufacturer_;
  uint8_t batteryLevel_;
  bool connected_;

}; // Device

} // namespace blekeyboard

#endif // _STREAMDECO_BLE_HID_HPP_
