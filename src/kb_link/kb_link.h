/*
 * Header for KB link service.
 * Copyright (C) 2018 Kittipong Yothaithiang
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

#ifndef _KB_LINK_H_
#define _KB_LINK_H_

#include "ble.h"
#include "ble_srv_common.h"

#include "kb_link_config.h"

#define KB_LINK_DEF(_name)                          \
    static kb_link_t _name;                         \
    NRF_SDH_BLE_OBSERVER(_name ## _obs,             \
                         KB_LINK_BLE_OBSERVER_PRIO, \
                         kb_link_on_ble_evt,        \
                         &_name)

typedef struct kb_link_init_s {
    uint8_t *key_index;
    uint8_t len;
} kb_link_init_t;

typedef struct kb_link_s {
    uint16_t conn_handle;
    uint16_t service_handle;
    uint8_t uuid_type;
    ble_gatts_char_handles_t key_index_char_handles;
} kb_link_t;

uint32_t kb_link_init(kb_link_t *p_kb_link, kb_link_init_t const *p_kb_link_init);

void kb_link_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context);

uint32_t kb_link_key_index_update(kb_link_t *p_kb_link, uint8_t *p_key_index, uint8_t len);

#endif
