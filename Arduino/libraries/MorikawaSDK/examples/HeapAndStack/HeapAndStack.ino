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
**      HeapAndStack.ino
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

static void func1(void);
static void func2(void);
static void func3(void);
static void dump(char const* format, ...);

void setup(void)
{
    MemorySpec spec;
    MemoryInfo info;
    
    if (Morikawa.setup() == TSTERROR_OK) {
    
        // use debug serial
        Serial.begin(9600);
        
        // get AVR memory spec
        Morikawa.getMemorySpec(&spec);
        dump("---------- memory spec ----------");
        dump("        ram_start: %p", spec.ram_start);
        dump("         ram_size: %d bytes", spec.ram_size);
        dump("       data_start: %p", spec.data_start);
        dump("        data_size: %d bytes", spec.data_size);
        dump("        bss_start: %p", spec.bss_start);
        dump("         bss_size: %d bytes", spec.bss_size);
        dump(" heap_stack_start: %p", spec.heap_stack_start);
        dump("  heap_stack_size: %d bytes", spec.heap_stack_size);
        dump("heap_stack_margin: %d bytes", spec.heap_stack_margin);
        dump("");
        
        // get current memory usage
        Morikawa.getMemoryInfo(&info);
        dump("------ current memory info ------");
        dump("        heap_size: %d bytes", info.heap_size);
        dump("        heap_free: %d bytes", info.heap_free);
        dump("       stack_size: %d bytes", info.stack_size);
        dump("       stack_free: %d bytes", info.stack_free);
        dump("");
        
        // get logged peak memory usage
        Morikawa.getMemoryLog(&info);
        dump("------- peak memory usage -------");
        dump("        heap_size: %d bytes", info.heap_size);
        dump("        heap_free: %d bytes", info.heap_free);
        dump("       stack_size: %d bytes", info.stack_size);
        dump("       stack_free: %d bytes", info.stack_free);
        dump("");
    }
    else {
        Morikawa.shutdown();
    }
    return;
}

void loop(void)
{
    MemoryInfo info;
    
    Morikawa.loop();
    
    // call func1 to use heap and stack
    func1();
    
    // get current memory usage
    Morikawa.getMemoryInfo(&info);
    dump("------ current memory info ------");
    dump("        heap_size: %d bytes", info.heap_size);
    dump("        heap_free: %d bytes", info.heap_free);
    dump("       stack_size: %d bytes", info.stack_size);
    dump("       stack_free: %d bytes", info.stack_free);
    dump("");
    
    // get logged peak memory usage
    Morikawa.getMemoryLog(&info);
    dump("------- peak memory usage -------");
    dump("        heap_size: %d bytes", info.heap_size);
    dump("        heap_free: %d bytes", info.heap_free);
    dump("       stack_size: %d bytes", info.stack_size);
    dump("       stack_free: %d bytes", info.stack_free);
    dump("");
    
    Morikawa.shutdown();
    return;
}

static void func1(void)
{
    char volatile temp[100];
    
    dump("> func1");
    
    // log current memory usage
    Morikawa.saveMemoryLog();
    
    func2();
    return;
}

static void func2(void)
{
    char volatile temp[50];
    void* buf = malloc(16);
    
    dump("> func2");
    
    // log current memory usage
    Morikawa.saveMemoryLog();
    
    func3();
    free(buf);
    return;
}

static void func3(void)
{
    char volatile temp[25];
    void* buf = malloc(32);
    
    dump("> func3");
    
    // log current memory usage
    Morikawa.saveMemoryLog();
    
    free(buf);
    return;
}

static void dump(char const* format, ...)
{
    va_list ap;
    char temp[256];
    
    // log current memory usage
    Morikawa.saveMemoryLog();
    
    va_start(ap, format);
    vsnprintf(temp, sizeof(temp), format, ap);
    va_end(ap);
    Serial.println(temp);
    return;
}
