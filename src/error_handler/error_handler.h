#ifndef _ERROR_HANDLER_H_
#define _ERROR_HANDLER_H_

#include "app_error.h"
#include "nrf_assert.h"

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name);
void adv_error_handler(ret_code_t err_code);
void conn_params_error_handler(ret_code_t err_code);
void hid_error_handler(ret_code_t err_code);

#endif
