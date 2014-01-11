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
**      TSTSharedMemory.h
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

#ifndef __TST_SHAREDMEMORY_H
#define __TST_SHAREDMEMORY_H

#include "TSTType.h"
#include "TSTTrinity.h"

namespace tst {

enum TextEnum {
    TEXT_X,
    TEXT_Y,
    TEXT_Z,
    TEXT_DEBUG,
    TEXT_LIMIT
};
typedef unsigned char       TextType;

struct SelfTestLog;
struct NoteParam {
    unsigned char           size;
    unsigned char           data[255];
};
struct MorseParam {
    unsigned char           size;
    unsigned char           data[63];
};
struct DigitalkerParam {
    unsigned char           size;
    unsigned char           data[127];
};
struct CameraParam {
    unsigned char           size;
    unsigned char           data[63];
};
class TSTMorikawa;
class TSTSharedMemory {
    private:
        struct BootParamRec {
            unsigned long           time;
            unsigned char           count;
            unsigned char           mode;
            unsigned char           flag[32];
        };
    
    private:
                TSTTrinity<TSTMorikawa*>
                                    _morikawa;
    
    public:
        static  unsigned long       getSize                     (void);
        static  unsigned int        getPageSize                 (void);
        static  unsigned long       getSectorSize               (void);
                bool                isValid                     (void) const;
                TSTError            setup                       (TSTMorikawa* morikawa);
                void                cleanup                     (void);
                TSTError            write                       (unsigned long address, void const* data, unsigned int size, unsigned int* result = NULL);
                TSTError            writePGM                    (unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result = NULL);
                TSTError            writeSelfTestLog            (SelfTestLog const& log);
                TSTError            writeTextResult             (TextType index, char const* text, int length = -1);
                TSTError            writeTextResultPGM          (TextType index, char const PROGMEM* text, int length = -1);
                TSTError            read                        (unsigned long address, void* data, unsigned int size, unsigned int* result = NULL);
                TSTError            readBootParam               (BootParamRec* result);
                TSTError            readTextParam               (TextType index, char* text, unsigned int length, int* result = NULL);
                TSTError            readNoteParam               (NoteParam* result);
                TSTError            readMorseParam              (MorseParam* result);
                TSTError            readDigitalkerParam         (DigitalkerParam* result);
                TSTError            readCameraParam             (CameraParam* result);
                TSTError            format                      (void);
    private:
        explicit                    TSTSharedMemory             (void);
                                    ~TSTSharedMemory            (void);
                TSTError            checkGeneral                (unsigned long address, void const* data, unsigned int* size, unsigned int* result) const;
                TSTError            checkParam                  (void const* data) const;
                TSTError            checkText                   (TextType index, char const* ram, char const PROGMEM* rom, int* length) const;
                TSTError            checkText                   (TextType index) const;
                TSTError            writeGeneral                (unsigned long address, unsigned char const* ram, unsigned char const PROGMEM* rom, unsigned int size);
                TSTError            writeSpecial                (unsigned long address, unsigned char const* ram, unsigned char const PROGMEM* rom, unsigned int size);
                TSTError            writeByte                   (unsigned long address, unsigned char const** ram, unsigned char const PROGMEM** rom, unsigned int size);
                TSTError            writePage                   (unsigned long address, unsigned char const** ram, unsigned char const PROGMEM** rom);
                TSTError            writePage                   (unsigned long address, unsigned char const** ram, unsigned char const PROGMEM** rom, unsigned int size);
                TSTError            readGeneral                 (unsigned long address, unsigned char* data, unsigned int size);
                TSTError            readParam                   (unsigned long address, unsigned char* data, unsigned int size, unsigned char* result);
                TSTError            readSpecial                 (unsigned long address, unsigned char* data, unsigned int size);
                TSTError            readSequence                (unsigned long address, unsigned char** data, unsigned int size);
        static  unsigned char       control                     (unsigned long address);
        static  TSTError            send                        (unsigned char i2c, bool stop);
    private:
                                    TSTSharedMemory             (TSTSharedMemory const&);
                TSTSharedMemory&    operator=                   (TSTSharedMemory const&);
    friend      class               TSTMorikawa;
};

/*private */inline TSTSharedMemory::TSTSharedMemory(void) : _morikawa(NULL)
{
}

/*private */inline TSTSharedMemory::~TSTSharedMemory(void)
{
    cleanup();
}

/*public */inline bool TSTSharedMemory::isValid(void) const
{
    return (_morikawa != NULL);
}

}// end of namespace

#endif
