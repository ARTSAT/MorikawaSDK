/*
**      ARTSAT MorikawaSDK
**
**      Original Copyright (C) 2012 - 2012 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) 2012 - 2012 OZAKI Naoya.
**                                          naoya.ozaki11@gmail.com
**      Portions Copyright (C) 2012 - 2013 SHINTANI Kodai.
**                                          shinkoko43@gmail.com
**      Portions Copyright (C) 2012 - 2013 TOKITAKE Yuta.
**                                          toki211@gmail.com
**      Portions Copyright (C) 2013 - 2014 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      All rights reserved.
**
**      Version     Arduino
**      Website     http://artsat.jp/
**      E-mail      info@artsat.jp
**
**      This source code is for Arduino IDE.
**      Arduino 1.0.5
**
**      TSTType.h
**
**      ------------------------------------------------------------------------
**
**      GNU GENERAL PUBLIC LICENSE (GPLv3)
**
**      This program is free software: you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation, either version 3 of the License,
**      or (at your option) any later version.
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**      See the GNU General Public License for more details.
**      You should have received a copy of the GNU General Public License
**      along with this program. If not, see <http://www.gnu.org/licenses/>.
**
**      このプログラムはフリーソフトウェアです。あなたはこれをフリーソフトウェア財団によって発行された
**      GNU 一般公衆利用許諾書（バージョン 3 か、それ以降のバージョンのうちどれか）が定める条件の下で
**      再頒布または改変することができます。このプログラムは有用であることを願って頒布されますが、
**      *全くの無保証* です。商業可能性の保証や特定目的への適合性は、言外に示されたものも含め全く存在しません。
**      詳しくは GNU 一般公衆利用許諾書をご覧ください。
**      あなたはこのプログラムと共に GNU 一般公衆利用許諾書のコピーを一部受け取っているはずです。
**      もし受け取っていなければ <http://www.gnu.org/licenses/> をご覧ください。
*/

#ifndef __TST_TYPE_H
#define __TST_TYPE_H

#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <stddef.h>
#include <Arduino.h>

#define TARGET_MORIKAWA_FM1
#define OPTION_BUILD_MEMORYLOG

namespace tst {

#define lengthof(param)     (sizeof(param) / sizeof(param[0]))
#define asciiesof(param)    (lengthof(param) - 1)

#define EEPROM_LIMIT        (0x1000)
#define EEPROM_SELFTEST     (0x0F00)
#define EEPROM_FLASHROM     (0x0B00)

enum PinEnum {
    PIN_FRAM_CS             = 10,
    PIN_FLASHROM_CS         = 11,
    PIN_LED_X               = 5,
    PIN_LED_Y               = 2,
    PIN_TONE_WAVE           = 3,
    PIN_DIGITALKER_PLAY     = 26,
    PIN_DIGITALKER_SLEEP    = 27,
    PIN_CAMERA_POWER        = 29,
    PIN_CAMERA_VSYNC        = 49,
    PIN_CAMERA_HREF         = 48,
    PIN_CAMERA_WEN          = 47,
    PIN_CAMERA_RRST         = 46,
    PIN_CAMERA_OE           = 45,
    PIN_CAMERA_RCLK         = 44,
    PIN_SCCB_SIOC           = 43,
    PIN_SCCB_SIOD           = 42
};
typedef unsigned char       PinType;
enum StorageEnum {
    STORAGE_NONE,
    STORAGE_SHAREDMEMORY,
    STORAGE_FRAM,
    STORAGE_FLASHROM,
    STORAGE_LIMIT
};
typedef unsigned char       StorageType;

}// end of namespace

#include "TSTError.h"

#endif
