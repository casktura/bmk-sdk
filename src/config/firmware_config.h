#ifndef _FIRMWARE_CONFIG_H_
#define _FIRMWARE_CONFIG_H_

#include "app_util.h"

// BLE parameters.
#define APP_BLE_OBSERVER_PRIO 3 // Application's BLE observer priority. You shouldn't need to modify this value.
#define APP_BLE_CONN_CFG_TAG  1 // A tag identifying the SoftDevice BLE configuration.

// GAP parameters.
#define MIN_CONN_INTERVAL MSEC_TO_UNITS(7.5, UNIT_1_25_MS) // Minimum connection interval (7.5 ms).
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(30, UNIT_1_25_MS)  // Maximum connection interval (30 ms).
#define SLAVE_LATENCY     6                                // Slave latency.
#define CONN_SUP_TIMEOUT  MSEC_TO_UNITS(430, UNIT_10_MS)   // Connection supervisory timeout (430 ms).

// Advertising parameters.
#define APP_ADV_FAST_INTERVAL 0x0028 // Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.).
#define APP_ADV_SLOW_INTERVAL 0x0C80 // Slow advertising interval (in units of 0.625 ms. This value corresponds to 2 seconds).
#define APP_ADV_FAST_DURATION 3000   // The advertising duration of fast advertising in units of 10 milliseconds.
#define APP_ADV_SLOW_DURATION 18000  // The advertising duration of slow advertising in units of 10 milliseconds.

// Connection parameters.
#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000)  // Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds).
#define NEXT_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(30000) // Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds).
#define MAX_CONN_PARAMS_UPDATE_COUNT   3                      // Number of attempts before giving up the connection parameter negotiation.

// Peer manager parameters.
#define SEC_PARAM_BOND 1                               // Perform bonding.
#define SEC_PARAM_MITM 0                               // Man In The Middle protection not required.
#define SEC_PARAM_LESC 0                               // LE Secure Connections not enabled.
#define SEC_PARAM_KEYPRESS 0                           // Keypress notifications not enabled.
#define SEC_PARAM_IO_CAPABILITIES BLE_GAP_IO_CAPS_NONE // No I/O capabilities.
#define SEC_PARAM_OOB 0                                // Out Of Band data not available.
#define SEC_PARAM_MIN_KEY_SIZE 7                       // Minimum encryption key size.
#define SEC_PARAM_MAX_KEY_SIZE 16                      // Maximum encryption key size.

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
#define SCHED_QUEUE_SIZE          20 // Maximum number of events in the scheduler queue. More is needed in case of Serialization.
#else
#define SCHED_QUEUE_SIZE          10 // Maximum number of events in the scheduler queue.
#endif

// Firmware parameter
#define KEY_NUM              20
#define SCAN_DELAY           8
#define SCAN_DELAY_TICKS     APP_TIMER_TICKS(SCAN_DELAY)
#define KEY_PRESS_DEBOUNCE   10
#define KEY_RELEASE_DEBOUNCE 15

#endif
