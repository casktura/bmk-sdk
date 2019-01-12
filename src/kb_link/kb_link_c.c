/*
 * KB link service client.
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

#include "nrf_log.h"

#include "kb_link_c.h"

static void on_hvx(kb_link_c_t *p_kb_link_c, ble_evt_t const *p_ble_evt);

static uint32_t cccd_configure(uint16_t conn_handle, uint16_t cccd_handle, bool enable);

uint32_t kb_link_c_init(kb_link_c_t *p_kb_link_c, kb_link_c_init_t *p_kb_link_init) {
    VERIFY_PARAM_NOT_NULL(p_kb_link_c);
    VERIFY_PARAM_NOT_NULL(p_kb_link_init);

    uint32_t err_code;
    ble_uuid_t ble_uuid;
    ble_uuid128_t base_uuid = {KB_LINK_SERVICE_BASE_UUID};

    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_kb_link_c->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_kb_link_c->uuid_type;
    ble_uuid.uuid = KB_LINK_SERVICE_UUID;

    p_kb_link_c->conn_handle = BLE_CONN_HANDLE_INVALID;
    p_kb_link_c->evt_handler = p_kb_link_init->evt_handler;
    p_kb_link_c->handles.key_index_handle = BLE_CONN_HANDLE_INVALID;
    p_kb_link_c->handles.key_index_cccd_handle = BLE_CONN_HANDLE_INVALID;

    return ble_db_discovery_evt_register(&ble_uuid);
}

void kb_link_c_on_ble_evt(ble_evt_t const *p_ble_evt, void * p_context) {
    kb_link_c_t *p_kb_link_c = (kb_link_c_t *)p_context;

    if (p_ble_evt == NULL || p_kb_link_c == NULL) {
        return;
    }

    if (p_kb_link_c->conn_handle != BLE_CONN_HANDLE_INVALID && p_kb_link_c->conn_handle != p_ble_evt->evt.gap_evt.conn_handle) {
        return;
    }

    //NRF_LOG_INFO("KB link client evt; evt: 0x%X.", p_ble_evt->header.evt_id);

    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTC_EVT_HVX:
            NRF_LOG_INFO("Receive notification.");

            on_hvx(p_kb_link_c, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");

            if (p_ble_evt->evt.gap_evt.conn_handle == p_kb_link_c->conn_handle) {
                p_kb_link_c->conn_handle = BLE_CONN_HANDLE_INVALID;
                p_kb_link_c->handles.key_index_handle = BLE_CONN_HANDLE_INVALID;
                p_kb_link_c->handles.key_index_cccd_handle = BLE_CONN_HANDLE_INVALID;

                if (p_kb_link_c->evt_handler != NULL) {
                    kb_link_c_evt_t kb_link_c_evt;

                    kb_link_c_evt.evt_type = KB_LINK_C_EVT_DISCONNECTED;

                    p_kb_link_c->evt_handler(p_kb_link_c, &kb_link_c_evt);
                }
            }

        default:
            // No implementation needed.
            break;
    }
}

static void on_hvx(kb_link_c_t *p_kb_link_c, ble_evt_t const *p_ble_evt) {
    if (p_kb_link_c->handles.key_index_handle != BLE_CONN_HANDLE_INVALID && p_kb_link_c->evt_handler != NULL && p_ble_evt->evt.gattc_evt.params.hvx.handle == p_kb_link_c->handles.key_index_handle && p_ble_evt->evt.gattc_evt.params.hvx.type == BLE_GATT_HVX_NOTIFICATION) {
        kb_link_c_evt_t kb_link_c_evt;

        kb_link_c_evt.evt_type = KB_LINK_C_EVT_KEY_INDEX_UPDATE;
        kb_link_c_evt.len = p_ble_evt->evt.gattc_evt.params.hvx.len;
        kb_link_c_evt.p_data = (uint8_t *)p_ble_evt->evt.gattc_evt.params.hvx.data;

        p_kb_link_c->evt_handler(p_kb_link_c, &kb_link_c_evt);
    }
}

void kb_link_c_on_db_disc_evt(kb_link_c_t *p_kb_link_c, ble_db_discovery_evt_t *p_evt) {
    NRF_LOG_INFO("kb_link_c_on_db_disc_evt.");

    kb_link_c_evt_t kb_link_c_evt = {0};

    ble_gatt_db_char_t *p_chars = p_evt->params.discovered_db.charateristics;

    // Check if KB link was discovered.
    if (p_evt->evt_type == BLE_DB_DISCOVERY_COMPLETE && p_evt->params.discovered_db.srv_uuid.uuid == KB_LINK_SERVICE_UUID && p_evt->params.discovered_db.srv_uuid.type == p_kb_link_c->uuid_type) {
        for (int i = 0; i < p_evt->params.discovered_db.char_count; i++) {
            switch (p_chars[i].characteristic.uuid.uuid) {
                case KB_LINK_KEY_INDEX_CHAR_UUID:
                    kb_link_c_evt.handles.key_index_handle = p_chars[i].characteristic.handle_value;
                    kb_link_c_evt.handles.key_index_cccd_handle = p_chars[i].cccd_handle;
                    break;

                default:
                    break;
            }
        }

        if (p_kb_link_c->evt_handler != NULL) {
            kb_link_c_evt.conn_handle = p_evt->conn_handle;
            kb_link_c_evt.evt_type = KB_LINK_C_EVT_DISCOVERY_COMPLETE;

            p_kb_link_c->evt_handler(p_kb_link_c, &kb_link_c_evt);
        }
    }
}

uint32_t kb_link_c_key_index_notif_enable(kb_link_c_t *p_kb_link_c) {
    VERIFY_PARAM_NOT_NULL(p_kb_link_c);

    if (p_kb_link_c->conn_handle == BLE_CONN_HANDLE_INVALID || p_kb_link_c->handles.key_index_cccd_handle == BLE_CONN_HANDLE_INVALID) {
        return NRF_ERROR_INVALID_STATE;
    }

    return cccd_configure(p_kb_link_c->conn_handle, p_kb_link_c->handles.key_index_cccd_handle, true);
}

static uint32_t cccd_configure(uint16_t conn_handle, uint16_t cccd_handle, bool enable) {
    uint8_t buffer[BLE_CCCD_VALUE_LEN];

    buffer[0] = enable ? BLE_GATT_HVX_NOTIFICATION : 0;
    buffer[1] = 0;

    ble_gattc_write_params_t const write_params = {
        .write_op = BLE_GATT_OP_WRITE_REQ,
        .flags = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE,
        .handle = cccd_handle,
        .offset = 0,
        .len = sizeof(buffer),
        .p_value = buffer
    };

    return sd_ble_gattc_write(conn_handle, &write_params);
}

uint32_t kb_link_c_handles_assign(kb_link_c_t *p_kb_link_c, uint16_t conn_handle, kb_link_c_handles_t const *p_peer_handles) {
    VERIFY_PARAM_NOT_NULL(p_kb_link_c);

    p_kb_link_c->conn_handle = conn_handle;

    if (p_peer_handles != NULL) {
        p_kb_link_c->handles.key_index_handle = p_peer_handles->key_index_handle;
        p_kb_link_c->handles.key_index_cccd_handle = p_peer_handles->key_index_cccd_handle;
    }

    return NRF_SUCCESS;
}
