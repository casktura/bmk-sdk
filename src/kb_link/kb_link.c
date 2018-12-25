/*
 * KB link service.
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

#include "kb_link.h"
#include "../firmware_config.h"

static uint32_t key_index_characteristics_add(kb_link_t *p_kb_link, const kb_link_init_t *p_kb_link_init);

uint32_t kb_link_init(kb_link_t *p_kb_link, const kb_link_init_t *p_kb_link_init) {
    VERIFY_PARAM_NOT_NULL(p_kb_link);
    VERIFY_PARAM_NOT_NULL(p_kb_link_init);

    uint32_t err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_kb_link->conn_handle = BLE_CONN_HANDLE_INVALID;

    // Add KB link service uuid
    ble_uuid128_t base_uuid = {KB_LINK_SERVICE_BASE_UUID};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_kb_link->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_kb_link->uuid_type;
    ble_uuid.uuid = KB_LINK_SERVICE_UUID;

    // Add KB link service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_kb_link->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add key index characteristics
    return key_index_characteristics_add(p_kb_link, p_kb_link_init);
}

static uint32_t key_index_characteristics_add(kb_link_t *p_kb_link, const kb_link_init_t *p_kb_link_init) {
    ble_add_char_params_t add_char_params = {0};

    add_char_params.uuid = KB_LINK_KEY_INDEX_CHAR_UUID;
    add_char_params.uuid_type = p_kb_link->uuid_type;
    add_char_params.max_len = SLAVE_KEY_NUM;
    add_char_params.p_init_value = p_kb_link_init->key_index;
    add_char_params.init_len = p_kb_link_init->len;
    add_char_params.is_var_len = true;
    add_char_params.read_access = SEC_OPEN;
    add_char_params.write_access = SEC_NO_ACCESS;
    add_char_params.cccd_write_access = SEC_OPEN;
    add_char_params.char_props.read = 1;
    add_char_params.char_props.notify = 1;

    return characteristic_add(p_kb_link->service_handle, &add_char_params, &p_kb_link->key_index_char_handles);
}

void kb_link_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context) {
    kb_link_t *p_kb_link_service = (kb_link_t *)p_context;

    if (p_ble_evt == NULL || p_kb_link_service == NULL) {
        return;
    }

    NRF_LOG_INFO("KB link evt; evt: 0x%X.", p_ble_evt->header.evt_id);

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.");

            p_kb_link_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected.");

            p_kb_link_service->conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        default:
            // No implementation needed.
            break;
    }
}

uint32_t kb_link_key_index_update(kb_link_t *p_kb_link, uint8_t *p_key_index, uint8_t len) {
    VERIFY_PARAM_NOT_NULL(p_kb_link);

    NRF_LOG_INFO("kb_link_key_index_update.");

    uint32_t err_code;
    ble_gatts_value_t gatts_value = {0};

    gatts_value.len = len;
    gatts_value.p_value = p_key_index;

    err_code = sd_ble_gatts_value_set(p_kb_link->conn_handle, p_kb_link->key_index_char_handles.value_handle, &gatts_value);
    VERIFY_SUCCESS(err_code);

    // Try to notify master if connected
    if (p_kb_link->conn_handle != BLE_CONN_HANDLE_INVALID) {
        ble_gatts_hvx_params_t hvx_params = {0};

        hvx_params.handle = p_kb_link->key_index_char_handles.value_handle;
        hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.p_len = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(p_kb_link->conn_handle, &hvx_params);
        NRF_LOG_INFO("sd_ble_gatts_hvx; ret: 0x%X.", err_code);
    }

    return err_code;
}
