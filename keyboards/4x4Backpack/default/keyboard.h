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

#define MANUFACTURER_NAME "JPConstantineau.com"

#define MASTER_NAME "4x4 Backpack" // 4x4 Backpack

// Matrix.
#define MATRIX_ROW_NUM 4
#define MATRIX_COL_NUM 4

#define MATRIX_ROW_PINS {25, 26, 27, 28}
#define MATRIX_COL_PINS {29, 30, 2, 3}

// Master keyboard definition.
#ifdef MASTER
// If keyboard has slave side.
//#define HAS_SLAVE
#define DEVICE_NAME MASTER_NAME
#define SOURCE      SOURCE_MASTER
#define MATRIX_DEFINE     \
    {                     \
        {1,  2,  3,  4},  \
        {5,  6,  7,  8},  \
        {9,  10, 11, 12}, \
        {13, 14, 15, 16}  \
    }
#endif

extern const uint8_t ROWS[MATRIX_ROW_NUM];
extern const uint8_t COLS[MATRIX_COL_NUM];
extern const int8_t MATRIX[MATRIX_ROW_NUM][MATRIX_COL_NUM];

#endif
