// display.cpp - OLED display implementation using Adafruit SSD1306
// Hardware: SSD1306 1.3" 128x64 I2C on SDA=GPIO6, SCL=GPIO7

#include "display.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <stdio.h>

// ---- Static display instance ----
static Adafruit_SSD1306 s_display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1);
static bool s_initialized = false;

// Day-of-week abbreviations (Dutch, 1=Monday..7=Sunday)
static const char* DAY_NAMES[] = {
    "Ma", "Di", "Wo", "Do", "Vr", "Za", "Zo"
};

// ---- Public API ----

bool display_init(void) {
    Wire.begin(DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);

    if (!s_display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDR)) {
        // Display not found - continue without it
        return false;
    }

    s_display.clearDisplay();
    s_display.setTextColor(SSD1306_WHITE);
    s_display.cp437(true);
    s_initialized = true;

    display_show_splash();
    return true;
}

void display_update_clock(
    uint8_t hour, uint8_t minute, uint8_t second,
    uint8_t day_of_month, uint8_t month, uint8_t day_of_week,
    const char* next_alarm, bool wifi_ok, bool alarm_enabled)
{
    if (!s_initialized) return;

    s_display.clearDisplay();

    // ---- Large time (HH:MM) centered at top ----
    char time_buf[6];
    snprintf(time_buf, sizeof(time_buf), "%02d:%02d", hour, minute);
    s_display.setTextSize(3);  // ~18x24px per character, 5 chars = 90px wide
    s_display.setCursor((DISPLAY_WIDTH - (5 * 18)) / 2, 0);
    s_display.print(time_buf);

    // ---- Date and day of week ----
    char date_buf[16];
    uint8_t dow_idx = (day_of_week - 1) % 7;
    snprintf(date_buf, sizeof(date_buf), "%s %02d-%02d",
             DAY_NAMES[dow_idx], day_of_month, month);
    s_display.setTextSize(1);
    s_display.setCursor(0, 42);
    s_display.print(date_buf);

    // ---- Next alarm ----
    char alarm_buf[12];
    snprintf(alarm_buf, sizeof(alarm_buf), "A:%s", next_alarm);
    s_display.setCursor(0, 54);
    s_display.print(alarm_buf);

    // ---- WiFi status (top right, small) ----
    s_display.setTextSize(1);
    if (wifi_ok) {
        s_display.setCursor(DISPLAY_WIDTH - 6, 0);
        s_display.print("W");
    } else {
        s_display.setCursor(DISPLAY_WIDTH - 6, 0);
        s_display.print("!");
    }

    // ---- Alarm master status (below WiFi icon) ----
    if (alarm_enabled) {
        s_display.setCursor(DISPLAY_WIDTH - 12, 10);
        s_display.print("ON");
    } else {
        s_display.setCursor(DISPLAY_WIDTH - 18, 10);
        s_display.print("off");
    }

    s_display.display();
}

void display_update_alarm(void) {
    if (!s_initialized) return;

    // Toggle inversion every call (called ~2x/sec from main loop)
    static bool inverted = false;
    inverted = !inverted;

    s_display.clearDisplay();
    if (inverted) {
        s_display.fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, SSD1306_WHITE);
        s_display.setTextColor(SSD1306_BLACK);
    } else {
        s_display.setTextColor(SSD1306_WHITE);
    }

    s_display.setTextSize(2);
    s_display.setCursor(20, 10);
    s_display.print("ALARM!");

    s_display.setTextSize(1);
    s_display.setCursor(12, 42);
    s_display.print("Druk op de knop");

    // Restore default text color
    s_display.setTextColor(SSD1306_WHITE);
    s_display.display();
}

void display_show_splash(void) {
    if (!s_initialized) return;

    s_display.clearDisplay();
    s_display.setTextSize(1);
    s_display.setCursor(10, 20);
    s_display.print("Zigbee Wekker");
    s_display.setCursor(20, 35);
    s_display.print("Bezig...");
    s_display.display();
    delay(1500);
}

void display_set_brightness_auto(uint8_t hour) {
    bool night = (hour >= NIGHT_HOUR_START || hour < NIGHT_HOUR_END);
    display_set_brightness(night ? BRIGHTNESS_DIM : BRIGHTNESS_NORMAL);
}

void display_set_brightness(uint8_t brightness) {
    if (!s_initialized) return;
    s_display.ssd1306_command(SSD1306_SETCONTRAST);
    s_display.ssd1306_command(brightness);
}
