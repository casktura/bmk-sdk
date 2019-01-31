#include "error_handler.h"

#include "nrf_log.h"

#include "../firmware_config.h"

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {
    NRF_LOG_INFO("Assert nrf callback.");

    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

void adv_error_handler(ret_code_t err_code) {
    NRF_LOG_INFO("ADV error.");

    APP_ERROR_HANDLER(err_code);
}

void conn_params_error_handler(ret_code_t err_code) {
    NRF_LOG_INFO("Conn params error.");

    APP_ERROR_HANDLER(err_code);
}

void hid_error_handler(ret_code_t err_code) {
    NRF_LOG_INFO("HID error.");

    APP_ERROR_HANDLER(err_code);
}
