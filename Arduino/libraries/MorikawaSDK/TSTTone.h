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
**      TSTTone.h
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

#ifndef __TST_TONE_H
#define __TST_TONE_H

#include "TSTType.h"
#include "TSTTrinity.h"

namespace tst {

enum FrequencyEnum {
    FREQUENCY_END           = -1,
    FREQUENCY_REST          = 0
};
typedef int                 FrequencyType;
enum NoteEnum {
    NOTE_END                = -1,
    NOTE_REST               = 0,
    NOTE_B0                 = 31,
    NOTE_C1                 = 33,
    NOTE_CS1                = 35,
    NOTE_D1                 = 37,
    NOTE_DS1                = 39,
    NOTE_E1                 = 41,
    NOTE_F1                 = 44,
    NOTE_FS1                = 46,
    NOTE_G1                 = 49,
    NOTE_GS1                = 52,
    NOTE_A1                 = 55,
    NOTE_AS1                = 58,
    NOTE_B1                 = 62,
    NOTE_C2                 = 65,
    NOTE_CS2                = 69,
    NOTE_D2                 = 73,
    NOTE_DS2                = 78,
    NOTE_E2                 = 82,
    NOTE_F2                 = 87,
    NOTE_FS2                = 93,
    NOTE_G2                 = 98,
    NOTE_GS2                = 104,
    NOTE_A2                 = 110,
    NOTE_AS2                = 117,
    NOTE_B2                 = 123,
    NOTE_C3                 = 131,
    NOTE_CS3                = 139,
    NOTE_D3                 = 147,
    NOTE_DS3                = 156,
    NOTE_E3                 = 165,
    NOTE_F3                 = 175,
    NOTE_FS3                = 185,
    NOTE_G3                 = 196,
    NOTE_GS3                = 208,
    NOTE_A3                 = 220,
    NOTE_AS3                = 233,
    NOTE_B3                 = 247,
    NOTE_C4                 = 262,
    NOTE_CS4                = 277,
    NOTE_D4                 = 294,
    NOTE_DS4                = 311,
    NOTE_E4                 = 330,
    NOTE_F4                 = 349,
    NOTE_FS4                = 370,
    NOTE_G4                 = 392,
    NOTE_GS4                = 415,
    NOTE_A4                 = 440,
    NOTE_AS4                = 466,
    NOTE_B4                 = 494,
    NOTE_C5                 = 523,
    NOTE_CS5                = 554,
    NOTE_D5                 = 587,
    NOTE_DS5                = 622,
    NOTE_E5                 = 659,
    NOTE_F5                 = 698,
    NOTE_FS5                = 740,
    NOTE_G5                 = 784,
    NOTE_GS5                = 831,
    NOTE_A5                 = 880,
    NOTE_AS5                = 932,
    NOTE_B5                 = 988,
    NOTE_C6                 = 1047,
    NOTE_CS6                = 1109,
    NOTE_D6                 = 1175,
    NOTE_DS6                = 1245,
    NOTE_E6                 = 1319,
    NOTE_F6                 = 1397,
    NOTE_FS6                = 1480,
    NOTE_G6                 = 1568,
    NOTE_GS6                = 1661,
    NOTE_A6                 = 1760,
    NOTE_AS6                = 1865,
    NOTE_B6                 = 1976,
    NOTE_C7                 = 2093,
    NOTE_CS7                = 2217,
    NOTE_D7                 = 2349,
    NOTE_DS7                = 2489,
    NOTE_E7                 = 2637,
    NOTE_F7                 = 2794,
    NOTE_FS7                = 2960,
    NOTE_G7                 = 3136,
    NOTE_GS7                = 3322,
    NOTE_A7                 = 3520,
    NOTE_AS7                = 3729,
    NOTE_B7                 = 3951,
    NOTE_C8                 = 4186,
    NOTE_CS8                = 4435,
    NOTE_D8                 = 4699,
    NOTE_DS8                = 4978
};
typedef int                 NoteType;
enum DurationEnum {
    DURATION_01             = 1,
    DURATION_02             = 2,
    DURATION_04             = 4,
    DURATION_08             = 8,
    DURATION_16             = 16,
    DURATION_32             = 32
};
typedef unsigned char       DurationType;
enum QualifierEnum {
    QUALIFIER_NONE,
    QUALIFIER_DOT,
    QUALIFIER_DOTDOT,
    QUALIFIER_TRIPLE,
    QUALIFIER_QUINTUPLE,
    QUALIFIER_SEXTUPLE,
    QUALIFIER_SEPTUPLE,
    QUALIFIER_NONUPLE,
    QUALIFIER_LIMIT
};
typedef unsigned char       QualifierType;

struct FrequencySequence {
    FrequencyType           frequency;
    unsigned long           duration;
};
struct NoteSequence {
    NoteType                note;
    DurationType            duration;
    QualifierType           qualifier;
};
class TSTMorikawa;
class TSTTone {
    private:
                TSTTrinity<TSTMorikawa*>
                                    _morikawa;
                TSTTrinity<int>     _bpm;
                TSTTrinity<int>     _wpm;
    
    public:
                TSTError            setNoteBPM                  (int param = -1);
                int                 getNoteBPM                  (void) const;
                TSTError            setMorseWPM                 (int param = -1);
                int                 getMorseWPM                 (void) const;
                bool                isValid                     (void) const;
                TSTError            setup                       (TSTMorikawa* morikawa);
                void                cleanup                     (void);
                TSTError            playFrequency               (FrequencyType frequency, unsigned long duration);
                TSTError            playFrequency               (FrequencySequence const* sequence, int length = -1);
                TSTError            playFrequencyPGM            (FrequencySequence const PROGMEM* sequence, int length = -1);
                TSTError            playNote                    (NoteType note, DurationType duration, QualifierType qualifier = QUALIFIER_NONE);
                TSTError            playNote                    (NoteSequence const* sequence, int length = -1);
                TSTError            playNotePGM                 (NoteSequence const PROGMEM* sequence, int length = -1);
                TSTError            playMorse                   (NoteType note, char character);
                TSTError            playMorse                   (NoteType note, char const* sequence, int length = -1);
                TSTError            playMorsePGM                (NoteType note, char const PROGMEM* sequence, int length = -1);
    private:
        explicit                    TSTTone                     (void);
                                    ~TSTTone                    (void);
                TSTError            playFrequency               (FrequencySequence const* ram, FrequencySequence const PROGMEM* rom, int length);
                TSTError            playNote                    (NoteSequence const* ram, NoteSequence const PROGMEM* rom, int length);
                TSTError            playMorse                   (NoteType note, char const* ram, char const PROGMEM* rom, int length);
                void                executeFrequency            (FrequencyType frequency, unsigned long duration);
                void                executeNote                 (NoteType note, DurationType duration, QualifierType qualifier);
                void                executeMorse                (NoteType note, char character);
    private:
                                    TSTTone                     (TSTTone const&);
                TSTTone&            operator=                   (TSTTone const&);
    friend      class               TSTMorikawa;
};

/*private */inline TSTTone::TSTTone(void) : _morikawa(NULL)
{
}

/*private */inline TSTTone::~TSTTone(void)
{
    cleanup();
}

/*public */inline bool TSTTone::isValid(void) const
{
    return (_morikawa != NULL);
}

}// end of namespace

#endif
