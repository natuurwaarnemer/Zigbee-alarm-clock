#pragma once
// alarm.h - Alarm management for the Zigbee Alarm Clock
// Hardware: ESP32-C6 + SSD1306 OLED + Piezo buzzer + Button

#include <stdint.h>
#include <stdbool.h>

// Number of alarms (one per day of the week)
#define ALARM_COUNT 7

// Day-of-week constants (ISO 8601: 1=Monday, 7=Sunday)
#define DOW_MONDAY    1
#define DOW_TUESDAY   2
#define DOW_WEDNESDAY 3
#define DOW_THURSDAY  4
#define DOW_FRIDAY    5
#define DOW_SATURDAY  6
#define DOW_SUNDAY    7

// Maximum buzzer volume level (0-100)
#define BUZZER_MAX_VOLUME 100

// Buzzer GPIO pin
#define BUZZER_PIN 8

// Button GPIO pin (active LOW with internal pull-up)
#define BUTTON_PIN 9

// Alarm fade-in: volume increases by this amount each beep cycle
#define BUZZER_FADE_STEP 10

// Alarm configuration for a single day
typedef struct {
    uint8_t  hour;      // 0-23
    uint8_t  minute;    // 0-59
    bool     enabled;   // alarm active for this day
} alarm_config_t;

// Runtime alarm state
typedef struct {
    bool     active;            // alarm is currently sounding
    uint8_t  current_volume;    // current buzzer volume (0-100)
    uint32_t triggered_at_ms;   // millis() when alarm started
} alarm_state_t;

// NVS (Non-Volatile Storage) namespace key
#define NVS_NAMESPACE "alarm_clock"

/**
 * Initialize alarm module.
 * Loads saved alarm configs from NVS and sets up buzzer PWM.
 */
void alarm_init(void);

/**
 * Check if any alarm should trigger at the given time.
 * Should be called once per minute.
 *
 * @param hour    Current hour (0-23)
 * @param minute  Current minute (0-59)
 * @param dow     Day of week (1=Monday, 7=Sunday)
 */
void alarm_check(uint8_t hour, uint8_t minute, uint8_t dow);

/**
 * Start the alarm sound (called when alarm triggers).
 * Begins fade-in buzzer sequence.
 */
void alarm_start(void);

/**
 * Stop the alarm sound (called on button press or HA command).
 */
void alarm_stop(void);

/**
 * Update alarm buzzer (call from main loop).
 * Handles beep on/off timing and volume fade-in.
 */
void alarm_update(void);

/**
 * Get alarm configuration for a specific day.
 *
 * @param dow  Day of week (1=Monday, 7=Sunday)
 * @return     Pointer to alarm configuration
 */
const alarm_config_t* alarm_get_config(uint8_t dow);

/**
 * Set alarm configuration for a specific day.
 * Saves to NVS automatically.
 *
 * @param dow     Day of week (1=Monday, 7=Sunday)
 * @param hour    Alarm hour (0-23)
 * @param minute  Alarm minute (0-59)
 * @param enabled Whether alarm is enabled
 */
void alarm_set_config(uint8_t dow, uint8_t hour, uint8_t minute, bool enabled);

/**
 * Check if alarm is currently active (sounding).
 */
bool alarm_is_active(void);

/**
 * Get time string for the next scheduled alarm.
 * Format: "HH:MM" or "--:--" if no alarm scheduled.
 *
 * @param buf    Output buffer (minimum 6 bytes)
 * @param hour   Current hour
 * @param minute Current minute
 * @param dow    Current day of week (1=Monday, 7=Sunday)
 */
void alarm_get_next_str(char* buf, uint8_t hour, uint8_t minute, uint8_t dow);

/**
 * Load alarm configurations from NVS (flash storage).
 */
void alarm_load_from_nvs(void);

/**
 * Save alarm configurations to NVS (flash storage).
 */
void alarm_save_to_nvs(void);
