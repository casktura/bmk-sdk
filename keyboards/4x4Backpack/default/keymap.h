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
