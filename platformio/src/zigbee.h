#pragma once
// zigbee.h - Zigbee device definition and communication
// Uses ESP-Zigbee-SDK (ESP-IDF component) for ESP32-C6 native Zigbee
//
// Device type: Zigbee End Device (ZED)
// Clusters implemented:
//   - Basic cluster (0x0000)
//   - Time cluster (0x000A) - receive time sync from coordinator
//   - Custom alarm cluster (0xFF00) - alarm configuration from HA
//   - On/Off cluster (0x0006) - button state to HA
//   - Binary Input cluster (0x000F) - alarm active status

#include <stdint.h>
#include <stdbool.h>

// Zigbee endpoint
#define ZB_ALARM_ENDPOINT 10

// Custom manufacturer cluster for alarm configuration
// Range 0xFC00-0xFFFF is manufacturer specific
#define ZB_CLUSTER_ALARM_CONFIG 0xFF00

// Attribute IDs within the custom alarm cluster
#define ZB_ATTR_ALARM_DAY      0x0001  // uint8  - day of week (1-7)
#define ZB_ATTR_ALARM_HOUR     0x0002  // uint8  - alarm hour
#define ZB_ATTR_ALARM_MINUTE   0x0003  // uint8  - alarm minute
#define ZB_ATTR_ALARM_ENABLED  0x0004  // bool   - alarm enabled
#define ZB_ATTR_MASTER_SWITCH  0x0005  // bool   - master alarm switch

// Status cluster attribute IDs
#define ZB_ATTR_ALARM_ACTIVE   0x0001  // bool - alarm currently sounding

/**
 * Initialize Zigbee stack and register device clusters.
 * Must be called before any other zigbee_* function.
 */
void zigbee_init(void);

/**
 * Start Zigbee network joining process.
 * Blocks until joined or timeout (~30 seconds).
 * @return true if successfully joined network.
 */
bool zigbee_start(void);

/**
 * Process Zigbee stack events.
 * Must be called from the main loop.
 */
void zigbee_update(void);

/**
 * Send button press event to Zigbee coordinator.
 * Maps to On/Off cluster toggle command.
 */
void zigbee_send_button_press(void);

/**
 * Send alarm active status to coordinator.
 * @param active true if alarm is sounding
 */
void zigbee_send_alarm_status(bool active);

/**
 * Check if Zigbee network is connected (joined).
 * @return true if joined and connected.
 */
bool zigbee_is_connected(void);

/**
 * Callback type for receiving alarm configuration from coordinator.
 * Implement in main.cpp to update alarm settings.
 *
 * @param day     Day of week (1=Monday, 7=Sunday)
 * @param hour    Alarm hour (0-23)
 * @param minute  Alarm minute (0-59)
 * @param enabled Whether alarm is enabled
 */
typedef void (*zigbee_alarm_config_cb_t)(uint8_t day, uint8_t hour,
                                          uint8_t minute, bool enabled);

/**
 * Register callback for received alarm configuration.
 * @param cb Callback function pointer
 */
void zigbee_set_alarm_config_callback(zigbee_alarm_config_cb_t cb);
