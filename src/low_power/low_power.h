#ifndef _LOW_POWER_H_
#define _LOW_POWER_H_

#include "app_timer.h"

void low_power_mode_init(const app_timer_id_t *p_scan_timer_id, void (*scan_timeout_handler)(void *));
void low_power_mode_start();

#endif
