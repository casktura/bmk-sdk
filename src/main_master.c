#include <stdint.h>
#include <string.h>

#include "nrf_ble_gq.h"
#include "app_error.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"
#include "ble_dis.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_hids.h"
#include "ble_srv_common.h"
#include "ble.h"
#include "fds.h"
#include "nordic_common.h"
#include "nrf_ble_gatt.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log_ctrl.h"
#include "nrf_log.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh.h"
#include "nrf.h"
#include "peer_manager_handler.h"
#include "peer_manager.h"

#include "config/keyboard.h"
#include "config/keymap.h"
#include "error_handler/error_handler.h"
#include "firmware_config.h"
#include "low_power/low_power.h"
#include "shared/shared.h"

#ifdef HAS_SLAVE
#include "ble_db_discovery.h"
#include "nrf_ble_scan.h"

#include "kb_link/kb_link_c.h"
#endif

/*
 * Variables declaration.
 */
// nRF52 variables.
APP_TIMER_DEF(m_scan_timer_id);
NRF_BLE_GQ_DEF(m_ble_gatt_queue, NRF_SDH_BLE_CENTRAL_LINK_COUNT, NRF_BLE_GQ_QUEUE_SIZE);
NRF_BLE_GATT_DEF(m_gatt);
BLE_ADVERTISING_DEF(m_advertising);
BLE_HIDS_DEF(m_hids, NRF_SDH_BLE_TOTAL_LINK_COUNT, KB_INPUT_REPORT_MAX_LEN, CC_INPUT_REPORT_MAX_LEN, OUTPUT_REPORT_MAX_LEN);

#ifdef HAS_SLAVE
NRF_BLE_SCAN_DEF(m_scan);
BLE_DB_DISCOVERY_DEF(m_db_disc);
KB_LINK_C_DEF(m_kb_link_c);
#endif

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; // Handle of the current connection.
static pm_peer_id_t m_peer_id = PM_PEER_ID_INVALID;      // Device reference handle to the current bonded central.
static ble_uuid_t m_adv_uuid = {BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE, BLE_UUID_TYPE_BLE};
static bool volatile m_fds_initialized = false;

// HID variables.
static bool m_hids_in_boot_mode = false; // Current protocol mode.
static bool m_caps_lock_on = false;      // Variable to indicate if Caps Lock is turned on.

// Firmware variables.
const uint8_t ROWS[MATRIX_ROW_NUM] = MATRIX_ROW_PINS;
const uint8_t COLS[MATRIX_COL_NUM] = MATRIX_COL_PINS;
const int8_t MATRIX[MATRIX_ROW_NUM][MATRIX_COL_NUM] = MATRIX_DEFINE;

static bool m_key_pressed[MATRIX_ROW_NUM][MATRIX_COL_NUM] = {false};
static int m_debounce[MATRIX_ROW_NUM][MATRIX_COL_NUM];
static int m_low_power_mode_counter = LOW_POWER_MODE_DELAY;

typedef enum {
    KEY_TYPE_NOT_TRANSLATED,
    KEY_TYPE_NO_REPORT,
    KEY_TYPE_KEY,
    KEY_TYPE_MODIFIER,
    KEY_TYPE_KEY_WITH_MODIFIER,
    KEY_TYPE_CONSUMER
} key_type_t;

typedef struct {
    uint8_t modifiers;
    uint8_t key;
} kb_data_t;

typedef union {
    kb_data_t kb;
    uint16_t cc;
} key_data_t;

typedef struct {
    int8_t index;
    uint8_t source;
    key_type_t type;
    key_data_t data;
} key_t;

static key_t m_keys[KEY_NUM];
static int m_key_count = 0;

static bool m_translate_key_index_task_queued = false;
static bool m_generate_hid_report_task_queued = false;

// Device connection.
typedef struct {
    uint8_t current_device;
    uint8_t addrs[3];
    pm_peer_id_t peer_ids[3];
    uint16_t padding; // Not used, needed to ensure size of this structure is multiple of 4 bytes.
} device_connection_t;

static device_connection_t m_device_connection = {
    .current_device = 0,
    .addrs = {0},
    .peer_ids = { PM_PEER_ID_INVALID, PM_PEER_ID_INVALID, PM_PEER_ID_INVALID }
};

static const fds_record_t m_device_connection_record = {
    .file_id = CONFIG_FILE_ID,
    .key = DEVICE_CONNECTION_KEY,
    .data.p_data = &m_device_connection,
    .data.length_words = (sizeof(m_device_connection) + 3) / sizeof(uint32_t) // length_words is multiple of 4 bytes.
};

static fds_record_desc_t m_device_connection_record_desc = {0};
static bool m_reset_device_connection_update = false;

// HID report.
typedef enum {
    HID_TYPE_KB_REPORT,
    HID_TYPE_CC_REPORT
} hid_report_type_t;

typedef union {
    uint8_t kb[KB_INPUT_REPORT_MAX_LEN];
    uint16_t cc;
} hid_report_data_t;

typedef struct {
    hid_report_type_t type;
    hid_report_data_t data;
} hid_report_t;

typedef struct {
    hid_report_t reports[HID_REPORT_BUFFER_NUM];
    int8_t start;
    int8_t end;
    int8_t count;
} hid_report_buffer_t;

static hid_report_buffer_t m_hid_buffer = {0};

/*
 * Functions declaration.
 */
// nRF52 functions.
static void timers_init(void);
static void scan_timeout_handler(void *p_context);
static void ble_stack_init(void);
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context);
static void gatt_init(void);
static void dis_init(void);
static void hids_init(void);
static void hids_evt_handler(ble_hids_t *p_hids, ble_hids_evt_t *p_evt);
static void on_hid_rep_char_write(ble_hids_evt_t *p_evt);
static void advertising_init(void);
static void adv_evt_handler(ble_adv_evt_t ble_adv_evt);
static void identities_set(pm_peer_id_list_skip_t skip);
static void peer_manager_init(void);
static void pm_evt_handler(pm_evt_t const *p_evt);
static void gap_address_init(void);
static void flash_data_init(void);
static void fds_evt_handler(fds_evt_t const * p_evt);
static void reset_device(void);
static void peers_refresh(void);
static void set_whitelist(void);
static void advertising_start(void);
static void timers_start(void);
static void hids_send_report(hid_report_t *p_report);
#ifdef HAS_SLAVE
static void db_discovery_init(void);
static void db_disc_handler(ble_db_discovery_evt_t *p_evt);
static void kbl_c_init(void);
static void kbl_c_evt_handler(kb_link_c_t *p_kb_link_c, kb_link_c_evt_t const * p_evt);
static void scan_init(void);
static void scan_start(void);
#endif

// Firmware functions.
static void firmware_init(void);
static void scan_matrix_task(void *p_data, uint16_t size);
static bool update_key_index(int8_t index, uint8_t source);
static void put_translate_key_index_task(void);
static void translate_key_index_task(void *p_data, uint16_t size);
static void put_generate_hid_report_task(void);
static void generate_hid_report_task(void *p_data, uint16_t size);
#ifdef HAS_SLAVE
static void process_slave_key_index_task(void *p_data, uint16_t size);
static void clear_slave_key_index_task(void *p_data, uint16_t size);
#endif

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
    hids_init();
#ifdef HAS_SLAVE
    db_discovery_init();
    kbl_c_init();
    scan_init();
#endif

    // Init advertising after all services.
    advertising_init();
    conn_params_init();
    peer_manager_init();
    gap_address_init();
    peers_refresh();
    set_whitelist(); // Set whitelist once when device newly started.

    // Firmware.
    pins_init();
    firmware_init();
    low_power_mode_init(&m_scan_timer_id);

    // Start.
    advertising_start();
#ifdef HAS_SLAVE
    scan_start();
#endif
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

    // Matrix scan timer.
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

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.");
            if (p_ble_evt->evt.gap_evt.params.connected.role == BLE_GAP_ROLE_PERIPH) {
                NRF_LOG_INFO("As peripheral.");
                NRF_LOG_INFO("Conn params; conn interval: %i, conn sup timeout: %i.", p_ble_evt->evt.gap_evt.params.connected.conn_params.min_conn_interval * 1.25, p_ble_evt->evt.gap_evt.params.connected.conn_params.conn_sup_timeout * 10);

                m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            }
#ifdef HAS_SLAVE
            else if (p_ble_evt->evt.gap_evt.params.connected.role == BLE_GAP_ROLE_CENTRAL) {
                NRF_LOG_INFO("As central.");

                err_code = kb_link_c_handles_assign(&m_kb_link_c, p_ble_evt->evt.gap_evt.conn_handle, NULL);
                APP_ERROR_CHECK(err_code);

                err_code = ble_db_discovery_start(&m_db_disc, p_ble_evt->evt.gap_evt.conn_handle);
                APP_ERROR_CHECK(err_code);
            }
#endif
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
            NRF_LOG_INFO("Conn param update request.");

            sd_ble_gap_conn_param_update(p_ble_evt->evt.gap_evt.conn_handle, &p_ble_evt->evt.gap_evt.params.conn_param_update_request.conn_params);
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
            NRF_LOG_INFO("Conn params update; conn interval: %i, conn sup timeout: %i.", p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.min_conn_interval * 1.25, p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.conn_sup_timeout * 10);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected; reason: 0x%X.", p_ble_evt->evt.gap_evt.params.disconnected.reason);

            if (p_ble_evt->evt.gap_evt.conn_handle == m_conn_handle) {
                m_conn_handle = BLE_CONN_HANDLE_INVALID;
                m_peer_id = PM_PEER_ID_INVALID;
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

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            if (p_ble_evt->evt.gatts_evt.conn_handle == m_conn_handle && m_hid_buffer.count > 0) {
                hids_send_report(NULL);
            }
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            NRF_LOG_DEBUG("GATT sys attr missing.");

            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}

static void gatt_init(void) {
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

static void dis_init(void) {
    ret_code_t err_code;
    ble_dis_init_t dis_init_obj = {0};

    ble_srv_ascii_to_utf8(&dis_init_obj.manufact_name_str, MANUFACTURER_NAME);
    dis_init_obj.dis_char_rd_sec = SEC_JUST_WORKS;

    err_code = ble_dis_init(&dis_init_obj);
    APP_ERROR_CHECK(err_code);
}

static void hids_init(void) {
    ret_code_t err_code;
    ble_hids_init_t hids_init_obj = {0};
    ble_hids_inp_rep_init_t *p_kb_input_report;
    ble_hids_inp_rep_init_t *p_cc_input_report;
    ble_hids_outp_rep_init_t *p_output_report;
    uint8_t hid_info_flags;

    static ble_hids_inp_rep_init_t input_report_array[INPUT_REPORT_NUM];
    static ble_hids_outp_rep_init_t output_report_array[1];
    static uint8_t report_map_data[] = {
        // Keyboard report.
        0x05, 0x01,       // Usage Page (Generic Desktop).
        0x09, 0x06,       // Usage (Keyboard).
        0xA1, 0x01,       // Collection (Application).
        0x85, 0x01,       // Report Id (1).

        // Modifiers key, 1 byte to represent 8 keys. 1 bit for 1 key.
        0x05, 0x07,       // Usage Page (Keyboard).
        0x19, 0xE0,       // Usage Minimum (Left Control).
        0x29, 0xE7,       // Usage Maximum (Right Gui).
        0x15, 0x00,       // Logical Minimum (0).
        0x25, 0x01,       // Logical Maximum (1).
        0x95, 0x08,       // Report Count (8).
        0x75, 0x01,       // Report Size (1).
        0x81, 0x02,       // Input (Data, Variable, Absolute).

        // Reserved byte (8 bits).
        0x95, 0x01,       // Report Count (1).
        0x75, 0x08,       // Report Size (8).
        0x81, 0x01,       // Input (Constant, Array, Absolute).

        // Keyboard report, 6 bytes for 6 keys.
        0x19, 0x00,       // Usage Minimum (Reserved (No Event Indicated)).
        0x29, 0xA4,       // Usage Maximum (Keyboard ExSel).
        0x15, 0x00,       // Logical Minimum (0).
        0x25, 0xA4,       // Logical Maximum (164).
        0x95, 0x06,       // Report Count (6).
        0x75, 0x08,       // Report Size (8).
        0x81, 0x00,       // Input (Data, Array, Absolute).

        // LEDs output report.
        // Represent by each bit, Kana | Compose | Scroll Lock | Caps Lock | Num Lock.
        0x05, 0x08,       // Usage Page (LEDs).
        0x19, 0x01,       // Usage Minimum (Num Lock).
        0x29, 0x05,       // Usage Maximum (Kana).
        0x95, 0x05,       // Report Count (5).
        0x75, 0x01,       // Report Size (1).
        0x91, 0x02,       // Output (Data, Variable, Absolute).

        // LEDs output report padding.
        0x95, 0x01,       // Report Count (1).
        0x75, 0x03,       // Report Size (3).
        0x91, 0x01,       // Output (Constant, Variable, Absolute).

        0xC0,             // End Collection (Application).

        // Consumer Control report.
        0x05, 0x0C,       // Usage Page (Consumer Devices).
        0x09, 0x01,       // Usage (Consumer Control).
        0xA1, 0x01,       // Collection (Application).
        0x85, 0x02,       // Report Id (2).

        // Consumer Control, 1 key at a time.
        0x19, 0x00,       // Usage Minimum (Unassigned).
        0x2A, 0xFF, 0x00, // Usage Maximum (255).
        0x15, 0x00,       // Logical Minimum (0).
        0x26, 0xFF, 0x00, // Logical Maximum (255).
        0x95, 0x01,       // Report Count (1).
        0x75, 0x10,       // Report Size (16).
        0x81, 0x00,       // Input (Data, Array, Absolute).

        0xC0              // End Collection (Application).
    };

    memset((void *)input_report_array, 0, sizeof(ble_hids_inp_rep_init_t) * INPUT_REPORT_NUM);
    memset((void *)output_report_array, 0, sizeof(ble_hids_outp_rep_init_t));

    // Initialize HID Service.
    // Keyboard input report.
    p_kb_input_report = &input_report_array[KB_INPUT_REPORT_INDEX];
    p_kb_input_report->max_len = KB_INPUT_REPORT_MAX_LEN;
    p_kb_input_report->rep_ref.report_id = KB_INPUT_REPORT_ID;
    p_kb_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;

    p_kb_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_kb_input_report->sec.wr = SEC_JUST_WORKS;
    p_kb_input_report->sec.rd = SEC_JUST_WORKS;

    // Keyboard input report.
    p_cc_input_report = &input_report_array[CC_INPUT_REPORT_INDEX];
    p_cc_input_report->max_len = CC_INPUT_REPORT_MAX_LEN;
    p_cc_input_report->rep_ref.report_id = CC_INPUT_REPORT_ID;
    p_cc_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;

    p_cc_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_cc_input_report->sec.wr = SEC_JUST_WORKS;
    p_cc_input_report->sec.rd = SEC_JUST_WORKS;

    // LEDs output report.
    p_output_report = &output_report_array[OUTPUT_REPORT_INDEX];
    p_output_report->max_len = OUTPUT_REPORT_MAX_LEN;
    p_output_report->rep_ref.report_id = OUTPUT_REPORT_ID;
    p_output_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_OUTPUT;

    p_output_report->sec.wr = SEC_JUST_WORKS;
    p_output_report->sec.rd = SEC_JUST_WORKS;

    hid_info_flags = HID_INFO_FLAG_REMOTE_WAKE_MSK | HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK;

    hids_init_obj.evt_handler = hids_evt_handler;
    hids_init_obj.error_handler = hid_error_handler;
    hids_init_obj.is_kb = true;
    hids_init_obj.is_mouse = false;
    hids_init_obj.inp_rep_count = INPUT_REPORT_NUM;
    hids_init_obj.p_inp_rep_array = input_report_array;
    hids_init_obj.outp_rep_count = 1;
    hids_init_obj.p_outp_rep_array = output_report_array;
    hids_init_obj.feature_rep_count = 0;
    hids_init_obj.p_feature_rep_array = NULL;
    hids_init_obj.rep_map.data_len = sizeof(report_map_data);
    hids_init_obj.rep_map.p_data = report_map_data;
    hids_init_obj.hid_information.bcd_hid = BASE_USB_HID_SPEC_VERSION;
    hids_init_obj.hid_information.b_country_code = 0;
    hids_init_obj.hid_information.flags = hid_info_flags;
    hids_init_obj.included_services_count = 0;
    hids_init_obj.p_included_services_array = NULL;

    hids_init_obj.rep_map.rd_sec = SEC_JUST_WORKS;
    hids_init_obj.hid_information.rd_sec = SEC_JUST_WORKS;

    hids_init_obj.boot_kb_inp_rep_sec.cccd_wr = SEC_JUST_WORKS;
    hids_init_obj.boot_kb_inp_rep_sec.rd = SEC_JUST_WORKS;

    hids_init_obj.boot_kb_outp_rep_sec.rd = SEC_JUST_WORKS;
    hids_init_obj.boot_kb_outp_rep_sec.wr = SEC_JUST_WORKS;

    hids_init_obj.protocol_mode_rd_sec = SEC_JUST_WORKS;
    hids_init_obj.protocol_mode_wr_sec = SEC_JUST_WORKS;
    hids_init_obj.ctrl_point_wr_sec = SEC_JUST_WORKS;

    err_code = ble_hids_init(&m_hids, &hids_init_obj);
    APP_ERROR_CHECK(err_code);
}

static void hids_evt_handler(ble_hids_t *p_hids, ble_hids_evt_t *p_evt) {
    NRF_LOG_INFO("HIDs evt; evt: 0x%X.", p_evt->evt_type);

    switch (p_evt->evt_type) {
        case BLE_HIDS_EVT_BOOT_MODE_ENTERED:
            NRF_LOG_INFO("Boot mode entered.");

            m_hids_in_boot_mode = true;
            break;

        case BLE_HIDS_EVT_REPORT_MODE_ENTERED:
            NRF_LOG_INFO("Report mode entered.");

            m_hids_in_boot_mode = false;
            break;

        case BLE_HIDS_EVT_REP_CHAR_WRITE:
            NRF_LOG_INFO("Rep char write.");

            on_hid_rep_char_write(p_evt);
            break;

        case BLE_HIDS_EVT_NOTIF_ENABLED:
            NRF_LOG_INFO("Notify enabled.");
            break;

        default:
            // No implementation needed.
            break;
    }
}

static void on_hid_rep_char_write(ble_hids_evt_t *p_evt) {
    if (p_evt->params.char_write.char_id.rep_type == BLE_HIDS_REP_TYPE_OUTPUT) {
        ret_code_t err_code;
        uint8_t report_val;
        uint8_t report_index = p_evt->params.char_write.char_id.rep_index;

        if (report_index == OUTPUT_REPORT_INDEX) {
            // This code assumes that the output report is one byte long. Hence the following static assert is made.
            STATIC_ASSERT(OUTPUT_REPORT_MAX_LEN == 1);

            err_code = ble_hids_outp_rep_get(&m_hids, report_index, OUTPUT_REPORT_MAX_LEN, 0, m_conn_handle, &report_val);
            APP_ERROR_CHECK(err_code);

            // Set Caps Lock indicator here.
            if (!m_caps_lock_on && ((report_val & OUTPUT_REPORT_BIT_MASK_CAPS_LOCK) != 0)) {
                // Caps Lock is turned On.
                NRF_LOG_INFO("Caps Lock is turned On!");

                m_caps_lock_on = true;
            } else if (m_caps_lock_on && ((report_val & OUTPUT_REPORT_BIT_MASK_CAPS_LOCK) == 0)) {
                // Caps Lock is turned Off.
                NRF_LOG_INFO("Caps Lock is turned Off!");

                m_caps_lock_on = false;
            }
        }
    }
}

static void advertising_init(void) {
    ret_code_t err_code;
    ble_advertising_init_t init = {0};

    init.advdata.include_appearance = true;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = 1;
    init.advdata.uuids_complete.p_uuids = &m_adv_uuid;

    init.srdata.name_type = BLE_ADVDATA_FULL_NAME;

    init.config.ble_adv_whitelist_enabled = true;
    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = MASTER_ADV_FAST_INTERVAL;
    init.config.ble_adv_fast_timeout = MASTER_ADV_FAST_DURATION;
    init.config.ble_adv_slow_enabled = true;
    init.config.ble_adv_slow_interval = MASTER_ADV_SLOW_INTERVAL;
    init.config.ble_adv_slow_timeout = MASTER_ADV_SLOW_DURATION;

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
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising.");
            break;

        case BLE_ADV_EVT_FAST_WHITELIST:
            NRF_LOG_INFO("Fast advertising with whitelist.");
            break;

        case BLE_ADV_EVT_SLOW:
            NRF_LOG_INFO("Slow advertising.");
            break;

        case BLE_ADV_EVT_SLOW_WHITELIST:
            NRF_LOG_INFO("Slow advertising with whitelist.");
            break;

        case BLE_ADV_EVT_IDLE:
            NRF_LOG_INFO("Stop advertising.");
            break;

        case BLE_ADV_EVT_WHITELIST_REQUEST:{
            ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
            ble_gap_irk_t  whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
            uint32_t       addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
            uint32_t       irk_cnt  = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

            NRF_LOG_INFO("Whitelist request.");

            err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt, whitelist_irks,  &irk_cnt);
            APP_ERROR_CHECK(err_code);

            // Set the correct identities list (no excluding peers with no Central Address Resolution).
            identities_set(PM_PEER_ID_LIST_SKIP_NO_IRK);

            // Apply the whitelist.
            err_code = ble_advertising_whitelist_reply(&m_advertising, whitelist_addrs, addr_cnt, whitelist_irks, irk_cnt);
            APP_ERROR_CHECK(err_code);
        } break;

        default:
            break;
    }
}

static void identities_set(pm_peer_id_list_skip_t skip) {
    ret_code_t err_code;
    pm_peer_id_t peer_ids[BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT];
    uint32_t     peer_id_count = BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT;

    err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip);
    APP_ERROR_CHECK(err_code);

    err_code = pm_device_identities_list_set(peer_ids, peer_id_count);
    APP_ERROR_CHECK(err_code);
}

static void peer_manager_init(void) {
    ret_code_t err_code;
    ble_gap_sec_params_t sec_param = {0};

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    // Security parameters to be used for all security procedures.
    sec_param.bond = SEC_PARAM_BOND;
    sec_param.mitm = SEC_PARAM_MITM;
    sec_param.lesc = SEC_PARAM_LESC;
    sec_param.keypress = SEC_PARAM_KEYPRESS;
    sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob = SEC_PARAM_OOB;
    sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc = 1;
    sec_param.kdist_own.id = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

static void pm_evt_handler(pm_evt_t const *p_evt) {
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    ret_code_t err_code;

    switch (p_evt->evt_id) {
        case PM_EVT_CONN_SEC_SUCCEEDED:
            NRF_LOG_INFO("Connection secured.");

            m_peer_id = p_evt->peer_id;
            break;

        case PM_EVT_CONN_SEC_CONFIG_REQ: {
            NRF_LOG_INFO("Repairing request.");

            pm_conn_sec_config_t conn_sec_config = { .allow_repairing = true };

            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        }
        break;

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
            if (p_evt->params.peer_data_update_succeeded.flash_changed && p_evt->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_BONDING) {
                // Set peer id for current device.
                m_device_connection.peer_ids[m_device_connection.current_device] = p_evt->peer_id;

                err_code = fds_record_update(&m_device_connection_record_desc, &m_device_connection_record);
                APP_ERROR_CHECK(err_code);
            }
            break;

        case PM_EVT_PEER_DELETE_SUCCEEDED:
            NRF_LOG_INFO("Peer deleted.");
            break;

        default:
            break;
    }
}

static void gap_address_init(void) {
    ret_code_t err_code;

    flash_data_init();

    ble_gap_addr_t gap_addr;

    err_code = sd_ble_gap_addr_get(&gap_addr);
    APP_ERROR_CHECK(err_code);

    gap_addr.addr[3] = m_device_connection.addrs[m_device_connection.current_device];

    err_code = sd_ble_gap_addr_set(&gap_addr);
    APP_ERROR_CHECK(err_code);
}

static void flash_data_init(void) {
    ret_code_t err_code;

    err_code = fds_register(fds_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = fds_init();
    APP_ERROR_CHECK(err_code);

    while (!m_fds_initialized) {
        idle_state_handle();
    }

    // Device connection init.
    fds_find_token_t token = {0};
    bool generate_new_device_connection = false;

    err_code = fds_record_find(CONFIG_FILE_ID, DEVICE_CONNECTION_KEY, &m_device_connection_record_desc, &token);

    if (err_code == NRF_SUCCESS) {
        fds_flash_record_t device_connection_record;

        err_code = fds_record_open(&m_device_connection_record_desc, &device_connection_record);

        if (err_code == NRF_SUCCESS) {
            NRF_LOG_INFO("Found device connection record.");

            memcpy(&m_device_connection, device_connection_record.p_data, sizeof(device_connection_t));

            NRF_LOG_INFO("Addrs; 0x%X, 0x%X, 0x%X.", m_device_connection.addrs[0], m_device_connection.addrs[1], m_device_connection.addrs[2]);

            err_code = fds_record_close(&m_device_connection_record_desc);
            APP_ERROR_CHECK(err_code);
        } else {
            NRF_LOG_INFO("Cannot open record: 0x%X", err_code);

            generate_new_device_connection = true;
        }
    } else {
        NRF_LOG_INFO("Record not found: 0x%X.", err_code);

        generate_new_device_connection = true;
    }

    if (generate_new_device_connection) {
        NRF_LOG_INFO("Not found device connection record, generating new config.");

        uint8_t bytes_available;

        do {
            err_code = sd_rand_application_bytes_available_get(&bytes_available);
            APP_ERROR_CHECK(err_code);

            while (bytes_available < 3) {
                nrf_delay_ms(OPERATION_DELAY);

                err_code = sd_rand_application_bytes_available_get(&bytes_available);
                APP_ERROR_CHECK(err_code);
            }

            err_code = sd_rand_application_vector_get(&m_device_connection.addrs[0], 3);
            APP_ERROR_CHECK(err_code);
        } while (m_device_connection.addrs[0] == m_device_connection.addrs[1] || m_device_connection.addrs[1] == m_device_connection.addrs[2] || m_device_connection.addrs[2] == m_device_connection.addrs[0]); // To ensure unique addresses.

        NRF_LOG_INFO("Addrs; 0x%X, 0x%X, 0x%X.", m_device_connection.addrs[0], m_device_connection.addrs[1], m_device_connection.addrs[2]);

        err_code = fds_record_write(&m_device_connection_record_desc, &m_device_connection_record);
        APP_ERROR_CHECK(err_code);

        NRF_LOG_INFO("New device connection config is written, device will restart soon.");
    }
}

static void fds_evt_handler(fds_evt_t const * p_evt) {
    ret_code_t err_code;
    switch (p_evt->id) {
        case FDS_EVT_INIT:
            NRF_LOG_INFO("FDS initialized.");

            if (p_evt->result == NRF_SUCCESS) {
                m_fds_initialized = true;
            }
            break;

        case FDS_EVT_WRITE:
        case FDS_EVT_UPDATE:
            NRF_LOG_INFO("FDS record write.");

            if (p_evt->result == NRF_SUCCESS && p_evt->write.file_id == CONFIG_FILE_ID && p_evt->write.record_key == DEVICE_CONNECTION_KEY) {
                err_code = fds_gc();
                APP_ERROR_CHECK(err_code);
            }
            break;

        case FDS_EVT_GC:
            NRF_LOG_INFO("FDS garbage collected.");

            // Reset device if needed.
            if (m_reset_device_connection_update) {
                reset_device();
            }
            break;

        default:
            // No implementation needed.
            break;
    }
}

static void reset_device(void) {
    NRF_LOG_INFO("Restarting.");
    NRF_LOG_FINAL_FLUSH();

    ret_code_t err_code;

    err_code = sd_nvic_SystemReset();
    APP_ERROR_CHECK(err_code);
}

static void peers_refresh(void) {
    ret_code_t err_code;
    pm_peer_id_t peer_id;

    // Delete not old peers.
    peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
    while (peer_id != PM_PEER_ID_INVALID) {
        if (peer_id != m_device_connection.peer_ids[0] && peer_id != m_device_connection.peer_ids[1] && peer_id != m_device_connection.peer_ids[2]) {
            pm_peer_delete(peer_id);
        }

        peer_id = pm_next_peer_id_get(peer_id);
    }
}

static void set_whitelist() {
    ret_code_t err_code;
    pm_peer_id_t peer_id = m_device_connection.peer_ids[m_device_connection.current_device];

    if (peer_id != PM_PEER_ID_INVALID) {
        err_code = pm_whitelist_set(&peer_id, 1);
        APP_ERROR_CHECK(err_code);
    } else {
        err_code = pm_whitelist_set(NULL, 1);
        APP_ERROR_CHECK(err_code);
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

static void hids_send_report(hid_report_t *p_report) {
    ret_code_t err_code;

    if (m_conn_handle != BLE_CONN_HANDLE_INVALID) {
        if (p_report != NULL && m_hid_buffer.count < HID_REPORT_BUFFER_NUM) {
            memcpy(&m_hid_buffer.reports[m_hid_buffer.end], p_report, sizeof(hid_report_t));
            m_hid_buffer.count++;
            m_hid_buffer.end++;

            if (m_hid_buffer.end >= HID_REPORT_BUFFER_NUM) {
                m_hid_buffer.end = 0;
            }
        }

        while (m_hid_buffer.count > 0) {
            err_code = NRF_SUCCESS;
            hid_report_type_t report_type = m_hid_buffer.reports[m_hid_buffer.start].type;

            if (m_hids_in_boot_mode) {
                if (report_type == HID_TYPE_KB_REPORT) {
                    err_code = ble_hids_boot_kb_inp_rep_send(&m_hids, KB_INPUT_REPORT_MAX_LEN, (uint8_t *)&m_hid_buffer.reports[m_hid_buffer.start].data.kb, m_conn_handle);
                } else if (report_type == HID_TYPE_CC_REPORT) {
                    err_code = NRF_ERROR_RESOURCES;
                }
            } else if (report_type == HID_TYPE_KB_REPORT) {
                err_code = ble_hids_inp_rep_send(&m_hids, KB_INPUT_REPORT_INDEX, KB_INPUT_REPORT_MAX_LEN, (uint8_t *)&m_hid_buffer.reports[m_hid_buffer.start].data.kb, m_conn_handle);
            } else if (report_type == HID_TYPE_CC_REPORT) {
                err_code = ble_hids_inp_rep_send(&m_hids, CC_INPUT_REPORT_INDEX, CC_INPUT_REPORT_MAX_LEN, (uint8_t *)&m_hid_buffer.reports[m_hid_buffer.start].data.cc, m_conn_handle);
            }

            NRF_LOG_INFO("HIDs report; ret: 0x%X.", err_code);

            if (err_code != NRF_ERROR_RESOURCES) {
                m_hid_buffer.count--;
                m_hid_buffer.start++;

                if (m_hid_buffer.start >= HID_REPORT_BUFFER_NUM) {
                    m_hid_buffer.start = 0;
                }
            } else if (err_code == NRF_ERROR_RESOURCES) {
                break;
            }

            NRF_LOG_INFO("HIDs report queue: %i", m_hid_buffer.count);

            if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE && err_code != NRF_ERROR_RESOURCES && err_code != NRF_ERROR_BUSY && err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING && err_code != NRF_ERROR_FORBIDDEN) {
                APP_ERROR_CHECK(err_code);
            }
        }
    }
}

#ifdef HAS_SLAVE
void db_discovery_init(void) {
    ret_code_t err_code;
    ble_db_discovery_init_t init;

    init.evt_handler = db_disc_handler;
    init.p_gatt_queue = &m_ble_gatt_queue;

    err_code = ble_db_discovery_init(&init);
    APP_ERROR_CHECK(err_code);
}

static void db_disc_handler(ble_db_discovery_evt_t *p_evt) {
    kb_link_c_on_db_disc_evt(&m_kb_link_c, p_evt);
}

static void kbl_c_init(void) {
    ret_code_t err_code;
    kb_link_c_init_t init;

    init.evt_handler = kbl_c_evt_handler;

    err_code = kb_link_c_init(&m_kb_link_c, &init);
    APP_ERROR_CHECK(err_code);
}

static void kbl_c_evt_handler(kb_link_c_t *p_kb_link_c, kb_link_c_evt_t const * p_evt) {
    ret_code_t err_code;

    switch (p_evt->evt_type) {
        case KB_LINK_C_EVT_DISCOVERY_COMPLETE:
            NRF_LOG_INFO("KB link discovery complete.");

            err_code = kb_link_c_handles_assign(p_kb_link_c, p_evt->conn_handle, &p_evt->handles);
            APP_ERROR_CHECK(err_code);

            NRF_LOG_INFO("Enable notification.");

            err_code = kb_link_c_key_index_notif_enable(p_kb_link_c);
            APP_ERROR_CHECK(err_code);
            break;

        case KB_LINK_C_EVT_KEY_INDEX_UPDATE:
            NRF_LOG_INFO("Receive notification from KB link; len: %d.", p_evt->len);

            app_sched_event_put(p_evt->p_data, p_evt->len, process_slave_key_index_task);
            break;

        case KB_LINK_C_EVT_DISCONNECTED:
            NRF_LOG_INFO("KB link disconnected.");

            // Clear all keys that have been registered by slave.
            app_sched_event_put(NULL, 0, clear_slave_key_index_task);

            // Scan for slave will start automatically.
            break;
    }
}

static void scan_init(void) {
    ret_code_t err_code;
    nrf_ble_scan_init_t init = {0};
    ble_gap_scan_params_t scan_params = {0};
    ble_uuid_t scan_uuid = {SLAVE_UUID, BLE_UUID_TYPE_VENDOR_BEGIN};
    ble_gap_conn_params_t conn_params = {
        .min_conn_interval = SLAVE_MIN_CONN_INTERVAL,
        .max_conn_interval = SLAVE_MAX_CONN_INTERVAL,
        .slave_latency = SLAVE_LATENCY,
        .conn_sup_timeout = CONN_SUP_TIMEOUT
    };

    scan_params.scan_phys = BLE_GAP_PHY_AUTO;
    scan_params.interval = SCAN_INTERVAL;
    scan_params.window = SCAN_WINDOW;
    scan_params.timeout = SCAN_DURATION;

    init.connect_if_match = true;
    init.conn_cfg_tag = APP_BLE_CONN_CFG_TAG;
    init.p_scan_param = &scan_params;
    init.p_conn_param = &conn_params;

    err_code = nrf_ble_scan_init(&m_scan, &init, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filter_set(&m_scan, SCAN_UUID_FILTER, &scan_uuid);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filters_enable(&m_scan, NRF_BLE_SCAN_UUID_FILTER, true);
    APP_ERROR_CHECK(err_code);
}

static void scan_start(void) {
    NRF_LOG_INFO("scan_start.");

    ret_code_t err_code;

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);
}
#endif

/*
 * Firmware section.
 */
static void firmware_init(void) {
    NRF_LOG_INFO("firmware_init.");

    // Init m_keys array.
    memset(&m_keys, 0, sizeof(m_keys));

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
    bool has_key_press = false;
    bool has_key_release = false;

    for (int col = 0; col < MATRIX_COL_NUM; col++) {
        nrf_gpio_pin_set(COLS[col]);
        nrf_delay_us(100);

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
                        // On key press.
                        m_key_pressed[row][col] = true;
                        m_debounce[row][col] = KEY_RELEASE_DEBOUNCE;

                        has_key_press = true;
                        update_key_index(MATRIX[row][col], SOURCE);
                    } else {
                        // On key release.
                        m_key_pressed[row][col] = false;
                        m_debounce[row][col] = KEY_PRESS_DEBOUNCE;

                        has_key_release = true;
                        update_key_index(-MATRIX[row][col], SOURCE);
                    }
                } else {
                    m_debounce[row][col] -= SCAN_DELAY;
                }
            }
        }

        nrf_gpio_pin_clear(COLS[col]);
    }

    if (has_key_press) {
        // If has key press, translate it first.
        put_translate_key_index_task();
    } else if (has_key_release) {
        // If has only key release, just sent it to device.
        put_generate_hid_report_task();
    }

    if (has_key_press || has_key_release) {
        m_low_power_mode_counter = LOW_POWER_MODE_DELAY;
    } else {
        m_low_power_mode_counter -= SCAN_DELAY;
    }

    if (m_low_power_mode_counter <= 0) {
        m_low_power_mode_counter = LOW_POWER_MODE_DELAY;
        low_power_mode_start();
    }
}

static bool update_key_index(int8_t index, uint8_t source) {
    key_t key = {0};

    key.index = index;
    key.source = source;

    if (m_key_count < KEY_NUM && key.index > 0) {
        m_keys[m_key_count++] = key;
    } else if (m_key_count > 0 && key.index < 0) {
        int i = 0;
        key.index = -key.index;

        while (i < m_key_count) {
            while (!(m_keys[i].index == key.index && m_keys[i].source == key.source) && i < m_key_count) {
                i++;
            }

            if (i < m_key_count) {
                for (int j = i; j < m_key_count - 1; j++) {
                    m_keys[j] = m_keys[j + 1];
                }

                m_key_count--;
            }
        }
    }
}

static void put_translate_key_index_task(void) {
    ret_code_t err_code;

    if (!m_translate_key_index_task_queued) {
        err_code = app_sched_event_put(NULL, 0, translate_key_index_task);
        APP_ERROR_CHECK(err_code);

        m_translate_key_index_task_queued = true;
    }
}

static void translate_key_index_task(void *p_data, uint16_t size) {
    UNUSED_PARAMETER(p_data);
    UNUSED_PARAMETER(size);

    m_translate_key_index_task_queued = false;

    ret_code_t err_code;
    uint8_t layer = _BASE_LAYER;

    for (int i = 0; i < m_key_count; i++) {
        if (m_keys[i].type != KEY_TYPE_NOT_TRANSLATED) {
            continue;
        }

        int8_t index = m_keys[i].index - 1;
        uint32_t code = KEYMAP[layer][index];

        if (IS_LAYER(code)) {
            layer = LAYER(code);
            continue;
        }

        if (code == KC_TRANSPARENT) {
            m_keys[i].type = KEY_TYPE_NO_REPORT;
            uint8_t temp_layer = layer;

            while (temp_layer >= 0 && KEYMAP[temp_layer][index] == KC_TRANSPARENT) {
                temp_layer--;
            }

            if (temp_layer < 0) {
                continue;
            } else {
                code = KEYMAP[temp_layer][index];
            }
        }

        if (IS_MOD(code)) {
            m_keys[i].type = KEY_TYPE_MODIFIER;
            m_keys[i].data.kb.modifiers = MOD_BIT(code);

            code = MOD_CODE(code);
        }

        if (IS_KEY(code)) {
            if (m_keys[i].type == KEY_TYPE_MODIFIER) {
                m_keys[i].type = KEY_TYPE_KEY_WITH_MODIFIER;
            } else {
                m_keys[i].type = KEY_TYPE_KEY;
            }

            m_keys[i].data.kb.key = code;
            continue;
        }

        if (IS_CONSUMER(code)) {
            m_keys[i].type = KEY_TYPE_CONSUMER;
            m_keys[i].data.cc = CONSUMER_CODE(code);
        }

        if (IS_DEVICE_CONNECTION(code)) {
            NRF_LOG_INFO("Device connection.");

            if (IS_DEVICE_SWITCHING(code)) {
                uint8_t device = DEVICE(code);

                NRF_LOG_INFO("Switching to device %u.", device);

                if (device != m_device_connection.current_device) {
                    m_device_connection.current_device = device;

                    m_reset_device_connection_update = true;
                    err_code = fds_record_update(&m_device_connection_record_desc, &m_device_connection_record);
                    APP_ERROR_CHECK(err_code);
                } else {
                    reset_device();
                }
            }

            if (IS_DEVICE_CONNECT(code)) {
                NRF_LOG_INFO("Reconnect device.");

                uint8_t bytes_available;
                uint8_t new_addr;

                // Generate new unique address for current device.
                do {
                    err_code = sd_rand_application_bytes_available_get(&bytes_available);
                    APP_ERROR_CHECK(err_code);

                    while (bytes_available < 1) {
                        nrf_delay_ms(OPERATION_DELAY);

                        err_code = sd_rand_application_bytes_available_get(&bytes_available);
                        APP_ERROR_CHECK(err_code);
                    }

                    err_code = sd_rand_application_vector_get(&new_addr, 1);
                    APP_ERROR_CHECK(err_code);
                } while (new_addr == m_device_connection.addrs[0] || new_addr == m_device_connection.addrs[1] || new_addr == m_device_connection.addrs[2]); // To ensure new unique address.

                // Save the generated address.
                m_device_connection.addrs[m_device_connection.current_device] = new_addr;

                // Reset peer id for current device.
                m_device_connection.peer_ids[m_device_connection.current_device] = PM_PEER_ID_INVALID;

                m_reset_device_connection_update = true;
                err_code = fds_record_update(&m_device_connection_record_desc, &m_device_connection_record);
                APP_ERROR_CHECK(err_code);
            }
        }
    }

    // Schedule hid report task.
    put_generate_hid_report_task();
}

static void put_generate_hid_report_task(void) {
    ret_code_t err_code;

    if (!m_generate_hid_report_task_queued) {
        err_code = app_sched_event_put(NULL, 0, generate_hid_report_task);
        APP_ERROR_CHECK(err_code);

        m_generate_hid_report_task_queued = true;
    }
}

static void generate_hid_report_task(void *p_data, uint16_t size) {
    UNUSED_PARAMETER(p_data);
    UNUSED_PARAMETER(size);

    m_generate_hid_report_task_queued = false;

    static bool empty_kb_report_sent = true;
    int kb_report_index = 2;
    hid_report_t kb_report = {0};
    kb_report.type = HID_TYPE_KB_REPORT;

    static bool empty_cc_report_sent = true;
    hid_report_t cc_report = {0};
    cc_report.type = HID_TYPE_CC_REPORT;

    for (int i = 0; i < m_key_count; i++) {
        if (m_keys[i].type == KEY_TYPE_NOT_TRANSLATED || m_keys[i].type == KEY_TYPE_NO_REPORT) {
            continue;
        }

        if (m_keys[i].type == KEY_TYPE_MODIFIER || m_keys[i].type == KEY_TYPE_KEY_WITH_MODIFIER) {
            kb_report.data.kb[0] |= m_keys[i].data.kb.modifiers;
        }

        if ((m_keys[i].type == KEY_TYPE_KEY || m_keys[i].type == KEY_TYPE_KEY_WITH_MODIFIER) && kb_report_index < KB_INPUT_REPORT_MAX_LEN) {
            kb_report.data.kb[kb_report_index++] = m_keys[i].data.kb.key;
        }

        if (m_keys[i].type == KEY_TYPE_CONSUMER) {
            cc_report.data.cc = m_keys[i].data.cc;
        }
    }

    bool is_empty_kb_report = kb_report.data.kb[0] == 0 && kb_report_index == 2;

    if (!empty_kb_report_sent || !is_empty_kb_report) {
        empty_kb_report_sent = is_empty_kb_report;

        NRF_LOG_INFO("generate_hid_report_task; kb len: %d", kb_report_index - 2);

        hids_send_report(&kb_report);
    }

    bool is_empty_cc_report = cc_report.data.cc == 0;

    if (!empty_cc_report_sent || !is_empty_cc_report) {
        empty_cc_report_sent = is_empty_cc_report;

        NRF_LOG_INFO("generate_hid_report_task; cc len: %d", is_empty_cc_report ? 0 : 1);

        hids_send_report(&cc_report);
    }
}

#ifdef HAS_SLAVE
static void process_slave_key_index_task(void *p_data, uint16_t size) {
    int8_t *key_index = (int8_t *)p_data;

    for (int i = 0; i < size; i++) {
        NRF_LOG_INFO("process_slave_key_index_task; key: %i.", key_index[i]);

        update_key_index(key_index[i], SOURCE_SLAVE);
    }

    put_translate_key_index_task();
}

static void clear_slave_key_index_task(void *p_data, uint16_t size) {
    UNUSED_PARAMETER(p_data);
    UNUSED_PARAMETER(size);

    NRF_LOG_INFO("clear_slave_key_index_task.");

    int i = 0;

    while (i < m_key_count) {
        while (m_keys[i].source != SOURCE_SLAVE && i < m_key_count) {
            i++;
        }

        if (i < m_key_count) {
            for (int j = i; j < m_key_count - 1; j++) {
                m_keys[j] = m_keys[j + 1];
            }

            m_key_count--;
        }
    }

    // Only remove keys, so no translation needed.
    put_generate_hid_report_task();
}
#endif
