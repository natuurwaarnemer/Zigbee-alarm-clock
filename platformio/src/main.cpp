// main.cpp - Zigbee Alarm Clock - Main entry point
// Hardware: ESP32-C6 (Seeed Studio)
//   - OLED SSD1306 1.3" I2C:  SDA=GPIO6, SCL=GPIO7
//   - Piezo Buzzer (active):  GPIO8
//   - Button (momentary NO):  GPIO9 (internal pull-up, active LOW)
//   - Power: USB-C 5V

#include <Arduino.h>
#include <time.h>
#include "display.h"
#include "alarm.h"
#include "zigbee.h"

// ---- Configuration ----
#define BUTTON_DEBOUNCE_MS 50

// ---- State ----
static bool     s_button_last_state = HIGH;
static uint32_t s_button_changed_ms = 0;
static uint32_t s_last_alarm_check_ms = 0;
static uint32_t s_last_display_update_ms = 0;
static bool     s_master_alarm_enabled = true;

// ---- Prototypes ----
static void handle_button(void);
static void sync_time_from_zigbee(void);
static void on_alarm_config_received(uint8_t day, uint8_t hour,
                                     uint8_t minute, bool enabled);

// ============================================================
// Arduino setup
// ============================================================
void setup() {
    Serial.begin(115200);
    Serial.println("[Boot] Zigbee Alarm Clock starting...");

    // Initialize display first (shows splash screen)
    if (!display_init()) {
        Serial.println("[Display] WARNING: SSD1306 not found at 0x3C");
    }

    // Initialize alarm module (loads NVS config, sets up PWM)
    alarm_init();

    // Configure button with internal pull-up
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Register Zigbee alarm config callback
    zigbee_set_alarm_config_callback(on_alarm_config_received);

    // Initialize and start Zigbee
    zigbee_init();
    if (!zigbee_start()) {
        Serial.println("[Zigbee] Failed to start - check Zigbee coordinator");
    }

    Serial.println("[Boot] Setup complete");
}

// ============================================================
// Arduino main loop
// ============================================================
void loop() {
    uint32_t now_ms = millis();

    // Process Zigbee stack events (must be called every loop iteration)
    zigbee_update();

    // Handle button press with debouncing
    handle_button();

    // Update alarm buzzer state machine
    alarm_update();

    // Check alarms every minute (at second 0, approximately)
    if (now_ms - s_last_alarm_check_ms >= 60000UL) {
        s_last_alarm_check_ms = now_ms;

        if (s_master_alarm_enabled) {
            // Get current time from system clock (set via Zigbee time cluster)
            time_t t = time(nullptr);
            struct tm* tm_info = localtime(&t);
            if (tm_info && t > 1000000) { // Sanity check - time must be set
                // Convert tm_wday (0=Sunday) to 1=Monday..7=Sunday
                uint8_t dow = (tm_info->tm_wday == 0) ? 7 : tm_info->tm_wday;
                alarm_check(tm_info->tm_hour, tm_info->tm_min, dow);
            }
        }
    }

    // Update display at 2 Hz (500ms interval)
    if (now_ms - s_last_display_update_ms >= 500UL) {
        s_last_display_update_ms = now_ms;

        if (alarm_is_active()) {
            display_update_alarm();
        } else {
            time_t t = time(nullptr);
            struct tm* tm_info = localtime(&t);
            bool time_valid = (t > 1000000);

            uint8_t hour = time_valid ? tm_info->tm_hour : 0;
            uint8_t min  = time_valid ? tm_info->tm_min  : 0;
            uint8_t sec  = time_valid ? tm_info->tm_sec  : 0;
            uint8_t mday = time_valid ? tm_info->tm_mday : 1;
            uint8_t mon  = time_valid ? (tm_info->tm_mon + 1) : 1;
            uint8_t dow  = time_valid ?
                ((tm_info->tm_wday == 0) ? 7 : tm_info->tm_wday) : 1;

            char next_alarm_buf[6];
            alarm_get_next_str(next_alarm_buf, hour, min, dow);

            display_set_brightness_auto(hour);
            display_update_clock(
                hour, min, sec, mday, mon, dow,
                next_alarm_buf,
                zigbee_is_connected(),
                s_master_alarm_enabled);
        }
    }
}

// ============================================================
// Button handling
// ============================================================
static void handle_button(void) {
    bool current = digitalRead(BUTTON_PIN);
    uint32_t now_ms = millis();

    if (current != s_button_last_state) {
        s_button_changed_ms = now_ms;
        s_button_last_state = current;
    }

    // Debounce: wait BUTTON_DEBOUNCE_MS ms after last change
    if ((now_ms - s_button_changed_ms) >= BUTTON_DEBOUNCE_MS && current == LOW) {
        // Button pressed (active LOW)
        if (alarm_is_active()) {
            alarm_stop();
            zigbee_send_alarm_status(false);
            Serial.println("[Button] Alarm stopped by button press");
        }
        // Always send button press event to coordinator
        zigbee_send_button_press();
        Serial.println("[Button] Button press sent to coordinator");

        // Wait for button release before next action
        while (digitalRead(BUTTON_PIN) == LOW) {
            zigbee_update();
            delay(10);
        }
    }
}

// ============================================================
// Callback: alarm configuration received via Zigbee
// ============================================================
static void on_alarm_config_received(uint8_t day, uint8_t hour,
                                     uint8_t minute, bool enabled) {
    Serial.printf("[Zigbee] Alarm config: day=%d %02d:%02d enabled=%d\n",
                  day, hour, minute, enabled);
    alarm_set_config(day, hour, minute, enabled);
}
