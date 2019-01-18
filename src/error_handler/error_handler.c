/*
 * Error handlers.
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
