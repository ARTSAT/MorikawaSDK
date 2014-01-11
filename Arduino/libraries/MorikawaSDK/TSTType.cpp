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
**      TSTType.cpp
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

#include "TSTType.h"
#include "TSTMorikawa.h"

void* operator new(size_t param)
{
#ifdef OPTION_BUILD_MEMORYLOG
    tst::TSTMorikawa::saveMemoryLog();
#endif
    return malloc(param);
}

void* operator new[](size_t param)
{
#ifdef OPTION_BUILD_MEMORYLOG
    tst::TSTMorikawa::saveMemoryLog();
#endif
    return malloc(param);
}

void operator delete(void* param)
{
#ifdef OPTION_BUILD_MEMORYLOG
    tst::TSTMorikawa::saveMemoryLog();
#endif
    free(param);
    return;
}

void operator delete[](void* param)
{
#ifdef OPTION_BUILD_MEMORYLOG
    tst::TSTMorikawa::saveMemoryLog();
#endif
    free(param);
    return;
}

__extension__ typedef int __guard __attribute__((mode(__DI__)));
extern "C"
int __cxa_guard_acquire(__guard* param)
{
    return !*reinterpret_cast<char*>(param);
}

extern "C"
void __cxa_guard_release(__guard* param)
{
    *reinterpret_cast<char*>(param) = 1;
}

extern "C"
void __cxa_guard_abort(__guard* param)
{
    return;
}

extern "C"
void __cxa_pure_virtual(void)
{
    return;
}

extern "C"
void atexit(void)
{
    return;
}
