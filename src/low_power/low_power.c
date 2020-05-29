#include "low_power.h"

#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrfx_gpiote.h"

#include "../config/keyboard.h"
#include "../firmware_config.h"

static const app_timer_id_t *m_p_scan_timer_id;

static void (*m_scan_timeout_handler)(void *);

static void gpiote_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void low_power_mode_init(const app_timer_id_t *p_scan_timer_id, void (*scan_timeout_handler)(void *)) {
    ret_code_t err_code;

    NRF_LOG_INFO("low_power_mode_init.");

    m_p_scan_timer_id = p_scan_timer_id;
    m_scan_timeout_handler = scan_timeout_handler;

    // Init GPIOTE module.
    if (!nrfx_gpiote_is_init()) {
        err_code = nrfx_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }

    // Init GPIOTE pins.
    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_RAW_CONFIG_IN_SENSE_LOTOHI(false);

    for (int i = 0; i < MATRIX_ROW_NUM; i++) {
        err_code = nrfx_gpiote_in_init(ROWS[i], &config, gpiote_evt_handler);
        APP_ERROR_CHECK(err_code);
    }
}

static void gpiote_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    ret_code_t err_code;

    NRF_LOG_INFO("GPIOTE evt.");

    for (int i = 0; i < MATRIX_ROW_NUM; i++) {
        nrfx_gpiote_in_event_disable(ROWS[i]);
    }

    for (int i = 0; i < MATRIX_COL_NUM; i++) {
        nrf_gpio_pin_clear(COLS[i]);
    }

    // Scan matrix.
    m_scan_timeout_handler(NULL);

    // Start scan timer.
    err_code = app_timer_start(*m_p_scan_timer_id, SCAN_DELAY_TICKS, NULL);
    APP_ERROR_CHECK(err_code);
}

void low_power_mode_start() {
    ret_code_t err_code;

    NRF_LOG_INFO("low_power_mode_start.");

    err_code = app_timer_stop(*m_p_scan_timer_id);
    APP_ERROR_CHECK(err_code);

    for (int i = 0; i < MATRIX_ROW_NUM; i++) {
        nrfx_gpiote_in_event_enable(ROWS[i], true);
    }

    for (int i = 0; i < MATRIX_COL_NUM; i++) {
        nrf_gpio_pin_set(COLS[i]);
    }
}
