/*
**      ARTSAT MorikawaSDK
**
**      Original Copyright (C) 2013 - 2014 HORIGUCHI Junshi.
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
**      TSTCamera.cpp
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

#include "TSTCamera.h"
#include "TSTSCCB.h"
#include "TSTMorikawa.h"

namespace tst {

#define SCCB_ADDRESS        (0x42)
#define BOOT_DELAY          (100)
#define REGISTER_DELAY      (300)

static CameraFormat const g_format[CAMERA_LIMIT] PROGMEM = {
    {640, 480, 1},
    {320, 240, 1},
    {176, 144, 1},
    {320, 240, 2},
    {160, 120, 2},
    { 88,  72, 2},
    {320, 240, 2},
    {160, 120, 2},
    { 88,  72, 2}
};

/*public static */TSTError TSTCamera::getFormat(CameraType mode, CameraFormat* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((0 <= mode && mode < CAMERA_LIMIT) && result != NULL) {
        result->width = pgm_read_word(&g_format[mode].width);
        result->height = pgm_read_word(&g_format[mode].height);
        result->depth = pgm_read_byte(&g_format[mode].depth);
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TSTError TSTCamera::setup(TSTMorikawa* morikawa)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
#ifdef TARGET_MORIKAWA_FM1
    if (morikawa != NULL) {
        if (_morikawa == NULL) {
            _morikawa = morikawa;
            pinMode(PIN_CAMERA_POWER, OUTPUT);
            digitalWrite(PIN_CAMERA_POWER, LOW);
            pinMode(PIN_CAMERA_VSYNC, INPUT);
            pinMode(PIN_CAMERA_HREF, INPUT);
            pinMode(PIN_CAMERA_WEN, OUTPUT);
            digitalWrite(PIN_CAMERA_WEN, LOW);
            pinMode(PIN_CAMERA_RRST, OUTPUT);
            digitalWrite(PIN_CAMERA_RRST, HIGH);
            pinMode(PIN_CAMERA_OE, OUTPUT);
            digitalWrite(PIN_CAMERA_OE, HIGH);
            pinMode(PIN_CAMERA_RCLK, OUTPUT);
            digitalWrite(PIN_CAMERA_RCLK, LOW);
            SCCB.begin();
        }
        else {
            error = TSTERROR_INVALID_STATE;
        }
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
#else
    error = TSTERROR_NO_SUPPORT;
#endif
    return error;
}

/*public */void TSTCamera::cleanup(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        digitalWrite(PIN_CAMERA_POWER, LOW);
        _morikawa = NULL;
    }
    return;
}

/*public */TSTError TSTCamera::snapshot(CameraType mode, StorageType storage, unsigned long address, unsigned long size, unsigned long* result)
{
    static RegisterRec const s_initialize[] PROGMEM = {
        {0x11, 0x81},   // CLKRC 80 = 12M 81 = 6M bf = 1/128
        {0x15, 0x02},   // vsync negative
        {0x6B, 0x4A},   // PLL control 0a dis /4a x4 (for 12MHz OSC(FM))
        {0x01, 0x40},   // AWB BLUE
        {0x02, 0x60},   // AWB RED
        {0x09, 0x03},   // Output drive[1:0] 00~11 x1 to x4
        {0x0D, 0x40},   // [5:4]window def:40 (90 1/2) (A0 1/4) &com17[7:6]
        {0x0E, 0x61},   // com5 ?
        {0x0F, 0x4B},   // com6 default(43) ?
        {0x13, 0x87},   // com8 Analog
        {0x14, 0x4A},   // com9 AGC (00/4A)
        {0x16, 0x02},   // ?
        {0x20, 0x0F},   // ADC gain x0.55 (Sunlight)
        {0x1E, 0x07},   // Mirror / Vflip
        {0x21, 0x02},   // ADC ctrl1
        {0x22, 0x91},   // ADC ctrl2
        {0x29, 0x07},   // rsvd
        {0x33, 0x0B},   // CHLF array current
        {0x34, 0x11},   // ARBLM Array reference ctrl
        {0x35, 0x0B},   // rsvd
        {0x37, 0x1D},   // ADC
        {0x38, 0x71},   // ACOM
        {0x39, 0x2A},   // OFCON ADC offset
        {0x3A, 0x0C},   // TLSB // UYVY (&com13)
        {0x3B, 0x12},   // COM11 (12) (night mode EA)
        {0x3C, 0x78},   // COM12
        {0x3D, 0xC1},   // COM13 set UYVY
        {0x3F, 0x00},   // edge
        {0x40, 0xD0},   // com15 (08)
        {0x41, 0x38},   // com16 (DSP)
        {0x42, 0x40},   // com17 def:08 1/2 1/4 40 (DSP)
        {0x43, 0x0A},   // AWB
        {0x44, 0xF0},   // AWB
        {0x45, 0x34},   // AWB
        {0x46, 0x58},   // AWB
        {0x47, 0x28},   // AWB
        {0x48, 0x3A},   // AWB
        {0x4B, 0x09},   // reg4B
        {0x4C, 0x00},   // DNSTH denoise
        {0x4D, 0x40},   // dummy row position
        {0x4E, 0x20},   // rsvd
        {0x4F, 0x80},   // MTX1
        {0x50, 0x80},   // MTX1
        {0x51, 0x00},   // MTX1
        {0x52, 0x22},   // MTX1
        {0x53, 0x5E},   // MTX1
        {0x54, 0x80},   // MTX1
        {0x56, 0x40},   // CONTRAST
        {0x58, 0x9E},   // MTXS
        {0x59, 0x88},   // AWB7
        {0x5A, 0x88},   // AWB8
        {0x5B, 0x44},   // AWB9
        {0x5C, 0x67},   // AWB10
        {0x5D, 0x49},   // AWB11
        {0x5E, 0x0E},   // AWB12
        {0x69, 0x00},   // fix GAIN
        {0x6A, 0x40},   // GGAIN
        {0x6C, 0x0A},   // AWB10
        {0x6D, 0x55},   // AWB10
        {0x6E, 0x11},   // AWB10
        {0x6F, 0x9F},   // AWB10
        {0x74, 0x10},   // reg74 DSP
        {0x75, 0x05},   // reg75 DSP
        {0x76, 0xE1},   // reg76 DSP
        {0x77, 0x01},   // reg77 DSP
        {0x78, 0x04},   // rsvd
        {0x79, 0x01},   // rsvd
        {0x8D, 0x4F},   // rsvd
        {0x8E, 0x00},   // rsvd
        {0x8F, 0x00},   // rsvd
        {0x90, 0x00},   // rsvd
        {0x91, 0x00},   // rsvd
        {0x96, 0x00},   // rsvd
        {0x96, 0x00},   // rsvd
        {0x97, 0x30},   // rsvd
        {0x98, 0x20},   // rsvd
        {0x99, 0x30},   // rsvd
        {0x9A, 0x00},   // rsvd
        {0x9B, 0x29},   // rsvd
        {0x9C, 0x03},   // rsvd
        {0x9D, 0x4C},   // 50Hz
        {0x9E, 0x3F},   // 60Hz
        {0xA4, 0x88},   // NT ctrl
        {0xB0, 0x84},   // rsvd
        {0xB1, 0x0C},   // AVLC1
        {0xB2, 0x0E},   // rsvd
        {0xB3, 0x82},   // THL_st
        {0xB8, 0x0A},   // rsvd
        {0xC8, 0xF0},   // rsvd
        {0xC9, 0x60}    // SATCTR saturation
    };
    static RegisterRec const s_standby[] PROGMEM = {
        {0x6B, 0x0A},   // PLL control 0A = x1, 4A = x4
        {0x11, 0xBF}    // Clockrc 1/128
    };
    static RegisterRec const s_RAW_VGA[] PROGMEM = {        // RAW VGA (640x480 1byte/pixel)
        {0x12, 0x01},   // COM7
        {0x0C, 0x00},   // COM3
        {0x3E, 0x00},   // COM14
        {0x70, 0x3A},   // SCALING_XSC
        {0x71, 0x35},   // SCALING_YSC
        {0x72, 0x11},   // SCALING_DCWCTR
        {0x73, 0xF0},   // SCALING_PCLK_DIV
        {0xA2, 0x02},   // SCALING_PCLK_DELAY
        {0x17, 0x13},   // HSTART
        {0x18, 0x01},   // HSTOP
        {0x19, 0x02},   // VSTART
        {0x1A, 0x7A},   // VSTOP
        {0x32, 0xB6},   // HREF
        {0x03, 0x0A}    // VREF
    };
    static RegisterRec const s_RAW_QVGA[] PROGMEM = {       // RAW QVGA (320x240 1byte/pixel)
        {0x12, 0x11},   // COM7
        {0x0C, 0x04},   // COM3
        {0x3E, 0x1A},   // COM14
        {0x70, 0x3A},   // SCALING_XSC
        {0x71, 0x35},   // SCALING_YSC
        {0x72, 0x11},   // SCALING_DCWCTR
        {0x73, 0xF9},   // SCALING_PCLK_DIV
        {0xA2, 0x02},   // SCALING_PCLK_DELAY
        {0x17, 0x16},   // HSTART
        {0x18, 0x04},   // HSTOP
        {0x19, 0x02},   // VSTART
        {0x1A, 0x7A},   // VSTOP
        {0x32, 0x80},   // HREF
        {0x03, 0x0A}    // VREF
    };
    static RegisterRec const s_RAW_QCIF[] PROGMEM = {       // RAW QCIF (176x144 1byte/pixel)
        {0x12, 0x0D},   // COM7 0d
        {0x0C, 0x0C},   // COM3: scaling on
        {0x3E, 0x1A},   // COM14
        {0x70, 0x3A},   // SCALING_XSC
        {0x71, 0x35},   // SCALING_YSC
        {0x72, 0x11},   // SCALING_DCWCTR
        {0x73, 0xF9},   // SCALING_PCLK_DIV
        {0xA2, 0x52},   // SCALING_PCLK_DELAY
        {0x17, 0x39},   // HSTART
        {0x18, 0x03},   // HSTOP
        {0x19, 0x03},   // VSTART
        {0x1A, 0x7B},   // VSTOP
        {0x32, 0x03},   // HREF
        {0x03, 0x02}    // VREF
    };
    static RegisterRec const s_RGB565_QVGA[] PROGMEM = {    // RGB565 QVGA (320x240 2byte/pixel)
        {0x12, 0x04},   // COM7
        {0x0C, 0x04},   // COM3
        {0x3E, 0x19},   // COM14
        {0x70, 0x3A},   // SCALING_XSC
        {0x71, 0x35},   // SCALING_YSC
        {0x72, 0x11},   // SCALING_DCWCTR
        {0x73, 0xF1},   // SCALING_PCLK_DIV
        {0xA2, 0x02},   // SCALING_PCLK_DELAY
        {0x17, 0x16},   // HSTART
        {0x18, 0x04},   // HSTOP
        {0x19, 0x02},   // VSTART
        {0x1A, 0x7A},   // VSTOP
        {0x32, 0x80},   // HREF
        {0x03, 0x0A}    // VREF
    };
    static RegisterRec const s_RGB565_QQVGA[] PROGMEM = {   // RGB565 QQVGA (160x120 2byte/pixel)
        {0x12, 0x04},   // COM7
        {0x0C, 0x04},   // COM3
        {0x3E, 0x1A},   // COM14
        {0x70, 0x3A},   // SCALING_XSC
        {0x71, 0x35},   // SCALING_YSC
        {0x72, 0x22},   // SCALING_DCWCTR
        {0x73, 0xF2},   // SCALING_PCLK_DIV
        {0xA2, 0x02},   // SCALING_PCLK_DELAY
        {0x17, 0x16},   // HSTART
        {0x18, 0x04},   // HSTOP
        {0x19, 0x02},   // VSTART
        {0x1A, 0x7A},   // VSTOP
        {0x32, 0xA4},   // HREF
        {0x03, 0x0A}    // VREF
    };
    static RegisterRec const s_RGB565_QQCIF[] PROGMEM = {   // RGB565 QQCIF (88x72 2byte/pixel)
        {0x12, 0x04},   // COM7
        {0x0C, 0x0C},   // COM3
        {0x3E, 0x12},   // COM14
        {0x70, 0x3A},   // SCALING_XSC
        {0x71, 0x35},   // SCALING_YSC
        {0x72, 0x22},   // SCALING_DCWCTR down sample by 4
        {0x73, 0xF2},   // SCALING_PCLK_DIV F2 = DSP clock div by 4
        {0xA2, 0x2A},   // SCALING_PCLK_DELAY
        {0x17, 0x16},   // HSTART
        {0x18, 0x04},   // HSTOP
        {0x19, 0x02},   // VSTART
        {0x1A, 0x7A},   // VSTOP
        {0x32, 0xA4},   // HREF
        {0x03, 0x0A}    // VREF
    };
    static RegisterRec const s_YUV422_QVGA[] PROGMEM = {       // YUV QVGA (320x240 2byte/pixel)
        {0x12, 0x00},   // COM7
        {0x0C, 0x0C},   // COM3
        {0x3E, 0x19},   // COM14
        {0x70, 0x3A},   // SCALING_XSC
        {0x71, 0x35},   // SCALING_YSC
        {0x72, 0x11},   // SCALING_DCWCTR
        {0x73, 0xF1},   // SCALING_PCLK_DIV
        {0xA2, 0x02},   // SCALING_PCLK_DELAY
        {0x17, 0x16},   // HSTART
        {0x18, 0x04},   // HSTOP
        {0x19, 0x02},   // VSTART
        {0x1A, 0x7A},   // VSTOP
        {0x32, 0x80},   // HREF
        {0x03, 0x0A}    // VREF
    };
    static RegisterRec const s_YUV422_QQVGA[] PROGMEM = {      // YUV QQVGA (160x120 2byte/pixel)
        {0x12, 0x00},   // COM7
        {0x0C, 0x04},   // COM3
        {0x3E, 0x1A},   // COM14
        {0x70, 0x3A},   // SCALING_XSC
        {0x71, 0x35},   // SCALING_YSC
        {0x72, 0x22},   // SCALING_DCWCTR
        {0x73, 0xF2},   // SCALING_PCLK_DIV
        {0xA2, 0x02},   // SCALING_PCLK_DELAY
        {0x17, 0x16},   // HSTART
        {0x18, 0x04},   // HSTOP
        {0x19, 0x02},   // VSTART
        {0x1A, 0x7A},   // VSTOP
        {0x32, 0xA4},   // HREF
        {0x03, 0x0A}    // VREF
    };
    static RegisterRec const s_YUV422_QQCIF[] PROGMEM = {      // YUV QQCIF (88x72 2byte/pixel)
        {0x12, 0x00},   // COM7
        {0x0C, 0x0C},   // COM3
        {0x3E, 0x12},   // COM14
        {0x70, 0x3A},   // SCALING_XSC
        {0x71, 0x35},   // SCALING_YSC
        {0x72, 0x22},   // SCALING_DCWCTR down sample by 4
        {0x73, 0xF2},   // SCALING_PCLK_DIV F2 = DSP clock div by 4
        {0xA2, 0x2A},   // SCALING_PCLK_DELAY
        {0x17, 0x16},   // HSTART
        {0x18, 0x04},   // HSTOP
        {0x19, 0x02},   // VSTART
        {0x1A, 0x7A},   // VSTOP
        {0x32, 0xA4},   // HREF
        {0x03, 0x0A}    // VREF
    };
    static ModeRec const s_mode[CAMERA_LIMIT] PROGMEM = {
        {s_RAW_VGA,      lengthof(s_RAW_VGA)},
        {s_RAW_QVGA,     lengthof(s_RAW_QVGA)},
        {s_RAW_QCIF,     lengthof(s_RAW_QCIF)},
        {s_RGB565_QVGA,  lengthof(s_RGB565_QVGA)},
        {s_RGB565_QQVGA, lengthof(s_RGB565_QQVGA)},
        {s_RGB565_QQCIF, lengthof(s_RGB565_QQCIF)},
        {s_YUV422_QVGA,  lengthof(s_YUV422_QVGA)},
        {s_YUV422_QQVGA, lengthof(s_YUV422_QQVGA)},
        {s_YUV422_QQCIF, lengthof(s_YUV422_QQCIF)}
    };
    static unsigned long (*const s_getSize[STORAGE_LIMIT])(void) = {
        NULL, &TSTMorikawa::getSizeSharedMemory, &TSTMorikawa::getSizeFRAM, &TSTMorikawa::getSizeFlashROM
    };
    static unsigned int (*const s_getPageSize[STORAGE_LIMIT])(void) = {
        NULL, &TSTMorikawa::getPageSizeSharedMemory, &TSTMorikawa::getPageSizeFRAM, &TSTMorikawa::getPageSizeFlashROM
    };
    static TSTError (TSTMorikawa::*const s_write[STORAGE_LIMIT])(unsigned long, void const*, unsigned int, unsigned int*) = {
        NULL, &TSTMorikawa::writeSharedMemory, &TSTMorikawa::writeFRAM, &TSTMorikawa::writeFlashROM
    };
    unsigned char temp[256];
    register unsigned int page;
    register int width;
    register int height;
    register int depth;
    unsigned long total;
    register int x;
    register int y;
    register int i;
    register int j;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((0 <= mode && mode < CAMERA_LIMIT) && result != NULL) {
        if (STORAGE_NONE < storage && storage < STORAGE_LIMIT) {
            if (address + size <= (*s_getSize[storage])()) {
                if ((page = (*s_getPageSize[storage])()) <= sizeof(temp)) {
                    width = pgm_read_word(&g_format[mode].width);
                    height = pgm_read_word(&g_format[mode].height);
                    depth = pgm_read_byte(&g_format[mode].depth);
                    total = static_cast<unsigned long>(width) * height * depth;
                    if (size >= total) {
                        if (_morikawa != NULL) {
                            if (!_morikawa->hasAbnormalShutdown()) {
                                wake();
                                reset();
                                for (i = 0; i < 128; ++i) {
                                    if (_morikawa->hasAbnormalShutdown()) {
                                        error = TSTERROR_ABNORMAL_SHUTDOWN;
                                        break;
                                    }
                                    read();
                                }
                                if (error == TSTERROR_OK) {
                                    if ((error = transfer(s_initialize, lengthof(s_initialize))) == TSTERROR_OK) {
                                        if ((error = transfer(reinterpret_cast<RegisterRec const PROGMEM*>(pgm_read_word(&s_mode[mode].data)), pgm_read_word(&s_mode[mode].length))) == TSTERROR_OK) {
                                            if ((error = synchronize(true)) == TSTERROR_OK) {
                                                digitalWrite(PIN_CAMERA_WEN, HIGH);
                                                if ((error = synchronize(false)) == TSTERROR_OK) {
                                                    error = synchronize(true);
                                                }
                                                digitalWrite(PIN_CAMERA_WEN, LOW);
                                                if (error == TSTERROR_OK) {
                                                    if ((error = transfer(s_standby, lengthof(s_standby))) == TSTERROR_OK) {
                                                        reset();
                                                        for (y = 0, j = 0; y < height; ++y) {
                                                            for (x = 0; x < width; ++x) {
                                                                for (i = 0; i < depth; ++i) {
                                                                    if (_morikawa->hasAbnormalShutdown()) {
                                                                        error = TSTERROR_ABNORMAL_SHUTDOWN;
                                                                        break;
                                                                    }
                                                                    temp[j] = read();
                                                                    if (++j >= page) {
                                                                        if ((error = (_morikawa->*s_write[storage])(address, temp, j, NULL)) == TSTERROR_OK) {
                                                                            address += j;
                                                                        }
                                                                        else {
                                                                            break;
                                                                        }
                                                                        j = 0;
                                                                    }
                                                                }
                                                                if (error != TSTERROR_OK) {
                                                                    break;
                                                                }
                                                            }
                                                            if (error != TSTERROR_OK) {
                                                                break;
                                                            }
                                                        }
                                                        if (error == TSTERROR_OK) {
                                                            if ((error = (_morikawa->*s_write[storage])(address, temp, j, NULL)) == TSTERROR_OK) {
                                                                *result = total;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                sleep();
                            }
                            else {
                                error = TSTERROR_INVALID_STATE;
                            }
                        }
                        else {
                            error = TSTERROR_INVALID_STATE;
                        }
                    }
                    else {
                        error = TSTERROR_INVALID_PARAM;
                    }
                }
                else {
                    error = TSTERROR_INVALID_IMPLEMENT;
                }
            }
            else {
                error = TSTERROR_INVALID_PARAM;
            }
        }
        else {
            error = TSTERROR_INVALID_STORAGE;
        }
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*private static */void TSTCamera::wake(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    digitalWrite(PIN_CAMERA_POWER, HIGH);
    digitalWrite(PIN_CAMERA_WEN, LOW);
    digitalWrite(PIN_CAMERA_RRST, HIGH);
    digitalWrite(PIN_CAMERA_OE, HIGH);
    digitalWrite(PIN_CAMERA_RCLK, LOW);
    digitalWrite(PIN_CAMERA_OE, LOW);
    delay(BOOT_DELAY);
    return;
}

/*private static */void TSTCamera::sleep(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    digitalWrite(PIN_CAMERA_OE, HIGH);
    digitalWrite(PIN_CAMERA_POWER, LOW);
    return;
}

/*private static */void TSTCamera::reset(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    digitalWrite(PIN_CAMERA_RRST, LOW);
    digitalWrite(PIN_CAMERA_RCLK, HIGH);
    delayMicroseconds(1);
    digitalWrite(PIN_CAMERA_RCLK, LOW);
    digitalWrite(PIN_CAMERA_RRST, HIGH);
    return;
}

/*private static */unsigned char TSTCamera::read(void)
{
    unsigned char result(PINC);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    digitalWrite(PIN_CAMERA_RCLK, HIGH);
    delayMicroseconds(1);
    digitalWrite(PIN_CAMERA_RCLK, LOW);
    return result;
}

/*private */TSTError TSTCamera::synchronize(bool state) const
{
    register unsigned char value;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    value = (state) ? (HIGH) : (LOW);
    while (digitalRead(PIN_CAMERA_VSYNC) == value) {
        if (_morikawa->hasAbnormalShutdown()) {
            error = TSTERROR_ABNORMAL_SHUTDOWN;
            break;
        }
    }
    return error;
}

/*private */TSTError TSTCamera::transfer(register RegisterRec const PROGMEM* data, register unsigned int length) const
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    for (; length > 0; --length, ++data) {
        if (_morikawa->hasAbnormalShutdown()) {
            error = TSTERROR_ABNORMAL_SHUTDOWN;
            break;
        }
        SCCB.beginTransmission(SCCB_ADDRESS);
        SCCB.write(pgm_read_byte(&data->address));
        SCCB.write(pgm_read_byte(&data->value));
        SCCB.endTransmission();
    }
    if (error == TSTERROR_OK) {
        delay(REGISTER_DELAY);
    }
    return error;
}

}// end of namespace
