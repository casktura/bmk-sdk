/*
 * Keymap definition for ErgoTravel.
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

#ifndef _KEYMAP_H_
#define _KEYMAP_H_

#include <stdint.h>

#include "../keycodes.h"
#include "keyboard.h"

const uint32_t KEYMAP[][MATRIX_COL_NUM * MATRIX_ROW_NUM * 2] = {
    [_BS] = {
        KC_Q,    KC_W,    KC_E,    KC_R,
        KC_A,    KC_S,    KC_D,    KC_F,
        KC_Z,    KC_X,    KC_C,    KC_V,
        KC_L1,   KC_LGUI, KC_LALT, KC_SPC
    },
    [_L1] = {
        KC_DVC1, KC_DVC2, KC_DVC3, KC_DVCN,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
    }
};

#endif
