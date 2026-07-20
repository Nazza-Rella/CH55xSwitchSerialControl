# CH55xSwitchSerialControl (composite USB fork)

[608/CH55xSwitchSerialControl](https://github.com/608/CH55xSwitchSerialControl) の fork です。
USB を Gamepad + Boot Keyboard の複合デバイス構成に変更し、Switch 2 でゲームパッドとキーボードを同時に使えるようにしました。

## 変更点

- USB を composite 構成 (Gamepad + Boot Keyboard の 2 インターフェース) に変更
  - v2.3.1 の USB handler だと composite 構成が Switch 2 で認識されないので、USB スタックを CH55xduino の stock handler ベースに置き換え (`src/userUsbHidKeyboard/`)
  - `HIDdevice.c` / `USBconstant.c/h` / `USBhandler.c/h` は `src/userUsbHidKeyboard/` に統合
- コード整理 (パース処理の共通化など)
- `disconnect` コマンドを追加。送信すると CH552 がソフトウェアリセットされ、Switch との接続が切れる

## 対応プロトコル

以下のプロトコルに対応しています。

- テキスト形式 (`Button A` / `HAT TOP` / `LX MIN` など)
- Poke-Controller 形式 (`0x2 8 80 80 80 80` / `"text"` / `Key` / `Press` / `Release`)
- バイナリ 0xAA (11 bytes)
- バイナリ 0xAB (11 bytes) + キーボード拡張
  - byte 8 = mode: 1=pressKey / 2=releaseKey / 3=pressSpecialKey / 4=releaseSpecialKey / 5=releaseAllKey
  - byte 9 = キーコード
- 日本語キーボード配列
- `disconnect` CH552 をソフトウェアリセット。Switch との接続が切れる

## ビルド

[CH55xduino](https://github.com/DeqingSun/ch55xduino) を導入した Arduino 環境でビルドします。

```
arduino-cli compile -b "CH55xDuino:mcs51:ch552:clock=24internal,usb_settings=usbcdc"
```

- USB Settings: `Default CDC`
- EP アドレス配置: EP0=0, EP1=0, EP2=72
- Gamepad EP: 64B / 1ms polling

## License

The upstream repository [608/CH55xSwitchSerialControl](https://github.com/608/CH55xSwitchSerialControl) has no license. All rights to the original code remain with the original author, and the modifications in this fork are treated the same way.

The USB stack under `src/userUsbHidKeyboard/` (`USBconstant.*` / `USBhandler.*`) is derived from [CH55xduino](https://github.com/DeqingSun/ch55xduino), licensed under LGPL-2.1.
