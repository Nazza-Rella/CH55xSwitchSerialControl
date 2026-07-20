// clang-format off
#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"
#include "../../HIDdevice.h"
// clang-format on

// clang-format off
extern __xdata __at (EP1_ADDR) uint8_t Ep1Buffer[];
extern __xdata __at (EP2_ADDR) uint8_t Ep2Buffer[];
// clang-format on

volatile __xdata uint8_t UpPoint1_Busy =
    0; // Flag of whether upload pointer is busy

// EP1 IN callback (KEYBOARD, EP 0x81)
void USB_EP1_IN() {
  UEP1_T_LEN = 0;
  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;
  UpPoint1_Busy = 0;
}

// EP2 IN callback (GAMEPAD, EP 0x82)
void USB_EP2_IN() {
  UEP2_T_LEN = 0;
  UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;
}
void USB_EP2_OUT() {
  if (U_TOG_OK) { }
}

void sendReport(uint8_t* p) {
  if (UsbConfig == 0) return;
  for (__data uint8_t i = 0; i < 8; i++) {
    Ep2Buffer[64 + i] = p[i];
  }
  UEP2_T_LEN = 8;
  UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}

__xdata uint8_t HIDKey[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

// 日本語キーボード配列の ASCII → HID usage 変換表 (上位8bit=修飾キー)
__code uint16_t _asciimap[128] = {
  KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,
  KEY_BACKSPACE,KEY_TAB,KEY_ENTER,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,
  KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,
  KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,KEY_RESERVED,
  KEY_SPACE,
  KEY_1|MOD_LEFT_SHIFT, KEY_2|MOD_LEFT_SHIFT, KEY_3|MOD_LEFT_SHIFT,
  KEY_4|MOD_LEFT_SHIFT, KEY_5|MOD_LEFT_SHIFT, KEY_6|MOD_LEFT_SHIFT,
  KEY_7|MOD_LEFT_SHIFT, KEY_8|MOD_LEFT_SHIFT, KEY_9|MOD_LEFT_SHIFT,
  KEY_QUOTE|MOD_LEFT_SHIFT, KEY_SEMICOLON|MOD_LEFT_SHIFT,
  KEY_COMMA, KEY_MINUS, KEY_PERIOD, KEY_SLASH,
  KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
  KEY_QUOTE, KEY_SEMICOLON,
  KEY_COMMA|MOD_LEFT_SHIFT, KEY_MINUS|MOD_LEFT_SHIFT,
  KEY_PERIOD|MOD_LEFT_SHIFT, KEY_SLASH|MOD_LEFT_SHIFT,
  KEY_LEFT_BRACE,
  KEY_A|MOD_LEFT_SHIFT,KEY_B|MOD_LEFT_SHIFT,KEY_C|MOD_LEFT_SHIFT,KEY_D|MOD_LEFT_SHIFT,
  KEY_E|MOD_LEFT_SHIFT,KEY_F|MOD_LEFT_SHIFT,KEY_G|MOD_LEFT_SHIFT,KEY_H|MOD_LEFT_SHIFT,
  KEY_I|MOD_LEFT_SHIFT,KEY_J|MOD_LEFT_SHIFT,KEY_K|MOD_LEFT_SHIFT,KEY_L|MOD_LEFT_SHIFT,
  KEY_M|MOD_LEFT_SHIFT,KEY_N|MOD_LEFT_SHIFT,KEY_O|MOD_LEFT_SHIFT,KEY_P|MOD_LEFT_SHIFT,
  KEY_Q|MOD_LEFT_SHIFT,KEY_R|MOD_LEFT_SHIFT,KEY_S|MOD_LEFT_SHIFT,KEY_T|MOD_LEFT_SHIFT,
  KEY_U|MOD_LEFT_SHIFT,KEY_V|MOD_LEFT_SHIFT,KEY_W|MOD_LEFT_SHIFT,KEY_X|MOD_LEFT_SHIFT,
  KEY_Y|MOD_LEFT_SHIFT,KEY_Z|MOD_LEFT_SHIFT,
  KEY_RIGHT_BRACE, KEY_JP_BACKSLASH, KEY_BACKSLASH, KEY_EQUAL,
  KEY_JP_BACKSLASH|MOD_LEFT_SHIFT, KEY_LEFT_BRACE|MOD_LEFT_SHIFT,
  KEY_A,KEY_B,KEY_C,KEY_D,KEY_E,KEY_F,KEY_G,KEY_H,KEY_I,KEY_J,KEY_K,KEY_L,KEY_M,
  KEY_N,KEY_O,KEY_P,KEY_Q,KEY_R,KEY_S,KEY_T,KEY_U,KEY_V,KEY_W,KEY_X,KEY_Y,KEY_Z,
  KEY_RIGHT_BRACE|MOD_LEFT_SHIFT, KEY_JP_YEN|MOD_LEFT_SHIFT,
  KEY_BACKSLASH|MOD_LEFT_SHIFT, KEY_EQUAL|MOD_LEFT_SHIFT,
  KEY_RESERVED
};

void USBInit() {
  USBDeviceCfg();
  USBDeviceEndPointCfg();
  USBDeviceIntCfg();
  UEP0_T_LEN = 0;
  UEP1_T_LEN = 0;
  UEP2_T_LEN = 0;
}

void USB_EP1_OUT() {
  if (U_TOG_OK) { }
}

void USB_EP1_send() {
  for (uint8_t i = 0; i < sizeof(HIDKey); i++) {
    Ep1Buffer[64 + i] = HIDKey[i];
  }
  UEP1_T_LEN = sizeof(HIDKey);
  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}

void releaseAllKey(void) {
  for (uint8_t i = 0; i < sizeof(HIDKey); i++) { HIDKey[i] = 0; }
  USB_EP1_send();
}

// HID usage コードをそのまま送るキー (ASCII 変換しない)
static bool isRawUsageCode(uint8_t c) {
  return c == KEY_JP_HANZEN || c == KEY_JP_BACKSLASH || c == KEY_JP_HIRAGANA ||
         c == KEY_JP_YEN || c == KEY_JP_HENKAN || c == KEY_JP_MUHENKAN ||
         c == KEY_ENTER || c == KEY_BACKSPACE || c == KEY_DELETE ||
         c == KEY_UP_ARROW || c == KEY_DOWN_ARROW || c == KEY_LEFT_ARROW ||
         c == KEY_RIGHT_ARROW;
}

static void pressCode(uint8_t c) {
  uint8_t i;
  if (HIDKey[2] != c && HIDKey[3] != c && HIDKey[4] != c && HIDKey[5] != c && HIDKey[6] != c && HIDKey[7] != c) {
    for (i = 2; i < 8; i++) { if (HIDKey[i] == 0x00) { HIDKey[i] = c; break; } }
    if (i == 8) return;
  }
  USB_EP1_send();
}

static void releaseCode(uint8_t c) {
  uint8_t i;
  for (i = 2; i < 8; i++) { if (HIDKey[i] == c) { HIDKey[i] = 0; break; } }
  if (i == 8) return;
  USB_EP1_send();
}

void pressKey(uint8_t c) {
  uint16_t k = _asciimap[c & 0x7F];
  HIDKey[0] = k >> 8;
  pressCode(k & 0xFF);
}

void pressSpecialKey(uint8_t c) {
  if (!isRawUsageCode(c)) {
    uint16_t k = _asciimap[c & 0x7F];
    HIDKey[0] = k >> 8;
    c = k & 0xFF;
  }
  pressCode(c);
}

void releaseKey(uint8_t c) {
  uint16_t k = _asciimap[c & 0x7F];
  HIDKey[0] &= ~(k >> 8);
  releaseCode(k & 0xFF);
}

void releaseSpecialKey(uint8_t c) {
  if (!isRawUsageCode(c)) {
    uint16_t k = _asciimap[c & 0x7F];
    HIDKey[0] &= ~(k >> 8);
    c = k & 0xFF;
  }
  releaseCode(c);
}

void pushKey(uint8_t c) {
  pressKey(c);
  delay(30);
  releaseKey(c);
  delay(30);
}

void pushSpecialKey(uint8_t c) {
  pressSpecialKey(c);
  delay(30);
  releaseSpecialKey(c);
  delay(30);
}
