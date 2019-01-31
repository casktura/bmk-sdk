#ifndef _SHARED_H_
#define _SHARED_H_

/*
 * nRF52 section.
 */
void conn_params_init(void);
void gap_params_init(void);
void idle_state_handle(void);
void log_init(void);
void power_management_init(void);
void scheduler_init(void);

/*
 * Firmware section.
 */
void pins_init(void);

#endif
