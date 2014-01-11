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
**      TSTCamera.h
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

#ifndef __TST_CAMERA_H
#define __TST_CAMERA_H

#include "TSTType.h"
#include "TSTTrinity.h"

namespace tst {

enum CameraEnum {
    CAMERA_RAW_VGA,
    CAMERA_RAW_QVGA,
    CAMERA_RAW_QCIF,
    CAMERA_RGB565_QVGA,
    CAMERA_RGB565_QQVGA,
    CAMERA_RGB565_QQCIF,
    CAMERA_YUV422_QVGA,
    CAMERA_YUV422_QQVGA,
    CAMERA_YUV422_QQCIF,
    CAMERA_LIMIT
};
typedef unsigned char       CameraType;

struct CameraFormat {
    unsigned int            width;
    unsigned int            height;
    unsigned char           depth;
};
class TSTMorikawa;
class TSTCamera {
    private:
        struct RegisterRec {
            unsigned char           address;
            unsigned char           value;
        };
        struct ModeRec {
            RegisterRec const PROGMEM*
                                    data;
            unsigned int            length;
        };
    
    private:
                TSTTrinity<TSTMorikawa*>
                                    _morikawa;
    
    public:
        static  TSTError            getFormat                   (CameraType mode, CameraFormat* result);
                bool                isValid                     (void) const;
                TSTError            setup                       (TSTMorikawa* morikawa);
                void                cleanup                     (void);
                TSTError            snapshot                    (CameraType mode, StorageType storage, unsigned long address, unsigned long size, unsigned long* result);
    private:
        explicit                    TSTCamera                   (void);
                                    ~TSTCamera                  (void);
        static  void                wake                        (void);
        static  void                sleep                       (void);
        static  void                reset                       (void);
        static  unsigned char       read                        (void);
                TSTError            synchronize                 (bool state) const;
                TSTError            transfer                    (RegisterRec const PROGMEM* data, unsigned int length) const;
    private:
                                    TSTCamera                   (TSTCamera const&);
                TSTCamera&          operator=                   (TSTCamera const&);
    friend      class               TSTMorikawa;
};

/*private */inline TSTCamera::TSTCamera(void) : _morikawa(NULL)
{
}

/*private */inline TSTCamera::~TSTCamera(void)
{
    cleanup();
}

/*public */inline bool TSTCamera::isValid(void) const
{
    return (_morikawa != NULL);
}

}// end of namespace

#endif
