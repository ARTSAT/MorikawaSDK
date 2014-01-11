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
**      TSTMorikawa.h
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

#ifndef __TST_MORIKAWA_H
#define __TST_MORIKAWA_H

#include "TSTType.h"
#include "TSTSharedMemory.h"
#include "TSTFRAM.h"
#include "TSTFlashROM.h"
#include "TSTLED.h"
#include "TSTTone.h"
#include "TSTDigitalker.h"
#include "TSTCamera.h"

namespace tst {

enum TimeEnum {
    TIME_OBCTIME,
    TIME_LIMIT
};
typedef unsigned char       TimeType;
enum VoltageEnum {
    VOLTAGE_BUS,
    VOLTAGE_BATTERY,
    VOLTAGE_SOLAR,
    VOLTAGE_LIMIT
};
typedef unsigned char       VoltageType;
enum CurrentEnum {
    CURRENT_BUS,
    CURRENT_BATTERY,
    CURRENT_SOLAR,
    CURRENT_SOLAR_PX,
    CURRENT_SOLAR_MX,
    CURRENT_SOLAR_PY1,
    CURRENT_SOLAR_MY1,
    CURRENT_SOLAR_PY2,
    CURRENT_SOLAR_MY2,
    CURRENT_SOLAR_PZ,
    CURRENT_SOLAR_MZ,
    CURRENT_POWERCPU,
    CURRENT_MAINCPU,
    CURRENT_MISSIONCPU,
    CURRENT_ANTENNA,
    CURRENT_BATTERYHEATER,
    CURRENT_TX,
    CURRENT_CW,
    CURRENT_RX,
    CURRENT_LIMIT
};
typedef unsigned char       CurrentType;
enum TemperatureEnum {
    TEMPERATURE_BATTERY1,
    TEMPERATURE_BATTERY2,
    TEMPERATURE_BATTERY3,
    TEMPERATURE_SOLAR_PX,
    TEMPERATURE_SOLAR_MX,
    TEMPERATURE_SOLAR_PY1,
    TEMPERATURE_SOLAR_MY1,
    TEMPERATURE_SOLAR_PY2,
    TEMPERATURE_SOLAR_MY2,
    TEMPERATURE_SOLAR_PZ1,
    TEMPERATURE_SOLAR_MZ1,
    TEMPERATURE_SOLAR_PZ2,
    TEMPERATURE_SOLAR_MZ2,
    TEMPERATURE_POWERCPU,
    TEMPERATURE_MAINCPU,
    TEMPERATURE_MISSIONCPU,
    TEMPERATURE_CWTX,
    TEMPERATURE_RX,
    TEMPERATURE_LIMIT
};
typedef unsigned char       TemperatureType;
enum GyroEnum {
    GYRO_X,
    GYRO_Y,
    GYRO_Z,
    GYRO_LIMIT
};
typedef unsigned char       GyroType;
enum MagnetEnum {
    MAGNET_X,
    MAGNET_Y,
    MAGNET_Z,
    MAGNET_LIMIT
};
typedef unsigned char       MagnetType;

struct MemorySpec {
    void const*             ram_start;
    size_t                  ram_size;
    void const*             data_start;
    size_t                  data_size;
    void const*             bss_start;
    size_t                  bss_size;
    void const*             heap_stack_start;
    size_t                  heap_stack_size;
    size_t                  heap_stack_margin;
};
struct MemoryInfo {
    size_t                  heap_size;
    size_t                  heap_free;
    size_t                  stack_size;
    size_t                  stack_free;
};
struct SelfTestLog {
    TSTError                saveMemoryLog_getmemoryinfo;
    TSTError                setup_param;
    TSTError                setup_shared;
    TSTError                setup_fram;
    TSTError                setup_flash;
    TSTError                setup_led;
    TSTError                setup_tone;
    TSTError                setup_digitalker;
    TSTError                setup_camera;
    TSTError                shutdown_audiobusoff;
    TSTError                shutdown_normalsd;
    TSTError                enableAudioBus_audiobuson;
    TSTError                disableAudioBus_audiobusoff;
    TSTError                shareSelfTestLog_getselftestlog;
    TSTError                shareSelfTestLog_writeselftestlog;
    TSTError                onReceiveRequest_echo;
    TSTError                onReceiveRequest_telemetry;
};
class TSTMorikawa {
    private:
        enum TelemetryEnum {
            TELEMETRY_TIME_OBCTIME_0,
            TELEMETRY_TIME_OBCTIME_1,
            TELEMETRY_TIME_OBCTIME_2,
            TELEMETRY_TIME_OBCTIME_3,
            TELEMETRY_VOLTAGE_BATTERY,
            TELEMETRY_VOLTAGE_BUS,
            TELEMETRY_VOLTAGE_SOLAR,
            TELEMETRY_RESERVED_0,
            TELEMETRY_RESERVED_1,
            TELEMETRY_RESERVED_2,
            TELEMETRY_CURRENT_POWERCPU,
            TELEMETRY_CURRENT_BUS,
            TELEMETRY_CURRENT_BATTERY,
            TELEMETRY_CURRENT_SOLAR,
            TELEMETRY_CURRENT_SOLAR_MY2,
            TELEMETRY_CURRENT_SOLAR_PY2,
            TELEMETRY_CURRENT_SOLAR_MZ,
            TELEMETRY_CURRENT_SOLAR_PZ,
            TELEMETRY_CURRENT_SOLAR_MY1,
            TELEMETRY_CURRENT_SOLAR_PY1,
            TELEMETRY_CURRENT_SOLAR_MX,
            TELEMETRY_CURRENT_SOLAR_PX,
            TELEMETRY_CURRENT_ANTENNA,
            TELEMETRY_CURRENT_BATTERYHEATER,
            TELEMETRY_CURRENT_TX,
            TELEMETRY_CURRENT_CW,
            TELEMETRY_CURRENT_RX,
            TELEMETRY_CURRENT_MAINCPU,
            TELEMETRY_CURRENT_MISSIONCPU,
            TELEMETRY_RESERVED_3,
            TELEMETRY_TEMPERATURE_BATTERY1,
            TELEMETRY_TEMPERATURE_BATTERY2,
            TELEMETRY_TEMPERATURE_BATTERY3,
            TELEMETRY_TEMPERATURE_SOLAR_PX,
            TELEMETRY_TEMPERATURE_SOLAR_MX,
            TELEMETRY_TEMPERATURE_SOLAR_PY1,
            TELEMETRY_TEMPERATURE_SOLAR_PY2,
            TELEMETRY_TEMPERATURE_SOLAR_MY1,
            TELEMETRY_TEMPERATURE_SOLAR_MY2,
            TELEMETRY_TEMPERATURE_SOLAR_PZ1,
            TELEMETRY_TEMPERATURE_SOLAR_PZ2,
            TELEMETRY_TEMPERATURE_SOLAR_MZ1,
            TELEMETRY_TEMPERATURE_SOLAR_MZ2,
            TELEMETRY_TEMPERATURE_POWERCPU,
            TELEMETRY_TEMPERATURE_MISSIONCPU,
            TELEMETRY_TEMPERATURE_CWTX,
            TELEMETRY_TEMPERATURE_RX,
            TELEMETRY_TEMPERATURE_MAINCPU,
            TELEMETRY_RESERVED_4,
            TELEMETRY_RESERVED_5,
            TELEMETRY_GYRO_X,
            TELEMETRY_GYRO_Y,
            TELEMETRY_GYRO_Z,
            TELEMETRY_MAGNET_X,
            TELEMETRY_MAGNET_Y,
            TELEMETRY_MAGNET_Z,
            TELEMETRY_RESERVED_6,
            TELEMETRY_RESERVED_7,
            TELEMETRY_RESERVED_8,
            TELEMETRY_LIMIT
        };
        typedef unsigned char       TelemetryType;
        enum FormulaEnum {
            FORMULA_DEFAULT,
            FORMULA_TIME,
            FORMULA_CURRENT1,
            FORMULA_CURRENT2,
            FORMULA_TEMPERATURE,
            FORMULA_GYRO,
            FORMULA_MAGNET,
            FORMULA_LIMIT
        };
        typedef unsigned char       FormulaType;
    
    private:
        struct RuleRec {
            TelemetryType           telemetry;
            FormulaType             formula;
        };
        struct PacketRec {
            char                    mode;
            char                    from;
            char                    command[3];
            unsigned char const*    data;
            unsigned int            length;
        };
    
    private:
        static  MemoryInfo          _memory;
        static  TSTTrinity<bool>    _selftest;
                TSTTrinity<bool>    _state;
                TSTTrinity<unsigned int>
                                    _index;
                TSTTrinity<bool>    _overflow;
                unsigned char       _buffer[80];
                TSTTrinity<bool>    _page;
                TSTTrinity<bool>    _receive;
                unsigned char       _telemetry[2][TELEMETRY_LIMIT];
        mutable TSTTrinity<bool>    _update;
                TSTTrinity<bool>    _shutdown;
                TSTTrinity<bool>    _audio;
                TSTSharedMemory::BootParamRec
                                    _param;
                TSTSharedMemory     _shared;
                TSTFRAM             _fram;
                TSTFlashROM         _flash;
                TSTLED              _led;
                TSTTone             _tone;
                TSTDigitalker       _digitalker;
                TSTCamera           _camera;
    
    public:
        static  TSTMorikawa&        getInstance                 (void);
        static  TSTError            getMemorySpec               (MemorySpec* result);
        static  TSTError            getMemoryInfo               (MemoryInfo* result);
        static  TSTError            getMemoryLog                (MemoryInfo* result);
        static  void                saveMemoryLog               (void);
        static  TSTError            getSelfTestLog              (SelfTestLog* result);
        static  void                eraseSelfTestLog            (void);
        static  unsigned long       getSizeEEPROM               (void);
        static  unsigned long       getSizeSharedMemory         (void);
        static  unsigned long       getSizeFRAM                 (void);
        static  unsigned long       getSizeFlashROM             (void);
        static  unsigned int        getPageSizeEEPROM           (void);
        static  unsigned int        getPageSizeSharedMemory     (void);
        static  unsigned int        getPageSizeFRAM             (void);
        static  unsigned int        getPageSizeFlashROM         (void);
        static  unsigned long       getSectorSizeEEPROM         (void);
        static  unsigned long       getSectorSizeSharedMemory   (void);
        static  unsigned long       getSectorSizeFRAM           (void);
        static  unsigned long       getSectorSizeFlashROM       (void);
        static  TSTError            getCameraFormat             (CameraType mode, CameraFormat* result);
                unsigned long       getBootTime                 (void) const;
                unsigned char       getBootCount                (void) const;
                unsigned char       getBootMode                 (void) const;
                TSTError            getParamNote                (NoteParam* result);
                TSTError            getParamMorse               (MorseParam* result);
                TSTError            getParamDigitalker          (DigitalkerParam* result);
                TSTError            getParamCamera              (CameraParam* result);
                TSTError            getTelemetryTime            (TimeType type, unsigned long* result) const;
                TSTError            getTelemetryVoltage         (VoltageType type, unsigned char* result) const;
                TSTError            getTelemetryVoltage         (VoltageType type, double* result) const;
                TSTError            getTelemetryCurrent         (CurrentType type, unsigned char* result) const;
                TSTError            getTelemetryCurrent         (CurrentType type, double* result) const;
                TSTError            getTelemetryTemperature     (TemperatureType type, unsigned char* result) const;
                TSTError            getTelemetryTemperature     (TemperatureType type, double* result) const;
                TSTError            getTelemetryGyro            (GyroType type, unsigned char* result) const;
                TSTError            getTelemetryGyro            (GyroType type, double* result) const;
                TSTError            getTelemetryMagnet          (MagnetType type, unsigned char* result) const;
                TSTError            getTelemetryMagnet          (MagnetType type, double* result) const;
                TSTError            setFlashROMEraseMode        (bool param);
                bool                getFlashROMEraseMode        (void) const;
                TSTError            setText                     (TextType index, char const* text, int length = -1);
                TSTError            setTextPGM                  (TextType index, char const PROGMEM* text, int length = -1);
                TSTError            getText                     (TextType index, char* text, unsigned int length, int* result = NULL);
                TSTError            setLED                      (LEDType index, unsigned char pwm);
                unsigned char       getLED                      (LEDType index) const;
                TSTError            setNoteBPM                  (int param = -1);
                int                 getNoteBPM                  (void) const;
                TSTError            setMorseWPM                 (int param = -1);
                int                 getMorseWPM                 (void) const;
                TSTError            setSpeakAsyncMode           (bool param);
                bool                getSpeakAsyncMode           (void) const;
                bool                isValid                     (void) const;
                bool                isValidSharedMemory         (void) const;
                bool                isValidFRAM                 (void) const;
                bool                isValidFlashROM             (void) const;
                bool                isValidLED                  (void) const;
                bool                isValidTone                 (void) const;
                bool                isValidDigitalker           (void) const;
                bool                isValidCamera               (void) const;
                TSTError            isBusyDigitalker            (bool* result) const;
                bool                hasTelemetryUpdate          (void) const;
                bool                hasAbnormalShutdown         (void) const;
                TSTError            setup                       (void);
                void                cleanup                     (void);
        static  void                shutdown                    (void);
                void                loop                        (void);
        static  TSTError            writeEEPROM                 (unsigned long address, void const* data, unsigned int size, unsigned int* result = NULL);
        static  TSTError            writeEEPROMPGM              (unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result = NULL);
        static  TSTError            readEEPROM                  (unsigned long address, void* data, unsigned int size, unsigned int* result = NULL);
        static  TSTError            formatEEPROM                (void);
                TSTError            writeSharedMemory           (unsigned long address, void const* data, unsigned int size, unsigned int* result = NULL);
                TSTError            writeSharedMemoryPGM        (unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result = NULL);
                TSTError            readSharedMemory            (unsigned long address, void* data, unsigned int size, unsigned int* result = NULL);
                TSTError            formatSharedMemory          (void);
                TSTError            writeFRAM                   (unsigned long address, void const* data, unsigned int size, unsigned int* result = NULL);
                TSTError            writeFRAMPGM                (unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result = NULL);
                TSTError            readFRAM                    (unsigned long address, void* data, unsigned int size, unsigned int* result = NULL);
                TSTError            formatFRAM                  (void);
                TSTError            writeFlashROM               (unsigned long address, void const* data, unsigned int size, unsigned int* result = NULL);
                TSTError            writeFlashROMPGM            (unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result = NULL);
                TSTError            readFlashROM                (unsigned long address, void* data, unsigned int size, unsigned int* result = NULL);
                TSTError            formatFlashROM              (void);
                TSTError            playFrequency               (FrequencyType frequency, unsigned long duration);
                TSTError            playFrequency               (FrequencySequence const* sequence, int length = -1);
                TSTError            playFrequencyPGM            (FrequencySequence const PROGMEM* sequence, int length = -1);
                TSTError            playNote                    (NoteType note, DurationType duration, QualifierType qualifier = QUALIFIER_NONE);
                TSTError            playNote                    (NoteSequence const* sequence, int length = -1);
                TSTError            playNotePGM                 (NoteSequence const PROGMEM* sequence, int length = -1);
                TSTError            playMorse                   (NoteType note, char character);
                TSTError            playMorse                   (NoteType note, char const* sequence, int length = -1);
                TSTError            playMorsePGM                (NoteType note, char const PROGMEM* sequence, int length = -1);
                TSTError            speakPhrase                 (char const* phrase, int length = -1);
                TSTError            speakPhrasePGM              (char const PROGMEM* phrase, int length = -1);
                TSTError            waitPhrase                  (void);
                TSTError            stopPhrase                  (void);
                TSTError            snapshotCamera              (CameraType mode, StorageType storage, unsigned long address, unsigned long size, unsigned long* result);
                TSTError            freezeFastLZ                (StorageType istorage, unsigned long iaddress, unsigned long isize, StorageType ostorage, unsigned long oaddress, unsigned long osize, StorageType wstorage, unsigned long waddress, unsigned long wsize, unsigned long* result);
                TSTError            meltFastLZ                  (StorageType istorage, unsigned long iaddress, unsigned long isize, StorageType ostorage, unsigned long oaddress, unsigned long osize, unsigned long* result);
                TSTError            enableAudioBus              (void);
                void                disableAudioBus             (void);
    private:
        explicit                    TSTMorikawa                 (void);
                                    ~TSTMorikawa                (void);
        static  void                initializeI2C               (void);
        static  void                initializeSPI               (void);
        static  void                initializeRandom            (unsigned long time);
        static  void                writeSelfTestLog            (unsigned int address, void const* data, unsigned int size);
                void                shareSelfTestLog            (void);
                TSTError            checkTelemetryTime          (TimeType type, void* result, RuleRec const PROGMEM** rule) const;
                TSTError            checkTelemetryVoltage       (VoltageType type, void* result, RuleRec const PROGMEM** rule) const;
                TSTError            checkTelemetryCurrent       (CurrentType type, void* result, RuleRec const PROGMEM** rule) const;
                TSTError            checkTelemetryTemperature   (TemperatureType type, void* result, RuleRec const PROGMEM** rule) const;
                TSTError            checkTelemetryGyro          (GyroType type, void* result, RuleRec const PROGMEM** rule) const;
                TSTError            checkTelemetryMagnet        (MagnetType type, void* result, RuleRec const PROGMEM** rule) const;
                void                convertTelemetry            (RuleRec const PROGMEM* rule, bool convert, void* result) const;
        static  TSTError            checkEEPROM                 (unsigned long address, void const* data, unsigned int* size, unsigned int* result);
                TSTError            checkFastLZ                 (StorageType istorage, unsigned long iaddress, unsigned long isize, StorageType ostorage, unsigned long oaddress, unsigned long osize, StorageType wstorage, unsigned long waddress, unsigned long wsize, unsigned long* result) const;
        static  TSTError            sendRequest                 (char to, char const PROGMEM* command);
        static  TSTError            sendResponse                (char to, char const PROGMEM* command);
        static  TSTError            sendPacket                  (char mode, char to, char const PROGMEM* command);
        static  void                onTimer                     (void);
                void                onReceivePacket             (void);
                void                onReceiveRequest            (PacketRec const& packet);
                void                onReceiveResponse           (PacketRec const& packet);
    private:
                                    TSTMorikawa                 (TSTMorikawa const&);
                TSTMorikawa&        operator=                   (TSTMorikawa const&);
};

/*private */inline TSTMorikawa::TSTMorikawa(void) : _state(false)
{
}

/*private */inline TSTMorikawa::~TSTMorikawa(void)
{
    shutdown();
}

/*public static */inline unsigned long TSTMorikawa::getSizeSharedMemory(void)
{
    return TSTSharedMemory::getSize();
}

/*public static */inline unsigned long TSTMorikawa::getSizeFRAM(void)
{
    return TSTFRAM::getSize();
}

/*public static */inline unsigned long TSTMorikawa::getSizeFlashROM(void)
{
    return TSTFlashROM::getSize();
}

/*public static */inline unsigned int TSTMorikawa::getPageSizeSharedMemory(void)
{
    return TSTSharedMemory::getPageSize();
}

/*public static */inline unsigned int TSTMorikawa::getPageSizeFRAM(void)
{
    return TSTFRAM::getPageSize();
}

/*public static */inline unsigned int TSTMorikawa::getPageSizeFlashROM(void)
{
    return TSTFlashROM::getPageSize();
}

/*public static */inline unsigned long TSTMorikawa::getSectorSizeSharedMemory(void)
{
    return TSTSharedMemory::getSectorSize();
}

/*public static */inline unsigned long TSTMorikawa::getSectorSizeFRAM(void)
{
    return TSTFRAM::getSectorSize();
}

/*public static */inline unsigned long TSTMorikawa::getSectorSizeFlashROM(void)
{
    return TSTFlashROM::getSectorSize();
}

/*public static */inline TSTError TSTMorikawa::getCameraFormat(CameraType mode, CameraFormat* result)
{
    return TSTCamera::getFormat(mode, result);
}

/*public */inline TSTError TSTMorikawa::getParamNote(NoteParam* result)
{
    return _shared.readNoteParam(result);
}

/*public */inline TSTError TSTMorikawa::getParamMorse(MorseParam* result)
{
    return _shared.readMorseParam(result);
}

/*public */inline TSTError TSTMorikawa::getParamDigitalker(DigitalkerParam* result)
{
    return _shared.readDigitalkerParam(result);
}

/*public */inline TSTError TSTMorikawa::getParamCamera(CameraParam* result)
{
    return _shared.readCameraParam(result);
}

/*public */inline TSTError TSTMorikawa::setFlashROMEraseMode(bool param)
{
    return _flash.setEraseMode(param);
}

/*public */inline bool TSTMorikawa::getFlashROMEraseMode(void) const
{
    return _flash.getEraseMode();
}

/*public */inline TSTError TSTMorikawa::setText(TextType index, char const* text, int length)
{
    return _shared.writeTextResult(index, text, length);
}

/*public */inline TSTError TSTMorikawa::setTextPGM(TextType index, char const PROGMEM* text, int length)
{
    return _shared.writeTextResultPGM(index, text, length);
}

/*public */inline TSTError TSTMorikawa::getText(TextType index, char* text, unsigned int length, int* result)
{
    return _shared.readTextParam(index, text, length, result);
}

/*public */inline TSTError TSTMorikawa::setLED(LEDType index, unsigned char pwm)
{
    return _led.set(index, pwm);
}

/*public */inline unsigned char TSTMorikawa::getLED(LEDType index) const
{
    return _led.get(index);
}

/*public */inline TSTError TSTMorikawa::setNoteBPM(int param)
{
    return _tone.setNoteBPM(param);
}

/*public */inline int TSTMorikawa::getNoteBPM(void) const
{
    return _tone.getNoteBPM();
}

/*public */inline TSTError TSTMorikawa::setMorseWPM(int param)
{
    return _tone.setMorseWPM(param);
}

/*public */inline int TSTMorikawa::getMorseWPM(void) const
{
    return _tone.getMorseWPM();
}

/*public */inline TSTError TSTMorikawa::setSpeakAsyncMode(bool param)
{
    return _digitalker.setAsyncMode(param);
}

/*public */inline bool TSTMorikawa::getSpeakAsyncMode(void) const
{
    return _digitalker.getAsyncMode();
}

/*public */inline bool TSTMorikawa::isValid(void) const
{
    return _state;
}

/*public */inline bool TSTMorikawa::isValidSharedMemory(void) const
{
    return _shared.isValid();
}

/*public */inline bool TSTMorikawa::isValidFRAM(void) const
{
    return _fram.isValid();
}

/*public */inline bool TSTMorikawa::isValidFlashROM(void) const
{
    return _flash.isValid();
}

/*public */inline bool TSTMorikawa::isValidLED(void) const
{
    return _led.isValid();
}

/*public */inline bool TSTMorikawa::isValidTone(void) const
{
    return _tone.isValid();
}

/*public */inline bool TSTMorikawa::isValidDigitalker(void) const
{
    return _digitalker.isValid();
}

/*public */inline bool TSTMorikawa::isValidCamera(void) const
{
    return _camera.isValid();
}

/*public */inline TSTError TSTMorikawa::isBusyDigitalker(bool* result) const
{
    return _digitalker.isBusy(result);
}

/*public */inline TSTError TSTMorikawa::writeSharedMemory(unsigned long address, void const* data, unsigned int size, unsigned int* result)
{
    return _shared.write(address, data, size, result);
}

/*public */inline TSTError TSTMorikawa::writeSharedMemoryPGM(unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result)
{
    return _shared.writePGM(address, data, size, result);
}

/*public */inline TSTError TSTMorikawa::readSharedMemory(unsigned long address, void* data, unsigned int size, unsigned int* result)
{
    return _shared.read(address, data, size, result);
}

/*public */inline TSTError TSTMorikawa::formatSharedMemory(void)
{
    return _shared.format();
}

/*public */inline TSTError TSTMorikawa::writeFRAM(unsigned long address, void const* data, unsigned int size, unsigned int* result)
{
    return _fram.write(address, data, size, result);
}

/*public */inline TSTError TSTMorikawa::writeFRAMPGM(unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result)
{
    return _fram.writePGM(address, data, size, result);
}

/*public */inline TSTError TSTMorikawa::readFRAM(unsigned long address, void* data, unsigned int size, unsigned int* result)
{
    return _fram.read(address, data, size, result);
}

/*public */inline TSTError TSTMorikawa::formatFRAM(void)
{
    return _fram.format();
}

/*public */inline TSTError TSTMorikawa::writeFlashROM(unsigned long address, void const* data, unsigned int size, unsigned int* result)
{
    return _flash.write(address, data, size, result);
}

/*public */inline TSTError TSTMorikawa::writeFlashROMPGM(unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result)
{
    return _flash.writePGM(address, data, size, result);
}

/*public */inline TSTError TSTMorikawa::readFlashROM(unsigned long address, void* data, unsigned int size, unsigned int* result)
{
    return _flash.read(address, data, size, result);
}

/*public */inline TSTError TSTMorikawa::formatFlashROM(void)
{
    return _flash.format();
}

/*public */inline TSTError TSTMorikawa::playFrequency(FrequencyType frequency, unsigned long duration)
{
    return _tone.playFrequency(frequency, duration);
}

/*public */inline TSTError TSTMorikawa::playFrequency(FrequencySequence const* sequence, int length)
{
    return _tone.playFrequency(sequence, length);
}

/*public */inline TSTError TSTMorikawa::playFrequencyPGM(FrequencySequence const PROGMEM* sequence, int length)
{
    return _tone.playFrequencyPGM(sequence, length);
}

/*public */inline TSTError TSTMorikawa::playNote(NoteType note, DurationType duration, QualifierType qualifier)
{
    return _tone.playNote(note, duration, qualifier);
}

/*public */inline TSTError TSTMorikawa::playNote(NoteSequence const* sequence, int length)
{
    return _tone.playNote(sequence, length);
}

/*public */inline TSTError TSTMorikawa::playNotePGM(NoteSequence const PROGMEM* sequence, int length)
{
    return _tone.playNotePGM(sequence, length);
}

/*public */inline TSTError TSTMorikawa::playMorse(NoteType note, char character)
{
    return _tone.playMorse(note, character);
}

/*public */inline TSTError TSTMorikawa::playMorse(NoteType note, char const* sequence, int length)
{
    return _tone.playMorse(note, sequence, length);
}

/*public */inline TSTError TSTMorikawa::playMorsePGM(NoteType note, char const PROGMEM* sequence, int length)
{
    return _tone.playMorsePGM(note, sequence, length);
}

/*public */inline TSTError TSTMorikawa::speakPhrase(char const* phrase, int length)
{
    return _digitalker.speakPhrase(phrase, length);
}

/*public */inline TSTError TSTMorikawa::speakPhrasePGM(char const PROGMEM* phrase, int length)
{
    return _digitalker.speakPhrasePGM(phrase, length);
}

/*public */inline TSTError TSTMorikawa::waitPhrase(void)
{
    return _digitalker.waitPhrase();
}

/*public */inline TSTError TSTMorikawa::stopPhrase(void)
{
    return _digitalker.stopPhrase();
}

/*public */inline TSTError TSTMorikawa::snapshotCamera(CameraType mode, StorageType storage, unsigned long address, unsigned long size, unsigned long* result)
{
    return _camera.snapshot(mode, storage, address, size, result);
}

}// end of namespace

#endif
