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
**      TSTMorikawa.cpp
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

#include "TSTMorikawa.h"
#include "TimerOne.h"
#include "EEPROM.h"
#include "I2Cm.h"
#include "SPI.h"
#include "fastlz.h"
#include "TSTCriticalSection.h"

extern "C" {

extern unsigned int const __data_start;
extern unsigned int const __data_end;
extern unsigned int const __bss_start;
extern unsigned int const __bss_end;
extern void const* const __brkval;

}
namespace tst {

#define MISSION_MAIN_BAUD   (4800)
#define TIMER_INTERVAL      (80000UL)
#define BOOT_DELAY          (1000)
#define SHUTDOWN_DELAY      (1000)
#define AUDIOBUS_DELAY      (1000)
#define PACKET_SEPARATOR    ('-')
#define PACKET_REQUEST      ('c')
#define PACKET_RESPONSE     ('r')
#define PACKET_POWER        ('p')
#define PACKET_MAIN         ('c')
#define PACKET_MISSION      ('m')
// # -> mode
// ! -> remote
// * -> command
#define PACKET_SEND         ("#-!-m-***")
#define PACKET_RECEIVE      ("#-m-!-***")
#define PACKET_DELIMITER    (" \r\n")
#define COMMAND_ECHO        (PSTR("eco"))
#define COMMAND_TELEMETRY   (PSTR("smm"))
#define COMMAND_ABNORMALSD  (PSTR("asd"))
#define COMMAND_NORMALSD    (PSTR("nsd"))
#define COMMAND_AUDIOBUSON  (PSTR("don"))
#define COMMAND_AUDIOBUSOFF (PSTR("dof"))

static char const g_send[] PROGMEM = PACKET_SEND;
static char const g_receive[] PROGMEM = PACKET_RECEIVE;
static char const g_delimiter[] PROGMEM = PACKET_DELIMITER;

MemoryInfo TSTMorikawa::_memory = {
    0, RAMEND + 1, 0, RAMEND + 1
};
TSTTrinity<bool> TSTMorikawa::_selftest(false);

/*public static */TSTMorikawa& TSTMorikawa::getInstance(void)
{
    static TSTMorikawa s_singleton;
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    return s_singleton;
}

/*public static */TSTError TSTMorikawa::getMemorySpec(MemorySpec* result)
{
    TSTCriticalSection mutex;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (result != NULL) {
        mutex.lock();
        result->ram_start = &__data_start;
        result->ram_size = RAMEND - reinterpret_cast<size_t>(&__data_start) + 1;
        result->data_start = &__data_start;
        result->data_size = reinterpret_cast<size_t>(&__data_end) - reinterpret_cast<size_t>(&__data_start);
        result->bss_start = &__bss_start;
        result->bss_size = reinterpret_cast<size_t>(&__bss_end) - reinterpret_cast<size_t>(&__bss_start);
        result->heap_stack_start = __malloc_heap_start;
        result->heap_stack_size = RAMEND - reinterpret_cast<size_t>(__malloc_heap_start) + 1;
        result->heap_stack_margin = __malloc_margin;
        mutex.unlock();
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public static */TSTError TSTMorikawa::getMemoryInfo(MemoryInfo* result)
{
    TSTCriticalSection mutex;
    size_t heap_top;
    size_t limit;
    size_t stack_top;
    TSTError error(TSTERROR_OK);
    
    if (result != NULL) {
        mutex.lock();
        if ((heap_top = reinterpret_cast<size_t>(__brkval)) == 0) {
            heap_top = reinterpret_cast<size_t>(__malloc_heap_start);
        }
        if ((limit = reinterpret_cast<size_t>(__malloc_heap_end)) == 0) {
            limit = AVR_STACK_POINTER_REG - __malloc_margin;
        }
        if ((stack_top = reinterpret_cast<size_t>(__malloc_heap_end)) == 0) {
            stack_top = heap_top;
        }
        stack_top += __malloc_margin;
        result->heap_size = heap_top - reinterpret_cast<size_t>(__malloc_heap_start);
        result->heap_free = (limit > heap_top) ? (limit - heap_top) : (0);
        result->stack_size = RAMEND - AVR_STACK_POINTER_REG;
        result->stack_free = (AVR_STACK_POINTER_REG > stack_top) ? (AVR_STACK_POINTER_REG - stack_top) : (0);
        mutex.unlock();
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public static */TSTError TSTMorikawa::getMemoryLog(MemoryInfo* result)
{
    TSTCriticalSection mutex;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (result != NULL) {
        saveMemoryLog();
        mutex.lock();
        *result = _memory;
        mutex.unlock();
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public static */void TSTMorikawa::saveMemoryLog(void)
{
    TSTCriticalSection mutex;
    MemoryInfo info;
    TSTError log;
    
    if ((log = getMemoryInfo(&info)) == TSTERROR_OK) {
        mutex.lock();
        _memory.heap_size = max(info.heap_size, _memory.heap_size);
        _memory.heap_free = min(info.heap_free, _memory.heap_free);
        _memory.stack_size = max(info.stack_size, _memory.stack_size);
        _memory.stack_free = min(info.stack_free, _memory.stack_free);
        mutex.unlock();
    }
    else {
        writeSelfTestLog(offsetof(SelfTestLog, saveMemoryLog_getmemoryinfo), &log, sizeof(log));
    }
    return;
}

/*public static */TSTError TSTMorikawa::getSelfTestLog(SelfTestLog* result)
{
    TSTCriticalSection mutex;
    register unsigned char* temp;
    register unsigned int size;
    register unsigned int address;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (sizeof(*result) <= EEPROM_LIMIT - EEPROM_SELFTEST) {
        if ((temp = reinterpret_cast<unsigned char*>(result)) != NULL) {
            mutex.lock();
            for (size = sizeof(*result), address = EEPROM_SELFTEST; size > 0; --size, ++address, ++temp) {
                *temp = EEPROM.read(address);
            }
            mutex.unlock();
        }
        else {
            error = TSTERROR_INVALID_PARAM;
        }
    }
    else {
        error = TSTERROR_INVALID_IMPLEMENT;
    }
    return error;
}

/*public static */void TSTMorikawa::eraseSelfTestLog(void)
{
    TSTCriticalSection mutex;
    register unsigned int address;
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    mutex.lock();
    for (address = EEPROM_SELFTEST; address < EEPROM_LIMIT; ++address) {
        EEPROM.write(address, 0x00);
    }
    mutex.unlock();
    return;
}

/*public static */unsigned long TSTMorikawa::getSizeEEPROM(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    return EEPROM_FLASHROM;
}

/*public static */unsigned int TSTMorikawa::getPageSizeEEPROM(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    return 0;
}

/*public static */unsigned long TSTMorikawa::getSectorSizeEEPROM(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    return 0;
}

/*public */unsigned long TSTMorikawa::getBootTime(void) const
{
    unsigned long result(0);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (_state) {
        result = _param.time;
    }
    return result;
}

/*public */unsigned char TSTMorikawa::getBootCount(void) const
{
    unsigned char result(0);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (_state) {
        result = _param.count;
    }
    return result;
}

/*public */unsigned char TSTMorikawa::getBootMode(void) const
{
    unsigned char result(0);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (_state) {
        result = _param.mode;
    }
    return result;
}

/*public */TSTError TSTMorikawa::getTelemetryTime(TimeType type, unsigned long* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryTime(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, true, result);
    }
    return error;
}

/*public */TSTError TSTMorikawa::getTelemetryVoltage(VoltageType type, unsigned char* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryVoltage(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, false, result);
    }
    return error;
}

/*public */TSTError TSTMorikawa::getTelemetryVoltage(VoltageType type, double* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryVoltage(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, true, result);
    }
    return error;
}

/*public */TSTError TSTMorikawa::getTelemetryCurrent(CurrentType type, unsigned char* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryCurrent(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, false, result);
    }
    return error;
}

/*public */TSTError TSTMorikawa::getTelemetryCurrent(CurrentType type, double* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryCurrent(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, true, result);
    }
    return error;
}

/*public */TSTError TSTMorikawa::getTelemetryTemperature(TemperatureType type, unsigned char* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryTemperature(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, false, result);
    }
    return error;
}

/*public */TSTError TSTMorikawa::getTelemetryTemperature(TemperatureType type, double* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryTemperature(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, true, result);
    }
    return error;
}

/*public */TSTError TSTMorikawa::getTelemetryGyro(GyroType type, unsigned char* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryGyro(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, false, result);
    }
    return error;
}

/*public */TSTError TSTMorikawa::getTelemetryGyro(GyroType type, double* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryGyro(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, true, result);
    }
    return error;
}

/*public */TSTError TSTMorikawa::getTelemetryMagnet(MagnetType type, unsigned char* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryMagnet(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, false, result);
    }
    return error;
}

/*public */TSTError TSTMorikawa::getTelemetryMagnet(MagnetType type, double* result) const
{
    RuleRec const PROGMEM* rule;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkTelemetryMagnet(type, result, &rule)) == TSTERROR_OK) {
        convertTelemetry(rule, true, result);
    }
    return error;
}

/*public */bool TSTMorikawa::hasTelemetryUpdate(void) const
{
    bool result(false);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (_state) {
        result = _update;
    }
    return result;
}

/*public */bool TSTMorikawa::hasAbnormalShutdown(void) const
{
    bool result(false);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (_state) {
        result = _shutdown;
    }
    return result;
}

/*public */TSTError TSTMorikawa::setup(void)
{
    TSTError log;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (!_state) {
        _state = true;
        _index = 0;
        _overflow = false;
        _page = false;
        _receive = false;
        memset(&_telemetry, 0, sizeof(_telemetry));
        _update = false;
        _shutdown = false;
        _audio = true;
        _param.time = 0;
        _param.count = 1;
        _param.mode = 0;
        initializeI2C();
        initializeSPI();
        if ((log = _shared.setup(this)) != TSTERROR_OK) {
            writeSelfTestLog(offsetof(SelfTestLog, setup_shared), &log, sizeof(log));
        }
        else if ((log = _shared.readBootParam(&_param)) != TSTERROR_OK) {
            writeSelfTestLog(offsetof(SelfTestLog, setup_param), &log, sizeof(log));
        }
        initializeRandom(_param.time);
        if ((log = _fram.setup(this)) != TSTERROR_OK) {
            writeSelfTestLog(offsetof(SelfTestLog, setup_fram), &log, sizeof(log));
        }
        if ((log = _flash.setup(this)) != TSTERROR_OK) {
            writeSelfTestLog(offsetof(SelfTestLog, setup_flash), &log, sizeof(log));
        }
        if ((log = _led.setup(this)) != TSTERROR_OK) {
            writeSelfTestLog(offsetof(SelfTestLog, setup_led), &log, sizeof(log));
        }
        if ((log = _tone.setup(this)) != TSTERROR_OK) {
            writeSelfTestLog(offsetof(SelfTestLog, setup_tone), &log, sizeof(log));
        }
        if ((log = _digitalker.setup(this)) != TSTERROR_OK) {
            writeSelfTestLog(offsetof(SelfTestLog, setup_digitalker), &log, sizeof(log));
        }
        if ((log = _camera.setup(this)) != TSTERROR_OK) {
            writeSelfTestLog(offsetof(SelfTestLog, setup_camera), &log, sizeof(log));
        }
        Serial1.begin(MISSION_MAIN_BAUD);
        disableAudioBus();
        Timer1.initialize(TIMER_INTERVAL);
        Timer1.attachInterrupt(&onTimer);
        delay(BOOT_DELAY);
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*public */void TSTMorikawa::cleanup(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (_state) {
        shareSelfTestLog();
        Timer1.stop();
        disableAudioBus();
        Serial1.end();
        _camera.cleanup();
        _digitalker.cleanup();
        _tone.cleanup();
        _led.cleanup();
        _flash.cleanup();
        _fram.cleanup();
        _shared.cleanup();
        _state = false;
    }
    return;
}

/*public static */void TSTMorikawa::shutdown(void)
{
    TSTError log;
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    getInstance().cleanup();
    Serial1.begin(MISSION_MAIN_BAUD);
    if ((log = sendRequest(PACKET_MAIN, COMMAND_AUDIOBUSOFF)) != TSTERROR_OK) {
        writeSelfTestLog(offsetof(SelfTestLog, shutdown_audiobusoff), &log, sizeof(log));
    }
    if ((log = sendRequest(PACKET_MAIN, COMMAND_NORMALSD)) != TSTERROR_OK) {
        writeSelfTestLog(offsetof(SelfTestLog, shutdown_normalsd), &log, sizeof(log));
    }
    Serial1.end();
    delay(SHUTDOWN_DELAY);
    while (true) {
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();
    }
    return;
}

/*public */void TSTMorikawa::loop(void)
{
    TSTCriticalSection mutex;
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (_state) {
        mutex.lock();
        if (_receive) {
            _page = !_page;
            _update = true;
            _receive = false;
        }
        mutex.unlock();
        if (_shutdown) {
            shutdown();
        }
        _digitalker.loop();
        shareSelfTestLog();
    }
    return;
}

/*public static */TSTError TSTMorikawa::writeEEPROM(register unsigned long address, void const* data, register unsigned int size, unsigned int* result)
{
    register unsigned char const* temp;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkEEPROM(address, data, &size, result)) == TSTERROR_OK) {
        for (temp = static_cast<unsigned char const*>(data); size > 0; --size, ++address, ++temp) {
            EEPROM.write(address, *temp);
        }
    }
    return error;
}

/*public static */TSTError TSTMorikawa::writeEEPROMPGM(register unsigned long address, void const PROGMEM* data, register unsigned int size, unsigned int* result)
{
    register unsigned char const PROGMEM* temp;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkEEPROM(address, data, &size, result)) == TSTERROR_OK) {
        for (temp = static_cast<unsigned char const PROGMEM*>(data); size > 0; --size, ++address, ++temp) {
            EEPROM.write(address, pgm_read_byte(temp));
        }
    }
    return error;
}

/*public static */TSTError TSTMorikawa::readEEPROM(register unsigned long address, void* data, register unsigned int size, unsigned int* result)
{
    register unsigned char* temp;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkEEPROM(address, data, &size, result)) == TSTERROR_OK) {
        for (temp = static_cast<unsigned char*>(data); size > 0; --size, ++address, ++temp) {
            *temp = EEPROM.read(address);
        }
    }
    return error;
}

/*public static */TSTError TSTMorikawa::formatEEPROM(void)
{
    register unsigned int address;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    for (address = 0; address < EEPROM_FLASHROM; ++address) {
        EEPROM.write(address, 0xFF);
    }
    return error;
}

/*public */TSTError TSTMorikawa::freezeFastLZ(StorageType istorage, unsigned long iaddress, unsigned long isize, StorageType ostorage, unsigned long oaddress, unsigned long osize, StorageType wstorage, unsigned long waddress, unsigned long wsize, unsigned long* result)
{
    unsigned long size;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkFastLZ(istorage, iaddress, isize, ostorage, oaddress, osize, wstorage, waddress, wsize, result)) == TSTERROR_OK) {
        if (ostorage == STORAGE_NONE) {
            *result = isize * 105 / 100;
        }
        else if (wstorage == STORAGE_NONE) {
            *result = 32768;
        }
        else if (isize >= 16 && (osize >= 66 && osize >= isize * 105 / 100) && wsize >= 32768) {
            size = fastlz_compress(this, istorage, iaddress, isize, ostorage, oaddress, wstorage, waddress, &error);
            if (error == TSTERROR_OK) {
                *result = size;
            }
        }
        else {
            error = TSTERROR_INVALID_PARAM;
        }
    }
    return error;
}

/*public */TSTError TSTMorikawa::meltFastLZ(StorageType istorage, unsigned long iaddress, unsigned long isize, StorageType ostorage, unsigned long oaddress, unsigned long osize, unsigned long* result)
{
    unsigned long size;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((error = checkFastLZ(istorage, iaddress, isize, ostorage, oaddress, osize, STORAGE_NONE, 0, 0, result)) == TSTERROR_OK) {
        if (ostorage != STORAGE_NONE) {
            size = fastlz_decompress(this, istorage, iaddress, isize, ostorage, oaddress, osize, &error);
            if (error == TSTERROR_OK) {
                if (size > 0) {
                    *result = size;
                }
                else {
                    error = TSTERROR_FAILED;
                }
            }
        }
        else {
            error = TSTERROR_INVALID_STORAGE;
        }
    }
    return error;
}

/*public */TSTError TSTMorikawa::enableAudioBus(void)
{
    TSTError log;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (_state) {
        if (!_audio) {
            if ((log = sendRequest(PACKET_MAIN, COMMAND_AUDIOBUSON)) == TSTERROR_OK) {
                delay(AUDIOBUS_DELAY);
                _audio = true;
            }
            else {
                writeSelfTestLog(offsetof(SelfTestLog, enableAudioBus_audiobuson), &log, sizeof(log));
            }
        }
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*public */void TSTMorikawa::disableAudioBus(void)
{
    TSTError log;
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (_state) {
        if (_audio) {
            if ((log = sendRequest(PACKET_MAIN, COMMAND_AUDIOBUSOFF)) == TSTERROR_OK) {
                delay(AUDIOBUS_DELAY);
                _audio = false;
            }
            else {
                writeSelfTestLog(offsetof(SelfTestLog, disableAudioBus_audiobusoff), &log, sizeof(log));
            }
        }
    }
    return;
}

/*private static */void TSTMorikawa::initializeI2C(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    I2Cm.begin();
    return;
}

/*private static */void TSTMorikawa::initializeSPI(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    return;
}

/*private static */void TSTMorikawa::initializeRandom(unsigned long time)
{
    static PinType const s_pin[] PROGMEM = {
        A0,
        A1,
        A2,
        A3,
        A4,
        A5,
        A6,
        A7,
        A8,
        A9,
        A10,
        A11,
        A12,
        A13,
        A14,
        A15
    };
    register long seed;
    register PinType pin;
    register int i;
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    seed = 0;
    for (i = 0; i < lengthof(s_pin); ++i) {
        pin = pgm_read_byte(&s_pin[i]);
        pinMode(pin, INPUT);
        seed += analogRead(pin);
    }
    seed ^= time;
    randomSeed(seed);
    return;
}

/*private static */void TSTMorikawa::writeSelfTestLog(register unsigned int address, void const* data, register unsigned int size)
{
    TSTCriticalSection mutex;
    register unsigned char const* temp;
    
    address += EEPROM_SELFTEST;
    if (address + size <= EEPROM_LIMIT) {
        mutex.lock();
        for (temp = static_cast<unsigned char const*>(data); size > 0; --size, ++address, ++temp) {
            EEPROM.write(address, *temp);
        }
        _selftest = true;
        mutex.unlock();
    }
    return;
}

/*private */void TSTMorikawa::shareSelfTestLog(void)
{
    TSTCriticalSection mutex;
    bool flag;
    SelfTestLog selftest;
    TSTError log;
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    flag = false;
    mutex.lock();
    if (_selftest) {
        flag = true;
        _selftest = false;
    }
    mutex.unlock();
    if (flag) {
        if ((log = getSelfTestLog(&selftest)) == TSTERROR_OK) {
            if ((log = _shared.writeSelfTestLog(selftest)) != TSTERROR_OK) {
                writeSelfTestLog(offsetof(SelfTestLog, shareSelfTestLog_writeselftestlog), &log, sizeof(log));
            }
        }
        else {
            writeSelfTestLog(offsetof(SelfTestLog, shareSelfTestLog_getselftestlog), &log, sizeof(log));
        }
    }
    return;
}

/*private */TSTError TSTMorikawa::checkTelemetryTime(TimeType type, void* result, RuleRec const PROGMEM** rule) const
{
    static RuleRec const s_rule[TIME_LIMIT] PROGMEM = {
        {TELEMETRY_TIME_OBCTIME_0, FORMULA_TIME}
    };
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((0 <= type && type <= TIME_LIMIT) && result != NULL) {
        if (_state) {
            *rule = &s_rule[type];
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

/*private */TSTError TSTMorikawa::checkTelemetryVoltage(VoltageType type, void* result, RuleRec const PROGMEM** rule) const
{
    static RuleRec const s_rule[VOLTAGE_LIMIT] PROGMEM = {
        {TELEMETRY_VOLTAGE_BUS, FORMULA_DEFAULT},
        {TELEMETRY_VOLTAGE_BATTERY, FORMULA_DEFAULT},
        {TELEMETRY_VOLTAGE_SOLAR, FORMULA_DEFAULT}
    };
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((0 <= type && type <= VOLTAGE_LIMIT) && result != NULL) {
        if (_state) {
            *rule = &s_rule[type];
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

/*private */TSTError TSTMorikawa::checkTelemetryCurrent(CurrentType type, void* result, RuleRec const PROGMEM** rule) const
{
    static RuleRec const s_rule[CURRENT_LIMIT] PROGMEM = {
        {TELEMETRY_CURRENT_BUS, FORMULA_CURRENT2},
        {TELEMETRY_CURRENT_BATTERY, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_SOLAR, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_SOLAR_PX, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_SOLAR_MX, FORMULA_CURRENT2},
        {TELEMETRY_CURRENT_SOLAR_PY1, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_SOLAR_MY1, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_SOLAR_PY2, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_SOLAR_MY2, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_SOLAR_PZ, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_SOLAR_MZ, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_POWERCPU, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_MAINCPU, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_MISSIONCPU, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_ANTENNA, FORMULA_CURRENT2},
        {TELEMETRY_CURRENT_BATTERYHEATER, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_TX, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_CW, FORMULA_CURRENT1},
        {TELEMETRY_CURRENT_RX, FORMULA_CURRENT1}
    };
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((0 <= type && type <= CURRENT_LIMIT) && result != NULL) {
        if (_state) {
            *rule = &s_rule[type];
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

/*private */TSTError TSTMorikawa::checkTelemetryTemperature(TemperatureType type, void* result, RuleRec const PROGMEM** rule) const
{
    static RuleRec const s_rule[TEMPERATURE_LIMIT] PROGMEM = {
        {TELEMETRY_TEMPERATURE_BATTERY1, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_BATTERY2, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_BATTERY3, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_SOLAR_PX, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_SOLAR_MX, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_SOLAR_PY1, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_SOLAR_MY1, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_SOLAR_PY2, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_SOLAR_MY2, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_SOLAR_PZ1, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_SOLAR_MZ1, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_SOLAR_PZ2, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_SOLAR_MZ2, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_POWERCPU, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_MAINCPU, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_MISSIONCPU, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_CWTX, FORMULA_TEMPERATURE},
        {TELEMETRY_TEMPERATURE_RX, FORMULA_TEMPERATURE}
    };
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((0 <= type && type <= TEMPERATURE_LIMIT) && result != NULL) {
        if (_state) {
            *rule = &s_rule[type];
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

/*private */TSTError TSTMorikawa::checkTelemetryGyro(GyroType type, void* result, RuleRec const PROGMEM** rule) const
{
    static RuleRec const s_rule[GYRO_LIMIT] PROGMEM = {
        {TELEMETRY_GYRO_X, FORMULA_GYRO},
        {TELEMETRY_GYRO_Y, FORMULA_GYRO},
        {TELEMETRY_GYRO_Z, FORMULA_GYRO}
    };
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((0 <= type && type <= GYRO_LIMIT) && result != NULL) {
        if (_state) {
            *rule = &s_rule[type];
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

/*private */TSTError TSTMorikawa::checkTelemetryMagnet(MagnetType type, void* result, RuleRec const PROGMEM** rule) const
{
    static RuleRec const s_rule[MAGNET_LIMIT] PROGMEM = {
        {TELEMETRY_MAGNET_X, FORMULA_MAGNET},
        {TELEMETRY_MAGNET_Y, FORMULA_MAGNET},
        {TELEMETRY_MAGNET_Z, FORMULA_MAGNET}
    };
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if ((0 <= type && type <= MAGNET_LIMIT) && result != NULL) {
        if (_state) {
            *rule = &s_rule[type];
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

/*private */void TSTMorikawa::convertTelemetry(RuleRec const PROGMEM* rule, bool convert, void* result) const
{
    TelemetryType index;
    unsigned char telemetry;
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    index = pgm_read_byte(&rule->telemetry);
    telemetry = _telemetry[_page][index];
    if (convert) {
        switch (pgm_read_byte(&rule->formula)) {
            case FORMULA_TIME:
                *static_cast<unsigned long*>(result) = (static_cast<unsigned long>(telemetry)                    << 24) |
                                                       (static_cast<unsigned long>(_telemetry[_page][index + 1]) << 16) |
                                                       (static_cast<unsigned long>(_telemetry[_page][index + 2]) <<  8) |
                                                       (static_cast<unsigned long>(_telemetry[_page][index + 3]) <<  0);
                break;
            case FORMULA_CURRENT1:
                *static_cast<double*>(result) = telemetry / 255.0;
                break;
            case FORMULA_CURRENT2:
                *static_cast<double*>(result) = telemetry * 2.5 / 255.0;
                break;
            case FORMULA_TEMPERATURE:
                *static_cast<double*>(result) = -1481.96 + sqrt(2.1952e+6 + (1.8639 - ((telemetry * 5.0 / 255.0 - 2.5) / 4.0 + 5.0 / 3.0)) / 3.88e-6);
                break;
            case FORMULA_GYRO:
                *static_cast<double*>(result) = ((telemetry * 5.0 / 255.0 - 2.5) / 51.0 + 1.65 - 1.1 * 1.5) / (1.1 * 1.2e-2);
                break;
            case FORMULA_MAGNET:
                *static_cast<double*>(result) = ((telemetry * 5.0 / 255.0 - 2.5) / 7.25 + 1.65 - 1.1 * 1.5) / (1.1 * 2.4e-6);
                break;
            case FORMULA_DEFAULT:
            default:
                *static_cast<double*>(result) = telemetry;
                break;
        }
    }
    else {
        *static_cast<unsigned char*>(result) = telemetry;
    }
    _update = false;
    return;
}

/*private static */TSTError TSTMorikawa::checkEEPROM(unsigned long address, void const* data, unsigned int* size, unsigned int* result)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (data != NULL) {
        if (address < EEPROM_FLASHROM) {
            if (address + *size > EEPROM_FLASHROM) {
                if (result != NULL) {
                    *size = EEPROM_FLASHROM - address;
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
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*private */TSTError TSTMorikawa::checkFastLZ(StorageType istorage, unsigned long iaddress, unsigned long isize, StorageType ostorage, unsigned long oaddress, unsigned long osize, StorageType wstorage, unsigned long waddress, unsigned long wsize, unsigned long* result) const
{
    static unsigned long (*const s_getSize[STORAGE_LIMIT])(void) = {
        NULL, &getSizeSharedMemory, &getSizeFRAM, &getSizeFlashROM
    };
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    switch (istorage) {
        case STORAGE_SHAREDMEMORY:
        case STORAGE_FRAM:
        case STORAGE_FLASHROM:
            if (iaddress + isize <= (*s_getSize[istorage])()) {
                switch (ostorage) {
                    case STORAGE_NONE:
                        // nop
                        break;
                    case STORAGE_SHAREDMEMORY:
                    case STORAGE_FRAM:
                        if (oaddress + osize <= (*s_getSize[ostorage])()) {
                            if (istorage == ostorage) {
                                if (oaddress < iaddress + isize && iaddress < oaddress + osize) {
                                    error = TSTERROR_INVALID_PARAM;
                                }
                            }
                            if (error == TSTERROR_OK) {
                                switch (wstorage) {
                                    case STORAGE_NONE:
                                        // nop
                                        break;
                                    case STORAGE_FRAM:
                                        if (waddress + wsize <= (*s_getSize[wstorage])()) {
                                            if (ostorage == wstorage) {
                                                if (waddress < oaddress + osize && oaddress < waddress + wsize) {
                                                    error = TSTERROR_INVALID_PARAM;
                                                }
                                            }
                                            if (error == TSTERROR_OK) {
                                                if (wstorage == istorage) {
                                                    if (iaddress < waddress + wsize && waddress < iaddress + isize) {
                                                        error = TSTERROR_INVALID_PARAM;
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            error = TSTERROR_INVALID_PARAM;
                                        }
                                        break;
                                    default:
                                        error = TSTERROR_INVALID_STORAGE;
                                        break;
                                }
                            }
                        }
                        else {
                            error = TSTERROR_INVALID_PARAM;
                        }
                        break;
                    default:
                        error = TSTERROR_INVALID_STORAGE;
                        break;
                }
                if (error == TSTERROR_OK) {
                    if (result != NULL) {
                        if (!_state) {
                            error = TSTERROR_INVALID_STATE;
                        }
                    }
                    else {
                        error = TSTERROR_INVALID_PARAM;
                    }
                }
            }
            else {
                error = TSTERROR_INVALID_PARAM;
            }
            break;
        default:
            error = TSTERROR_INVALID_STORAGE;
            break;
    }
    return error;
}

/*private static */TSTError TSTMorikawa::sendRequest(char to, char const PROGMEM* command)
{
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    return sendPacket(PACKET_REQUEST, to, command);
}

/*private static */TSTError TSTMorikawa::sendResponse(char to, char const PROGMEM* command)
{
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    return sendPacket(PACKET_RESPONSE, to, command);
}

/*private static */TSTError TSTMorikawa::sendPacket(char mode, char to, char const PROGMEM* command)
{
    unsigned char timer1;
    char buffer[asciiesof(g_send) + lengthof(g_delimiter)];
    register char header;
    register int i;
    register int j;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    for (i = 0, j = 0; i < asciiesof(g_send); ++i) {
        header = pgm_read_byte(&g_send[i]);
        if (header == '#') {
            buffer[i] = mode;
        }
        else if (header == '!') {
            buffer[i] = to;
        }
        else if (header == '*') {
            if ((header = pgm_read_byte(&command[j])) != '\0') {
                buffer[i] = header;
                ++j;
            }
            else {
                break;
            }
        }
        else {
            buffer[i] = header;
        }
    }
    if (i >= asciiesof(g_send)) {
        memcpy_P(&buffer[i], g_delimiter, lengthof(g_delimiter));
        timer1 = TIMSK1 & _BV(TOIE1);
        TIMSK1 &= ~_BV(TOIE1);
        Serial1.print(buffer);
        TIMSK1 |= timer1;
    }
    else {
        error = TSTERROR_FAILED;
    }
    return error;
}

/*private static */void TSTMorikawa::onTimer(void)
{
    unsigned char timer1;
    unsigned char usart1;
    unsigned char i2cm;
    
    timer1 = TIMSK1 & _BV(TOIE1);
    TIMSK1 &= ~_BV(TOIE1);
    usart1 = UCSR1B & _BV(RXCIE1);
    UCSR1B &= ~_BV(RXCIE1);
    i2cm = TWCR & _BV(TWIE);
    TWCR &= ~(_BV(TWIE) | _BV(TWINT));
    sei();
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    getInstance().onReceivePacket();
    
    cli();
    TWCR = (TWCR | i2cm) & ~_BV(TWINT);
    UCSR1B |= usart1;
    TIMSK1 |= timer1;
    return;
}

/*private */void TSTMorikawa::onReceivePacket(void)
{
    register int rx;
    PacketRec packet;
    register char header;
    register int i;
    register int j;
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    while ((rx = Serial1.read()) >= 0) {
        if (_index >= lengthof(_buffer)) {
            --_index;
            memmove(&_buffer[0], &_buffer[1], _index);
            _overflow = true;
        }
        _buffer[_index] = rx;
        if (++_index >= asciiesof(g_delimiter)) {
            if (memcmp_P(&_buffer[_index - asciiesof(g_delimiter)], g_delimiter, asciiesof(g_delimiter)) == 0) {
                if (!_overflow) {
                    if ((_index -= asciiesof(g_delimiter)) >= asciiesof(g_receive)) {
                        memset(&packet, 0, sizeof(packet));
                        for (i = 0, j = 0; i < asciiesof(g_receive); ++i) {
                            header = pgm_read_byte(&g_receive[i]);
                            if (header == '#') {
                                packet.mode = _buffer[i];
                            }
                            else if (header == '!') {
                                packet.from = _buffer[i];
                            }
                            else if (header == '*') {
                                if (j < lengthof(packet.command)) {
                                    packet.command[j] = _buffer[i];
                                    ++j;
                                }
                                else {
                                    break;
                                }
                            }
                            else if (_buffer[i] != header) {
                                break;
                            }
                        }
                        if (i >= asciiesof(g_receive)) {
                            if (_index > i) {
                                if (_buffer[i] == PACKET_SEPARATOR) {
                                    ++i;
                                    packet.data = &_buffer[i];
                                    packet.length = _index - i;
                                }
                                else {
                                    packet.mode = 0;
                                }
                            }
                            switch (packet.mode) {
                                case PACKET_REQUEST:
                                    onReceiveRequest(packet);
                                    break;
                                case PACKET_RESPONSE:
                                    onReceiveResponse(packet);
                                    break;
                                default:
                                    // nop
                                    break;
                            }
                        }
                    }
                }
                _index = 0;
                _overflow = false;
            }
        }
    }
    return;
}

/*private */void TSTMorikawa::onReceiveRequest(PacketRec const& packet)
{
    TSTCriticalSection mutex;
    TSTError log;
    
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    if (memcmp_P(packet.command, COMMAND_ECHO, lengthof(packet.command)) == 0) {
        if ((log = sendResponse(packet.from, COMMAND_ECHO)) != TSTERROR_OK) {
            writeSelfTestLog(offsetof(SelfTestLog, onReceiveRequest_echo), &log, sizeof(log));
        }
    }
    else {
        switch (packet.from) {
            case PACKET_MAIN:
                if (memcmp_P(packet.command, COMMAND_TELEMETRY, lengthof(packet.command)) == 0) {
                    if (packet.length == lengthof(_telemetry[!_page])) {
                        mutex.lock();
                        memcpy(_telemetry[!_page], packet.data, packet.length);
                        _receive = true;
                        mutex.unlock();
                    }
                    else {
                        log = TSTERROR_INVALID_FORMAT;
                        writeSelfTestLog(offsetof(SelfTestLog, onReceiveRequest_telemetry), &log, sizeof(log));
                    }
                }
                else if (memcmp_P(packet.command, COMMAND_ABNORMALSD, lengthof(packet.command)) == 0) {
                    mutex.lock();
                    _shutdown = true;
                    mutex.unlock();
                }
                break;
            default:
                // nop
                break;
        }
    }
    return;
}

/*private */void TSTMorikawa::onReceiveResponse(PacketRec const& packet)
{
#ifdef OPTION_BUILD_MEMORYLOG
    saveMemoryLog();
#endif
    return;
}

}// end of namespace
