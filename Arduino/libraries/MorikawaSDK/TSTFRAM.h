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
**      TSTFRAM.h
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

#ifndef __TST_FRAM_H
#define __TST_FRAM_H

#include "TSTType.h"
#include "TSTTrinity.h"

namespace tst {

class TSTMorikawa;
class TSTFRAM {
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
                TSTError            read                        (unsigned long address, void* data, unsigned int size, unsigned int* result = NULL);
                TSTError            format                      (void);
    private:
        explicit                    TSTFRAM                     (void);
                                    ~TSTFRAM                    (void);
        static  void                open                        (void);
        static  void                close                       (void);
                TSTError            check                       (unsigned long address, void const* data, unsigned int* size, unsigned int* result) const;
                TSTError            write                       (unsigned long address, unsigned char const* ram, unsigned char const PROGMEM* rom, unsigned int size);
                TSTError            read                        (unsigned long address, unsigned char* data, unsigned int size);
        static  void                executeCommand              (unsigned char command);
        static  void                beginTransfer               (unsigned char command);
        static  void                endTransfer                 (void);
        static  void                transferAddress             (unsigned long address);
    private:
                                    TSTFRAM                     (TSTFRAM const&);
                TSTFRAM&            operator=                   (TSTFRAM const&);
    friend      class               TSTMorikawa;
};

/*private */inline TSTFRAM::TSTFRAM(void) : _morikawa(NULL)
{
}

/*private */inline TSTFRAM::~TSTFRAM(void)
{
    cleanup();
}

/*public */inline bool TSTFRAM::isValid(void) const
{
    return (_morikawa != NULL);
}

}// end of namespace

#endif
