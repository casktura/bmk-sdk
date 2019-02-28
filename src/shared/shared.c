#include "shared.h"

#include "app_scheduler.h"
#include "ble_conn_params.h"
#include "nrf_gpio.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"

#include "../config/keyboard.h"
#include "../error_handler/error_handler.h"
#include "../firmware_config.h"

/*
 * nRF52 section.
 */
void conn_params_init(void) {
    ret_code_t err_code;
    ble_conn_params_init_t cp_init = {0};

    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = NULL;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

void conn_evt_length_ext_init(void) {
    ret_code_t err_code;
    ble_opt_t ble_opt = {0};

    ble_opt.common_opt.conn_evt_ext.enable = 1;

    err_code = sd_ble_opt_set(BLE_COMMON_OPT_CONN_EVT_EXT, &ble_opt);
    APP_ERROR_CHECK(err_code);
}

void gap_params_init(void) {
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_HID);
    APP_ERROR_CHECK(err_code);

#ifdef MASTER
    gap_conn_params.min_conn_interval = MASTER_MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MASTER_MAX_CONN_INTERVAL;
#endif
#ifdef SLAVE
    gap_conn_params.min_conn_interval = SLAVE_MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = SLAVE_MAX_CONN_INTERVAL;
#endif
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

void idle_state_handle(void) {
    app_sched_execute();

    if (NRF_LOG_PROCESS() == false) {
        nrf_pwr_mgmt_run();
    }
}

void log_init(void) {
    ret_code_t err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}
void power_management_init(void) {
    ret_code_t err_code;

    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

void scheduler_init(void) {
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/*
 * Firmware section.
 */
void pins_init(void) {
    NRF_LOG_INFO("pins_init.");

    for (int i = 0; i < MATRIX_COL_NUM; i++) {
        nrf_gpio_cfg_output(COLS[i]);
        nrf_gpio_pin_clear(COLS[i]);
    }

    for (int i = 0; i < MATRIX_ROW_NUM; i++) {
        nrf_gpio_cfg_input(ROWS[i], NRF_GPIO_PIN_PULLDOWN);
    }
}
