#ifndef _FIRMWARE_CONFIG_H_
#define _FIRMWARE_CONFIG_H_

#include "app_timer.h"
#include "app_util.h"

// BLE parameters.
#define APP_BLE_OBSERVER_PRIO 3 // Application's BLE observer priority. You shouldn't need to modify this value.
#define APP_BLE_CONN_CFG_TAG  1 // A tag identifying the SoftDevice BLE configuration.

// GATT Queue parameters.
#define NRF_BLE_GQ_QUEUE_SIZE 4

// GAP parameters.
#define SLAVE_LATENCY    4                                // Slave latency.
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(2000, UNIT_10_MS)  // Connection supervisory timeout (2000 ms).

// Let's try to use prime numbers for con interval.
// For master.
#define MASTER_MIN_CONN_INTERVAL MSEC_TO_UNITS(7.5, UNIT_1_25_MS) // Minimum connection interval for master part.
#define MASTER_MAX_CONN_INTERVAL MSEC_TO_UNITS(20, UNIT_1_25_MS)  // Maximum connection interval for master part.
// For slave.
#define SLAVE_MIN_CONN_INTERVAL  MSEC_TO_UNITS(7.5, UNIT_1_25_MS) // Minimum connection interval for slave part.
#define SLAVE_MAX_CONN_INTERVAL  MSEC_TO_UNITS(20, UNIT_1_25_MS)  // Maximum connection interval for slave part.

// Advertising parameters.
// For master.
#define MASTER_ADV_FAST_INTERVAL MSEC_TO_UNITS(25, UNIT_0_625_MS)  // Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.).
#define MASTER_ADV_FAST_DURATION MSEC_TO_UNITS(60000, UNIT_10_MS)  // The advertising duration of fast advertising in units of 10 milliseconds.
#define MASTER_ADV_SLOW_INTERVAL MSEC_TO_UNITS(100, UNIT_0_625_MS) // Slow advertising interval (in units of 0.625 ms. This value corresponds to 100 ms).
#define MASTER_ADV_SLOW_DURATION MSEC_TO_UNITS(60000, UNIT_10_MS)  // The advertising duration of slow advertising in units of 10 milliseconds.

// For slave.
#define SLAVE_ADV_FAST_INTERVAL MSEC_TO_UNITS(25, UNIT_0_625_MS) // Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.).
#define SLAVE_ADV_FAST_DURATION MSEC_TO_UNITS(60000, UNIT_10_MS) // The advertising duration of fast advertising in units of 10 milliseconds.

// Scanning parameters to scan for slave.
#define SCAN_INTERVAL MSEC_TO_UNITS(50, UNIT_0_625_MS) // 50 ms.
#define SCAN_WINDOW   MSEC_TO_UNITS(30, UNIT_0_625_MS) // 30 ms.
#define SCAN_DURATION MSEC_TO_UNITS(60000, UNIT_10_MS) // 30 seconds.

// Connection parameters.
#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(3000)  // Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (3 seconds).
#define NEXT_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(10000) // Time between each call to sd_ble_gap_conn_param_update after the first call (10 seconds).
#define MAX_CONN_PARAMS_UPDATE_COUNT   5                      // Number of attempts before giving up the connection parameter negotiation.

// Peer manager parameters.
#define SEC_PARAM_BOND            1                    // Perform bonding.
#define SEC_PARAM_MITM            0                    // Man In The Middle protection not required.
#define SEC_PARAM_LESC            0                    // LE Secure Connections not enabled.
#define SEC_PARAM_KEYPRESS        0                    // Keypress notifications not enabled.
#define SEC_PARAM_IO_CAPABILITIES BLE_GAP_IO_CAPS_NONE // No I/O capabilities.
#define SEC_PARAM_OOB             0                    // Out Of Band data not available.
#define SEC_PARAM_MIN_KEY_SIZE    7                    // Minimum encryption key size.
#define SEC_PARAM_MAX_KEY_SIZE    16                   // Maximum encryption key size.

// HID report parameters.
#define INPUT_REPORT_NUM 2 // Keyboard report and Consumer Control report.

// Keyboard input report.
#define KB_INPUT_REPORT_INDEX   0 // Index of Keyboard Input Report.
#define KB_INPUT_REPORT_ID      1 // Report Id in HID descriptor.
#define KB_INPUT_REPORT_MAX_LEN 8 // Maximum length of the Keyboard Input Report characteristic.

// Consumer Control input report.
#define CC_INPUT_REPORT_INDEX   1
#define CC_INPUT_REPORT_ID      2
#define CC_INPUT_REPORT_MAX_LEN sizeof(uint16_t)

// Output (LEDs) report.
#define OUTPUT_REPORT_INDEX              0    // Index of Output Report.
#define OUTPUT_REPORT_ID                 1    // Report Id in HID descriptor.
#define OUTPUT_REPORT_MAX_LEN            1    // Maximum length of Output Report.
#define OUTPUT_REPORT_BIT_MASK_CAPS_LOCK 0x02 // CAPS LOCK bit in Output Report (based on 'LED Page (0x08)' of the Universal Serial Bus HID Usage Tables).

// HID parameters.
#define BASE_USB_HID_SPEC_VERSION 0x0101 // Version number of base USB HID Specification implemented by this application.

#define DEAD_BEEF 0xDEADBEEF // Value used as error code on stack dump, can be used to identify stack location on stack unwind.

// Scheduler parameters.
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE // Maximum size of scheduler events.
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE 20 // Maximum number of events in the scheduler queue. More is needed in case of Serialization.
#else
#define SCHED_QUEUE_SIZE 10 // Maximum number of events in the scheduler queue.
#endif

// Devices connection parameters.
#define CONFIG_FILE_ID        0x41C6
#define DEVICE_CONNECTION_KEY 0x4816

// Firmware parameters.
#define KEY_NUM               (MASTER_KEY_NUM + SLAVE_KEY_NUM)
#define MASTER_KEY_NUM        10
#define SLAVE_KEY_NUM         10
#define HID_REPORT_BUFFER_NUM 5

#define PIN_SET_DELAY        50 // In us (micro seconds), 50us should be enough.
#define SCAN_DELAY           3
#define SCAN_DELAY_TICKS     APP_TIMER_TICKS(SCAN_DELAY)
#define KEY_PRESS_DEBOUNCE   5
#define KEY_RELEASE_DEBOUNCE 19
#define OPERATION_DELAY      1 // In ms, 1ms should be enough.
#define LOW_POWER_MODE_DELAY 3000 // In ms.

#endif
