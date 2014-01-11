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
**      TSTDigitalker.cpp
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

#include "TSTDigitalker.h"
#include "I2Cm.h"
#include "TSTMorikawa.h"

namespace tst {

#define I2C_ADDRESS         (0x2E)
#define PHRASE_LIMIT        (128)
#define BUSY_DELAY          (10)
#define PULSE_DELAY         (10)

/*public */TSTError TSTDigitalker::setAsyncMode(bool param)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        _async = param;
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*public */bool TSTDigitalker::getAsyncMode(void) const
{
    bool result(false);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        result = _async;
    }
    return result;
}

/*public */TSTError TSTDigitalker::isBusy(bool* result) const
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        if (_wake) {
            if (I2Cm.receive(I2C_ADDRESS, 1, true) > 0) {
                if (I2Cm.available() > 0) {
                    switch (I2Cm.read()) {
                        case 0x00:
                            error = TSTERROR_FAILED;
                            break;
                        case 0xFF:
                        case '*':
                            if (result != NULL) {
                                *result = true;
                            }
                            delay(BUSY_DELAY);
                            break;
                        default:
                            if (result != NULL) {
                                *result = false;
                            }
                            break;
                    }
                }
                else {
                    error = TSTERROR_FAILED;
                }
            }
            else {
                error = TSTERROR_FAILED;
            }
        }
        else if (result != NULL) {
            *result = false;
        }
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*public */TSTError TSTDigitalker::setup(TSTMorikawa* morikawa)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (morikawa != NULL) {
        if (_morikawa == NULL) {
            _morikawa = morikawa;
            _wake = true;
            _async = false;
            pinMode(PIN_DIGITALKER_PLAY, INPUT);
            pinMode(PIN_DIGITALKER_SLEEP, OUTPUT);
            digitalWrite(PIN_DIGITALKER_SLEEP, HIGH);
            stopPhrase();
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

/*public */void TSTDigitalker::cleanup(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        stopPhrase();
        _morikawa = NULL;
    }
    return;
}

/*public */void TSTDigitalker::loop(void)
{
    bool busy;
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        if (isBusy(&busy) == TSTERROR_OK) {
            if (!busy) {
                sleep();
            }
        }
        else {
            sleep();
        }
    }
    return;
}

/*public */TSTError TSTDigitalker::speakPhrase(char const* phrase, int length)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (phrase != NULL) {
        error = speakPhrase(phrase, NULL, length);
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TSTError TSTDigitalker::speakPhrasePGM(char const PROGMEM* phrase, int length)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (phrase != NULL) {
        error = speakPhrase(NULL, phrase, length);
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TSTError TSTDigitalker::waitPhrase(void)
{
    bool busy;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    while ((error = isBusy(&busy)) == TSTERROR_OK) {
        if (!busy) {
            break;
        }
        else if (_morikawa->hasAbnormalShutdown()) {
            error = TSTERROR_ABNORMAL_SHUTDOWN;
            break;
        }
    }
    return error;
}

/*public */TSTError TSTDigitalker::stopPhrase(void)
{
    bool busy;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        if ((error = isBusy(&busy)) == TSTERROR_OK) {
            if (busy) {
                I2Cm.clear();
                I2Cm.write('$');
                if (I2Cm.send(I2C_ADDRESS, true) != 0) {
                    error = TSTERROR_FAILED;
                }
            }
        }
        sleep();
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*private */TSTError TSTDigitalker::speakPhrase(register char const* ram, register char const PROGMEM* rom, register int length)
{
    register char temp;
    register int i;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (length < 0) {
        length = (ram != NULL) ? (strlen(ram)) : (strlen_P(rom));
    }
    if (length < PHRASE_LIMIT) {
        if (_morikawa != NULL) {
            if (!_morikawa->hasAbnormalShutdown()) {
                if (_morikawa->enableAudioBus() != TSTERROR_OK) {
                    // force execute
                }
                wake();
                if ((error = waitPhrase()) == TSTERROR_OK) {
                    I2Cm.clear();
                    while (true) {
                        if (ram != NULL) {
                            temp = *ram;
                            ++ram;
                        }
                        else {
                            temp = pgm_read_byte(rom);
                            ++rom;
                        }
                        if (temp == '\0') {
                            break;
                        }
                        else if (length >= 0) {
                            if (--length < 0) {
                                break;
                            }
                        }
                        I2Cm.write(temp);
                    }
                    I2Cm.write('\r');
                    if (I2Cm.send(I2C_ADDRESS, true) != 0) {
                        error = TSTERROR_FAILED;
                    }
                    else if (!_async) {
                        if ((error = waitPhrase()) == TSTERROR_OK) {
                            sleep();
                        }
                    }
                }
                if (error != TSTERROR_OK) {
                    sleep();
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
        error = TSTERROR_INVALID_FORMAT;
    }
    return error;
}

/*private */void TSTDigitalker::wake(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (!_wake) {
        _wake = true;
        digitalWrite(PIN_DIGITALKER_SLEEP, HIGH);
        delay(PULSE_DELAY);
    }
    return;
}

/*private */void TSTDigitalker::sleep(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_wake) {
        digitalWrite(PIN_DIGITALKER_SLEEP, LOW);
        delay(PULSE_DELAY);
        _wake = false;
    }
    return;
}

}// end of namespace
