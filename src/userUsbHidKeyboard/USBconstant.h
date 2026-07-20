#ifndef __USB_CONST_DATA_H__
#define __USB_CONST_DATA_H__

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "usbCommonDescriptors/StdDescriptors.h"
#include "usbCommonDescriptors/HIDClassCommon.h"
// clang-format on

// v2.3.1 と同じ EP アドレス配置
#define EP0_ADDR 0
#define EP1_ADDR 0     // EP0 と共有 (v2.3.1 と同じ)
#define EP2_ADDR 72

// Revert to v2.3.1 + CH55xduino stock layout (known-working keyboard)
#define KEYBOARD_EPADDR 0x81
#define KEYBOARD_EPSIZE 8
#define GAMEPAD_EPADDR  0x82
#define GAMEPAD_EPSIZE  64  // v2.3.1 と同じ

/** Composite: Interface 0 = HID Gamepad (POKKEN compatible),
 *             Interface 1 = HID Boot Keyboard */
typedef struct {
  USB_Descriptor_Configuration_Header_t Config;

  // Gamepad HID Interface (interface 0)
  USB_Descriptor_Interface_t Gamepad_Interface;
  USB_HID_Descriptor_HID_t Gamepad_HID;
  USB_Descriptor_Endpoint_t Gamepad_EP;

  // Keyboard HID Interface (interface 1)
  USB_Descriptor_Interface_t Keyboard_Interface;
  USB_HID_Descriptor_HID_t Keyboard_HID;
  USB_Descriptor_Endpoint_t Keyboard_EP;
} USB_Descriptor_Configuration_t;

extern __code USB_Descriptor_Device_t DeviceDescriptor;
extern __code USB_Descriptor_Configuration_t ConfigurationDescriptor;
extern __code uint8_t KeyboardReportDescriptor[];
extern __code uint8_t GamepadReportDescriptor[];
extern __code uint8_t LanguageDescriptor[];
extern __code uint16_t SerialDescriptor[];
extern __code uint16_t ProductDescriptor[];
extern __code uint16_t ManufacturerDescriptor[];

#endif
