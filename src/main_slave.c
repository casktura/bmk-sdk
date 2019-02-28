#include <stdint.h>

#include "app_error.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "ble_advertising.h"
#include "ble_dis.h"
#include "ble_err.h"
#include "ble.h"
#include "nordic_common.h"
#include "nrf_assert.h"
#include "nrf_ble_gatt.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh.h"
#include "nrf.h"

#include "config/keyboard.h"
#include "error_handler/error_handler.h"
#include "firmware_config.h"
#include "kb_link/kb_link.h"
#include "low_power/low_power.h"
#include "shared/shared.h"

/*
 * Variables declaration.
 */
// nRF52 variables.
APP_TIMER_DEF(m_scan_timer_id);
NRF_BLE_GATT_DEF(m_gatt);
BLE_ADVERTISING_DEF(m_advertising);
KB_LINK_DEF(m_kb_link);

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; // Handle of the current connection.
static ble_uuid_t m_adv_uuid = {SLAVE_UUID, BLE_UUID_TYPE_VENDOR_BEGIN};

// Firmware variables.
const uint8_t ROWS[MATRIX_ROW_NUM] = MATRIX_ROW_PINS;
const uint8_t COLS[MATRIX_COL_NUM] = MATRIX_COL_PINS;
const int8_t MATRIX[MATRIX_ROW_NUM][MATRIX_COL_NUM] = MATRIX_DEFINE;

static bool m_key_pressed[MATRIX_ROW_NUM][MATRIX_COL_NUM] = {0};
static int m_debounce[MATRIX_ROW_NUM][MATRIX_COL_NUM];
static int m_low_power_mode_counter = LOW_POWER_MODE_DELAY;

/*
 * Functions declaration.
 */
// nRF52 functions.
static void timers_init(void);
static void scan_timeout_handler(void *p_context);
static void ble_stack_init(void);
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context);
static void gatt_init(void);
static void advertising_init(void);
static void adv_evt_handler(ble_adv_evt_t ble_adv_evt);
static void dis_init(void);
static void kbl_init(void);
static void advertising_start(void);
static void timers_start(void);

// Firmware functions.
static void firmware_init(void);
static void scan_matrix_task(void *p_data, uint16_t size);

int main(void) {
    // Initialize.
    // nRF52.
    log_init();
    timers_init();
    power_management_init();
    ble_stack_init();
    conn_evt_length_ext_init();
    scheduler_init();
    gap_params_init();
    gatt_init();
    dis_init();
    kbl_init();

    // Init advertising after all services.
    advertising_init();
    conn_params_init();

    // Firmware.
    firmware_init();
    pins_init();
    low_power_mode_init(&m_scan_timer_id);

    // Start.
    advertising_start();
    timers_start();

    NRF_LOG_INFO("main; started.");

    // Enter main loop.
    for (;;) {
        idle_state_handle();
    }
}

/*
 * nRF52 section.
 */
static void timers_init(void) {
    ret_code_t err_code;

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Matrix scan timer
    err_code = app_timer_create(&m_scan_timer_id, APP_TIMER_MODE_REPEATED, scan_timeout_handler);
    APP_ERROR_CHECK(err_code);
}

static void scan_timeout_handler(void *p_context) {
    UNUSED_PARAMETER(p_context);
    ret_code_t err_code;

    err_code = app_sched_event_put(NULL, 0, scan_matrix_task);
    APP_ERROR_CHECK(err_code);
}

static void ble_stack_init(void) {
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings. Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
    ret_code_t err_code;

    NRF_LOG_INFO("BLE evt; evt: 0x%X.", p_ble_evt->header.evt_id);

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.");

            if (p_ble_evt->evt.gap_evt.params.connected.role == BLE_GAP_ROLE_PERIPH) {
                NRF_LOG_INFO("As peripheral.");
                NRF_LOG_INFO("Conn params; conn interval: %i, conn sup timeout: %i.", p_ble_evt->evt.gap_evt.params.connected.conn_params.min_conn_interval * 1.25, p_ble_evt->evt.gap_evt.params.connected.conn_params.conn_sup_timeout * 10);

                m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            }
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
            NRF_LOG_INFO("Conn params update; conn interval: %i, conn sup timeout: %i.", p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.min_conn_interval * 1.25, p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.conn_sup_timeout * 10);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected; reason: 0x%X.", p_ble_evt->evt.gap_evt.params.disconnected.reason);

            if (p_ble_evt->evt.gap_evt.conn_handle == m_conn_handle) {
                m_conn_handle = BLE_CONN_HANDLE_INVALID;
            }
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            NRF_LOG_DEBUG("PHY update request.");

            ble_gap_phys_t const phys = {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };

            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        }
        break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT client timeout.");

            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT server simeout.");

            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}

static void gatt_init(void) {
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

static void dis_init(void) {
    ret_code_t err_code;
    ble_dis_init_t dis_init_obj = {0};

    ble_srv_ascii_to_utf8(&dis_init_obj.manufact_name_str, MANUFACTURER_NAME);
    dis_init_obj.dis_char_rd_sec = SEC_OPEN;

    err_code = ble_dis_init(&dis_init_obj);
    APP_ERROR_CHECK(err_code);
}

static void kbl_init(void) {
    ret_code_t err_code;
    kb_link_init_t init = {0};

    init.len = 0;
    init.key_index = NULL;

    err_code = kb_link_init(&m_kb_link, &init);
    APP_ERROR_CHECK(err_code);
}

static void advertising_init(void) {
    uint32_t err_code;
    ble_advertising_init_t init = {0};

    init.advdata.include_appearance = true;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = 1;
    init.advdata.uuids_complete.p_uuids = &m_adv_uuid;

    init.srdata.name_type = BLE_ADVDATA_FULL_NAME;

    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = SLAVE_ADV_FAST_INTERVAL;
    init.config.ble_adv_fast_timeout = SLAVE_ADV_FAST_DURATION;

    init.evt_handler = adv_evt_handler;
    init.error_handler = adv_error_handler;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

static void adv_evt_handler(const ble_adv_evt_t ble_adv_evt) {
    ret_code_t err_code;

    NRF_LOG_INFO("ADV evt; evt: 0x%X.", ble_adv_evt);

    switch (ble_adv_evt) {
        case BLE_ADV_EVT_IDLE:
            NRF_LOG_INFO("Stop advertising.");
            break;

        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising.");
            break;

        default:
            break;
    }
}

static void advertising_start(void) {
    ret_code_t err_code;

    err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}

static void timers_start(void) {
    ret_code_t err_code;

    err_code = app_timer_start(m_scan_timer_id, SCAN_DELAY_TICKS, NULL);
    APP_ERROR_CHECK(err_code);
}

/*
 * Firmware section.
 */
static void firmware_init(void) {
    NRF_LOG_INFO("firmware_init.");

    for (int i = 0; i < MATRIX_ROW_NUM; i++) {
        for (int j = 0; j < MATRIX_COL_NUM; j++) {
            m_debounce[i][j] = KEY_PRESS_DEBOUNCE;
        }
    }
}

static void scan_matrix_task(void *p_data, uint16_t size) {
    UNUSED_PARAMETER(p_data);
    UNUSED_PARAMETER(size);

    ret_code_t err_code;
    bool buffer_changed = false;
    int buffer_len = 0;
    int8_t buffer[SLAVE_KEY_NUM] = {0};

    for (int col = 0; col < MATRIX_COL_NUM; col++) {
        nrf_gpio_pin_set(COLS[col]);
        nrf_delay_us(PIN_SET_DELAY);

        for (int row = 0; row < MATRIX_ROW_NUM; row++) {
            bool pressed = nrf_gpio_pin_read(ROWS[row]) > 0;

            if (m_key_pressed[row][col] == pressed) {
                if (pressed) {
                    m_debounce[row][col] = KEY_RELEASE_DEBOUNCE;
                } else {
                    m_debounce[row][col] = KEY_PRESS_DEBOUNCE;
                }
            } else {
                if (m_debounce[row][col] <= 0) {
                    if (pressed) {
                        // On key press
                        m_key_pressed[row][col] = true;
                        m_debounce[row][col] = KEY_RELEASE_DEBOUNCE;

                        if (buffer_len < SLAVE_KEY_NUM) {
                            buffer_changed = true;
                            buffer[buffer_len++] = MATRIX[row][col];
                        }
                    } else {
                        // On key release
                        m_key_pressed[row][col] = false;
                        m_debounce[row][col] = KEY_PRESS_DEBOUNCE;

                        if (buffer_len < SLAVE_KEY_NUM) {
                            buffer_changed = true;
                            buffer[buffer_len++] = -MATRIX[row][col];
                        }
                    }
                } else {
                    m_debounce[row][col] -= SCAN_DELAY;
                }
            }
        }

        nrf_gpio_pin_clear(COLS[col]);
    }

    if (buffer_changed) {
        m_low_power_mode_counter = LOW_POWER_MODE_DELAY;

        // Set key index characteristics
        kb_link_key_index_update(&m_kb_link, (uint8_t *)buffer, buffer_len);
    } else {
        m_low_power_mode_counter -= SCAN_DELAY;
    }

    if (m_low_power_mode_counter <= 0) {
        m_low_power_mode_counter = LOW_POWER_MODE_DELAY;
        low_power_mode_start();
    }
}
