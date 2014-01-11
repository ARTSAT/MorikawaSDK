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
**      TSTTone.cpp
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

#include "TSTTone.h"
#include "TSTMorikawa.h"

namespace tst {

#define DEFAULT_BPM         (120)
#define DEFAULT_WPM         (20)

/*public */TSTError TSTTone::setNoteBPM(int param)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        _bpm = (param < 0) ? (DEFAULT_BPM) : (max(param, 1));
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*public */int TSTTone::getNoteBPM(void) const
{
    int result(-1);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        result = _bpm;
    }
    return result;
}

/*public */TSTError TSTTone::setMorseWPM(int param)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        _wpm = (param < 0) ? (DEFAULT_WPM) : (max(param, 1));
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*public */int TSTTone::getMorseWPM(void) const
{
    int result(-1);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        result = _wpm;
    }
    return result;
}

/*public */TSTError TSTTone::setup(TSTMorikawa* morikawa)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (morikawa != NULL) {
        if (_morikawa == NULL) {
            _morikawa = morikawa;
            _bpm = DEFAULT_BPM;
            _wpm = DEFAULT_WPM;
            noTone(PIN_TONE_WAVE);
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

/*public */void TSTTone::cleanup(void)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        noTone(PIN_TONE_WAVE);
        _morikawa = NULL;
    }
    return;
}

/*public */TSTError TSTTone::playFrequency(FrequencyType frequency, unsigned long duration)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        if (!_morikawa->hasAbnormalShutdown()) {
            if (_morikawa->enableAudioBus() != TSTERROR_OK) {
                // force execute
            }
            executeFrequency(frequency, duration);
        }
        else {
            error = TSTERROR_INVALID_STATE;
        }
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*public */TSTError TSTTone::playFrequency(FrequencySequence const* sequence, int length)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (sequence != NULL) {
        error = playFrequency(sequence, NULL, length);
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TSTError TSTTone::playFrequencyPGM(FrequencySequence const PROGMEM* sequence, int length)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (sequence != NULL) {
        error = playFrequency(NULL, sequence, length);
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TSTError TSTTone::playNote(NoteType note, DurationType duration, QualifierType qualifier)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        if (!_morikawa->hasAbnormalShutdown()) {
            if (_morikawa->enableAudioBus() != TSTERROR_OK) {
                // force execute
            }
            executeNote(note, duration, qualifier);
        }
        else {
            error = TSTERROR_INVALID_STATE;
        }
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*public */TSTError TSTTone::playNote(NoteSequence const* sequence, int length)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (sequence != NULL) {
        error = playNote(sequence, NULL, length);
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TSTError TSTTone::playNotePGM(NoteSequence const PROGMEM* sequence, int length)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (sequence != NULL) {
        error = playNote(NULL, sequence, length);
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TSTError TSTTone::playMorse(NoteType note, char character)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        if (!_morikawa->hasAbnormalShutdown()) {
            if (_morikawa->enableAudioBus() != TSTERROR_OK) {
                // force execute
            }
            executeMorse(note, character);
        }
        else {
            error = TSTERROR_INVALID_STATE;
        }
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*public */TSTError TSTTone::playMorse(NoteType note, char const* sequence, int length)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (sequence != NULL) {
        error = playMorse(note, sequence, NULL, length);
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TSTError TSTTone::playMorsePGM(NoteType note, char const PROGMEM* sequence, int length)
{
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (sequence != NULL) {
        error = playMorse(note, NULL, sequence, length);
    }
    else {
        error = TSTERROR_INVALID_PARAM;
    }
    return error;
}

/*private */TSTError TSTTone::playFrequency(register FrequencySequence const* ram, register FrequencySequence const PROGMEM* rom, register int length)
{
    FrequencyType frequency;
    unsigned long duration;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        if (!_morikawa->hasAbnormalShutdown()) {
            if (_morikawa->enableAudioBus() != TSTERROR_OK) {
                // force execute
            }
            while (true) {
                if (ram != NULL) {
                    frequency = ram->frequency;
                    duration = ram->duration;
                    ++ram;
                }
                else {
                    frequency = pgm_read_word(&rom->frequency);
                    duration = pgm_read_dword(&rom->duration);
                    ++rom;
                }
                if (frequency <= FREQUENCY_END) {
                    break;
                }
                else if (length >= 0) {
                    if (--length < 0) {
                        break;
                    }
                }
                else if (_morikawa->hasAbnormalShutdown()) {
                    error = TSTERROR_ABNORMAL_SHUTDOWN;
                    break;
                }
                executeFrequency(frequency, duration);
            }
        }
        else {
            error = TSTERROR_INVALID_STATE;
        }
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*private */TSTError TSTTone::playNote(register NoteSequence const* ram, register NoteSequence const PROGMEM* rom, register int length)
{
    NoteType note;
    DurationType duration;
    QualifierType qualifier;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        if (!_morikawa->hasAbnormalShutdown()) {
            if (_morikawa->enableAudioBus() != TSTERROR_OK) {
                // force execute
            }
            while (true) {
                if (ram != NULL) {
                    note = ram->note;
                    duration = ram->duration;
                    qualifier = ram->qualifier;
                    ++ram;
                }
                else {
                    note = pgm_read_word(&rom->note);
                    duration = pgm_read_byte(&rom->duration);
                    qualifier = pgm_read_byte(&rom->qualifier);
                    ++rom;
                }
                if (note <= NOTE_END) {
                    break;
                }
                else if (length >= 0) {
                    if (--length < 0) {
                        break;
                    }
                }
                else if (_morikawa->hasAbnormalShutdown()) {
                    error = TSTERROR_ABNORMAL_SHUTDOWN;
                    break;
                }
                executeNote(note, duration, qualifier);
            }
        }
        else {
            error = TSTERROR_INVALID_STATE;
        }
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*private */TSTError TSTTone::playMorse(NoteType note, register char const* ram, register char const PROGMEM* rom, register int length)
{
    char character;
    TSTError error(TSTERROR_OK);
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (_morikawa != NULL) {
        if (!_morikawa->hasAbnormalShutdown()) {
            if (_morikawa->enableAudioBus() != TSTERROR_OK) {
                // force execute
            }
            while (true) {
                if (ram != NULL) {
                    character = *ram;
                    ++ram;
                }
                else {
                    character = pgm_read_byte(rom);
                    ++rom;
                }
                if (character <= '\0') {
                    break;
                }
                else if (length >= 0) {
                    if (--length < 0) {
                        break;
                    }
                }
                else if (_morikawa->hasAbnormalShutdown()) {
                    error = TSTERROR_ABNORMAL_SHUTDOWN;
                    break;
                }
                executeMorse(note, character);
            }
        }
        else {
            error = TSTERROR_INVALID_STATE;
        }
    }
    else {
        error = TSTERROR_INVALID_STATE;
    }
    return error;
}

/*private */void TSTTone::executeFrequency(FrequencyType frequency, unsigned long duration)
{
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (duration > 0) {
        if (frequency > FREQUENCY_REST) {
            tone(PIN_TONE_WAVE, frequency, duration);
            while (TIMSK2 & (1 << OCIE2A));
        }
        else {
            delay(duration);
        }
    }
    return;
}

/*private */void TSTTone::executeNote(NoteType note, DurationType duration, QualifierType qualifier)
{
    unsigned long time;
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    if (duration > 0) {
        switch (qualifier) {
            case QUALIFIER_DOT:
                time = 1500000UL / duration;
                break;
            case QUALIFIER_DOTDOT:
                time = 1750000UL / duration;
                break;
            case QUALIFIER_TRIPLE:
                time = 666667UL / duration;
                break;
            case QUALIFIER_QUINTUPLE:
                time = 800000UL / duration;
                break;
            case QUALIFIER_SEXTUPLE:
                time = 666667UL / duration;
                break;
            case QUALIFIER_SEPTUPLE:
                time = 571429UL / duration;
                break;
            case QUALIFIER_NONUPLE:
                time = 888889UL / duration;
                break;
            default:
                time = 1000000UL / duration;
                break;
        }
        time = time * 240 / _bpm / 1000;
        if (note > NOTE_REST) {
            tone(PIN_TONE_WAVE, note, time);
            while (TIMSK2 & (1 << OCIE2A));
        }
        else {
            delay(time);
        }
    }
    return;
}

/*private */void TSTTone::executeMorse(NoteType note, char character)
{
    static unsigned char const s_bit[] PROGMEM = {
        107,    // ascii 33 !
        82,     // ascii 34 "
        1,      // ascii 35 #
        137,    // ascii 36 $
        1,      // ascii 37 %
        40,     // ascii 38 &
        94,     // ascii 39 '
        109,    // ascii 40 (
        109,    // ascii 41 )
        1,      // ascii 42 *
        42,     // ascii 43 +
        115,    // ascii 44 ,
        97,     // ascii 45 -
        85,     // ascii 46 .
        50,     // ascii 47 /
        63,     // ascii 48 0
        62,     // ascii 49 1
        60,     // ascii 50 2
        56,     // ascii 51 3
        48,     // ascii 52 4
        32,     // ascii 53 5
        33,     // ascii 54 6
        35,     // ascii 55 7
        39,     // ascii 56 8
        47,     // ascii 57 9
        120,    // ascii 58 :
        53,     // ascii 59 ;
        1,      // ascii 60 <
        49,     // ascii 61 =
        1,      // ascii 62 >
        76,     // ascii 63 ?
        69,     // ascii 64 @
        6,      // ascii 65 A
        17,     // ascii 66 B
        21,     // ascii 67 C
        9,      // ascii 68 D
        2,      // ascii 69 E
        20,     // ascii 70 F
        11,     // ascii 71 G
        16,     // ascii 72 H
        4,      // ascii 73 I
        30,     // ascii 74 J
        13,     // ascii 75 K
        18,     // ascii 76 L
        7,      // ascii 77 M
        5,      // ascii 78 N
        15,     // ascii 79 O
        22,     // ascii 80 P
        27,     // ascii 81 Q
        10,     // ascii 82 R
        8,      // ascii 83 S
        3,      // ascii 84 T
        12,     // ascii 85 U
        24,     // ascii 86 V
        14,     // ascii 87 W
        25,     // ascii 88 X
        29,     // ascii 89 Y
        19,     // ascii 90 Z
        1,      // ascii 91 [
        1,      // ascii 92
        1,      // ascii 93 ]
        1,      // ascii 94 ^
        77,     // ascii 95 _
        94,     // ascii 96 `
        6,      // ascii 97 a
        17,     // ascii 98 b
        21,     // ascii 99 c
        9,      // ascii 100 d
        2,      // ascii 101 e
        20,     // ascii 102 f
        11,     // ascii 103 g
        16,     // ascii 104 h
        4,      // ascii 105 i
        30,     // ascii 106 j
        13,     // ascii 107 k
        18,     // ascii 108 l
        7,      // ascii 109 m
        5,      // ascii 110 n
        15,     // ascii 111 o
        22,     // ascii 112 p
        27,     // ascii 113 q
        10,     // ascii 114 r
        8,      // ascii 115 s
        3,      // ascii 116 t
        12,     // ascii 117 u
        24,     // ascii 118 v
        14,     // ascii 119 w
        25,     // ascii 120 x
        29,     // ascii 121 y
        19      // ascii 122 z
    };
    unsigned int dot;
    unsigned int dash;
    register unsigned char bit;
    
#ifdef OPTION_BUILD_MEMORYLOG
    TSTMorikawa::saveMemoryLog();
#endif
    dot = 1200 / _wpm;
    dash = 3 * dot;
    if (character == ' ') {
        delay(7 * dot);
    }
    else if (33 <= character && character <= 122) {
        for (bit = pgm_read_byte(&s_bit[character - 33]); bit != 1; bit >>= 1) {
            tone(PIN_TONE_WAVE, note, (bit & 0x01) ? (dash) : (dot));
            while (TIMSK2 & (1 << OCIE2A));
            delay(dot);
        }
        delay(2 * dot);
    }
    return;
}

}// end of namespace
