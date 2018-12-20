#ifndef _KB_LINK_C_H_
#define _KB_LINK_C_H_

#include "ble.h"
#include "ble_srv_common.h"
#include "ble_db_discovery.h"

#define KB_LINK_C_BLE_OBSERVER_PRIO 2

#define KB_LINK_C_DEF(_name)                          \
    static kb_link_c_t _name;                         \
    NRF_SDH_BLE_OBSERVER(_name ## _obs,               \
                         KB_LINK_C_BLE_OBSERVER_PRIO, \
                         kb_link_c_on_ble_evt,        \
                         &_name)

// Base UUID: 0D660000-AF06-44F6-A004-F8A8138518C0
#define KB_LINK_SERVICE_BASE_UUID {0xC0, 0x18, 0x85, 0x13, 0xA8, 0xF8, 0x04, 0xA0, 0xF6, 0x44, 0x06, 0xAF, 0x00, 0x00, 0x66, 0x0D}

// Service & characteristics UUIDs
#define KB_LINK_SERVICE_UUID        0xF36B
#define KB_LINK_KEY_INDEX_CHAR_UUID 0xC74B

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