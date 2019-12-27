#ifndef _KEYMAP_H_
#define _KEYMAP_H_

#include <stdint.h>

#include "../keycodes.h"
#include "keyboard.h"

const uint32_t KEYMAP[][MATRIX_COL_NUM * MATRIX_ROW_NUM * 2] = {
    [_BS] = {
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_ESC,  XXXXXXX, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    XXXXXXX, XXXXXXX, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    XXXXXXX, KC_DEL,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
        KC_ESC,  KC_L4,   KC_LGUI, KC_LALT, KC_L1,   KC_SPC,  XXXXXXX, XXXXXXX, KC_RSFT, KC_L2,   KC_RALT, KC_RGUI, XXXXXXX, XXXXXXX
    },
    [_L1] = {
        KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_TILD, KC_PIPE, KC_CIRC, KC_AMPR, KC_ASTR, KC_MINS, KC_EQL,  KC_PIPE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_GRV,  KC_BSLS, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSLS,
        _______, _______, _______, KC_LCBR, KC_LBRC, KC_LPRN, _______, _______, KC_RPRN, KC_RBRC, KC_RCBR, KC_UNDS, KC_PLUS, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_L3,   _______, _______, _______, _______
    },
    [_L2] = {
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   _______, _______, KC_PGUP, KC_PGDN, KC_HOME, KC_END,  _______, KC_DEL,
        KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, KC_PRWD, KC_NXWD, _______, _______, _______, _______,
        _______, _______, _______, _______, KC_L3,   _______, _______, _______, _______, _______, _______, _______, _______, _______
    },
    [_L3] = {
        _______, _______, _______, _______, _______, _______, _______, _______, KC_PAST, KC_P7,   KC_P8,   KC_P9,   KC_PMNS, KC_NLCK,
        KC_CAPS, _______, _______, _______, _______, _______, _______, _______, KC_PSLS, KC_P4,   KC_P5,   KC_P6,   KC_PPLS, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, KC_P0,   KC_P1,   KC_P2,   KC_P3,   KC_PDOT, KC_PENT,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    },
    [_L4] = {
        KC_VOLU, KC_BRIU, _______, _______, KC_DVCN, KC_DVC1, _______, _______, _______, _______, _______, _______, _______, _______,
        KC_VOLD, KC_BRID, _______, _______, _______, KC_DVC2, _______, _______, _______, _______, _______, _______, _______, _______,
        KC_MUTE, _______, _______, _______, _______, KC_DVC3, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    }
};

#endif
