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
**      TSTSharedMemory.cpp
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

#include "TSTSharedMemory.h"
#include "I2Cm.h"
#include "TSTMorikawa.h"

namespace tst {

#define ADDRESS_LIMIT       (0x80000UL)
#define ADDRESS_BOOT        (0x7FF80UL)
#define ADDRESS_SELFTESTLOG (0x7FF00UL)
#define ADDRESS_TEXT_X      (0x7FE80UL)
#define ADDRESS_TEXT_Y      (0x7FE00UL)
#define ADDRESS_TEXT_Z      (0x7FD80UL)
#define ADDRESS_TEXT_DEBUG  (0x7FD00UL)
#define ADDRESS_NOTE        (0x7FC00UL)
#define ADDRESS_DIGITALKER  (0x7FB80UL)
#define ADDRESS_MORSE       (0x7FB40UL)
#define ADDRESS_CAMERA      (0x7FB00UL)
#define SECTOR_LIMIT        (8)
#define SECTOR_SIZE         (static_cast<unsigned long>(PAGE_LIMIT) * PAGE_SIZE)
#define PAGE_LIMIT          (512)
#define PAGE_SIZE           (128)

static unsigned long const g_text[TEXT_LIMIT] PROGMEM = {
    ADDRESS_TEXT_X,
    ADDRESS_TEXT_Y,
    ADDRESS_TEXT_Z,
    ADDRESS_TEXT_DEBUG
};

/*public static */unsigned long TSTSharedMemory::getSize(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    return ADDRESS_CAMERA;
}

/*public static */unsigned int TSTSharedMemory::getPageSize(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    return PAGE_SIZE;
}

/*public static */unsigned long TSTSharedMemory::getSectorSize(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    return 0;
}

/*public */TSTError TSTSharedMemory::setup(TSTMorikawa* morikawa)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (morikawa != NULL) {
        if (_morikawa == NULL) {
            _morikawa = morikawa;
        }
        else {
            error = TSTERROR_INVALID_STATE;
        }
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public */void TSTSharedMemory::cleanup(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        _morikawa = NULL;
    }
    return;
}

/*public */TSTError TSTSharedMemory::write(unsigned long address, void const* data, unsigned int size, unsigned int* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkGeneral(address, data, &size, result)) == TSTERROR_OK) {
        error = writeGeneral(address, static_cast<unsigned char const*>(data), NULL, size);
    }
    return error;
}

/*public */TSTError TSTSharedMemory::writePGM(unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkGeneral(address, data, &size, result)) == TSTERROR_OK) {
        error = writeGeneral(address, NULL, static_cast<unsigned char const PROGMEM*>(data), size);
    }
    return error;
}

/*public */TSTError TSTSharedMemory::writeSelfTestLog(SelfTestLog const& log)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (sizeof(log) < ADDRESS_BOOT - ADDRESS_SELFTESTLOG) {
        if (_morikawa != NULL) {
            if (!_morikawa->hasAbnormalShutdown()) {
                error = writeSpecial(ADDRESS_SELFTESTLOG, reinterpret_cast<unsigned char const*>(&log), NULL, sizeof(log));
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
        error = TSTERROR_INVALID_IMPLEMENT;
    }
    return error;
}

/*public */TSTError TSTSharedMemory::writeTextResult(TextType index, char const* text, int length)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkText(index, text, NULL, &length)) == TSTERROR_OK) {
        error = writeSpecial(pgm_read_dword(&g_text[index]), reinterpret_cast<unsigned char const*>(text), NULL, length);
    }
    return error;
}

/*public */TSTError TSTSharedMemory::writeTextResultPGM(TextType index, char const PROGMEM* text, int length)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkText(index, NULL, text, &length)) == TSTERROR_OK) {
        error = writeSpecial(pgm_read_dword(&g_text[index]), NULL, reinterpret_cast<unsigned char const PROGMEM*>(text), length);
    }
    return error;
}

/*public */TSTError TSTSharedMemory::read(unsigned long address, void* data, unsigned int size, unsigned int* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkGeneral(address, data, &size, result)) == TSTERROR_OK) {
        error = readGeneral(address, static_cast<unsigned char*>(data), size);
    }
    return error;
}

/*public */TSTError TSTSharedMemory::readBootParam(BootParamRec* result)
{
    BootParamRec temp[3];
    register int i;
    register int j;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkParam(result)) == TSTERROR_OK) {
        for (i = 0; i < lengthof(temp); ++i) {
            if ((error = readSpecial(ADDRESS_BOOT + sizeof(temp[i]) * i, reinterpret_cast<unsigned char*>(&temp[i]), sizeof(temp[i]))) == TSTERROR_OK) {
                temp[i].time = ((temp[i].time & 0x000000FFUL) << 24) |
                               ((temp[i].time & 0x0000FF00UL) <<  8) |
                               ((temp[i].time & 0x00FF0000UL) >>  8) |
                               ((temp[i].time & 0xFF000000UL) >> 24);
            }
            else {
                break;
            }
        }
        if (error == TSTERROR_OK) {
            result->time = 0;
            result->count = 1;
            result->mode = 0;
            for (i = 0; i < lengthof(temp); ++i) {
                j = (i + 1) % lengthof(temp);
                if (temp[i].time == temp[j].time) {
                    result->time = temp[i].time;
                }
                if (temp[i].count == temp[j].count) {
                    result->count = temp[i].count;
                }
                if (temp[i].mode == temp[j].mode) {
                    result->mode = temp[i].mode;
                }
            }
        }
    }
    return error;
}

/*public */TSTError TSTSharedMemory::readTextParam(TextType index, char* text, unsigned int length, int* result)
{
    unsigned long address;
    unsigned char temp;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkText(index)) == TSTERROR_OK) {
        address = pgm_read_dword(&g_text[index]);
        if ((error = readSpecial(address, &temp, sizeof(temp))) == TSTERROR_OK) {
            if (temp < PAGE_SIZE) {
                ++temp;
                if (text != NULL && length > 0) {
                    length = min(length, temp) - 1;
                    if ((error = readSpecial(address + sizeof(temp), reinterpret_cast<unsigned char*>(text), length)) == TSTERROR_OK) {
                        text[length] = '\0';
                    }
                }
                if (result != NULL) {
                    *result = temp;
                }
            }
            else {
                error = TSTERROR_FAILED;
            }
        }
    }
    return error;
}

/*public */TSTError TSTSharedMemory::readNoteParam(NoteParam* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkParam(result)) == TSTERROR_OK) {
        error = readParam(ADDRESS_NOTE, result->data, sizeof(result->data), &result->size);
    }
    return error;
}

/*public */TSTError TSTSharedMemory::readMorseParam(MorseParam* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkParam(result)) == TSTERROR_OK) {
        error = readParam(ADDRESS_MORSE, result->data, sizeof(result->data), &result->size);
    }
    return error;
}

/*public */TSTError TSTSharedMemory::readDigitalkerParam(DigitalkerParam* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkParam(result)) == TSTERROR_OK) {
        error = readParam(ADDRESS_DIGITALKER, result->data, sizeof(result->data), &result->size);
    }
    return error;
}

/*public */TSTError TSTSharedMemory::readCameraParam(CameraParam* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkParam(result)) == TSTERROR_OK) {
        error = readParam(ADDRESS_CAMERA, result->data, sizeof(result->data), &result->size);
    }
    return error;
}

/*public */TSTError TSTSharedMemory::format(void)
{
    register unsigned long address;
    register int i2c;
    register unsigned int i;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        if (!_morikawa->hasAbnormalShutdown()) {
            for (address = 0; address < ADDRESS_CAMERA; ) {
                if (_morikawa->hasAbnormalShutdown()) {
                    error = TSTERROR_ABNORMAL_SHUTDOWN;
                    break;
                }
                i2c = control(address);
                for (i = 0; i < PAGE_SIZE; ++i, ++address) {
                    I2Cm.write(0xFF);
                }
                if ((error = send(i2c, true)) != TSTERROR_OK) {
                    break;
                }
            }
        }
        else {
            error = TSTERROR_INVALID_STATE;
        }
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*private */TSTError TSTSharedMemory::checkGeneral(unsigned long address, void const* data, unsigned int* size, unsigned int* result) const
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = checkParam(data)) == TSTERROR_OK) {
        if (address < ADDRESS_CAMERA) {
            if (address + *size > ADDRESS_CAMERA) {
                if (result != NULL) {
                    *size = ADDRESS_CAMERA - address;
                }
                else {
                    error = TSTERROR_INVALID_FORMAT;
                }
            }
            if (error == TSTERROR_OK) {
                if (result != NULL) {
                    *result = *size;
                }
            }
        }
        else {
            error = TSTERROR_INVALID_FORMAT;
        }
    }
    return error;
}

/*private */TSTError TSTSharedMemory::checkParam(void const* data) const
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (data != NULL) {
        if (_morikawa != NULL) {
            if (_morikawa->hasAbnormalShutdown()) {
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
    return error;
}

/*private */TSTError TSTSharedMemory::checkText(TextType index, char const* ram, char const PROGMEM* rom, int* length) const
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (ram != NULL || rom != NULL) {
        if ((error = checkText(index)) == TSTERROR_OK) {
            if (*length < 0) {
                *length = (ram != NULL) ? (strlen(ram)) : (strlen_P(rom));
            }
            if (*length >= PAGE_SIZE) {
                error = TSTERROR_INVALID_FORMAT;
            }
        }
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*private */TSTError TSTSharedMemory::checkText(TextType index) const
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (0 <= index && index < TEXT_LIMIT) {
        if (_morikawa != NULL) {
            if (_morikawa->hasAbnormalShutdown()) {
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
    return error;
}

/*private */TSTError TSTSharedMemory::writeGeneral(unsigned long address, unsigned char const* ram, unsigned char const PROGMEM* rom, unsigned int size)
{
    register unsigned int pb;
    register unsigned int pe;
    register unsigned int mb;
    register unsigned int me;
    register unsigned int i;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (size > 0) {
        pb = address / PAGE_SIZE;
        pe = (address + size - 1) / PAGE_SIZE;
        mb = address % PAGE_SIZE;
        me = PAGE_SIZE;
        for (i = pb; i <= pe; ++i) {
            if (i >= pe) {
                me = (address + size - 1) % PAGE_SIZE + 1;
            }
            if (me - mb < PAGE_SIZE) {
                if ((error = writeByte(static_cast<unsigned long>(i) * PAGE_SIZE + mb, &ram, &rom, me - mb)) != TSTERROR_OK) {
                    break;
                }
            }
            else if ((error = writePage(static_cast<unsigned long>(i) * PAGE_SIZE, &ram, &rom)) != TSTERROR_OK) {
                break;
            }
            if (i <= pb) {
                mb = 0;
            }
        }
    }
    return error;
}

/*private */TSTError TSTSharedMemory::writeSpecial(unsigned long address, unsigned char const* ram, unsigned char const PROGMEM* rom, unsigned int size)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    return writePage(address, &ram, &rom, size);
}

/*private */TSTError TSTSharedMemory::writeByte(unsigned long address, register unsigned char const** ram, register unsigned char const PROGMEM** rom, register unsigned int size)
{
    register int i2c;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    for (; size > 0; --size, ++address) {
        if (_morikawa->hasAbnormalShutdown()) {
            error = TSTERROR_ABNORMAL_SHUTDOWN;
            break;
        }
        i2c = control(address);
        if (*ram != NULL) {
            I2Cm.write(**ram);
            ++*ram;
        }
        else {
            I2Cm.write(pgm_read_byte(*rom));
            ++*rom;
        }
        if ((error = send(i2c, true)) != TSTERROR_OK) {
            break;
        }
    }
    return error;
}

/*private */TSTError TSTSharedMemory::writePage(unsigned long address, register unsigned char const** ram, register unsigned char const PROGMEM** rom)
{
    register int i2c;
    register unsigned int i;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (!_morikawa->hasAbnormalShutdown()) {
        i2c = control(address);
        if (*ram != NULL) {
            for (i = 0; i < PAGE_SIZE; ++i, ++*ram) {
                I2Cm.write(**ram);
            }
        }
        else {
            for (i = 0; i < PAGE_SIZE; ++i, ++*rom) {
                I2Cm.write(pgm_read_byte(*rom));
            }
        }
        error = send(i2c, true);
    }
    else {
        error = TSTERROR_ABNORMAL_SHUTDOWN;
    }
    return error;
}

/*private */TSTError TSTSharedMemory::writePage(unsigned long address, register unsigned char const** ram, register unsigned char const PROGMEM** rom, register unsigned int size)
{
    register int i2c;
    register unsigned int i;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (!_morikawa->hasAbnormalShutdown()) {
        i2c = control(address);
        I2Cm.write(size);
        if (*ram != NULL) {
            for (i = 0; i < size; ++i, ++*ram) {
                I2Cm.write(**ram);
            }
        }
        else {
            for (i = 0; i < size; ++i, ++*rom) {
                I2Cm.write(pgm_read_byte(*rom));
            }
        }
        for (; i < PAGE_SIZE - 1; ++i) {
            I2Cm.write(0xFF);
        }
        error = send(i2c, true);
    }
    else {
        error = TSTERROR_ABNORMAL_SHUTDOWN;
    }
    return error;
}

/*private */TSTError TSTSharedMemory::readGeneral(unsigned long address, unsigned char* data, unsigned int size)
{
    register unsigned int pb;
    register unsigned int pe;
    register unsigned long mb;
    register unsigned long me;
    register unsigned int i;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (size > 0) {
        pb = address / SECTOR_SIZE;
        pe = (address + size - 1) / SECTOR_SIZE;
        mb = address % SECTOR_SIZE;
        me = SECTOR_SIZE;
        for (i = pb; i <= pe; ++i) {
            if (i >= pe) {
                me = (address + size - 1) % SECTOR_SIZE + 1;
            }
            if ((error = readSequence(static_cast<unsigned long>(i) * SECTOR_SIZE + mb, &data, me - mb)) != TSTERROR_OK) {
                break;
            }
            if (i <= pb) {
                mb = 0;
            }
        }
    }
    return error;
}

/*private */TSTError TSTSharedMemory::readParam(unsigned long address, unsigned char* data, unsigned int size, unsigned char* result)
{
    unsigned char temp;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = readSpecial(address, &temp, sizeof(temp))) == TSTERROR_OK) {
        if (temp <= size) {
            if ((error = readSpecial(address + sizeof(temp), data, temp)) == TSTERROR_OK) {
                *result = temp;
            }
        }
        else {
            error = TSTERROR_FAILED;
        }
    }
    return error;
}

/*private */TSTError TSTSharedMemory::readSpecial(unsigned long address, unsigned char* data, unsigned int size)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    return readSequence(address, &data, size);
}

/*private */TSTError TSTSharedMemory::readSequence(unsigned long address, register unsigned char** data, register unsigned int size)
{
    register int i2c;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    i2c = control(address);
    if ((error = send(i2c, false)) == TSTERROR_OK) {
        while (size > 0) {
            if (_morikawa->hasAbnormalShutdown()) {
                error = TSTERROR_ABNORMAL_SHUTDOWN;
                break;
            }
            else if (I2Cm.receive(i2c, min(size, PAGE_SIZE), false) > 0) {
                for (; size > 0; --size, ++*data) {
                    if (I2Cm.available() > 0) {
                        **data = I2Cm.read();
                    }
                    else {
                        break;
                    }
                }
            }
            else {
                error = TSTERROR_FAILED;
                break;
            }
        }
        I2Cm.receive(i2c, 0, true);
    }
    return error;
}

/*private static */unsigned char TSTSharedMemory::control(unsigned long address)
{
    static unsigned char const s_address[SECTOR_LIMIT] PROGMEM = {
        0x50,
        0x54,
        0x51,
        0x55,
        0x52,
        0x56,
        0x53,
        0x57
    };
    unsigned char result(pgm_read_byte(&s_address[(address >> 16) & 0x07]));
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    while (true) {
        I2Cm.clear();
        if (I2Cm.send(result, true) != 2) {
            break;
        }
    }
    I2Cm.clear();
    I2Cm.write((address >> 8) & 0xFF);
    I2Cm.write((address >> 0) & 0xFF);
    return result;
}

/*private static */TSTError TSTSharedMemory::send(unsigned char i2c, bool stop)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (I2Cm.send(i2c, stop) != 0) {
        error = TSTERROR_FAILED;
    }
    return error;
}

}// end of namespace
