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
**      MemoryStorage.ino
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

#define FLASH_ADDR_1    (0)
#define FLASH_ADDR_2    (777)
#define FRAM_ADDR       (0)
#define EEPROM_ADDR     (10)
#define SHARED_ADDR     (111)

static char const g_sample[] PROGMEM = "This is Morikawa of the ARTSAT Project.";

void setup(void)
{
    TSTError error;
    
    if (Morikawa.setup() == TSTERROR_OK) {
    
        // use debug serial
        Serial.begin(9600);
        
        // print size info
        Serial.print(F("EEPROM size = "));
        Serial.println(Morikawa.getSizeEEPROM());
        Serial.print(F("SharedMemory size = "));
        Serial.println(Morikawa.getSizeSharedMemory());
        Serial.print(F("SharedMemory page size = "));
        Serial.println(Morikawa.getPageSizeSharedMemory());
        Serial.print(F("FRAM size = "));
        Serial.println(Morikawa.getSizeFRAM());
        Serial.print(F("FlashROM size = "));
        Serial.println(Morikawa.getSizeFlashROM());
        Serial.print(F("FlashROM page size = "));
        Serial.println(Morikawa.getPageSizeFlashROM());
        Serial.print(F("FlashROM sector size = "));
        Serial.println(Morikawa.getSectorSizeFlashROM());
        Serial.println(F("---"));
        
#if 0
        // format FlashROM
        Serial.print(F("formatting FlashROM, wait..."));
        Morikawa.formatFlashROM();
        Serial.println(F("done."));
#else
        // set sector overwrite mode
        Morikawa.setFlashROMEraseMode(true);
#endif
        
        // write into FlashROM
        error = Morikawa.writeFlashROMPGM(FLASH_ADDR_1, g_sample, lengthof(g_sample));
        Serial.print(F("written into FlashROM (err = "));
        Serial.print(error);
        Serial.println(F(")."));
        
        error = Morikawa.writeFlashROM(FLASH_ADDR_2, "HAPPY!", lengthof("HAPPY!"));
        Serial.print(F("written into FlashROM (err = "));
        Serial.print(error);
        Serial.println(F(")."));
    }
    else {
        Morikawa.shutdown();
    }
    return;
}

void loop(void)
{
    char temp[256];
    TSTError error;
    
    Morikawa.loop();
    
    // read from FlashROM
    Morikawa.readFlashROM(FLASH_ADDR_1, temp, sizeof(temp));
    Serial.print(F("FLASH_ADDR_1 = "));
    Serial.println(temp);
    
    // write info FRAM
    error = Morikawa.writeFRAM(FRAM_ADDR, temp, strlen(temp) + 1);
    Serial.print(F("written into FRAM (err = "));
    Serial.print(error);
    Serial.println(F(")."));
    
    // read from FlashROM
    Morikawa.readFlashROM(FLASH_ADDR_2, temp, sizeof(temp));
    Serial.print(F("FLASH_ADDR_2 = "));
    Serial.println(temp);
    
    // write info EEPROM
    error = Morikawa.writeEEPROM(EEPROM_ADDR, temp, strlen(temp) + 1);
    Serial.print(F("written into EEPROM (err = "));
    Serial.print(error);
    Serial.println(F(")."));
    
    // read from EEPROM
    Morikawa.readEEPROM(EEPROM_ADDR, temp, sizeof(temp));
    Serial.print(F("EEPROM_ADDR = "));
    Serial.println(temp);
    
    // write info FRAM
    error = Morikawa.writeFRAM(FRAM_ADDR + 17, temp, strlen(temp) + 1);
    Serial.print(F("written into FRAM (err = "));
    Serial.print(error);
    Serial.println(F(")."));
    
    // read from FRAM
    error = Morikawa.readFRAM(FRAM_ADDR, temp, sizeof(temp));
    Serial.print(F("FRAM_ADDR = "));
    Serial.println(temp);
    
    // write info SharedMemory
    error = Morikawa.writeSharedMemory(SHARED_ADDR, temp, strlen(temp) + 1);
    Serial.print(F("written into SharedMemory (err = "));
    Serial.print(error);
    Serial.println(F(")."));
    
    // read from SharedMemory
    error = Morikawa.readSharedMemory(SHARED_ADDR, temp, sizeof(temp));
    Serial.print(F("SHARED_ADDR = "));
    Serial.println(temp);
    
    Serial.println(F("--- bye!"));
    Serial.end();
    Morikawa.shutdown();
    return;
}
