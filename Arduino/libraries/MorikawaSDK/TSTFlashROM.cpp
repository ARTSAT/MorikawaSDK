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
**      TSTFlashROM.cpp
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

#include "TSTFlashROM.h"
#include "EEPROM.h"
#include "SPI.h"
#include "TSTMorikawa.h"

namespace tst {

#define ADDRESS_LIMIT       (0x100000UL)
#define SECTOR_LIMIT        (16)
#define SECTOR_SIZE         (static_cast<unsigned long>(PAGE_LIMIT) * PAGE_SIZE)
#define PAGE_LIMIT          (256)
#define PAGE_SIZE           (256)
#define PAGE_PER_EEPROM     (4)
#define POWERUP_DELAY       (40)
#define POWERDOWN_DELAY     (4)

enum CommandEnum {
    COMMAND_WREN            = 0x06,     // 0000 0110 WRITE ENABLE
    COMMAND_WRDI            = 0x04,     // 0000 0100 WRITE DISABLE
    COMMAND_WRSR            = 0x01,     // 0000 0001 WRITE STATUS REGISTER
    COMMAND_PGPGM           = 0x02,     // 0000 0010 PAGE PROGRAM
    COMMAND_SCTER           = 0xD8,     // 1101 1000 SECTOR ERASE
    COMMAND_BLKER           = 0xC7,     // 1100 0111 BULK ERASE
    COMMAND_RDSR            = 0x05,     // 0000 0101 READ STATUS REGISTER
    COMMAND_READ            = 0x03,     // 0000 0011 READ DATA BYTES
    COMMAND_PWDW            = 0xB9,     // 1011 1001 DEEP POWER-DOWN
    COMMAND_PWUP            = 0xAB      // 1010 1011 RELEASE from DEEP POWER-DOWN
};
enum StatusEnum {
    STATUS_WIP              = 1 << 0,   // write in progress bit
    STATUS_WEL              = 1 << 1,   // write enable latch bit
    STATUS_BP0              = 1 << 2,   // block protect bits 0
    STATUS_BP1              = 1 << 3,   // block protect bits 1
    STATUS_BP2              = 1 << 4,   // block protect bits 2
    STATUS_SRWD             = 1 << 7    // status register write protect
};
enum PageEnum {
    PAGE_GOOD               = 1 << 0,
    PAGE_FREE               = 1 << 1
};

/*public static */unsigned long TSTFlashROM::getSize(void)
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

/*public static */unsigned int TSTFlashROM::getPageSize(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
#ifdef TARGET_MORIKAWA_FM1
    return PAGE_SIZE;
#else
    return 0;
#endif
}

/*public static */unsigned long TSTFlashROM::getSectorSize(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
#ifdef TARGET_MORIKAWA_FM1
    return SECTOR_SIZE;
#else
    return 0;
#endif
}

/*public */TSTError TSTFlashROM::setEraseMode(bool param)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        _erase = param;
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*public */bool TSTFlashROM::getEraseMode(void) const
{
    bool result(false);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        result = _erase;
    }
    return result;
}

/*public */TSTError TSTFlashROM::setup(TSTMorikawa* morikawa)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
#ifdef TARGET_MORIKAWA_FM1
    if (morikawa != NULL) {
        if (_morikawa == NULL) {
            _morikawa = morikawa;
            _erase = false;
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

/*public */void TSTFlashROM::cleanup(void)
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

/*public */TSTError TSTFlashROM::write(unsigned long address, void const* data, unsigned int size, unsigned int* result)
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

/*public */TSTError TSTFlashROM::writePGM(unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result)
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

/*public */TSTError TSTFlashROM::read(unsigned long address, void* data, unsigned int size, unsigned int* result)
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

/*public */TSTError TSTFlashROM::format(void)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
#ifdef TARGET_MORIKAWA_FM1
    if (_morikawa != NULL) {
        if (!_morikawa->hasAbnormalShutdown()) {
            if ((error = fillFormat(0x00)) == TSTERROR_OK) {
                if ((error = verifyFormat(0x00, true)) == TSTERROR_OK) {
                    executeCommand(COMMAND_WREN);
                    executeCommand(COMMAND_BLKER);
                    error = verifyFormat(0xFF, false);
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
#else
    error = TSTERROR_NO_SUPPORT;
#endif
    return error;
}

/*private static */void TSTFlashROM::open(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    pinMode(PIN_FLASHROM_CS, OUTPUT);
    digitalWrite(PIN_FLASHROM_CS, HIGH);
    delayMicroseconds(1);
    executeCommand(COMMAND_PWUP);
    delayMicroseconds(POWERUP_DELAY);
    return;
}

/*private static */void TSTFlashROM::close(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    executeCommand(COMMAND_PWDW);
    delayMicroseconds(POWERDOWN_DELAY);
    digitalWrite(PIN_FLASHROM_CS, HIGH);
    return;
}

/*private */TSTError TSTFlashROM::check(unsigned long address, void const* data, unsigned int* size, unsigned int* result) const
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

/*private */TSTError TSTFlashROM::write(unsigned long address, unsigned char const* ram, unsigned char const PROGMEM* rom, unsigned int size)
{
    unsigned char sector[SECTOR_LIMIT / 8 + 1];
    register unsigned int pb;
    register unsigned int pe;
    register unsigned char flag;
    register unsigned int shift;
    register unsigned int mb;
    register unsigned int me;
    register unsigned int i;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (size > 0) {
        memset(sector, 0, sizeof(sector));
        pb = address / PAGE_SIZE;
        pe = (address + size - 1) / PAGE_SIZE;
        if (pb % PAGE_PER_EEPROM != 0) {
            flag = readEEPROM(pb / PAGE_PER_EEPROM);
        }
        for (i = pb; i <= pe; ++i) {
            if (i % PAGE_PER_EEPROM == 0) {
                flag = readEEPROM(i / PAGE_PER_EEPROM);
            }
            shift = (PAGE_PER_EEPROM - 1 - i % PAGE_PER_EEPROM) * 8 / PAGE_PER_EEPROM;
            if (_morikawa->hasAbnormalShutdown()) {
                error = TSTERROR_ABNORMAL_SHUTDOWN;
                break;
            }
            else if (!(flag & (PAGE_GOOD << shift))) {
                error = TSTERROR_BAD_MEMORY;
                break;
            }
            else if (!(flag & (PAGE_FREE << shift))) {
                sector[i / PAGE_LIMIT / 8] |= 1 << (i / PAGE_LIMIT % 8);
            }
        }
        if (error == TSTERROR_OK) {
            for (i = 0; i < SECTOR_LIMIT; ++i) {
                if (sector[i / 8] & (1 << (i % 8))) {
                    if (_morikawa->hasAbnormalShutdown()) {
                        error = TSTERROR_ABNORMAL_SHUTDOWN;
                        break;
                    }
                    else if (_erase) {
                        eraseSector(i);
                    }
                    else {
                        error = TSTERROR_USED_MEMORY;
                        break;
                    }
                }
            }
            if (error == TSTERROR_OK) {
                mb = address % PAGE_SIZE;
                me = PAGE_SIZE;
                flag = 0;
                for (i = pb; i <= pe; ++i) {
                    if (i >= pe) {
                        me = (address + size - 1) % PAGE_SIZE + 1;
                    }
                    if (_morikawa->hasAbnormalShutdown()) {
                        error = TSTERROR_ABNORMAL_SHUTDOWN;
                        break;
                    }
                    while (readStatus() & STATUS_WIP);
                    executeCommand(COMMAND_WREN);
                    beginTransfer(COMMAND_PGPGM);
                    transferAddress(static_cast<unsigned long>(i) * PAGE_SIZE);
                    transferValue(0xFF, mb - 0);
                    transferData(&ram, &rom, me - mb);
                    transferValue(0xFF, PAGE_SIZE - me);
                    endTransfer();
                    shift = (PAGE_PER_EEPROM - 1 - i % PAGE_PER_EEPROM) * 8 / PAGE_PER_EEPROM;
                    flag |= PAGE_FREE << shift;
                    if (i % PAGE_PER_EEPROM == PAGE_PER_EEPROM - 1) {
                        andEEPROM(i / PAGE_PER_EEPROM, ~flag);
                        flag = 0;
                    }
                    if (i <= pb) {
                        mb = 0;
                    }
                }
                if (i % PAGE_PER_EEPROM != 0) {
                    andEEPROM(i / PAGE_PER_EEPROM, ~flag);
                }
            }
        }
    }
    return error;
}

/*private */TSTError TSTFlashROM::read(unsigned long address, register unsigned char* data, register unsigned int size)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (size > 0) {
        while (readStatus() & STATUS_WIP);
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

/*private static */void TSTFlashROM::eraseSector(register unsigned int sector)
{
    register unsigned char flag;
    register unsigned int i;
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    while (readStatus() & STATUS_WIP);
    executeCommand(COMMAND_WREN);
    beginTransfer(COMMAND_SCTER);
    transferAddress(static_cast<unsigned long>(sector) * SECTOR_SIZE);
    endTransfer();
    flag = 0;
    for (i = 0; i < PAGE_PER_EEPROM; ++i) {
        flag <<= 8 / PAGE_PER_EEPROM;
        flag |= PAGE_FREE;
    }
    sector *= PAGE_LIMIT / PAGE_PER_EEPROM;
    for (i = 0; i < PAGE_LIMIT / PAGE_PER_EEPROM; ++i) {
        orEEPROM(sector + i, flag);
    }
    return;
}

/*private */TSTError TSTFlashROM::fillFormat(register unsigned char value)
{
    register unsigned int i;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    for (i = 0; i < SECTOR_LIMIT * PAGE_LIMIT; ++i) {
        if (_morikawa->hasAbnormalShutdown()) {
            error = TSTERROR_ABNORMAL_SHUTDOWN;
            break;
        }
        while (readStatus() & STATUS_WIP);
        executeCommand(COMMAND_WREN);
        beginTransfer(COMMAND_PGPGM);
        transferAddress(static_cast<unsigned long>(i) * PAGE_SIZE);
        transferValue(value, PAGE_SIZE);
        endTransfer();
    }
    return error;
}

/*private */TSTError TSTFlashROM::verifyFormat(register unsigned char value, bool overwrite)
{
    register unsigned char flag;
    register unsigned int i;
    register unsigned int j;
    register unsigned int k;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    while (readStatus() & STATUS_WIP);
    beginTransfer(COMMAND_READ);
    transferAddress(0);
    for (k = 0; k < SECTOR_LIMIT * PAGE_LIMIT / PAGE_PER_EEPROM; ++k) {
        if (_morikawa->hasAbnormalShutdown()) {
            error = TSTERROR_ABNORMAL_SHUTDOWN;
            break;
        }
        flag = 0;
        for (j = 0; j < PAGE_PER_EEPROM; ++j) {
            flag <<= 8 / PAGE_PER_EEPROM;
            for (i = 0; i < PAGE_SIZE; ++i) {
                if (SPI.transfer(0x00) != value) {
                    break;
                }
            }
            if (i >= PAGE_SIZE) {
                flag |= PAGE_GOOD;
            }
            flag |= PAGE_FREE;
        }
        if (!overwrite) {
            flag &= readEEPROM(k);
        }
        writeEEPROM(k, flag);
    }
    endTransfer();
    return error;
}

/*private static */void TSTFlashROM::executeCommand(unsigned char command)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    beginTransfer(command);
    endTransfer();
    return;
}

/*private static */unsigned char TSTFlashROM::readStatus(void)
{
    unsigned char result(0);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    beginTransfer(COMMAND_RDSR);
    result = SPI.transfer(0x00);
    endTransfer();
    return result;
}

/*private static */void TSTFlashROM::beginTransfer(unsigned char command)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    digitalWrite(PIN_FLASHROM_CS, LOW);
    SPI.transfer(command);
    return;
}

/*private static */void TSTFlashROM::endTransfer(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    digitalWrite(PIN_FLASHROM_CS, HIGH);
    delayMicroseconds(1);
    return;
}

/*private static */void TSTFlashROM::transferAddress(unsigned long address)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    SPI.transfer((address >> 16) & 0x0F);
    SPI.transfer((address >> 8) & 0xFF);
    SPI.transfer((address >> 0) & 0xFF);
    return;
}

/*private static */void TSTFlashROM::transferValue(register unsigned char value, register unsigned int size)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    for (; size > 0; --size) {
        SPI.transfer(value);
    }
    return;
}

/*private static */void TSTFlashROM::transferData(register unsigned char const** ram, register unsigned char const PROGMEM** rom, register unsigned int size)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (*ram != NULL) {
        for (; size > 0; --size, ++*ram) {
            SPI.transfer(**ram);
        }
    }
    else {
        for (; size > 0; --size, ++*rom) {
            SPI.transfer(pgm_read_byte(*rom));
        }
    }
    return;
}

/*private static */void TSTFlashROM::writeEEPROM(unsigned int address, unsigned char data)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    EEPROM.write(address + EEPROM_FLASHROM, data);
    return;
}

/*private static */unsigned char TSTFlashROM::readEEPROM(unsigned int address)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    return EEPROM.read(address + EEPROM_FLASHROM);
}

/*private static */void TSTFlashROM::andEEPROM(unsigned int address, unsigned char data)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    EEPROM.write(address + EEPROM_FLASHROM, EEPROM.read(address + EEPROM_FLASHROM) & data);
    return;
}

/*private static */void TSTFlashROM::orEEPROM(unsigned int address, unsigned char data)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    EEPROM.write(address + EEPROM_FLASHROM, EEPROM.read(address + EEPROM_FLASHROM) | data);
    return;
}

}// end of namespace
