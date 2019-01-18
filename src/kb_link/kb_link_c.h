/*
 * Header for KB link service client.
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

#ifndef _KB_LINK_C_H_
#define _KB_LINK_C_H_

#include "kb_link_config.h"

#include "ble_db_discovery.h"
#include "ble_srv_common.h"
#include "ble.h"

#define KB_LINK_C_DEF(_name)                        \
    static kb_link_c_t _name;                       \
    NRF_SDH_BLE_OBSERVER(_name ## _obs,             \
                         KB_LINK_BLE_OBSERVER_PRIO, \
                         kb_link_c_on_ble_evt,      \
                         &_name)

typedef enum kb_link_c_evt_type_e {
    KB_LINK_C_EVT_DISCOVERY_COMPLETE,
    KB_LINK_C_EVT_KEY_INDEX_UPDATE,
    KB_LINK_C_EVT_DISCONNECTED
} kb_link_c_evt_type_t;

typedef struct kb_link_c_handles_s {
    uint16_t key_index_handle;
    uint16_t key_index_cccd_handle;
} kb_link_c_handles_t;

typedef struct kb_link_c_evt_s {
    kb_link_c_evt_type_t evt_type;
    uint16_t conn_handle;
    uint8_t *p_data;
    uint8_t len;
    kb_link_c_handles_t handles;
} kb_link_c_evt_t;

typedef struct kb_link_c_s kb_link_c_t;

typedef void (*kb_link_c_evt_handler_t)(kb_link_c_t *p_kb_link_c, kb_link_c_evt_t const * p_evt);

typedef struct kb_link_c_s {
    uint8_t uuid_type;
    uint16_t conn_handle;
    kb_link_c_handles_t handles;
    kb_link_c_evt_handler_t evt_handler;
} kb_link_c_t;

typedef struct kb_link_c_init_s {
    kb_link_c_evt_handler_t evt_handler;
} kb_link_c_init_t;

uint32_t kb_link_c_init(kb_link_c_t *p_kb_link_c, kb_link_c_init_t *p_kb_link_init);

void kb_link_c_on_ble_evt(ble_evt_t const *p_ble_evt, void * p_context);

void kb_link_c_on_db_disc_evt(kb_link_c_t *p_kb_link_c, ble_db_discovery_evt_t *p_evt);

uint32_t kb_link_c_key_index_notif_enable(kb_link_c_t *p_kb_link_c);

uint32_t kb_link_c_handles_assign(kb_link_c_t *p_kb_link_c, uint16_t conn_handle, kb_link_c_handles_t const *p_peer_handles);

#endif
