#pragma once
// display.h - OLED display management for the Zigbee Alarm Clock
// Hardware: SSD1306 1.3" 128x64 I2C on GPIO6 (SDA) / GPIO7 (SCL)

#include <stdint.h>
#include <stdbool.h>

// I2C pins
#define DISPLAY_SDA_PIN 6
#define DISPLAY_SCL_PIN 7

// SSD1306 I2C address (standard for 1.3" modules)
#define DISPLAY_I2C_ADDR 0x3C

// Display dimensions
#define DISPLAY_WIDTH  128
#define DISPLAY_HEIGHT  64

// Brightness levels (0-255)
#define BRIGHTNESS_NORMAL 255
#define BRIGHTNESS_DIM     10

// Night mode hours (dim display to save power and avoid disturbing sleep)
#define NIGHT_HOUR_START 22
#define NIGHT_HOUR_END    7

/**
 * Initialize the SSD1306 display.
 * @return true if display found and initialized, false otherwise.
 */
bool display_init(void);

/**
 * Update the normal clock display.
 * Shows time (large), date, day of week, next alarm, WiFi and alarm status.
 *
 * @param hour          Current hour (0-23)
 * @param minute        Current minute (0-59)
 * @param second        Current second (0-59)
 * @param day_of_month  Day (1-31)
 * @param month         Month (1-12)
 * @param day_of_week   Day of week (1=Monday, 7=Sunday)
 * @param next_alarm    Next alarm string "HH:MM" or "--:--"
 * @param wifi_ok       true if WiFi is connected
 * @param alarm_enabled true if master alarm switch is on
 */
void display_update_clock(
    uint8_t hour, uint8_t minute, uint8_t second,
    uint8_t day_of_month, uint8_t month, uint8_t day_of_week,
    const char* next_alarm, bool wifi_ok, bool alarm_enabled);

/**
 * Show the flashing alarm screen (alternates inverted/normal every 500ms).
 * Call from main loop while alarm is active.
 */
void display_update_alarm(void);

/**
 * Show a startup / boot splash screen.
 */
void display_show_splash(void);

/**
 * Adjust display brightness based on time of day.
 * Dims automatically between NIGHT_HOUR_START and NIGHT_HOUR_END.
 *
 * @param hour Current hour (0-23)
 */
void display_set_brightness_auto(uint8_t hour);

/**
 * Explicitly set display brightness.
 * @param brightness 0-255
 */
void display_set_brightness(uint8_t brightness);
