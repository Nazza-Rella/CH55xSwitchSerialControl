#include <Serial.h>
#include <HardwareSerial.h>
#include "HIDdevice.h"
#define MAX_BUFFER 32

__xdata char pc_report_str[MAX_BUFFER];
__xdata uint16_t idx = 0;
__xdata uint32_t timeoutCnt = 0;

__xdata uint8_t pc_lx, pc_ly, pc_rx, pc_ry;

__xdata bool isNx2 = false;
__xdata bool isText = false;

USB_JoystickReport_Input_t pc_report;

void resetDirections() {
  memset(&pc_report, 0, sizeof(USB_JoystickReport_Input_t));
  pc_report.LX = STICK_NEUTRAL;
  pc_report.LY = STICK_NEUTRAL;
  pc_report.RX = STICK_NEUTRAL;
  pc_report.RY = STICK_NEUTRAL;
  pc_report.Hat = HAT_NEUTRAL;
}

void disconnect(void) {
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;
  GLOBAL_CFG = bSW_RESET;
  while (1);
}

__xdata uint8_t parse_pos;

// 16進フィールドを空白または'\r'まで読み、区切りの次へ進める
uint16_t parseHexField(char* line) {
  __xdata uint16_t value = 0;
  while (line[parse_pos] != ' ' && line[parse_pos] != '\r') {
    value *= 16;
    if (line[parse_pos] >= '0' && line[parse_pos] <= '9') {
      value += (line[parse_pos] - '0');
    } else if (line[parse_pos] >= 'A' && line[parse_pos] <= 'F') {
      value += (line[parse_pos] - 'A' + 10);
    } else if (line[parse_pos] >= 'a' && line[parse_pos] <= 'f') {
      value += (line[parse_pos] - 'a' + 10);
    }
    parse_pos++;
  }
  if (line[parse_pos] != '\r') parse_pos++;
  return value;
}

// 10進または "0x" つき16進のキーコードを読む
uint8_t parseKeyValue(char* line) {
  __xdata uint8_t value = 0;
  __xdata uint8_t is_hex = 0;
  while (line[parse_pos] != ' ' && line[parse_pos] != '\r') {
    if (is_hex) {
      value *= 16;
    } else {
      value *= 10;
    }
    if (line[parse_pos] >= '0' && line[parse_pos] <= '9') {
      value += (line[parse_pos] - '0');
    } else if (line[parse_pos] >= 'A' && line[parse_pos] <= 'F') {
      value += (line[parse_pos] - 'A' + 10);
    } else if (line[parse_pos] == 'x' || line[parse_pos] == 'X') {
      is_hex = 1;
    } else if (line[parse_pos] >= 'a' && line[parse_pos] <= 'f') {
      value += (line[parse_pos] - 'a' + 10);
    }
    parse_pos++;
  }
  return value;
}

uint8_t parseStickPosition(char* line) {
  if (strncmp(&line[3], "MIN", 3) == 0) return STICK_MIN;
  if (strncmp(&line[3], "MAX", 3) == 0) return STICK_MAX;
  return STICK_NEUTRAL;
}

void parseLine(char* line) {
  __xdata uint16_t btns = BUTTON_NONE;
  __xdata uint8_t hat = HAT_NEUTRAL;
  __xdata uint8_t lx = STICK_NEUTRAL;
  __xdata uint8_t ly = STICK_NEUTRAL;
  __xdata uint8_t rx = STICK_NEUTRAL;
  __xdata uint8_t ry = STICK_NEUTRAL;

  if (strncmp(line, "end", 3) == 0) {
    // pokecon
  } else if (strncmp(line, "disconnect", 10) == 0) {
    disconnect();
  } else if (strncmp(line, "RELEASE", 7) == 0) {
    // nx ver 1.0
  } else if (strncmp(line, "Button", 6) == 0) {
    // nx ver 1.0
    if (strncmp(&line[7], "SELECT", 6) == 0) {
      btns = BUTTON_MINUS;
    } else if (line[8] == 'T') {
      btns = BUTTON_PLUS;
    } else if (strncmp(&line[7], "LCLICK", 6) == 0) {
      btns = BUTTON_LCLICK;
    } else if (strncmp(&line[7], "RCLICK", 6) == 0) {
      btns = BUTTON_RCLICK;
    } else if (line[7] == 'H') {
      btns = BUTTON_HOME;
    } else if (line[7] == 'C') {
      btns = BUTTON_CAPTURE;
    } else if (strncmp(&line[7], "RELEASE", 7) == 0) {
      btns = BUTTON_NONE;
    } else if (line[7] == 'A') {
      btns = BUTTON_A;
    } else if (line[7] == 'B') {
      btns = BUTTON_B;
    } else if (line[7] == 'X') {
      btns = BUTTON_X;
    } else if (line[7] == 'Y') {
      btns = BUTTON_Y;
    } else if (line[7] == 'L') {
      btns = BUTTON_L;
    } else if (line[7] == 'R') {
      btns = BUTTON_R;
    } else if (line[8] == 'L') {
      btns = BUTTON_ZL;
    } else if (line[8] == 'R') {
      btns = BUTTON_ZR;
    }
  } else if (strncmp(line, "HAT", 3) == 0) {
    // nx ver 1.0
    if (strncmp(&line[4], "CENTER", 6) == 0) {
      hat = HAT_NEUTRAL;
    } else if (strncmp(&line[4], "TOP_RIGHT", 9) == 0) {
      hat = HAT_UP_RIGHT;
    } else if (line[4] == 'R') {
      hat = HAT_RIGHT;
    } else if (strncmp(&line[4], "BOTTOM_RIGHT", 12) == 0) {
      hat = HAT_DOWN_RIGHT;
    } else if (strncmp(&line[4], "BOTTOM_LEFT", 11) == 0) {
      hat = HAT_DOWN_LEFT;
    } else if (strncmp(&line[4], "BOTTOM", 6) == 0) {
      hat = HAT_DOWN;
    } else if (line[4] == 'L') {
      hat = HAT_LEFT;
    } else if (strncmp(&line[4], "TOP_LEFT", 8) == 0) {
      hat = HAT_UP_LEFT;
    } else if (strncmp(&line[4], "TOP", 3) == 0) {
      hat = HAT_UP;
    }
  } else if (strncmp(line, "LX", 2) == 0) {
    // nx ver 1.0
    lx = parseStickPosition(line);
  } else if (strncmp(line, "LY", 2) == 0) {
    // nx ver 1.0
    ly = parseStickPosition(line);
  } else if (strncmp(line, "RX", 2) == 0) {
    // nx ver 1.0
    rx = parseStickPosition(line);
  } else if (strncmp(line, "RY", 2) == 0) {
    // nx ver 1.0
    ry = parseStickPosition(line);
  } else if (line[0] == '\"') {
    // pokecon
    __xdata uint8_t char_pos = 1;
    while (line[char_pos] != 0 && line[char_pos] != '\"' || (line[char_pos + 1] != 0 && line[char_pos + 1] != '\r' && line[char_pos + 1] != '\n')) {
      pushKey(line[char_pos]);
      char_pos++;
    }
  } else if (strncmp(line, "Key", 3) == 0) {
    // pokecon
    parse_pos = 4;
    pushSpecialKey(parseKeyValue(line));
  } else if (strncmp(line, "Press", 5) == 0) {
    // pokecon
    parse_pos = 6;
    pressSpecialKey(parseKeyValue(line));
  } else if (strncmp(line, "Release", 7) == 0) {
    // pokecon
    parse_pos = 8;
    releaseSpecialKey(parseKeyValue(line));
  } else if (line[0] == 0xaa) {
    // nx2 ver 2.00 - 2.07
    btns = line[5] | (line[6] << 8);
    hat = line[7];
    if (line[8] & 1) lx = STICK_MIN;
    if (line[8] & 2) lx = STICK_MAX;
    if (line[8] & 4) ly = STICK_MIN;
    if (line[8] & 8) ly = STICK_MAX;
    if (line[9] & 1) rx = STICK_MIN;
    if (line[9] & 2) rx = STICK_MAX;
    if (line[9] & 4) ry = STICK_MIN;
    if (line[9] & 8) ry = STICK_MAX;
  } else if (line[0] == 0xab) {
    // nx2 ver 2.08
    btns = line[1] | (line[2] << 8);
    hat = line[3];
    lx = line[4];
    ly = line[5];
    rx = line[6];
    ry = line[7];

    // keyboard
    if (line[8] == 1) {
      // normal press
      pressKey(line[9]);
    } else if (line[8] == 2) {
      // normal release
      releaseKey(line[9]);
    } else if (line[8] == 3) {
      // special press
      pressSpecialKey(line[9]);
    } else if (line[8] == 4) {
      // special release
      releaseSpecialKey(line[9]);
    } else if (line[8] == 5) {
      // all release
      releaseAllKey();
    }
  } else if (line[0] >= '0' && line[0] <= '9') {
    // pokecon
    lx = pc_report.LX;
    ly = pc_report.LY;
    rx = pc_report.RX;
    ry = pc_report.RY;

    parse_pos = 0;
    btns = parseHexField(line);
    hat = (uint8_t)parseHexField(line);
    pc_lx = (uint8_t)parseHexField(line);
    pc_ly = (uint8_t)parseHexField(line);
    pc_rx = (uint8_t)parseHexField(line);
    pc_ry = (uint8_t)parseHexField(line);

    __xdata int use_right = btns & 0x1;
    __xdata int use_left = btns & 0x2;

    if ((use_right != 0) & (use_left != 0)) {
      lx = pc_lx;
      ly = pc_ly;
      rx = pc_rx;
      ry = pc_ry;
    } else if (use_right != 0) {
      rx = pc_lx;
      ry = pc_ly;
    } else if (use_left != 0) {
      lx = pc_lx;
      ly = pc_ly;
    }

    btns >>= 2;
  }

  pc_report.Button = btns;
  pc_report.Hat = hat;

  pc_report.LX = lx;
  pc_report.LY = ly;
  pc_report.RX = rx;
  pc_report.RY = ry;
}

void setup() {
  USBInit();
  Serial0_begin(9600);
  Serial1_begin(9600);
  resetDirections();
}


/*void Uart1_ISR(void) __interrupt(INT_NO_UART1) {  
    if (U1RI){
        uart1IntRxHandler();
        U1RI =0;
    }
    if (U1TI){
        uart1IntTxHandler();
        U1TI =0;
    }
    //isr_uart_func();
}*/

void loop() {
  while (Serial0_available() || Serial1_available()) {
    timeoutCnt = 1;
    char c = 0;
    if (Serial0_available()) {
      c = Serial0_read();
    } else {
      c = Serial1_read();
    }

    if (idx == 0) {
      isNx2 = (c == 0xaa || c == 0xab);
      isText = (c == '\"');
    }

    if ((c != '\n' || isNx2 || isText) && idx < MAX_BUFFER)
      pc_report_str[idx++] = c;

    if ((c == '\r' && !isNx2 && !isText) || (isNx2 && idx == 11) || (isText && c == '\r' && pc_report_str[idx - 2] == '\"')) {
      idx = 0;
      timeoutCnt = 0;
      parseLine(pc_report_str);
      if (!isText) sendReport((uint8_t*)&pc_report);
      memset(pc_report_str, 0, sizeof(pc_report_str));
    }
  }
  if (timeoutCnt > 1000) {
    idx = 0;
    timeoutCnt = 0;
  } else {
    if (timeoutCnt > 0) timeoutCnt++;
  }
}