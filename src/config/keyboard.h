#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdint.h>
#include "custom_board.h"

// Keyboard.
// Keyboard part, e.g. MASTER and SLAVE.
#define MASTER
// #define SLAVE
// Key source, to specify key stroke came from which part of the keyboard.
#define SOURCE_MASTER     1
#define SOURCE_SLAVE      2

#define DEVICE_MODEL      "BlueMicro"
#define MANUFACTURER_NAME "JPConstantineau.com"

#define MASTER_NAME "ErgoTravel"
#define SLAVE_NAME  "ErgoTravel - Slave Side"

#define PNP_ID_VENDOR_ID_SOURCE 0x02   // Vendor ID Source.
#define PNP_ID_VENDOR_ID        0x1915 // Vendor ID.
#define PNP_ID_PRODUCT_ID       0xEEEE // Product ID.
#define PNP_ID_PRODUCT_VERSION  0x0001 // Product Version.

// Matrix.
#define MATRIX_ROW_NUM 4
#define MATRIX_COL_NUM 7

#define MATRIX_ROW_PINS {C6, D7, E6, B4}
#define MATRIX_COL_PINS {F5, F6, F7, B1, B3, B2, B6}

#ifdef MASTER
// If keyboard has slave side.
#define HAS_SLAVE
#define DEVICE_NAME    MASTER_NAME
#define SOURCE         SOURCE_MASTER

const int8_t MATRIX[MATRIX_ROW_NUM][MATRIX_COL_NUM] = {
    {1,  2,  3,  4,  5,  6,  7},
    {15, 16, 17, 18, 19, 20, 21},
    {29, 30, 31, 32, 33, 34, 35},
    {43, 44, 45, 46, 47, 48, 49}
};
#endif

#ifdef SLAVE
#define DEVICE_NAME    SLAVE_NAME
#define SOURCE         SOURCE_SLAVE

const int8_t MATRIX[MATRIX_ROW_NUM][MATRIX_COL_NUM] = {
    {14, 13, 12, 11, 10, 9,  8},
    {28, 27, 26, 25, 24, 23, 22},
    {42, 41, 40, 39, 38, 37, 36},
    {56, 55, 54, 53, 52, 51, 50}
};
#endif

#endif
