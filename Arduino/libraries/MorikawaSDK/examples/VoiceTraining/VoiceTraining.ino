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
**      VoiceTraining.ino
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

static NoteSequence const g_note1[] PROGMEM = {
    {NOTE_D6, DURATION_04},
    {NOTE_G5, DURATION_08},
    {NOTE_A5, DURATION_08},
    {NOTE_B5, DURATION_08},
    {NOTE_C6, DURATION_08},
    {NOTE_D6, DURATION_16},
    {NOTE_REST, DURATION_16},
    {NOTE_REST, DURATION_08},
    {NOTE_G5, DURATION_16},
    {NOTE_REST, DURATION_16},
    {NOTE_REST, DURATION_08},
    {NOTE_G5, DURATION_16},
    {NOTE_REST, DURATION_16},
    {NOTE_REST, DURATION_08},
    {NOTE_END}
};
static NoteSequence const g_note2[] PROGMEM = {
    {NOTE_E6, DURATION_04},
    {NOTE_C6, DURATION_08},
    {NOTE_D6, DURATION_08},
    {NOTE_E6, DURATION_08},
    {NOTE_FS6, DURATION_08},
    {NOTE_G6, DURATION_16},
    {NOTE_REST, DURATION_16},
    {NOTE_REST, DURATION_08},
    {NOTE_G5, DURATION_16},
    {NOTE_REST, DURATION_16},
    {NOTE_REST, DURATION_08},
    {NOTE_G5, DURATION_16},
    {NOTE_REST, DURATION_16},
    {NOTE_REST, DURATION_08},
    {NOTE_END}
};
static NoteSequence const g_note3[] PROGMEM = {
    {NOTE_C6, DURATION_04},
    {NOTE_D6, DURATION_08},
    {NOTE_C6, DURATION_08},
    {NOTE_B5, DURATION_08},
    {NOTE_A5, DURATION_08},
    {NOTE_B5, DURATION_04},
    {NOTE_C6, DURATION_08},
    {NOTE_B5, DURATION_08},
    {NOTE_A5, DURATION_08},
    {NOTE_G5, DURATION_08},
    {NOTE_FS5, DURATION_04},
    {NOTE_G5, DURATION_08},
    {NOTE_A5, DURATION_08},
    {NOTE_B5, DURATION_08},
    {NOTE_G5, DURATION_08},
    {NOTE_B5, DURATION_04},
    {NOTE_A5, DURATION_02},
    {NOTE_END}
};
static char const g_speak[] PROGMEM = "menue'ttono rennshu-";
static char const g_song1[] PROGMEM = "re---so-ra-shi-do-re---so so ";
static char const g_song2[] PROGMEM = "mi---do-re-mi-fa-so---so so ";
static char const g_song3[] PROGMEM = "do---re-do-shi-ra--shi---do-shi-ra-so--fa---so-ra-shi-so-shi---ra------";

void setup(void)
{
    if (Morikawa.setup() == TSTERROR_OK) {
    
        // start speaking as blocking
        Morikawa.speakPhrasePGM(g_speak);
    }
    else {
        Morikawa.shutdown();
    }
    return;
}

void loop(void)
{
    bool busy;
    
    Morikawa.loop();
    
    Morikawa.isBusyDigitalker(&busy);
    if (!busy) {
    
        // start singing as non-blocking
        Morikawa.setSpeakAsyncMode(true);
        Morikawa.speakPhrasePGM(g_song1);
        
        // start playing notes as blocking
        Morikawa.playNotePGM(g_note1);
        
        Morikawa.waitPhrase();
        Morikawa.speakPhrasePGM(g_song2);
        Morikawa.playNotePGM(g_note2);
        
        Morikawa.waitPhrase();
        Morikawa.speakPhrasePGM(g_song3);
        Morikawa.playNotePGM(g_note3);
    }
    delay(3000);
    return;
}
