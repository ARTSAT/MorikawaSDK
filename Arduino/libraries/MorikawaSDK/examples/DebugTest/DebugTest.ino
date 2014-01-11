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
**      DebugTest.ino
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

#include <MorikawaSDK.h>
#include <utility/EEPROM.h>
#include <utility/I2Cm.h>
#include <utility/SPI.h>

#define DO_ERASE_TEST
#define DO_EEPROM_FORMAT_TEST
#define DO_SHAREDMEMORY_FORMAT_TEST
#define DO_FRAM_FORMAT_TEST
#define DO_FLASHROM_FORMAT_TEST

#define EEPROM_SIZE             (EEPROM_FLASHROM)
#define SHAREDMEMORY_SIZE       (0x7FB00UL)
#define FRAM_SIZE               (0x20000UL)
#define FLASHROM_SIZE           (0x100000UL)
#define SHAREDMEMORY_PAGE_SIZE  (128UL)
#define FLASHROM_PAGE_SIZE      (256UL)
#define FLASHROM_SECTOR_SIZE    (65536UL)

static void isValidTest(bool flag);
static void writeSharedMemory(unsigned long address, unsigned char data[128]);
static void pre(char const* message);
static void post(TSTError error);
static void dump(char const* message, void const* address);
static void dump(char const* message, TSTError error);
static void dump(char const* message, bool flag);
static void dump(char const* message, unsigned int value);
static void dump(char const* message, unsigned long value);

void setup(void)
{
    int loop;
    TSTError error;
    
    Serial.begin(9600);
    Serial.println(F("...debug test..."));
    
    for (loop = 0; loop < 2; ++loop) {
        Serial.print(F("--- loop "));
        Serial.print(loop + 1);
        Serial.println(F(" ---"));
////////////////////////////////////////////////////////////////////////////////
        
        // getInstance()
        {
            pre("getInstance");
            error = TSTERROR_OK;
            if (&Morikawa.getInstance() == NULL) {
                error = TSTERROR_FAILED;
            }
            post(error);
        }
        // getMemorySpec()
        {
            MemorySpec spec;
            
            pre("getMemorySpec");
            error = Morikawa.getMemorySpec(NULL);
            if (error == TSTERROR_INVALID_PARAM) {
                error = Morikawa.getMemorySpec(&spec);
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("spec.ram_start", spec.ram_start);
                dump("spec.ram_size", spec.ram_size);
                dump("spec.data_start", spec.data_start);
                dump("spec.data_size", spec.data_size);
                dump("spec.bss_start", spec.bss_start);
                dump("spec.bss_size", spec.bss_size);
                dump("spec.heap_stack_start", spec.heap_stack_start);
                dump("spec.heap_stack_size", spec.heap_stack_size);
                dump("spec.heap_stack_margin", spec.heap_stack_margin);
            }
        }
        // getMemoryInfo()
        {
            MemoryInfo info;
            
            pre("getMemoryInfo");
            error = Morikawa.getMemoryInfo(NULL);
            if (error == TSTERROR_INVALID_PARAM) {
                error = Morikawa.getMemoryInfo(&info);
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("info.heap_size", info.heap_size);
                dump("info.heap_free", info.heap_free);
                dump("info.stack_size", info.stack_size);
                dump("info.stack_free", info.stack_free);
            }
        }
        // getMemoryLog()
        {
            MemoryInfo info;
            
            pre("getMemoryLog");
            error = Morikawa.getMemoryLog(NULL);
            if (error == TSTERROR_INVALID_PARAM) {
                error = Morikawa.getMemoryLog(&info);
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("log.heap_size", info.heap_size);
                dump("log.heap_free", info.heap_free);
                dump("log.stack_size", info.stack_size);
                dump("log.stack_free", info.stack_free);
            }
        }
        // eraseSelfTestLog()
        {
#ifdef DO_ERASE_TEST
            unsigned long i;
            
            pre("eraseSelfTestLog");
            error = TSTERROR_OK;
            for (i = EEPROM_SELFTEST; i < EEPROM_LIMIT; ++i) {
                EEPROM.write(i, 0xBD);
            }
            Serial.print(F("."));
            Morikawa.eraseSelfTestLog();
            Serial.print(F("."));
            for (i = EEPROM_SELFTEST; i < EEPROM_LIMIT; ++i) {
                if (EEPROM.read(i) == 0xBD) {
                    error = TSTERROR_FAILED;
                    break;
                }
            }
            post(error);
#endif
        }
        // isValid() family
        {
            isValidTest(false);
        }
        // setup()
        {
            unsigned char data[128] = {
                0x00, 0x11, 0x22, 0x33, 0x02, 0x04,
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                0x00, 0x11, 0x22, 0x33, 0x02, 0x04,
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                0x00, 0x11, 0x22, 0x33, 0x02, 0x04,
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
            };
            
            writeSharedMemory(0x7FF80UL, data);
            
            pre("setup");
            error = Morikawa.setup();
            post(error);
        }
        // getSelfTestLog()
        {
            SelfTestLog log;

            pre("getSelfTestLog");
            error = Morikawa.getSelfTestLog(NULL);
            if (error == TSTERROR_INVALID_PARAM) {
                error = Morikawa.getSelfTestLog(&log);
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("log.saveMemoryLog_getmemoryinfo", log.saveMemoryLog_getmemoryinfo);
                dump("log.setup_param", log.setup_param);
                dump("log.setup_shared", log.setup_shared);
                dump("log.setup_fram", log.setup_fram);
                dump("log.setup_flash", log.setup_flash);
                dump("log.setup_led", log.setup_led);
                dump("log.setup_tone", log.setup_tone);
                dump("log.setup_digitalker", log.setup_digitalker);
                dump("log.setup_camera", log.setup_camera);
                dump("log.shutdown_audiobusoff", log.shutdown_audiobusoff);
                dump("log.shutdown_normalsd", log.shutdown_normalsd);
                dump("log.enableAudioBus_audiobuson", log.enableAudioBus_audiobuson);
                dump("log.disableAudioBus_audiobusoff", log.disableAudioBus_audiobusoff);
                dump("log.shareSelfTestLog_getselftestlog", log.shareSelfTestLog_getselftestlog);
                dump("log.shareSelfTestLog_writeselftestlog", log.shareSelfTestLog_writeselftestlog);
                dump("log.onReceiveRequest_echo", log.onReceiveRequest_echo);
                dump("log.onReceiveRequest_telemetry", log.onReceiveRequest_telemetry);
            }
        }
        // getSizeEEPROM()
        {
            unsigned int size;
            
            pre("getSizeEEPROM");
            error = TSTERROR_OK;
            size = Morikawa.getSizeEEPROM();
            if (size != EEPROM_SIZE) {
                error = TSTERROR_FAILED;
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("size EEPROM", size);
            }
        }
        // getSizeSharedMemory()
        {
            unsigned long size;
            
            pre("getSizeSharedMemory");
            error = TSTERROR_OK;
            size = Morikawa.getSizeSharedMemory();
            if (size != SHAREDMEMORY_SIZE) {
                error = TSTERROR_FAILED;
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("size SharedMemory", size);
            }
        }
        // getSizeFRAM()
        {
            unsigned long size;
            
            pre("getSizeFRAM");
            error = TSTERROR_OK;
            size = Morikawa.getSizeFRAM();
            if (size != FRAM_SIZE) {
                error = TSTERROR_FAILED;
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("size FRAM", size);
            }
        }
        // getSizeFlashROM()
        {
            unsigned long size;
            
            pre("getSizeFlashROM");
            error = TSTERROR_OK;
            size = Morikawa.getSizeFlashROM();
            if (size != FLASHROM_SIZE) {
                error = TSTERROR_FAILED;
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("size FlashROM", size);
            }
        }
        // getPageSizeSharedMemory()
        {
            unsigned long size;
            
            pre("getPageSizeSharedMemory");
            error = TSTERROR_OK;
            size = Morikawa.getPageSizeSharedMemory();
            if (size != SHAREDMEMORY_PAGE_SIZE) {
                error = TSTERROR_FAILED;
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("page size SharedMemory", size);
            }
        }
        // getPageSizeFlashROM()
        {
            unsigned long size;
            
            pre("getPageSizeFlashROM");
            error = TSTERROR_OK;
            size = Morikawa.getPageSizeFlashROM();
            if (size != FLASHROM_PAGE_SIZE) {
                error = TSTERROR_FAILED;
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("page size FlashROM", size);
            }
        }
        // getSectorSizeFlashROM()
        {
            unsigned long size;

            pre("getSectorSizeFlashROM");
            error = TSTERROR_OK;
            size = Morikawa.getSectorSizeFlashROM();
            if (size != FLASHROM_SECTOR_SIZE) {
                error = TSTERROR_FAILED;
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("sector size FlashROM", size);
            }
        }
        // getBootTime()
        {
            unsigned long time;
            
            time = Morikawa.getBootTime();
            Serial.print(F("getBootTime"));
            Serial.print(F(" = "));
            Serial.println(time);
        }
        // getBootCount()
        {
            unsigned char count;
            
            count = Morikawa.getBootCount();
            Serial.print(F("getBootCount"));
            Serial.print(F(" = "));
            Serial.println(count);
        }
        // getBootMode()
        {
            unsigned char mode;
            
            mode = Morikawa.getBootMode();
            Serial.print(F("getBootMode"));
            Serial.print(F(" = "));
            Serial.println(mode);
        }
        // getParamNote()
        {
            unsigned char data[128] = {
                140, 11, 12, 13, 14, 15, 16
            };
            unsigned char data2[128] = {
                1, 2, 3, 4, 5, 6, 7, 8
            };
            NoteParam param;
            int i;

            writeSharedMemory(0x7FC00UL, data);
            writeSharedMemory(0x7FC80UL, data2);
            
            pre("getParamNote");
            error = Morikawa.getParamNote(NULL);
            if (error == TSTERROR_INVALID_PARAM) {
                error = Morikawa.getParamNote(&param);
                if (error == TSTERROR_OK) {
                    for (i = 0; i < param.size; ++i) {
                        if (i < 127) {
                            if (data[i + 1] != param.data[i]) {
                                error = TSTERROR_FAILED;
                                break;
                            }
                        }
                        else {
                            if (data2[i - 127] != param.data[i]) {
                                error = TSTERROR_FAILED;
                                break;
                            }
                        }
                    }
                }
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("NoteParam->size", (unsigned int)param.size);
            }
        }
        // getParamMorse()
        {
            unsigned char data[128] = {
                8, 1, 2, 3, 4, 5, 6
            };
            MorseParam param;
            int i;
            
            writeSharedMemory(0x7FB40UL, data);
            
            pre("getParamMorse");
            error = Morikawa.getParamMorse(NULL);
            if (error == TSTERROR_INVALID_PARAM) {
                error = Morikawa.getParamMorse(&param);
                if (error == TSTERROR_OK) {
                    for (i = 0; i < param.size; ++i) {
                        if (data[i + 1] != param.data[i]) {
                            error = TSTERROR_FAILED;
                            break;
                        }
                    }
                }
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("MorseParam->size", (unsigned int)param.size);
            }
        }
        // getParamDigitalker()
        {
            unsigned char data[128] = {
                100, 34, 44, 54, 64, 74, 75, 88, 99
            };
            DigitalkerParam param;
            int i;
            
            writeSharedMemory(0x7FB80UL, data);
            
            pre("getParamDigitalker");
            error = Morikawa.getParamDigitalker(NULL);
            if (error == TSTERROR_INVALID_PARAM) {
                error = Morikawa.getParamDigitalker(&param);
                if (error == TSTERROR_OK) {
                    for (i = 0; i < param.size; ++i) {
                        if (data[i + 1] != param.data[i]) {
                            error = TSTERROR_FAILED;
                            break;
                        }
                    }
                }
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("DigitalkerParam->size", (unsigned int)param.size);
            }
        }
        // getParamCamera()
        {
            unsigned char data[128] = {
                63, 44, 22, 11, 0, 65, 1, 4, 66, 77, 22, 34
            };
            CameraParam param;
            int i;
            
            writeSharedMemory(0x7FB00UL, data);
            
            pre("getParamCamera");
            error = Morikawa.getParamCamera(NULL);
            if (error == TSTERROR_INVALID_PARAM) {
                error = Morikawa.getParamCamera(&param);
                if (error == TSTERROR_OK) {
                    for (i = 0; i < param.size; ++i) {
                        if (data[i + 1] != param.data[i]) {
                            error = TSTERROR_FAILED;
                            break;
                        }
                    }
                }
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("CameraParam->size", (unsigned int)param.size);
            }
        }
        // setFlashROMEraseMode() / getFlashROMEraseMode()
        {
            pre("setFlashROMEraseMode / getFlashROMEraseMode [true]");
            error = Morikawa.setFlashROMEraseMode(true);
            if (error == TSTERROR_OK) {
                if (!Morikawa.getFlashROMEraseMode()) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
            pre("setFlashROMEraseMode / getFlashROMEraseMode [false]");
            error = Morikawa.setFlashROMEraseMode(false);
            if (error == TSTERROR_OK) {
                if (Morikawa.getFlashROMEraseMode()) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
        }
        // setText() / getText()
        {
            char test[] = "TEXTTEXT_!!!_TEXTTEXT";
            char dump[256] = {0};
            int i;
            int size;
            
            for (i = 0; i < TEXT_LIMIT; ++i) {
                pre("setText / getText");
                error = Morikawa.setText(i, NULL);
                if (error == TSTERROR_INVALID_PARAM) {
                    error = Morikawa.setText(i, test);
                    if (error == TSTERROR_OK) {
                        error = Morikawa.getText(i, NULL, 10);
                        if (error == TSTERROR_OK) {
                            error = Morikawa.getText(i, NULL, 10, &size);
                            if (error == TSTERROR_OK) {
                                Serial.print(F("(size = "));
                                Serial.print(size);
                                Serial.print(F(")"));
                                error = Morikawa.getText(i, dump, 10, &size);
                                if (error == TSTERROR_OK) {
                                    Serial.print(F("(size = "));
                                    Serial.print(size);
                                    Serial.print(F(")"));
                                    if (strcmp_P(dump, PSTR("TEXTTEXT_")) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    if (error == TSTERROR_OK) {
                                        error = Morikawa.getText(i, dump, sizeof(dump), &size);
                                        if (error == TSTERROR_OK) {
                                            Serial.print(F("(size = "));
                                            Serial.print(size);
                                            Serial.print(F(")"));
                                            if (strcmp(test, dump) != 0) {
                                                error = TSTERROR_FAILED;
                                            }
                                            if (error == TSTERROR_OK) {
                                                error = Morikawa.setText(i, "");
                                                if (error == TSTERROR_OK) {
                                                    error = Morikawa.getText(i, dump, sizeof(dump), &size);
                                                    if (error == TSTERROR_OK) {
                                                        Serial.print(F("(size = "));
                                                        Serial.print(size);
                                                        Serial.print(F(")"));
                                                        if (strcmp_P(dump, PSTR("")) != 0) {
                                                            error = TSTERROR_FAILED;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                post(error);
            }
        }
        // setTextPGM() / getText()
        {
            static char const test[] PROGMEM = "0012344321_!!!_1234432100";
            static char const test_null[] PROGMEM = "";
            char dump[256] = {0};
            int i;
            int size;
            
            for (i = 0; i < TEXT_LIMIT; ++i) {
                pre("setTextPGM / getText");
                error = Morikawa.setTextPGM(i, NULL);
                if (error == TSTERROR_INVALID_PARAM) {
                    error = Morikawa.setTextPGM(i, test);
                    if (error == TSTERROR_OK) {
                        error = Morikawa.getText(i, NULL, 10);
                        if (error == TSTERROR_OK) {
                            error = Morikawa.getText(i, NULL, 10, &size);
                            if (error == TSTERROR_OK) {
                                Serial.print(F("(size = "));
                                Serial.print(size);
                                Serial.print(F(")"));
                                error = Morikawa.getText(i, dump, 10, &size);
                                if (error == TSTERROR_OK) {
                                    Serial.print(F("(size = "));
                                    Serial.print(size);
                                    Serial.print(F(")"));
                                    if (strcmp_P(dump, PSTR("001234432")) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    if (error == TSTERROR_OK) {
                                        error = Morikawa.getText(i, dump, sizeof(dump), &size);
                                        if (error == TSTERROR_OK) {
                                            Serial.print(F("(size = "));
                                            Serial.print(size);
                                            Serial.print(F(")"));
                                            if (strcmp_P(dump, PSTR("0012344321_!!!_1234432100")) != 0) {
                                                error = TSTERROR_FAILED;
                                            }
                                            if (error == TSTERROR_OK) {
                                                error = Morikawa.setTextPGM(i, test_null);
                                                if (error == TSTERROR_OK) {
                                                    error = Morikawa.getText(i, dump, sizeof(dump), &size);
                                                    if (error == TSTERROR_OK) {
                                                        Serial.print(F("(size = "));
                                                        Serial.print(size);
                                                        Serial.print(F(")"));
                                                        if (strcmp_P(dump, PSTR("")) != 0) {
                                                            error = TSTERROR_FAILED;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                post(error);
            }
        }
        // setLED() / getLED()
        {
            int i;
            int j;
            
            pre("setLED / getLED");
            for (i = LED_X; i <= LED_Y; ++i) {
                for (j = 255; j >= 0; --j) {
                    if ((error = Morikawa.setLED(i, j)) != TSTERROR_OK) {
                        break;
                    }
                    if (Morikawa.getLED(i) != j) {
                        error = TSTERROR_FAILED;
                        break;
                    }
                    delay(5);
                }
                if (error != TSTERROR_OK) {
                    break;
                }
            }
            post(error);
        }
        // setNoteBPM() / getNoteBPM()
        {
            pre("setNoteBPM / getNoteBPM [120]");
            error = Morikawa.setNoteBPM(120);
            if (error == TSTERROR_OK) {
                if (Morikawa.getNoteBPM() != 120) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
            pre("setNoteBPM / getNoteBPM [0]");
            error = Morikawa.setNoteBPM(0);
            if (error == TSTERROR_OK) {
                if (Morikawa.getNoteBPM() != 1) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
            pre("setNoteBPM / getNoteBPM [-1]");
            error = Morikawa.setNoteBPM(-1);
            if (error == TSTERROR_OK) {
                if (Morikawa.getNoteBPM() != 120) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
        }
        // setMorseWPM() / getMorseWPM()
        {
            pre("setMorseWPM / getMorseWPM [20]");
            error = Morikawa.setMorseWPM(20);
            if (error == TSTERROR_OK) {
                if (Morikawa.getMorseWPM() != 20) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
            pre("setMorseWPM / getMorseWPM [0]");
            error = Morikawa.setMorseWPM(0);
            if (error == TSTERROR_OK) {
                if (Morikawa.getMorseWPM() != 1) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
            pre("setMorseWPM / getMorseWPM [-1]");
            error = Morikawa.setMorseWPM(-1);
            if (error == TSTERROR_OK) {
                if (Morikawa.getMorseWPM() != 20) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
        }
        // setSpeakAsyncMode() / getSpeakAsyncMode()
        {
            pre("setSpeakAsyncMode / getSpeakAsyncMode [true]");
            error = Morikawa.setSpeakAsyncMode(true);
            if (error == TSTERROR_OK) {
                if (!Morikawa.getSpeakAsyncMode()) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
            pre("setSpeakAsyncMode / getSpeakAsyncMode [false]");
            error = Morikawa.setSpeakAsyncMode(false);
            if (error == TSTERROR_OK) {
                if (Morikawa.getSpeakAsyncMode()) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
        }
        // isValid() family
        {
            isValidTest(true);
        }
        // isBusyDigitalker()
        {
            bool busy;
            
            pre("isBusyDigitalker");
            error = Morikawa.isBusyDigitalker(NULL);
            if (error == TSTERROR_OK) {
                error = Morikawa.isBusyDigitalker(&busy);
                if (error == TSTERROR_OK) {
                    if (busy) {
                        error = TSTERROR_FAILED;
                    }
                }
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("busy", busy);
            }
        }
        // hasAbnormalShutdown()
        {
            bool shutdown;
            
            shutdown = Morikawa.hasAbnormalShutdown();
            Serial.print(F("hasAbnormalShutdown"));
            Serial.print(F(" = "));
            Serial.println(shutdown);
        }
        // formatEEPROM()
        {
#ifdef DO_EEPROM_FORMAT_TEST
            unsigned long i;
            
            pre("formatEEPROM");
            error = TSTERROR_OK;
            for (i = 0; i < EEPROM_SIZE; ++i) {
                EEPROM.write(i, 0xBD);
            }
            Serial.print(F("."));
            Morikawa.formatEEPROM();
            Serial.print(F("."));
            for (i = 0; i < EEPROM_SIZE; ++i) {
                if (EEPROM.read(i) == 0xBD) {
                    error = TSTERROR_FAILED;
                    break;
                }
            }
            post(error);
#endif
        }
        // writeEEPROM()
        {
            char test[] = "EEPROM_text_test";
            unsigned int length;
            
            pre("writeEEPROM");
            error = Morikawa.writeEEPROM(0, test, lengthof(test));
            if (error == TSTERROR_OK) {
                error = Morikawa.writeEEPROM(0, test, lengthof(test), &length);
                if (error == TSTERROR_OK) {
                    if (length != lengthof(test)) {
                        error = TSTERROR_FAILED;
                    }
                    if (error == TSTERROR_OK) {
                        error = Morikawa.writeEEPROM(EEPROM_SIZE - 5, test, lengthof(test));
                        if (error == TSTERROR_INVALID_FORMAT) {
                            error = Morikawa.writeEEPROM(EEPROM_SIZE - 5, test, lengthof(test), &length);
                            if (error == TSTERROR_OK) {
                                if (length != 5) {
                                    error = TSTERROR_FAILED;
                                }
                            }
                        }
                        else {
                            error = TSTERROR_FAILED;
                        }
                    }
                }
            }
            post(error);
        }
        // readEEPROM()
        {
            char test[] = "EEPROM_text_test";
            char dump[256] = {0};
            unsigned int length;
            
            pre("readEEPROM");
            error = Morikawa.readEEPROM(0, dump, lengthof(test));
            if (error == TSTERROR_OK) {
                if (strcmp(dump, test) != 0) {
                    error = TSTERROR_FAILED;
                }
                if (error == TSTERROR_OK) {
                    error = Morikawa.readEEPROM(0, dump, lengthof(dump), &length);
                    if (error == TSTERROR_OK) {
                        if (strcmp(dump, test) != 0) {
                            error = TSTERROR_FAILED;
                        }
                        else if (length != lengthof(dump)) {
                            error = TSTERROR_FAILED;
                        }
                        if (error == TSTERROR_OK) {
                            error = Morikawa.readEEPROM(EEPROM_SIZE - 5, dump, lengthof(test));
                            if (error == TSTERROR_INVALID_FORMAT) {
                                error = Morikawa.readEEPROM(EEPROM_SIZE - 5, dump, lengthof(dump), &length);
                                if (error == TSTERROR_OK) {
                                    if (strncmp(dump, test, 5) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    else if (length != 5) {
                                        error = TSTERROR_FAILED;
                                    }
                                }
                            }
                            else {
                                error = TSTERROR_FAILED;
                            }
                        }
                    }
                }
            }
            post(error);
        }
        // writeEEPROMPGM()
        {
            static char const test[] PROGMEM = "E_PROGMEM_text";
            unsigned int length;
            
            pre("writeEEPROMPGM");
            error = Morikawa.writeEEPROMPGM(0, test, lengthof(test));
            if (error == TSTERROR_OK) {
                error = Morikawa.writeEEPROMPGM(0, test, lengthof(test), &length);
                if (error == TSTERROR_OK) {
                    if (length != lengthof(test)) {
                        error = TSTERROR_FAILED;
                    }
                    if (error == TSTERROR_OK) {
                        error = Morikawa.writeEEPROMPGM(EEPROM_SIZE - 5, test, lengthof(test));
                        if (error == TSTERROR_INVALID_FORMAT) {
                            error = Morikawa.writeEEPROMPGM(EEPROM_SIZE - 5, test, lengthof(test), &length);
                            if (error == TSTERROR_OK) {
                                if (length != 5) {
                                    error = TSTERROR_FAILED;
                                }
                            }
                        }
                        else {
                            error = TSTERROR_FAILED;
                        }
                    }
                }
            }
            post(error);
        }
        // readEEPROM()
        {
            char test[] = "E_PROGMEM_text";
            char dump[256] = {0};
            unsigned int length;
            
            pre("readEEPROM");
            error = Morikawa.readEEPROM(0, dump, lengthof(test));
            if (error == TSTERROR_OK) {
                if (strcmp(dump, test) != 0) {
                    error = TSTERROR_FAILED;
                }
                if (error == TSTERROR_OK) {
                    error = Morikawa.readEEPROM(0, dump, lengthof(dump), &length);
                    if (error == TSTERROR_OK) {
                        if (strcmp(dump, test) != 0) {
                            error = TSTERROR_FAILED;
                        }
                        else if (length != lengthof(dump)) {
                            error = TSTERROR_FAILED;
                        }
                        if (error == TSTERROR_OK) {
                            error = Morikawa.readEEPROM(EEPROM_SIZE - 5, dump, lengthof(test));
                            if (error == TSTERROR_INVALID_FORMAT) {
                                error = Morikawa.readEEPROM(EEPROM_SIZE - 5, dump, lengthof(dump), &length);
                                if (error == TSTERROR_OK) {
                                    if (strncmp(dump, test, 5) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    else if (length != 5) {
                                        error = TSTERROR_FAILED;
                                    }
                                }
                            }
                            else {
                                error = TSTERROR_FAILED;
                            }
                        }
                    }
                }
            }
            post(error);
        }
        // formatSharedMemory()
        {
#ifdef DO_SHAREDMEMORY_FORMAT_TEST
            unsigned long i;
            int j;
            
            pre("formatSharedMemory");
            error = TSTERROR_OK;
            for (i = 0; i < SHAREDMEMORY_SIZE; ) {
                while (true) {
                    I2Cm.clear();
                    if (I2Cm.send(((i >> 16) & 0x07) + 0x50, true) == 0) {
                        break;
                    }
                }
                I2Cm.clear();
                I2Cm.write((i >> 8) & 0xFF);
                I2Cm.write((i >> 0) & 0xFF);
                for (j = 0; j < 128; ++j) {
                    I2Cm.write(0xBD);
                }
                if (I2Cm.send(((i >> 16) & 0x07) + 0x50, true) != 0) {
                    error = TSTERROR_FAILED;
                    break;
                }
                i += j;
            }
            if (error == TSTERROR_OK) {
                Serial.print(F("."));
                Morikawa.formatSharedMemory();
                Serial.print(F("."));
                for (i = 0; i < SHAREDMEMORY_SIZE; ) {
                    I2Cm.clear();
                    I2Cm.write((i >> 8) & 0xFF);
                    I2Cm.write((i >> 0) & 0xFF);
                    if (I2Cm.send(((i >> 16) & 0x07) + 0x50, false) != 0) {
                        error = TSTERROR_FAILED;
                        break;
                    }
                    I2Cm.receive(((i >> 16) & 0x07) + 0x50, 1, true);
                    if (I2Cm.available() > 0) {
                        if (I2Cm.read() == 0xBD) {
                            error = TSTERROR_FAILED;
                            break;
                        }
                        ++i;
                    }
                    else {
                        Serial.print(F("read error 0x"));
                        Serial.println(i, HEX);
                    }
                }
            }
            post(error);
#endif
        }
        // writeSharedMemory()
        {
            char test[] = "SHAREDMEMORY_text_test";
            unsigned int length;
            
            pre("writeSharedMemory");
            error = Morikawa.writeSharedMemory(0, test, lengthof(test));
            if (error == TSTERROR_OK) {
                error = Morikawa.writeSharedMemory(0, test, lengthof(test), &length);
                if (error == TSTERROR_OK) {
                    if (length != lengthof(test)) {
                        error = TSTERROR_FAILED;
                    }
                    if (error == TSTERROR_OK) {
                        error = Morikawa.writeSharedMemory(SHAREDMEMORY_SIZE - 5, test, lengthof(test));
                        if (error == TSTERROR_INVALID_FORMAT) {
                            error = Morikawa.writeSharedMemory(SHAREDMEMORY_SIZE - 5, test, lengthof(test), &length);
                            if (error == TSTERROR_OK) {
                                if (length != 5) {
                                    error = TSTERROR_FAILED;
                                }
                            }
                        }
                        else {
                            error = TSTERROR_FAILED;
                        }
                    }
                }
            }
            post(error);
        }
        // readSharedMemory()
        {
            char test[] = "SHAREDMEMORY_text_test";
            char dump[256] = {0};
            unsigned int length;
            
            pre("readSharedMemory");
            error = Morikawa.readSharedMemory(0, dump, lengthof(test));
            if (error == TSTERROR_OK) {
                if (strcmp(dump, test) != 0) {
                    error = TSTERROR_FAILED;
                }
                if (error == TSTERROR_OK) {
                    error = Morikawa.readSharedMemory(0, dump, lengthof(dump), &length);
                    if (error == TSTERROR_OK) {
                        if (strcmp(dump, test) != 0) {
                            error = TSTERROR_FAILED;
                        }
                        else if (length != lengthof(dump)) {
                            error = TSTERROR_FAILED;
                        }
                        if (error == TSTERROR_OK) {
                            error = Morikawa.readSharedMemory(SHAREDMEMORY_SIZE - 5, dump, lengthof(test));
                            if (error == TSTERROR_INVALID_FORMAT) {
                                error = Morikawa.readSharedMemory(SHAREDMEMORY_SIZE - 5, dump, lengthof(dump), &length);
                                if (error == TSTERROR_OK) {
                                    if (strncmp(dump, test, 5) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    else if (length != 5) {
                                        error = TSTERROR_FAILED;
                                    }
                                }
                            }
                            else {
                                error = TSTERROR_FAILED;
                            }
                        }
                    }
                }
            }
            post(error);
        }
        // writeSharedMemoryPGM()
        {
            static char const test[] PROGMEM = "S_PROGMEM_text";
            unsigned int length;
            
            pre("writeSharedMemoryPGM");
            error = Morikawa.writeSharedMemoryPGM(0, test, lengthof(test));
            if (error == TSTERROR_OK) {
                error = Morikawa.writeSharedMemoryPGM(0, test, lengthof(test), &length);
                if (error == TSTERROR_OK) {
                    if (length != lengthof(test)) {
                        error = TSTERROR_FAILED;
                    }
                    if (error == TSTERROR_OK) {
                        error = Morikawa.writeSharedMemoryPGM(SHAREDMEMORY_SIZE - 5, test, lengthof(test));
                        if (error == TSTERROR_INVALID_FORMAT) {
                            error = Morikawa.writeSharedMemoryPGM(65536 - 5, test, lengthof(test), &length);
                            if (error == TSTERROR_OK) {
                                if (length != lengthof(test)) {
                                    error = TSTERROR_FAILED;
                                }
                            }
                        }
                        else {
                            error = TSTERROR_FAILED;
                        }
                    }
                }
            }
            post(error);
        }
        // readSharedMemory()
        {
            char test[] = "S_PROGMEM_text";
            char dump[256] = {0};
            unsigned int length;
            
            pre("readSharedMemory");
            error = Morikawa.readSharedMemory(0, dump, lengthof(test));
            if (error == TSTERROR_OK) {
                if (strcmp(dump, test) != 0) {
                    error = TSTERROR_FAILED;
                }
                if (error == TSTERROR_OK) {
                    error = Morikawa.readSharedMemory(0, dump, lengthof(dump), &length);
                    if (error == TSTERROR_OK) {
                        if (strcmp(dump, test) != 0) {
                            error = TSTERROR_FAILED;
                        }
                        else if (length != lengthof(dump)) {
                            error = TSTERROR_FAILED;
                        }
                        if (error == TSTERROR_OK) {
                            error = Morikawa.readSharedMemory(SHAREDMEMORY_SIZE - 5, dump, lengthof(test));
                            if (error == TSTERROR_INVALID_FORMAT) {
                                error = Morikawa.readSharedMemory(65536 - 5, dump, lengthof(dump), &length);
                                if (error == TSTERROR_OK) {
                                    if (strcmp(dump, test) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    else if (length != lengthof(dump)) {
                                        error = TSTERROR_FAILED;
                                    }
                                }
                            }
                            else {
                                error = TSTERROR_FAILED;
                            }
                        }
                    }
                }
            }
            post(error);
        }
        // formatFRAM()
        {
#ifdef DO_FRAM_FORMAT_TEST
            unsigned long i;
            
            pre("formatFRAM");
            error = TSTERROR_OK;
            digitalWrite(PIN_FRAM_CS, LOW);
            SPI.transfer(0x06);
            digitalWrite(PIN_FRAM_CS, HIGH);
            delayMicroseconds(1);
            digitalWrite(PIN_FRAM_CS, LOW);
            SPI.transfer(0x02);
            SPI.transfer(0);
            SPI.transfer(0);
            SPI.transfer(0);
            for (i = 0; i < FRAM_SIZE; ++i) {
                SPI.transfer(0xBD);
            }
            digitalWrite(PIN_FRAM_CS, HIGH);
            delayMicroseconds(1);
            Serial.print(F("."));
            Morikawa.formatFRAM();
            Serial.print(F("."));
            digitalWrite(PIN_FRAM_CS, LOW);
            SPI.transfer(0x03);
            SPI.transfer(0);
            SPI.transfer(0);
            SPI.transfer(0);
            for (i = 0; i < FRAM_SIZE; ++i) {
                if (SPI.transfer(0x00) == 0xBD) {
                    error = TSTERROR_FAILED;
                    break;
                }
            }
            digitalWrite(PIN_FRAM_CS, HIGH);
            delayMicroseconds(1);
            post(error);
#endif
        }
        // writeFRAM()
        {
            char test[] = "FRAM_text_test";
            unsigned int length;
            
            pre("writeFRAM");
            error = Morikawa.writeFRAM(0, test, lengthof(test));
            if (error == TSTERROR_OK) {
                error = Morikawa.writeFRAM(0, test, lengthof(test), &length);
                if (error == TSTERROR_OK) {
                    if (length != lengthof(test)) {
                        error = TSTERROR_FAILED;
                    }
                    if (error == TSTERROR_OK) {
                        error = Morikawa.writeFRAM(FRAM_SIZE - 5, test, lengthof(test));
                        if (error == TSTERROR_INVALID_FORMAT) {
                            error = Morikawa.writeFRAM(FRAM_SIZE - 5, test, lengthof(test), &length);
                            if (error == TSTERROR_OK) {
                                if (length != 5) {
                                    error = TSTERROR_FAILED;
                                }
                            }
                        }
                        else {
                            error = TSTERROR_FAILED;
                        }
                    }
                }
            }
            post(error);
        }
        // readFRAM()
        {
            char test[] = "FRAM_text_test";
            char dump[256] = {0};
            unsigned int length;
            
            pre("readFRAM");
            error = Morikawa.readFRAM(0, dump, lengthof(test));
            if (error == TSTERROR_OK) {
                if (strcmp(dump, test) != 0) {
                    error = TSTERROR_FAILED;
                }
                if (error == TSTERROR_OK) {
                    error = Morikawa.readFRAM(0, dump, lengthof(dump), &length);
                    if (error == TSTERROR_OK) {
                        if (strcmp(dump, test) != 0) {
                            error = TSTERROR_FAILED;
                        }
                        else if (length != lengthof(dump)) {
                            error = TSTERROR_FAILED;
                        }
                        if (error == TSTERROR_OK) {
                            error = Morikawa.readFRAM(FRAM_SIZE - 5, dump, lengthof(test));
                            if (error == TSTERROR_INVALID_FORMAT) {
                                error = Morikawa.readFRAM(FRAM_SIZE - 5, dump, lengthof(dump), &length);
                                if (error == TSTERROR_OK) {
                                    if (strncmp(dump, test, 5) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    else if (length != 5) {
                                        error = TSTERROR_FAILED;
                                    }
                                }
                            }
                            else {
                                error = TSTERROR_FAILED;
                            }
                        }
                    }
                }
            }
            post(error);
        }
        // writeFRAMPGM()
        {
            static char const test[] PROGMEM = "F_PROGMEM_text";
            unsigned int length;
            
            pre("writeFRAMPGM");
            error = Morikawa.writeFRAMPGM(0, test, lengthof(test));
            if (error == TSTERROR_OK) {
                error = Morikawa.writeFRAMPGM(0, test, lengthof(test), &length);
                if (error == TSTERROR_OK) {
                    if (length != lengthof(test)) {
                        error = TSTERROR_FAILED;
                    }
                    if (error == TSTERROR_OK) {
                        error = Morikawa.writeFRAMPGM(FRAM_SIZE - 5, test, lengthof(test));
                        if (error == TSTERROR_INVALID_FORMAT) {
                            error = Morikawa.writeFRAMPGM(FRAM_SIZE - 5, test, lengthof(test), &length);
                            if (error == TSTERROR_OK) {
                                if (length != 5) {
                                    error = TSTERROR_FAILED;
                                }
                            }
                        }
                        else {
                            error = TSTERROR_FAILED;
                        }
                    }
                }
            }
            post(error);
        }
        // readFRAM()
        {
            char test[] = "F_PROGMEM_text";
            char dump[256] = {0};
            unsigned int length;
            
            pre("readFRAM");
            error = Morikawa.readFRAM(0, dump, lengthof(test));
            if (error == TSTERROR_OK) {
                if (strcmp(dump, test) != 0) {
                    error = TSTERROR_FAILED;
                }
                if (error == TSTERROR_OK) {
                    error = Morikawa.readFRAM(0, dump, lengthof(dump), &length);
                    if (error == TSTERROR_OK) {
                        if (strcmp(dump, test) != 0) {
                            error = TSTERROR_FAILED;
                        }
                        else if (length != lengthof(dump)) {
                            error = TSTERROR_FAILED;
                        }
                        if (error == TSTERROR_OK) {
                            error = Morikawa.readFRAM(FRAM_SIZE - 5, dump, lengthof(test));
                            if (error == TSTERROR_INVALID_FORMAT) {
                                error = Morikawa.readFRAM(FRAM_SIZE - 5, dump, lengthof(dump), &length);
                                if (error == TSTERROR_OK) {
                                    if (strncmp(dump, test, 5) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    else if (length != 5) {
                                        error = TSTERROR_FAILED;
                                    }
                                }
                            }
                            else {
                                error = TSTERROR_FAILED;
                            }
                        }
                    }
                }
            }
            post(error);
        }
        // formatFlashROM()
        {
#ifdef DO_FLASHROM_FORMAT_TEST
            unsigned long i;
            
            pre("formatFlashROM");
            for (i = EEPROM_FLASHROM; i < EEPROM_SELFTEST; ++i) {
                EEPROM.write(i, 0x00);
            }
            Serial.print(F("."));
            Morikawa.formatFlashROM();
            Serial.print(F("."));
            for (i = EEPROM_FLASHROM; i < EEPROM_SELFTEST; ++i) {
                if (EEPROM.read(i) == 0x00) {
                    error = TSTERROR_FAILED;
                    break;
                }
                else if (EEPROM.read(i) != 0xFF) {
                    Serial.println(F(""));
                    Serial.print(F("FlashROM bad page ["));
                    Serial.print(i);
                    Serial.print(F(" , "));
                    Serial.print(EEPROM.read(i));
                    Serial.print(F("] = "));
                }
            }
            post(error);
#endif
        }
        // writeFlashROM()
        {
            char test[] = "FLASHROM_text_test";
            unsigned int length;
            
            pre("writeFlashROM");
            error = Morikawa.writeFlashROM(0, test, lengthof(test));
            if (error == TSTERROR_OK) {
                error = Morikawa.writeFlashROM(256, test, lengthof(test), &length);
                if (error == TSTERROR_OK) {
                    if (length != lengthof(test)) {
                        error = TSTERROR_FAILED;
                    }
                    if (error == TSTERROR_OK) {
                        error = Morikawa.writeFlashROM(FLASHROM_SIZE - 5, test, lengthof(test));
                        if (error == TSTERROR_INVALID_FORMAT) {
                            error = Morikawa.writeFlashROM(FLASHROM_SIZE - 5, test, lengthof(test), &length);
                            if (error == TSTERROR_OK) {
                                if (length != 5) {
                                    error = TSTERROR_FAILED;
                                }
                            }
                        }
                        else {
                            error = TSTERROR_FAILED;
                        }
                    }
                }
            }
            post(error);
        }
        // readFlashROM()
        {
            char test[] = "FLASHROM_text_test";
            char dump[256] = {0};
            unsigned int length;
            
            pre("readFlashROM");
            error = Morikawa.readFlashROM(0, dump, lengthof(test));
            if (error == TSTERROR_OK) {
                if (strcmp(dump, test) != 0) {
                    error = TSTERROR_FAILED;
                }
                if (error == TSTERROR_OK) {
                    error = Morikawa.readFlashROM(256, dump, lengthof(dump), &length);
                    if (error == TSTERROR_OK) {
                        if (strcmp(dump, test) != 0) {
                            error = TSTERROR_FAILED;
                        }
                        else if (length != lengthof(dump)) {
                            error = TSTERROR_FAILED;
                        }
                        if (error == TSTERROR_OK) {
                            error = Morikawa.readFlashROM(FLASHROM_SIZE - 5, dump, lengthof(test));
                            if (error == TSTERROR_INVALID_FORMAT) {
                                error = Morikawa.readFlashROM(FLASHROM_SIZE - 5, dump, lengthof(dump), &length);
                                if (error == TSTERROR_OK) {
                                    if (strncmp(dump, test, 5) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    else if (length != 5) {
                                        error = TSTERROR_FAILED;
                                    }
                                }
                            }
                            else {
                                error = TSTERROR_FAILED;
                            }
                        }
                    }
                }
            }
            post(error);
        }
        // writeFlashROMPGM()
        {
            static char const test[] PROGMEM = "X_PROGMEM_text";
            unsigned int length;
            
            pre("writeFlashROMPGM");
            error = Morikawa.writeFlashROMPGM(512, test, lengthof(test));
            if (error == TSTERROR_OK) {
                error = Morikawa.writeFlashROMPGM(768, test, lengthof(test), &length);
                if (error == TSTERROR_OK) {
                    if (length != lengthof(test)) {
                        error = TSTERROR_FAILED;
                    }
                    if (error == TSTERROR_OK) {
                        error = Morikawa.writeFlashROMPGM(FLASHROM_SIZE - 5 - 256, test, lengthof(test));
                        if (error == TSTERROR_USED_MEMORY) {
                            error = Morikawa.writeFlashROMPGM(FLASHROM_SIZE - 5 - 512, test, lengthof(test), &length);
                            if (error == TSTERROR_OK) {
                                if (length != lengthof(test)) {
                                    error = TSTERROR_FAILED;
                                }
                            }
                        }
                        else {
                            error = TSTERROR_FAILED;
                        }
                    }
                }
            }
            post(error);
        }
        // readFlashROM()
        {
            char test[] = "X_PROGMEM_text";
            char dump[256] = {0};
            unsigned int length;
            
            pre("readFlashROM");
            error = Morikawa.readFlashROM(512, dump, lengthof(test));
            if (error == TSTERROR_OK) {
                if (strcmp(dump, test) != 0) {
                    error = TSTERROR_FAILED;
                }
                if (error == TSTERROR_OK) {
                    error = Morikawa.readFlashROM(768, dump, lengthof(dump), &length);
                    if (error == TSTERROR_OK) {
                        if (strcmp(dump, test) != 0) {
                            error = TSTERROR_FAILED;
                        }
                        else if (length != lengthof(dump)) {
                            error = TSTERROR_FAILED;
                        }
                        if (error == TSTERROR_OK) {
                            error = Morikawa.readFlashROM(FLASHROM_SIZE - 5, dump, lengthof(test));
                            if (error == TSTERROR_INVALID_FORMAT) {
                                error = Morikawa.readFlashROM(FLASHROM_SIZE - 5 - 512, dump, lengthof(dump), &length);
                                if (error == TSTERROR_OK) {
                                    if (strncmp(dump, test, lengthof(test)) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    else if (length != lengthof(dump)) {
                                        error = TSTERROR_FAILED;
                                    }
                                }
                            }
                            else {
                                error = TSTERROR_FAILED;
                            }
                        }
                    }
                }
            }
            post(error);
        }
        // FlashROM EEPROM verify
        {
            unsigned char flag;
            
            pre("FlashROM EEPROM verify");
            error = TSTERROR_OK;
            flag = EEPROM.read(EEPROM_FLASHROM);
            if (flag != 0b01010101) {
                error = TSTERROR_FAILED;
            }
            if (error == TSTERROR_OK) {
                flag = EEPROM.read(EEPROM_SELFTEST - 1);
                if (flag != 0b11010101) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
        }
        // writeFlashROM() erase
        {
            char test[] = "FLASHROM_text_test";
            unsigned int length;
            
            pre("writeFlashROM erase");
            error = Morikawa.setFlashROMEraseMode(true);
            if (error == TSTERROR_OK) {
                error = Morikawa.writeFlashROM(0, test, lengthof(test));
                if (error == TSTERROR_OK) {
                    error = Morikawa.writeFlashROM(256, test, lengthof(test), &length);
                    if (error == TSTERROR_OK) {
                        if (length != lengthof(test)) {
                            error = TSTERROR_FAILED;
                        }
                        if (error == TSTERROR_OK) {
                            error = Morikawa.writeFlashROM(FLASHROM_SIZE - 5, test, lengthof(test));
                            if (error == TSTERROR_INVALID_FORMAT) {
                                error = Morikawa.writeFlashROM(FLASHROM_SIZE - 5, test, lengthof(test), &length);
                                if (error == TSTERROR_OK) {
                                    if (length != 5) {
                                        error = TSTERROR_FAILED;
                                    }
                                }
                            }
                            else {
                                error = TSTERROR_FAILED;
                            }
                        }
                    }
                }
            }
            post(error);
        }
        // readFlashROM()
        {
            char test[] = "FLASHROM_text_test";
            char dump[256] = {0};
            unsigned int length;
            
            pre("readFlashROM");
            error = Morikawa.readFlashROM(0, dump, lengthof(test));
            if (error == TSTERROR_OK) {
                if (strcmp(dump, test) != 0) {
                    error = TSTERROR_FAILED;
                }
                if (error == TSTERROR_OK) {
                    error = Morikawa.readFlashROM(256, dump, lengthof(dump), &length);
                    if (error == TSTERROR_OK) {
                        if (strcmp(dump, test) != 0) {
                            error = TSTERROR_FAILED;
                        }
                        else if (length != lengthof(dump)) {
                            error = TSTERROR_FAILED;
                        }
                        if (error == TSTERROR_OK) {
                            error = Morikawa.readFlashROM(FLASHROM_SIZE - 5, dump, lengthof(test));
                            if (error == TSTERROR_INVALID_FORMAT) {
                                error = Morikawa.readFlashROM(FLASHROM_SIZE - 5, dump, lengthof(dump), &length);
                                if (error == TSTERROR_OK) {
                                    if (strncmp(dump, test, 5) != 0) {
                                        error = TSTERROR_FAILED;
                                    }
                                    else if (length != 5) {
                                        error = TSTERROR_FAILED;
                                    }
                                }
                            }
                            else {
                                error = TSTERROR_FAILED;
                            }
                        }
                    }
                }
            }
            post(error);
        }
        // FlashROM EEPROM verify
        {
            unsigned char flag;
            
            pre("FlashROM EEPROM verify");
            error = TSTERROR_OK;
            flag = EEPROM.read(EEPROM_FLASHROM);
            if (flag != 0b01011111) {
                error = TSTERROR_FAILED;
            }
            if (error == TSTERROR_OK) {
                flag = EEPROM.read(EEPROM_SELFTEST - 1);
                if (flag != 0b11111101) {
                    error = TSTERROR_FAILED;
                }
            }
            post(error);
        }
        // playNote()
        {
            pre("playNote");
            error = Morikawa.setNoteBPM(120);
            if (error == TSTERROR_OK) {
                error = Morikawa.playNote(NOTE_C6, DURATION_04, QUALIFIER_NONE);
                delay(2000);
            }
            post(error);
        }
        // playNote()
        {
            NoteSequence sequence[] = {
                {NOTE_C6, DURATION_04, QUALIFIER_DOT},
                {NOTE_REST, DURATION_04},
                {NOTE_G5, DURATION_08},
                {NOTE_END}
            };
            
            pre("playNote");
            error = Morikawa.playNote(sequence);
            delay(2000);
            if (error == TSTERROR_OK) {
                error = Morikawa.playNote(sequence, 3);
                delay(2000);
            }
            post(error);
        }
        // playNotePGM()
        {
            static NoteSequence const sequence[] PROGMEM = {
                {NOTE_C6, DURATION_04, QUALIFIER_DOT},
                {NOTE_REST, DURATION_04},
                {NOTE_G5, DURATION_08},
                {NOTE_END}
            };
            
            pre("playNotePGM");
            error = Morikawa.playNotePGM(sequence);
            delay(2000);
            if (error == TSTERROR_OK) {
                error = Morikawa.playNotePGM(sequence, 3);
                delay(2000);
            }
            post(error);
        }
        // playMorse()
        {
            pre("playMorse");
            error = Morikawa.setMorseWPM(20);
            if (error == TSTERROR_OK) {
                error = Morikawa.playMorse(NOTE_E5, 'S');
                delay(2000);
            }
            post(error);
        }
        // playMorse()
        {
            char sos[] = "sos";
            
            pre("playMorse");
            error = Morikawa.playMorse(NOTE_C5, sos);
            delay(2000);
            if (error == TSTERROR_OK) {
                error = Morikawa.playMorse(NOTE_E5, sos, 2);
                delay(2000);
            }
            post(error);
        }
        // playMorsePGM()
        {
            static char const sos[] PROGMEM = "sos";
            
            pre("playMorsePGM");
            error = Morikawa.playMorsePGM(NOTE_C6, sos);
            delay(2000);
            if (error == TSTERROR_OK) {
                error = Morikawa.playMorsePGM(NOTE_E6, sos, 2);
                delay(2000);
            }
            post(error);
        }
        // speakPhrase()
        {
            char aiueo[] = "a i u e o";
            
            pre("speakPhrase");
            error = Morikawa.speakPhrase(aiueo);
            delay(2000);
            if (error == TSTERROR_OK) {
                error = Morikawa.speakPhrase(aiueo, 3);
                delay(2000);
            }
            post(error);
        }
        // speakPhrasePGM()
        {
            static char const aiueo[] PROGMEM = "a i u e o";
            
            pre("speakPhrasePGM");
            error = Morikawa.speakPhrasePGM(aiueo);
            delay(2000);
            if (error == TSTERROR_OK) {
                error = Morikawa.speakPhrasePGM(aiueo, 3);
                delay(2000);
            }
            post(error);
        }
        // waitPhrase()
        {
            pre("waitPhrase");
            error = Morikawa.waitPhrase();
            post(error);
        }
        // stopPhrase()
        {
            pre("stopPhrase");
            error = Morikawa.stopPhrase();
            post(error);
        }
        // speakPhrase() async
        {
            char aiueo[] = "a i u e o";
            
            pre("speakPhrase async");
            error = Morikawa.setSpeakAsyncMode(true);
            if (error == TSTERROR_OK) {
                error = Morikawa.speakPhrase(aiueo);
            }
            post(error);
        }
        // isBusyDigitalker()
        {
            bool busy;
            
            pre("isBusyDigitalker");
            error = Morikawa.isBusyDigitalker(NULL);
            if (error == TSTERROR_OK) {
                error = Morikawa.isBusyDigitalker(&busy);
                if (error == TSTERROR_OK) {
                    if (!busy) {
                        error = TSTERROR_FAILED;
                    }
                }
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("busy", busy);
            }
        }
        // waitPhrase()
        {
            pre("waitPhrase");
            error = Morikawa.waitPhrase();
            post(error);
        }
        // speakPhrase() async-stop
        {
            char aiueo[] = "a i u e o";
            
            pre("speakPhrase async-stop");
            error = Morikawa.speakPhrase(aiueo);
            if (error == TSTERROR_OK) {
                delay(2000);
                error = Morikawa.stopPhrase();
            }
            post(error);
        }
        {
            static char const g_string[] PROGMEM = "This is MorikawaSDK. MorikawaSDK supports FastLZ data compression algorithm...MorikawaSDK MorikawaSDK MorikawaSDK";
            unsigned long output;
            
            // freezeFastLZ()
            {
                unsigned long work;
                
                Morikawa.writeSharedMemoryPGM(0, g_string, lengthof(g_string));
                pre("freezeFastLZ");
                error = Morikawa.freezeFastLZ(STORAGE_NONE, 0, 0, STORAGE_NONE, 0, 0, STORAGE_NONE, 0, 0, &output);
                if (error == TSTERROR_INVALID_STORAGE) {
                    error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_NONE, 0, 0, STORAGE_NONE, 0, 0, NULL);
                    if (error == TSTERROR_INVALID_PARAM) {
                        error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_NONE, 0, 0, STORAGE_FRAM, 0, 0, NULL);
                        if (error == TSTERROR_INVALID_PARAM) {
                            error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_FRAM, 0, 0, STORAGE_NONE, 0, 0, NULL);
                            if (error == TSTERROR_INVALID_PARAM) {
                                error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_FRAM, 0, 0, STORAGE_FRAM, 0, 0, NULL);
                                if (error == TSTERROR_INVALID_PARAM) {
                                    error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_NONE, 0, 0, STORAGE_NONE, 0, 0, &output);
                                    if (error == TSTERROR_OK) {
                                        error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_FRAM, 0, output, STORAGE_NONE, 0, 0, &work);
                                        if (error == TSTERROR_OK) {
                                            error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_FRAM, 0, output, STORAGE_FRAM, output - 1, work, &output);
                                            if (error == TSTERROR_INVALID_PARAM) {
                                                error = Morikawa.freezeFastLZ(STORAGE_FRAM, 0, lengthof(g_string), STORAGE_FRAM, 0, output, STORAGE_FRAM, output, work, &output);
                                                if (error == TSTERROR_INVALID_PARAM) {
                                                    error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_FRAM, 0, output, STORAGE_FRAM, output, work, &output);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                post(error);
            }
            // meltFastLZ()
            {
                char temp[256];
                
                pre("meltFastLZ");
                error = Morikawa.meltFastLZ(STORAGE_NONE, 0, 0, STORAGE_NONE, 0, 0, &output);
                if (error == TSTERROR_INVALID_STORAGE) {
                    error = Morikawa.meltFastLZ(STORAGE_FRAM, 0, 0, STORAGE_NONE, 0, 0, &output);
                    if (error == TSTERROR_INVALID_STORAGE) {
                        error = Morikawa.meltFastLZ(STORAGE_NONE, 0, 0, STORAGE_FRAM, 0, 0, &output);
                        if (error == TSTERROR_INVALID_STORAGE) {
                            error = Morikawa.meltFastLZ(STORAGE_FRAM, 0, output, STORAGE_SHAREDMEMORY, 512, lengthof(g_string), NULL);
                            if (error == TSTERROR_INVALID_PARAM) {
                                error = Morikawa.meltFastLZ(STORAGE_FRAM, 0, output, STORAGE_FRAM, output - 1, lengthof(g_string), &output);
                                if (error == TSTERROR_INVALID_PARAM) {
                                    error = Morikawa.meltFastLZ(STORAGE_FRAM, 0, output, STORAGE_SHAREDMEMORY, 512, lengthof(g_string), &output);
                                    if (error == TSTERROR_INVALID_STORAGE) {
                                        Morikawa.readSharedMemory(512, temp, output);
                                        if (strcmp_P(temp, g_string) != 0) {
                                            error = TSTERROR_FAILED;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                post(error);
            }
        }
        // enableAudioBus()
        {
            pre("enableAudioBus");
            error = Morikawa.enableAudioBus();
            post(error);
        }
        // disableAudioBus()
        {
            pre("disableAudioBus");
            Morikawa.disableAudioBus();
            post(TSTERROR_OK);
        }
        // loop()
        {
            pre("loop");
            Morikawa.loop();
            post(TSTERROR_OK);
        }
        // cleanup()
        {
            pre("cleanup");
            Morikawa.cleanup();
            post(TSTERROR_OK);
        }
        // getMemoryLog()
        {
            MemoryInfo info;
            
            pre("getMemoryLog");
            error = Morikawa.getMemoryLog(NULL);
            if (error == TSTERROR_INVALID_PARAM) {
                error = Morikawa.getMemoryLog(&info);
            }
            post(error);
            if (error == TSTERROR_OK) {
                dump("log.heap_size", info.heap_size);
                dump("log.heap_free", info.heap_free);
                dump("log.stack_size", info.stack_size);
                dump("log.stack_free", info.stack_free);
            }
        }
        
////////////////////////////////////////////////////////////////////////////////
    }
    // isValid() family
    {
        isValidTest(false);
    }
    Serial.println(F("...debug done..."));
    Serial.end();
    // shutdown()
    {
        Morikawa.shutdown();
    }
    return;
}

void loop(void)
{
    Morikawa.loop();
    
    // implement loop task here
    return;
}

static void isValidTest(bool flag)
{
    TSTError error;
    
    // isValid()
    {
        pre("isValid");
        error = TSTERROR_OK;
        if (Morikawa.isValid() != flag) {
            error = TSTERROR_FAILED;
        }
        post(error);
    }
    // isValidSharedMemory()
    {
        pre("isValidSharedMemory");
        error = TSTERROR_OK;
        if (Morikawa.isValidSharedMemory() != flag) {
            error = TSTERROR_FAILED;
        }
        post(error);
    }
    // isValidFRAM()
    {
        pre("isValidFRAM");
        error = TSTERROR_OK;
        if (Morikawa.isValidFRAM() != flag) {
            error = TSTERROR_FAILED;
        }
        post(error);
    }
    // isValidFlashROM()
    {
        pre("isValidFlashROM");
        error = TSTERROR_OK;
        if (Morikawa.isValidFlashROM() != flag) {
            error = TSTERROR_FAILED;
        }
        post(error);
    }
    // isValidLED()
    {
        pre("isValidLED");
        error = TSTERROR_OK;
        if (Morikawa.isValidLED() != flag) {
            error = TSTERROR_FAILED;
        }
        post(error);
    }
    // isValidTone()
    {
        pre("isValidTone");
        error = TSTERROR_OK;
        if (Morikawa.isValidTone() != flag) {
            error = TSTERROR_FAILED;
        }
        post(error);
    }
    // isValidDigitalker()
    {
        pre("isValidDigitalker");
        error = TSTERROR_OK;
        if (Morikawa.isValidDigitalker() != flag) {
            error = TSTERROR_FAILED;
        }
        post(error);
    }
    // isValidCamera()
    {
        pre("isValidCamera");
        error = TSTERROR_OK;
        if (Morikawa.isValidCamera() != flag) {
            error = TSTERROR_FAILED;
        }
        post(error);
    }
    return;
}

static void writeSharedMemory(unsigned long address, unsigned char data[128])
{
    int i;
    
    while (true) {
        I2Cm.clear();
        if (I2Cm.send(((address >> 16) & 0x07) + 0x50, true) == 0) {
            break;
        }
    }
    I2Cm.clear();
    I2Cm.write((address >> 8) & 0xFF);
    I2Cm.write((address >> 0) & 0xFF);
    for (i = 0; i < 128; ++i) {
        I2Cm.write(data[i]);
    }
    I2Cm.send(((address >> 16) & 0x07) + 0x50, true);
    return;
}

static void pre(char const* message)
{
    Serial.print(message);
    Serial.print(F(" = "));
    return;
}

static void post(TSTError error)
{
    if (error == TSTERROR_OK) {
        Serial.println(F("ok"));
    }
    else {
        Serial.print(F("/!\\ "));
        Serial.print(error);
        Serial.println(F(" /!\\"));
    }
    return;
}

static void dump(char const* message, void const* address)
{
    Serial.print(F("    "));
    Serial.print(message);
    Serial.print(F(" = 0x"));
    Serial.println(reinterpret_cast<size_t>(address), HEX);
    return;
}

static void dump(char const* message, TSTError error)
{
    Serial.print(F("    "));
    Serial.print(message);
    Serial.print(F(" = "));
    Serial.println(error);
    return;
}

static void dump(char const* message, bool flag)
{
    Serial.print(F("    "));
    Serial.print(message);
    Serial.print(F(" = "));
    Serial.println((flag) ? (F("true")) : (F("false")));
    return;
}

static void dump(char const* message, unsigned int value)
{
    Serial.print(F("    "));
    Serial.print(message);
    Serial.print(F(" = "));
    Serial.println(value);
    return;
}

static void dump(char const* message, unsigned long value)
{
    Serial.print(F("    "));
    Serial.print(message);
    Serial.print(F(" = "));
    Serial.println(value);
    return;
}
