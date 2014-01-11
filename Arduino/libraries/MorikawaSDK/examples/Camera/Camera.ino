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
**      Camera.ino
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

void setup(void)
{
    CameraFormat format;
    
    if (Morikawa.setup() == TSTERROR_OK) {
        
        // use debug serial
        Serial.begin(57600);
        
        Serial.println("--- supported format ---");
        Morikawa.getCameraFormat(CAMERA_RAW_VGA, &format);
        Serial.println("o RAW VGA");
        dump(format);
        Morikawa.getCameraFormat(CAMERA_RAW_QVGA, &format);
        Serial.println("o RAW QVGA");
        dump(format);
        Morikawa.getCameraFormat(CAMERA_RAW_QCIF, &format);
        Serial.println("o RAW QCIF");
        dump(format);
        Morikawa.getCameraFormat(CAMERA_RGB565_QVGA, &format);
        Serial.println("o RGB565 QVGA");
        dump(format);
        Morikawa.getCameraFormat(CAMERA_RGB565_QQVGA, &format);
        Serial.println("o RGB565 QQVGA");
        dump(format);
        Morikawa.getCameraFormat(CAMERA_RGB565_QQCIF, &format);
        Serial.println("o RGB565 QQCIF");
        dump(format);
        Morikawa.getCameraFormat(CAMERA_YUV422_QVGA, &format);
        Serial.println("o YUV422 QVGA");
        dump(format);
        Morikawa.getCameraFormat(CAMERA_YUV422_QQVGA, &format);
        Serial.println("o YUV422 QQVGA");
        dump(format);
        Morikawa.getCameraFormat(CAMERA_YUV422_QQCIF, &format);
        Serial.println("o YUV422 QQCIF");
        dump(format);
    }
    else {
        Morikawa.shutdown();
    }
    return;
}

void loop(void)
{
    unsigned long size;
    unsigned long addr;
    unsigned long len;
    unsigned char temp[128];
    TSTError error;
    
    Morikawa.loop();
    
    Serial.println("");
    Serial.println("");
    Morikawa.setLED(LED_X, 255);
    error = Morikawa.snapshotCamera(CAMERA_RAW_QCIF, STORAGE_SHAREDMEMORY, 0, Morikawa.getSizeSharedMemory(), &size);
    Morikawa.setLED(LED_X, 0);
    if (error == TSTERROR_OK) {
        for (addr = 0; size > 0; addr += sizeof(temp)) {
            error = Morikawa.readSharedMemory(addr, temp, sizeof(temp));
            if (error != TSTERROR_OK) {
                break;
            }
            len = min(size, sizeof(temp));
            Serial.write(temp, len);
            size -= len;
        }
    }
    if (error != TSTERROR_OK) {
        Morikawa.setLED(LED_Y, 255);
    }
    Serial.end();
    Morikawa.shutdown();
    return;
}

void dump(CameraFormat const& param)
{
    Serial.print("width: ");
    Serial.println(param.width);
    Serial.print("height: ");
    Serial.println(param.height);
    Serial.print("depth: ");
    Serial.println(param.depth);
    Serial.println("");
    return;
}
