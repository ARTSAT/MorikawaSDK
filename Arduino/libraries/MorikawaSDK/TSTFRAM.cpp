/*
**      ARTSAT MorikawaSDK
**
**      Original Copyright (C) 2013 - 2013 NAKAZAWA Kent.
**                                          http://blog.kemushicomputer.com
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
**      TSTFRAM.cpp
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

#include "TSTFRAM.h"
#include "SPI.h"
#include "TSTMorikawa.h"

namespace tst {

#define ADDRESS_LIMIT       (0x20000UL)
#define WAKE_DELAY          (500)
#define SLEEP_DELAY         (5)

enum CommandEnum {
    COMMAND_WREN            = 0x06,     // 0000 0110 Set Write Enable Latch
    COMMAND_WRDI            = 0x04,     // 0000 0100 Write Disable
    COMMAND_WRSR            = 0x01,     // 0000 0001 Write Status Register
    COMMAND_WRITE           = 0x02,     // 0000 0010 Write Memory Data
    COMMAND_RDSR            = 0x05,     // 0000 0101 Read Status Register
    COMMAND_READ            = 0x03,     // 0000 0011 Read Memory Data
    COMMAND_SLEEP           = 0xB9      // 1011 1001 Enter Sleep Mode
};
enum StatusEnum {
    STATUS_WEL              = 1 << 1,
    STATUS_BP0              = 1 << 2,
    STATUS_BP1              = 1 << 3,
    STATUS_WPEN             = 1 << 7
};

/*public static */unsigned long TSTFRAM::getSize(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
#ifdef TARGET_MORIKAWA_FM1
    return ADDRESS_LIMIT;
#else
    return 0;
#endif
}

/*public static */unsigned int TSTFRAM::getPageSize(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    return 0;
}

/*public static */unsigned long TSTFRAM::getSectorSize(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    return 0;
}

/*public */TSTError TSTFRAM::setup(TSTMorikawa* morikawa)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
#ifdef TARGET_MORIKAWA_FM1
    if (morikawa != NULL) {
        if (_morikawa == NULL) {
            _morikawa = morikawa;
            open();
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

/*public */void TSTFRAM::cleanup(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        close();
        _morikawa = NULL;
    }
    return;
}

/*public */TSTError TSTFRAM::write(unsigned long address, void const* data, unsigned int size, unsigned int* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = check(address, data, &size, result)) == TSTERROR_OK) {
        error = write(address, static_cast<unsigned char const*>(data), NULL, size);
    }
    return error;
}

/*public */TSTError TSTFRAM::writePGM(unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = check(address, data, &size, result)) == TSTERROR_OK) {
        error = write(address, NULL, static_cast<unsigned char const PROGMEM*>(data), size);
    }
    return error;
}

/*public */TSTError TSTFRAM::read(unsigned long address, void* data, unsigned int size, unsigned int* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if ((error = check(address, data, &size, result)) == TSTERROR_OK) {
        error = read(address, static_cast<unsigned char*>(data), size);
    }
    return error;
}

/*public */TSTError TSTFRAM::format(void)
{
    register unsigned long address;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
#ifdef TARGET_MORIKAWA_FM1
    if (_morikawa != NULL) {
        if (!_morikawa->hasAbnormalShutdown()) {
            executeCommand(COMMAND_WREN);
            beginTransfer(COMMAND_WRITE);
            transferAddress(0);
            for (address = 0; address < ADDRESS_LIMIT; ++address) {
                if (_morikawa->hasAbnormalShutdown()) {
                    error = TSTERROR_ABNORMAL_SHUTDOWN;
                    break;
                }
                SPI.transfer(0xFF);
            }
            endTransfer();
        }
        else {
            error = TSTERROR_INVALID_STATE;
        }
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
#else
    error = TSTERROR_NO_SUPPORT;
#endif
    return error;
}

/*private static */void TSTFRAM::open(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    pinMode(PIN_FRAM_CS, OUTPUT);
    digitalWrite(PIN_FRAM_CS, HIGH);
    delayMicroseconds(1);
    digitalWrite(PIN_FRAM_CS, LOW);
    delayMicroseconds(WAKE_DELAY);
    digitalWrite(PIN_FRAM_CS, HIGH);
    return;
}

/*private static */void TSTFRAM::close(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    executeCommand(COMMAND_SLEEP);
    delayMicroseconds(SLEEP_DELAY);
    digitalWrite(PIN_FRAM_CS, HIGH);
    return;
}

/*private */TSTError TSTFRAM::check(unsigned long address, void const* data, unsigned int* size, unsigned int* result) const
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (data != NULL) {
        if (_morikawa != NULL) {
            if (!_morikawa->hasAbnormalShutdown()) {
                if (address < ADDRESS_LIMIT) {
                    if (address + *size > ADDRESS_LIMIT) {
                        if (result != NULL) {
                            *size = ADDRESS_LIMIT - address;
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
    return error;
}

/*private */TSTError TSTFRAM::write(unsigned long address, register unsigned char const* ram, register unsigned char const PROGMEM* rom, register unsigned int size)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (size > 0) {
        executeCommand(COMMAND_WREN);
        beginTransfer(COMMAND_WRITE);
        transferAddress(address);
        if (ram != NULL) {
            for (; size > 0; --size, ++ram) {
                if (_morikawa->hasAbnormalShutdown()) {
                    error = TSTERROR_ABNORMAL_SHUTDOWN;
                    break;
                }
                SPI.transfer(*ram);
            }
        }
        else {
            for (; size > 0; --size, ++rom) {
                if (_morikawa->hasAbnormalShutdown()) {
                    error = TSTERROR_ABNORMAL_SHUTDOWN;
                    break;
                }
                SPI.transfer(pgm_read_byte(rom));
            }
        }
        endTransfer();
    }
    return error;
}

/*private */TSTError TSTFRAM::read(unsigned long address, register unsigned char* data, register unsigned int size)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (size > 0) {
        beginTransfer(COMMAND_READ);
        transferAddress(address);
        for (; size > 0; --size, ++data) {
            if (_morikawa->hasAbnormalShutdown()) {
                error = TSTERROR_ABNORMAL_SHUTDOWN;
                break;
            }
            *data = SPI.transfer(0x00);
        }
        endTransfer();
    }
    return error;
}

/*private static */void TSTFRAM::executeCommand(unsigned char command)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    beginTransfer(command);
    endTransfer();
    return;
}

/*private static */void TSTFRAM::beginTransfer(unsigned char command)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    digitalWrite(PIN_FRAM_CS, LOW);
    SPI.transfer(command);
    return;
}

/*private static */void TSTFRAM::endTransfer(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    digitalWrite(PIN_FRAM_CS, HIGH);
    delayMicroseconds(1);
    return;
}

/*private static */void TSTFRAM::transferAddress(unsigned long address)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    SPI.transfer((address >> 16) & 0x01);
    SPI.transfer((address >> 8) & 0xFF);
    SPI.transfer((address >> 0) & 0xFF);
    return;
}

}// end of namespace
