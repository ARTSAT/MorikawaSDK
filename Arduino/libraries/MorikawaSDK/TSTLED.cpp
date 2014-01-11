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
**      TSTLED.cpp
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

#include "TSTLED.h"
#include "TSTMorikawa.h"

namespace tst {

static PinType const g_pin[LED_LIMIT] PROGMEM = {
    PIN_LED_X,
    PIN_LED_Y
};

/*public */TSTError TSTLED::set(LEDType index, unsigned char pwm)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (0 <= index && index < LED_LIMIT) {
        if (_morikawa != NULL) {
            analogWrite(pgm_read_byte(&g_pin[index]), pwm);
            _pwm[index] = pwm;
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

/*public */unsigned char TSTLED::get(LEDType index) const
{
    unsigned char result(0);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (0 <= index && index < LED_LIMIT) {
        if (_morikawa != NULL) {
            result = _pwm[index];
        }
    }
    return result;
}

/*public */TSTError TSTLED::setup(TSTMorikawa* morikawa)
{
    register PinType pin;
    register int i;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
#ifdef TARGET_MORIKAWA_FM1
    if (morikawa != NULL) {
        if (_morikawa == NULL) {
            _morikawa = morikawa;
            for (i = 0; i < LED_LIMIT; ++i) {
                pin = pgm_read_byte(&g_pin[i]);
                pinMode(pin, OUTPUT);
                analogWrite(pin, 0);
                _pwm[i] = 0;
            }
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

/*public */void TSTLED::cleanup(void)
{
    register int i;
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        for (i = 0; i < LED_LIMIT; ++i) {
            analogWrite(pgm_read_byte(&g_pin[i]), 0);
        }
        _morikawa = NULL;
    }
    return;
}

}// end of namespace
