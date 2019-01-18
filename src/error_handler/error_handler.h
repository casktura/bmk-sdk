/*
 * Error handlers header file.
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

#ifndef _ERROR_HANDLER_H_
#define _ERROR_HANDLER_H_

#include "app_error.h"
#include "nrf_assert.h"

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name);
void adv_error_handler(ret_code_t err_code);
void conn_params_error_handler(ret_code_t err_code);
void hid_error_handler(ret_code_t err_code);

#endif
