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
**      FastLZ.ino
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

static char const g_string[] PROGMEM = "This is MorikawaSDK. MorikawaSDK supports FastLZ data compression algorithm...MorikawaSDK MorikawaSDK MorikawaSDK";

void setup(void)
{
    if (Morikawa.setup() == TSTERROR_OK) {
    
        // use debug serial
        Serial.begin(9600);
    }
    else {
        Morikawa.shutdown();
    }
    return;
}

void loop(void)
{
    unsigned long output;
    unsigned long work;
    char temp[256];
    TSTError error;
    
    Morikawa.loop();
    
    Serial.print("original size = ");
    Serial.println(lengthof(g_string));
    
    // write data to shared memory
    Morikawa.writeSharedMemoryPGM(0, g_string, lengthof(g_string));
    
    // get recommend output buffer size
    error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_NONE, 0, 0, STORAGE_NONE, 0, 0, &output);
    Serial.print("recommend output buffer size = ");
    Serial.print(output);
    Serial.print(", error = ");
    Serial.println(error);
    
    // get recommend work buffer size
    error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_FRAM, 0, output, STORAGE_NONE, 0, 0, &work);
    Serial.print("recommend work buffer size = ");
    Serial.print(work);
    Serial.print(", error = ");
    Serial.println(error);
    
    // compress data from shared memory to FRAM
    error = Morikawa.freezeFastLZ(STORAGE_SHAREDMEMORY, 0, lengthof(g_string), STORAGE_FRAM, 0, output, STORAGE_FRAM, output, work, &output);
    Serial.print("compressed size = ");
    Serial.print(output);
    Serial.print(", error = ");
    Serial.println(error);
    
    // decompress data from FRAM to shared memory
    error = Morikawa.meltFastLZ(STORAGE_FRAM, 0, output, STORAGE_SHAREDMEMORY, 512, lengthof(g_string), &output);
    Serial.print("decompressed size = ");
    Serial.print(output);
    Serial.print(", error = ");
    Serial.println(error);
    
    // read data from shared memory
    Morikawa.readSharedMemory(512, temp, output);
    
    // compare read data with original data
    if (strcmp_P(temp, g_string) == 0) {
        Serial.println("OK");
    }
    else {
        Serial.println("NG");
    }
    Serial.end();
    Morikawa.shutdown();
    return;
}
