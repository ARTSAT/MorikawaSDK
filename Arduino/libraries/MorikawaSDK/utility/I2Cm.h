/*
 twi.h - TWI/I2C library for Wiring & Arduino
 Copyright (c) 2006 Nicholas Zambetti.  All right reserved.
 Portions Copyright (C) 2013 - 2013 HORIGUCHI Junshi.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef I2Cm_h
#define I2Cm_h

#include <Arduino.h>

#ifndef TWI_FREQ
#define TWI_FREQ 400000L
#endif

#ifndef TWI_BUFFER_LENGTH
#define TWI_BUFFER_LENGTH 144
#endif

#define TWI_READY 0
#define TWI_MRX   1
#define TWI_MTX   2
#define TWI_SRX   3
#define TWI_STX   4

class I2CmClass {
public:
    static void begin(void);
    static void clear(void);
    static int write(uint8_t data);
    static uint8_t send(uint8_t address, uint8_t sendStop);
    static int receive(uint8_t address, int length, uint8_t sendStop);
    static int available(void);
    static int read(void);
};

extern I2CmClass I2Cm;

#endif
