#ifndef _SHARED_H_
#define _SHARED_H_

/*
 * nRF52 section.
 */
void conn_params_init(void);
void conn_evt_length_ext_init(void);
void gap_params_init(void);
void idle_state_handle(void);
void log_init(void);
void power_management_init(void);
void scheduler_init(void);
//void shutdown_system(void);

/*
 * Firmware section.
 */
void pins_init(void);

#endif
