/*
**      ARTSAT MorikawaSDK
**
**      Original Copyright (C) 2012 - 2012 OZAKI Naoya.
**                                          naoya.ozaki11@gmail.com
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
**      TSTSCCB.cpp
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

#include "TSTSCCB.h"

namespace tst {

#define SIOC_H              (pinMode(PIN_SCCB_SIOC, INPUT))
#define SIOC_L              (pinMode(PIN_SCCB_SIOC, OUTPUT))
#define SIOD_H              (pinMode(PIN_SCCB_SIOD, INPUT))
#define SIOD_L              (pinMode(PIN_SCCB_SIOD, OUTPUT))

SCCBStream& SCCB(SCCBStream::getInstance());

/*private */SCCBStream::SCCBStream(void)
{
    _size = 0;
}

/*private virtual */SCCBStream::~SCCBStream(void)
{
    sccbStop();
}

/*public static */SCCBStream& SCCBStream::getInstance(void)
{
    static SCCBStream s_singleton;
    
    return s_singleton;
}

/*public */void SCCBStream::begin(void)
{
    sccbStop();
    return;
}

/*public */void SCCBStream::beginTransmission(unsigned char address)
{
    sccbStart(0, address);
    return;
}

/*public */void SCCBStream::endTransmission(void)
{
    sccbStop();
    return;
}

/*public */void SCCBStream::requestFrom(unsigned char address)
{
    sccbStart(1, address);
    _buffer = sccbRead();
    ++_size;
    sccbStop();
    return;
}

/*public virtual */size_t SCCBStream::write(unsigned char data)
{
    sccbWrite(data);
    return 1;
}

/*public virtual */int SCCBStream::available(void)
{
    return _size;
}

/*public virtual */int SCCBStream::read(void)
{
    int result(-1);
    
    if (_size > 0) {
        result = _buffer;
        --_size;
    }
    return result;
}

/*public virtual */int SCCBStream::peek(void)
{
    int result(-1);
    
    if (_size > 0) {
        result = _buffer;
    }
    return result;
}

/*public virtual */void SCCBStream::flush(void)
{
    return;
}

/*private static */void SCCBStream::sccbStart(unsigned char mode, unsigned char address)
{
    SIOC_H; SIOD_H;
    SIOD_L; SIOC_L;
    sccbWrite((mode & 0x01) | address);
    return;
}

/*private static */void SCCBStream::sccbStop(void)
{
    SIOC_L; SIOD_L;
    SIOC_H; SIOD_H;
    return;
}

/*private static */void SCCBStream::sccbWrite(register unsigned char data)
{
    register int i;
    
    for (i = 0; i < 8; ++i) {
        if (data & 0x80) {
            SIOD_H;
        }
        else {
            SIOD_L;
        }
        data <<= 1;
        SIOC_H;
        while (digitalRead(PIN_SCCB_SIOC) == LOW);
        SIOC_L;
    }
    SIOD_H; SIOC_H; // Don't Care Bit (Spurious ACK)
    SIOD_L; SIOC_L; // Default
    return;
}

/*private static */unsigned char SCCBStream::sccbRead(void)
{
    register int i;
    register unsigned char result(0);
    
    SIOD_H;
    for (i = 0; i < 8; ++i) {
        SIOC_H;
        result <<= 1;
        if (digitalRead(PIN_SCCB_SIOD) == HIGH) {
            result |= 0x01;
        }
        while (digitalRead(PIN_SCCB_SIOC) == LOW);
        SIOC_L;
    }
    SIOD_H; SIOC_H; // Don't Care Bit (NACK)
    SIOC_L; SIOD_L;
    return result;
}

}// end of namespace
