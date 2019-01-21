/*
 * Keyboard definition for ErgoTravel.
 * Copyright (C) 2019 Kittipong Yothaithiang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdint.h>

#include "pin_mapping.h"

/*
 * Keyboard part, e.g. MASTER and SLAVE.
 * #define MASTER
 * #define SLAVE
 * This is defined in SEGGER Embedded Studio project file.
 */

// Key source, to specify key stroke came from which part of the keyboard.
#define SOURCE_MASTER 1
#define SOURCE_SLAVE  2

#define MANUFACTURER_NAME "JPConstantineau.com"

// UUID, random 16-bit UUIDs.
#define SLAVE_UUID  0xE3C7

#define MASTER_NAME "ErgoTravel" // ErgoTravel - Master
#define SLAVE_NAME  "ETS"        // ErgoTravel - Slave

// Matrix.
#define MATRIX_ROW_NUM 4
#define MATRIX_COL_NUM 7

#define MATRIX_ROW_PINS {C6, D7, E6, B4}
#define MATRIX_COL_PINS {F5, F6, F7, B1, B3, B2, B6}

// Master keyboard definition.
#ifdef MASTER
// If keyboard has slave side.
#define HAS_SLAVE
#define DEVICE_NAME MASTER_NAME
#define SOURCE      SOURCE_MASTER
#define MATRIX_DEFINE                 \
    {                                 \
        {1,  2,  3,  4,  5,  6,  7},  \
        {15, 16, 17, 18, 19, 20, 21}, \
        {29, 30, 31, 32, 33, 34, 35}, \
        {43, 44, 45, 46, 47, 48, 49}  \
    }
#endif

// Slave keyboard definition.
#ifdef SLAVE
#define DEVICE_NAME SLAVE_NAME
#define SOURCE      SOURCE_SLAVE
#define MATRIX_DEFINE                 \
    {                                 \
        {14, 13, 12, 11, 10, 9,  8},  \
        {28, 27, 26, 25, 24, 23, 22}, \
        {42, 41, 40, 39, 38, 37, 36}, \
        {56, 55, 54, 53, 52, 51, 50}  \
    }
#endif

extern const uint8_t ROWS[MATRIX_ROW_NUM];
extern const uint8_t COLS[MATRIX_COL_NUM];
extern const int8_t MATRIX[MATRIX_ROW_NUM][MATRIX_COL_NUM];

#endif
