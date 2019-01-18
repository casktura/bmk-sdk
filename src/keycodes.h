/*
 * All keycodes are listed in this file.
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

#ifndef _KEYCODES_H_
#define _KEYCODES_H_

/*
 * Keycodes less than 0xFFFF used for HID keycodes
 * Keycode that will be sent to a computer
 */

#define IS_KEY(code) (KC_A <= (code) && (code) <= KC_EXSEL)

#define IS_MOD(code)        ((code) & 0xFF00)
#define MOD(mod_code, code) ((mod_code) | (code))
#define MOD_BIT(code)       (((code) & 0xFF00) >> 8)
#define MOD_CODE(code)      ((code) & 0xFF)

#define IS_DEVICE_CONNECTION(code) (KC_DEVICE_1 <= (code) && (code) <= KC_DEVICE_CONNECT)
#define IS_DEVICE_CONNECT(code)       (code == KC_DEVICE_CONNECT)
#define IS_DEVICE_SWITCHING(code)     (KC_DEVICE_1 <= (code) && (code) <= KC_DEVICE_3)
#define DEVICE(code)                  ((code) - KC_DEVICE_1)

#define IS_LAYER(code) (KC_LAYER_BASE <= (code) && (code) <= KC_LAYER_F)
#define LAYER(code)    ((code) - KC_LAYER_BASE)

/*
 * Short names for ease of definition of keymap
 */
#define KC_LCTL KC_LCTRL
#define KC_RCTL KC_RCTRL
#define KC_LSFT KC_LSHIFT
#define KC_RSFT KC_RSHIFT
#define KC_ESC  KC_ESCAPE
#define KC_BSPC KC_BSPACE
#define KC_ENT  KC_ENTER
#define KC_DEL  KC_DELETE
#define KC_INS  KC_INSERT
#define KC_CAPS KC_CAPSLOCK
#define KC_CLCK KC_CAPSLOCK
#define KC_RGHT KC_RIGHT
#define KC_PGDN KC_PGDOWN
#define KC_PSCR KC_PSCREEN
#define KC_SLCK KC_SCROLLLOCK
#define KC_PAUS KC_PAUSE
#define KC_BRK  KC_PAUSE
#define KC_NLCK KC_NUMLOCK
#define KC_SPC  KC_SPACE
#define KC_MINS KC_MINUS
#define KC_EQL  KC_EQUAL
#define KC_GRV  KC_GRAVE
#define KC_RBRC KC_RBRACKET
#define KC_LBRC KC_LBRACKET
#define KC_COMM KC_COMMA
#define KC_BSLS KC_BSLASH
#define KC_SLSH KC_SLASH
#define KC_SCLN KC_SCOLON
#define KC_QUOT KC_QUOTE
#define KC_APP  KC_APPLICATION
#define KC_NUHS KC_NONUS_HASH
#define KC_NUBS KC_NONUS_BSLASH
#define KC_LCAP KC_LOCKING_CAPS
#define KC_LNUM KC_LOCKING_NUM
#define KC_LSCR KC_LOCKING_SCROLL
#define KC_ERAS KC_ALT_ERASE
#define KC_CLR  KC_CLEAR
// Keypad
#define KC_P1   KC_KP_1
#define KC_P2   KC_KP_2
#define KC_P3   KC_KP_3
#define KC_P4   KC_KP_4
#define KC_P5   KC_KP_5
#define KC_P6   KC_KP_6
#define KC_P7   KC_KP_7
#define KC_P8   KC_KP_8
#define KC_P9   KC_KP_9
#define KC_P0   KC_KP_0
#define KC_PDOT KC_KP_DOT
#define KC_PCMM KC_KP_COMMA
#define KC_PSLS KC_KP_SLASH
#define KC_PAST KC_KP_ASTERISK
#define KC_PMNS KC_KP_MINUS
#define KC_PPLS KC_KP_PLUS
#define KC_PEQL KC_KP_EQUAL
#define KC_PENT KC_KP_ENTER
// Unix function key
#define KC_EXEC KC_EXECUTE
#define KC_SLCT KC_SELECT
#define KC_AGIN KC_AGAIN
#define KC_PSTE KC_PASTE
// Transparent
#define KC_TRNS KC_TRANSPARENT
#define _______ KC_TRANSPARENT
// No operation
#define XXXXXXX KC_NO
// Volume
#define KC_VOLU KC_VOLUP
#define KC_VOLD KC_VOLDOWN
// Devices connection.
#define KC_DVC1 KC_DEVICE_1
#define KC_DVC2 KC_DEVICE_2
#define KC_DVC3 KC_DEVICE_3
#define KC_DVCN KC_DEVICE_CONNECT
// Layer switching.
#define KC_L1   KC_LAYER_1
#define KC_L2   KC_LAYER_2
#define KC_L3   KC_LAYER_3
#define KC_L4   KC_LAYER_4
#define KC_L5   KC_LAYER_5
#define KC_L6   KC_LAYER_6
#define KC_L7   KC_LAYER_7
#define KC_L8   KC_LAYER_8
#define KC_L9   KC_LAYER_9
#define KC_LA   KC_LAYER_A
#define KC_LB   KC_LAYER_B
#define KC_LC   KC_LAYER_C
#define KC_LD   KC_LAYER_D
#define KC_LE   KC_LAYER_E
#define KC_LF   KC_LAYER_F

/*
 * Shifted keys
 */
// Macros
#define LSHIFT(code)           MOD(KC_LSHIFT, code)
#define LSFT(code)             LSHIFT(code)
// Keys
#define KC_TILDE               LSFT(KC_GRAVE)
#define KC_EXCLAIM             LSFT(KC_1)
#define KC_AT                  LSFT(KC_2)
#define KC_HASH                LSFT(KC_3)
#define KC_DOLLAR              LSFT(KC_4)
#define KC_PERCENT             LSFT(KC_5)
#define KC_CIRCUMFLEX          LSFT(KC_6)
#define KC_AMPERSAND           LSFT(KC_7)
#define KC_ASTERISK            LSFT(KC_8)
#define KC_LEFT_PAREN          LSFT(KC_9)
#define KC_RIGHT_PAREN         LSFT(KC_0)
#define KC_UNDERSCORE          LSFT(KC_MINUS)
#define KC_PLUS                LSFT(KC_EQUAL)
#define KC_LEFT_CURLY_BRACE    LSFT(KC_LBRACKET)
#define KC_RIGHT_CURLY_BRACE   LSFT(KC_RBRACKET)
#define KC_PIPE                LSFT(KC_BSLASH)
#define KC_COLON               LSFT(KC_SCOLON)
#define KC_DOUBLE_QUOTE        LSFT(KC_QUOTE)
#define KC_LEFT_ANGLE_BRACKET  LSFT(KC_COMMA)
#define KC_RIGHT_ANGLE_BRACKET LSFT(KC_DOT)
#define KC_QUESTION            LSFT(KC_SLASH)

/*
 * Shifted keys short version
 */
#define KC_TILD KC_TILDE
#define KC_EXLM KC_EXCLAIM
#define KC_DLR  KC_DOLLAR
#define KC_PERC KC_PERCENT
#define KC_CIRC KC_CIRCUMFLEX
#define KC_AMPR KC_AMPERSAND
#define KC_ASTR KC_ASTERISK
#define KC_LPRN KC_LEFT_PAREN
#define KC_RPRN KC_RIGHT_PAREN
#define KC_UNDS KC_UNDERSCORE
#define KC_LCBR KC_LEFT_CURLY_BRACE
#define KC_RCBR KC_RIGHT_CURLY_BRACE
#define KC_COLN KC_COLON
#define KC_DQUO KC_DOUBLE_QUOTE
#define KC_DQT  KC_DOUBLE_QUOTE
#define KC_LABK KC_LEFT_ANGLE_BRACKET
#define KC_LT   KC_LEFT_ANGLE_BRACKET
#define KC_RABK KC_RIGHT_ANGLE_BRACKET
#define KC_GT   KC_RIGHT_ANGLE_BRACKET
#define KC_QUES KC_QUESTION

/*
 * Special keys.
 */
#define KC_PREVWORD MOD(KC_LCTRL, KC_LEFT)
#define KC_NEXTWORD MOD(KC_LCTRL, KC_RIGHT)

/*
 * Special keys short version.
 */
#define KC_PRWD KC_PREVWORD
#define KC_NXWD KC_NEXTWORD

// USB HID Keyboard/Keypad Usage(0x07)
enum basic_hid_keycodes {
    KC_NO               = 0x00,
    KC_ROLL_OVER,
    KC_POST_FAIL,
    KC_UNDEFINED,       // Used for KC_TRANSPARENT
    KC_A,
    KC_B,
    KC_C,
    KC_D,
    KC_E,
    KC_F,
    KC_G,
    KC_H,
    KC_I,
    KC_J,
    KC_K,
    KC_L,
    KC_M,               // 0x10
    KC_N,
    KC_O,
    KC_P,
    KC_Q,
    KC_R,
    KC_S,
    KC_T,
    KC_U,
    KC_V,
    KC_W,
    KC_X,
    KC_Y,
    KC_Z,
    KC_1,
    KC_2,
    KC_3,               // 0x20
    KC_4,
    KC_5,
    KC_6,
    KC_7,
    KC_8,
    KC_9,
    KC_0,
    KC_ENTER,
    KC_ESCAPE,
    KC_BSPACE,
    KC_TAB,
    KC_SPACE,
    KC_MINUS,
    KC_EQUAL,
    KC_LBRACKET,
    KC_RBRACKET,        // 0x30
    KC_BSLASH,          // \ (and |)
    KC_NONUS_HASH,      // Non-US # and ~ (Typically near the Enter key)
    KC_SCOLON,          // ; (and :)
    KC_QUOTE,           // ' and "
    KC_GRAVE,           // Grave accent and tilde
    KC_COMMA,           // , and <
    KC_DOT,             // . and >
    KC_SLASH,           // / and ?
    KC_CAPSLOCK,
    KC_F1,
    KC_F2,
    KC_F3,
    KC_F4,
    KC_F5,
    KC_F6,
    KC_F7,              // 0x40
    KC_F8,
    KC_F9,
    KC_F10,
    KC_F11,
    KC_F12,
    KC_PSCREEN,
    KC_SCROLLLOCK,
    KC_PAUSE,
    KC_INSERT,
    KC_HOME,
    KC_PGUP,
    KC_DELETE,
    KC_END,
    KC_PGDOWN,
    KC_RIGHT,
    KC_LEFT,            // 0x50
    KC_DOWN,
    KC_UP,
    KC_NUMLOCK,
    KC_KP_SLASH,
    KC_KP_ASTERISK,
    KC_KP_MINUS,
    KC_KP_PLUS,
    KC_KP_ENTER,
    KC_KP_1,
    KC_KP_2,
    KC_KP_3,
    KC_KP_4,
    KC_KP_5,
    KC_KP_6,
    KC_KP_7,
    KC_KP_8,            // 0x60
    KC_KP_9,
    KC_KP_0,
    KC_KP_DOT,
    KC_NONUS_BSLASH,    // Non-US \ and | (Typically near the Left-Shift key)
    KC_APPLICATION,
    KC_POWER,
    KC_KP_EQUAL,
    KC_F13,
    KC_F14,
    KC_F15,
    KC_F16,
    KC_F17,
    KC_F18,
    KC_F19,
    KC_F20,
    KC_F21,             // 0x70
    KC_F22,
    KC_F23,
    KC_F24,
    KC_EXECUTE,
    KC_HELP,
    KC_MENU,
    KC_SELECT,
    KC_STOP,
    KC_AGAIN,
    KC_UNDO,
    KC_CUT,
    KC_COPY,
    KC_PASTE,
    KC_FIND,
    KC_MUTE,
    KC_VOLUP,          // 0x80
    KC_VOLDOWN,
    KC_LOCKING_CAPS,    // locking Caps Lock
    KC_LOCKING_NUM,     // locking Num Lock
    KC_LOCKING_SCROLL,  // locking Scroll Lock
    KC_KP_COMMA,
    KC_KP_EQUAL_AS400,  // equal sign on AS/400
    KC_INT1,
    KC_INT2,
    KC_INT3,
    KC_INT4,
    KC_INT5,
    KC_INT6,
    KC_INT7,
    KC_INT8,
    KC_INT9,
    KC_LANG1,           // 0x90
    KC_LANG2,
    KC_LANG3,
    KC_LANG4,
    KC_LANG5,
    KC_LANG6,
    KC_LANG7,
    KC_LANG8,
    KC_LANG9,
    KC_ALT_ERASE,
    KC_SYSREQ,
    KC_CANCEL,
    KC_CLEAR,
    KC_PRIOR,
    KC_RETURN,
    KC_SEPARATOR,
    KC_OUT,             // 0xA0
    KC_OPER,
    KC_CLEAR_AGAIN,
    KC_CRSEL,
    KC_EXSEL,           // 0xA4

    // 0xA5 - 0xFF used for special keycodes

#if 0
    // NOTE: Following codes(0xB0-DD) are not used. Leave them for reference.
    KC_KP_00            = 0xB0,
    KC_KP_000,
    KC_THOUSANDS_SEPARATOR,
    KC_DECIMAL_SEPARATOR,
    KC_CURRENCY_UNIT,
    KC_CURRENCY_SUB_UNIT,
    KC_KP_LPAREN,
    KC_KP_RPAREN,
    KC_KP_LCBRACKET,    // {
    KC_KP_RCBRACKET,    // }
    KC_KP_TAB,
    KC_KP_BSPACE,
    KC_KP_A,
    KC_KP_B,
    KC_KP_C,
    KC_KP_D,
    KC_KP_E,            // 0xC0
    KC_KP_F,
    KC_KP_XOR,
    KC_KP_HAT,
    KC_KP_PERC,
    KC_KP_LT,
    KC_KP_GT,
    KC_KP_AND,
    KC_KP_LAZYAND,
    KC_KP_OR,
    KC_KP_LAZYOR,
    KC_KP_COLON,
    KC_KP_HASH,
    KC_KP_SPACE,
    KC_KP_ATMARK,
    KC_KP_EXCLAMATION,
    KC_KP_MEM_STORE,    // 0xD0
    KC_KP_MEM_RECALL,
    KC_KP_MEM_CLEAR,
    KC_KP_MEM_ADD,
    KC_KP_MEM_SUB,
    KC_KP_MEM_MUL,
    KC_KP_MEM_DIV,
    KC_KP_PLUS_MINUS,
    KC_KP_CLEAR,
    KC_KP_CLEAR_ENTRY,
    KC_KP_BINARY,
    KC_KP_OCTAL,
    KC_KP_DECIMAL,
    KC_KP_HEXADECIMAL,  // 0xDD
#endif
};

// Special keycodes.
enum special_keycodes {
    KC_TRANSPARENT     = 0x03,

    // Devices connection.
    KC_DEVICE_1        = 0xA5,
    KC_DEVICE_2,
    KC_DEVICE_3,
    KC_DEVICE_CONNECT,

    // Layer switching.
    KC_LAYER_BASE,
    KC_LAYER_1,
    KC_LAYER_2,
    KC_LAYER_3,
    KC_LAYER_4,
    KC_LAYER_5,
    KC_LAYER_6,
    KC_LAYER_7,
    KC_LAYER_8,
    KC_LAYER_9,
    KC_LAYER_A,
    KC_LAYER_B,
    KC_LAYER_C,
    KC_LAYER_D,
    KC_LAYER_E,
    KC_LAYER_F,
};

// Modifier keycodes
enum modifier_keycodes {
    /*
     * Modifiers or keys with modifiers.
     * 0x{00}00 - 0x{FF}00 used for modifier key.
     * Pattern: 0x{AB}CD
     * Digit A and B is for midifiers
     * Digit C and D is for keycodes
     */

    // Modifiers
    KC_LCTRL            = 0x0100,
    KC_LSHIFT           = 0x0200,
    KC_LALT             = 0x0400,
    KC_LGUI             = 0x0800,
    KC_RCTRL            = 0x1000,
    KC_RSHIFT           = 0x2000,
    KC_RALT             = 0x4000,
    KC_RGUI             = 0x8000
};

// Layers
#define _BASE_LAYER 0x0
#define _LAYER_1    0x1
#define _LAYER_2    0x2
#define _LAYER_3    0x3
#define _LAYER_4    0x4
#define _LAYER_5    0x5
#define _LAYER_6    0x6
#define _LAYER_7    0x7
#define _LAYER_8    0x8
#define _LAYER_9    0x9
#define _LAYER_A    0xA
#define _LAYER_B    0xB
#define _LAYER_C    0xC
#define _LAYER_D    0xD
#define _LAYER_E    0xE
#define _LAYER_F    0xF

// Layers short name
#define _BS _BASE_LAYER
#define _L1 _LAYER_1
#define _L2 _LAYER_2
#define _L3 _LAYER_3
#define _L4 _LAYER_4
#define _L5 _LAYER_5
#define _L6 _LAYER_6
#define _L7 _LAYER_7
#define _L8 _LAYER_8
#define _L9 _LAYER_9
#define _LA _LAYER_A
#define _LB _LAYER_B
#define _LC _LAYER_C
#define _LD _LAYER_D
#define _LE _LAYER_E
#define _LF _LAYER_F

#endif
