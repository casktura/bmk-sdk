/*
 * This file contains most of parameters to configure the firmware.
 * Copyright (C) 2018 Kittipong Yothaithiang
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

#ifndef _FIRMWARE_CONFIG_H_
#define _FIRMWARE_CONFIG_H_

#include "app_util.h"

// BLE parameters.
#define APP_BLE_OBSERVER_PRIO 3 // Application's BLE observer priority. You shouldn't need to modify this value.
#define APP_BLE_CONN_CFG_TAG  1 // A tag identifying the SoftDevice BLE configuration.

// GAP parameters.
#define MIN_CONN_INTERVAL MSEC_TO_UNITS(20, UNIT_1_25_MS) // Minimum connection interval (20 ms).
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(45, UNIT_1_25_MS) // Maximum connection interval (45 ms).
#define SLAVE_LATENCY     3                               // Slave latency.
#define CONN_SUP_TIMEOUT  MSEC_TO_UNITS(2000, UNIT_10_MS) // Connection supervisory timeout (2000 ms).

// Advertising parameters.
#define APP_ADV_FAST_INTERVAL MSEC_TO_UNITS(25, UNIT_0_625_MS)   // Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.).
#define APP_ADV_SLOW_INTERVAL MSEC_TO_UNITS(1000, UNIT_0_625_MS) // Slow advertising interval (in units of 0.625 ms. This value corresponds to 2 seconds).
#define APP_ADV_FAST_DURATION MSEC_TO_UNITS(30000, UNIT_10_MS)   // The advertising duration of fast advertising in units of 10 milliseconds.
#define APP_ADV_SLOW_DURATION MSEC_TO_UNITS(30000, UNIT_10_MS)   // The advertising duration of slow advertising in units of 10 milliseconds.

// Scanning parameters.
#define SCAN_INTERVAL MSEC_TO_UNITS(50, UNIT_0_625_MS) // 50 ms.
#define SCAN_WINDOW   MSEC_TO_UNITS(30, UNIT_0_625_MS) // 30 ms.
#define SCAN_DURATION MSEC_TO_UNITS(30000, UNIT_10_MS) // 30 seconds.

// Connection parameters.
#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(3000)  // Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (2 seconds).
#define NEXT_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(10000) // Time between each call to sd_ble_gap_conn_param_update after the first call (10 seconds).
#define MAX_CONN_PARAMS_UPDATE_COUNT   3                      // Number of attempts before giving up the connection parameter negotiation.

// Peer manager parameters.
#define SEC_PARAM_BOND            1                    // Perform bonding.
#define SEC_PARAM_MITM            0                    // Man In The Middle protection not required.
#define SEC_PARAM_LESC            0                    // LE Secure Connections not enabled.
#define SEC_PARAM_KEYPRESS        0                    // Keypress notifications not enabled.
#define SEC_PARAM_IO_CAPABILITIES BLE_GAP_IO_CAPS_NONE // No I/O capabilities.
#define SEC_PARAM_OOB             0                    // Out Of Band data not available.
#define SEC_PARAM_MIN_KEY_SIZE    7                    // Minimum encryption key size.
#define SEC_PARAM_MAX_KEY_SIZE    16                   // Maximum encryption key size.

// HID report parameters
#define OUTPUT_REPORT_INDEX              0    // Index of Output Report.
#define OUTPUT_REPORT_MAX_LEN            1    // Maximum length of Output Report.
#define INPUT_REPORT_KEYS_INDEX          0    // Index of Input Report.
#define OUTPUT_REPORT_BIT_MASK_CAPS_LOCK 0x02 // CAPS LOCK bit in Output Report (based on 'LED Page (0x08)' of the Universal Serial Bus HID Usage Tables).
#define INPUT_REP_REF_ID                 0    // Id of reference to Keyboard Input Report.
#define OUTPUT_REP_REF_ID                0    // Id of reference to Keyboard Output Report.
#define FEATURE_REP_REF_ID               0    // ID of reference to Keyboard Feature Report.
#define FEATURE_REPORT_MAX_LEN           2    // Maximum length of Feature Report.
#define FEATURE_REPORT_INDEX             0    // Index of Feature Report.
#define INPUT_REPORT_KEYS_MAX_LEN        8    // Maximum length of the Input Report characteristic.

// HID parameters.
#define BASE_USB_HID_SPEC_VERSION 0x0101 // Version number of base USB HID Specification implemented by this application.

#define DEAD_BEEF 0xDEADBEEF // Value used as error code on stack dump, can be used to identify stack location on stack unwind.

// Scheduler parameters
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE // Maximum size of scheduler events.
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE 20 // Maximum number of events in the scheduler queue. More is needed in case of Serialization.
#else
#define SCHED_QUEUE_SIZE 10 // Maximum number of events in the scheduler queue.
#endif

// Firmware parameter
#define KEY_NUM       20
#define SLAVE_KEY_NUM 10

#define PIN_SET_DELAY        100 // In us (micro seconds), 100us should be enough.
#define SCAN_DELAY           8
#define SCAN_DELAY_TICKS     APP_TIMER_TICKS(SCAN_DELAY)
#define KEY_PRESS_DEBOUNCE   10
#define KEY_RELEASE_DEBOUNCE 15
#define REPORT_DELAY         25
#define REPORT_DELAY_TICKS   APP_TIMER_TICKS(REPORT_DELAY)

#endif
