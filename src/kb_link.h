#ifndef _KB_LINK_H_
#define _KB_LINK_H_

#include "ble.h"
#include "ble_srv_common.h"

#define KB_LINK_BLE_OBSERVER_PRIO 2

#define KB_LINK_DEF(_name)                          \
    static kb_link_t _name;                         \
    NRF_SDH_BLE_OBSERVER(_name ## _obs,             \
                         KB_LINK_BLE_OBSERVER_PRIO, \
                         kb_link_on_ble_evt,        \
                         &_name)

// Base UUID: 0D660000-AF06-44F6-A004-F8A8138518C0
#define KB_LINK_SERVICE_BASE_UUID {0xC0, 0x18, 0x85, 0x13, 0xA8, 0xF8, 0x04, 0xA0, 0xF6, 0x44, 0x06, 0xAF, 0x00, 0x00, 0x66, 0x0D}

// Service & characteristics UUIDs
#define KB_LINK_SERVICE_UUID        0xF36B
#define KB_LINK_KEY_INDEX_CHAR_UUID 0xC74B


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

uint32_t kb_link_key_index_update(kb_link_t *p_kb_link, uint8_t *p_key_index, uint8_t p_len);

#endif
