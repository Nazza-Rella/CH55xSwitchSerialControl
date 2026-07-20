#include "USBconstant.h"

// Device descriptor — HORI POKKEN VID/PID (same as v2.3.1 gamepad, Switch 2 accepts)
__code USB_Descriptor_Device_t DeviceDescriptor = {
    .Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
    .USBSpecification = VERSION_BCD(1, 1, 0),
    .Class = 0x00,
    .SubClass = 0x00,
    .Protocol = 0x00,
    .Endpoint0Size = DEFAULT_ENDP0_SIZE,
    .VendorID = 0x0F0D,
    .ProductID = 0x0092,
    .ReleaseNumber = 0x0100,      // bcdDevice
    .ManufacturerStrIndex = 1,
    .ProductStrIndex = 2,
    .SerialNumStrIndex = 3,
    .NumberOfConfigurations = 0x01};

__code USB_Descriptor_Configuration_t ConfigurationDescriptor = {
    .Config = {.Header = {.Size = sizeof(USB_Descriptor_Configuration_Header_t),
                          .Type = DTYPE_Configuration},
               .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
               .TotalInterfaces = 2,
               .ConfigurationNumber = 1,
               .ConfigurationStrIndex = NO_DESCRIPTOR,
               .ConfigAttributes = (USB_CONFIG_ATTR_RESERVED),
               .MaxPowerConsumption = USB_CONFIG_POWER_MA(200)},

    // === Interface 0: HID Gamepad (POKKEN, non-boot) ===
    .Gamepad_Interface = {.Header = {.Size = sizeof(USB_Descriptor_Interface_t),
                                     .Type = DTYPE_Interface},
                          .InterfaceNumber = 0,
                          .AlternateSetting = 0x00,
                          .TotalEndpoints = 1,
                          .Class = HID_CSCP_HIDClass,
                          .SubClass = 0x00,  // non-boot
                          .Protocol = 0x00,  // none
                          .InterfaceStrIndex = NO_DESCRIPTOR},
    .Gamepad_HID = {.Header = {.Size = sizeof(USB_HID_Descriptor_HID_t),
                               .Type = HID_DTYPE_HID},
                    .HIDSpec = VERSION_BCD(1, 1, 0),
                    .CountryCode = 0x00,
                    .TotalReportDescriptors = 1,
                    .HIDReportType = HID_DTYPE_Report,
                    .HIDReportLength = 89},
    .Gamepad_EP = {.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t),
                               .Type = DTYPE_Endpoint},
                   .EndpointAddress = GAMEPAD_EPADDR,  // 0x82 IN
                   .Attributes =
                       (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC |
                        ENDPOINT_USAGE_DATA),
                   .EndpointSize = GAMEPAD_EPSIZE,
                   .PollingIntervalMS = 1},  // v2.3.1 と同じ

    // === Interface 1: HID Boot Keyboard ===
    .Keyboard_Interface = {.Header = {.Size = sizeof(USB_Descriptor_Interface_t),
                                      .Type = DTYPE_Interface},
                           .InterfaceNumber = 1,
                           .AlternateSetting = 0x00,
                           .TotalEndpoints = 1,
                           .Class = HID_CSCP_HIDClass,
                           .SubClass = HID_CSCP_BootSubclass,
                           .Protocol = HID_CSCP_KeyboardBootProtocol,
                           .InterfaceStrIndex = NO_DESCRIPTOR},
    .Keyboard_HID = {.Header = {.Size = sizeof(USB_HID_Descriptor_HID_t),
                                .Type = HID_DTYPE_HID},
                     .HIDSpec = VERSION_BCD(1, 1, 0),
                     .CountryCode = 0x00,
                     .TotalReportDescriptors = 1,
                     .HIDReportType = HID_DTYPE_Report,
                     .HIDReportLength = 63},  // sizeof(KeyboardReportDescriptor)
    .Keyboard_EP = {.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t),
                               .Type = DTYPE_Endpoint},
                    .EndpointAddress = KEYBOARD_EPADDR,  // 0x81 IN
                    .Attributes =
                        (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC |
                         ENDPOINT_USAGE_DATA),
                    .EndpointSize = KEYBOARD_EPSIZE,
                    .PollingIntervalMS = 10},
};

// POKKEN gamepad report descriptor (89 bytes, same as v2.3.1 stock)
__code uint8_t GamepadReportDescriptor[] = {
    0x05, 0x01, 0x09, 0x05, 0xa1, 0x01, 0x15, 0x00, 0x25, 0x01, 0x35, 0x00,
    0x45, 0x01, 0x75, 0x01, 0x95, 0x10, 0x05, 0x09, 0x19, 0x01, 0x29, 0x10,
    0x81, 0x02, 0x05, 0x01, 0x25, 0x07, 0x46, 0x3b, 0x01, 0x75, 0x04, 0x95,
    0x01, 0x65, 0x14, 0x09, 0x39, 0x81, 0x42, 0x65, 0x00, 0x95, 0x01, 0x81,
    0x01, 0x26, 0xff, 0x00, 0x46, 0xff, 0x00, 0x09, 0x30, 0x09, 0x31, 0x09,
    0x32, 0x09, 0x35, 0x75, 0x08, 0x95, 0x04, 0x81, 0x02, 0x06, 0x00, 0xff,
    0x09, 0x20, 0x95, 0x01, 0x81, 0x02, 0x0a, 0x21, 0x26, 0x95, 0x08, 0x91,
    0x02, 0xc0
};

// HID 1.11 Boot Keyboard Report Descriptor (63 bytes — CH55xduino stock)
__code uint8_t KeyboardReportDescriptor[] = {
    0x05, 0x01,
    0x09, 0x06,
    0xa1, 0x01,
    0x05, 0x07,
    0x19, 0xe0,
    0x29, 0xe7,
    0x15, 0x00,
    0x25, 0x01,
    0x95, 0x08,
    0x75, 0x01,
    0x81, 0x02,
    0x95, 0x01,
    0x75, 0x08,
    0x81, 0x03,
    0x95, 0x06,
    0x75, 0x08,
    0x15, 0x00,
    0x25, 0xff,
    0x05, 0x07,
    0x19, 0x00,
    0x29, 0xe7,
    0x81, 0x00,
    0x05, 0x08,
    0x19, 0x01,
    0x29, 0x05,
    0x15, 0x00,
    0x25, 0x01,
    0x95, 0x05,
    0x75, 0x01,
    0x91, 0x02,
    0x95, 0x01,
    0x75, 0x03,
    0x91, 0x03,
    0xc0
};

// String Descriptors
__code uint8_t LanguageDescriptor[] = {0x04, 0x03, 0x09, 0x04};

__code uint16_t SerialDescriptor[] = {
    (((1 + 1) * 2) | (DTYPE_String << 8)),
    '0'};

__code uint16_t ProductDescriptor[] = {
    (((17 + 1) * 2) | (DTYPE_String << 8)),
    'P', 'O', 'K', 'K', 'E', 'N', ' ',
    'C', 'O', 'N', 'T', 'R', 'O', 'L', 'L', 'E', 'R'};

__code uint16_t ManufacturerDescriptor[] = {
    (((13 + 1) * 2) | (DTYPE_String << 8)),
    'H', 'O', 'R', 'I', ' ',
    'C', 'O', '.', ',', 'L', 'T', 'D', '.'};
