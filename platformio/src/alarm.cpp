// alarm.cpp - Alarm management implementation
// Hardware: ESP32-C6 + Piezo buzzer on GPIO8 + Button on GPIO9

#include "alarm.h"
#include <Arduino.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <driver/ledc.h>
#include <string.h>
#include <stdio.h>

// ---- Internal state ----
static alarm_config_t s_alarms[ALARM_COUNT];
static alarm_state_t  s_state = {false, 0, 0};

// Buzzer timing state (managed in alarm_update)
static uint32_t s_buzzer_next_event_ms = 0;
static bool     s_buzzer_on            = false;
static uint8_t  s_beep_phase           = 0; // 0..3 for beep-pause pattern

// LEDC channel for buzzer PWM
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER   LEDC_TIMER_0
#define LEDC_FREQ_HZ 1000
#define LEDC_RES     LEDC_TIMER_10_BIT  // 0-1023

// ---- Internal helpers ----

static void buzzer_set_volume(uint8_t volume_pct) {
    // Convert 0-100% to 0-1023 duty (10-bit resolution)
    uint32_t duty = (volume_pct * 1023) / 100;
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL);
}

static void buzzer_off(void) {
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL, 0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL);
}

// ---- Public API ----

void alarm_init(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Configure button GPIO with internal pull-up
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Configure buzzer PWM via LEDC
    ledc_timer_config_t timer_cfg = {
        .speed_mode      = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_RES,
        .timer_num       = LEDC_TIMER,
        .freq_hz         = LEDC_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_cfg);

    ledc_channel_config_t ch_cfg = {
        .gpio_num   = BUZZER_PIN,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel    = LEDC_CHANNEL,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER,
        .duty       = 0,
        .hpoint     = 0,
    };
    ledc_channel_config(&ch_cfg);

    // Load saved alarm configs
    alarm_load_from_nvs();
}

void alarm_check(uint8_t hour, uint8_t minute, uint8_t dow) {
    if (s_state.active) return; // Already sounding

    // dow is 1=Monday..7=Sunday, maps to index 0..6
    uint8_t idx = (dow - 1) % ALARM_COUNT;
    if (s_alarms[idx].enabled &&
        s_alarms[idx].hour   == hour &&
        s_alarms[idx].minute == minute) {
        alarm_start();
    }
}

void alarm_start(void) {
    s_state.active           = true;
    s_state.current_volume   = 0;
    s_state.triggered_at_ms  = millis();
    s_beep_phase             = 0;
    s_buzzer_on              = false;
    s_buzzer_next_event_ms   = millis(); // Start immediately
}

void alarm_stop(void) {
    s_state.active         = false;
    s_state.current_volume = 0;
    s_buzzer_on            = false;
    buzzer_off();
}

void alarm_update(void) {
    if (!s_state.active) return;

    uint32_t now_ms = millis();
    if (now_ms < s_buzzer_next_event_ms) return;

    // Pattern: beep-beep pause (300ms on, 200ms off, 300ms on, 1200ms pause)
    switch (s_beep_phase) {
        case 0: // First beep ON
            if (s_state.current_volume < BUZZER_MAX_VOLUME) {
                s_state.current_volume = (uint8_t)
                    MIN(s_state.current_volume + BUZZER_FADE_STEP, BUZZER_MAX_VOLUME);
            }
            buzzer_set_volume(s_state.current_volume);
            s_buzzer_next_event_ms = now_ms + 300;
            s_beep_phase = 1;
            break;
        case 1: // First beep OFF
            buzzer_off();
            s_buzzer_next_event_ms = now_ms + 200;
            s_beep_phase = 2;
            break;
        case 2: // Second beep ON
            buzzer_set_volume(s_state.current_volume);
            s_buzzer_next_event_ms = now_ms + 300;
            s_beep_phase = 3;
            break;
        case 3: // Pause
            buzzer_off();
            s_buzzer_next_event_ms = now_ms + 1200;
            s_beep_phase = 0;
            break;
    }
}

const alarm_config_t* alarm_get_config(uint8_t dow) {
    uint8_t idx = (dow - 1) % ALARM_COUNT;
    return &s_alarms[idx];
}

void alarm_set_config(uint8_t dow, uint8_t hour, uint8_t minute, bool enabled) {
    uint8_t idx = (dow - 1) % ALARM_COUNT;
    s_alarms[idx].hour    = hour;
    s_alarms[idx].minute  = minute;
    s_alarms[idx].enabled = enabled;
    alarm_save_to_nvs();
}

bool alarm_is_active(void) {
    return s_state.active;
}

void alarm_get_next_str(char* buf, uint8_t hour, uint8_t minute, uint8_t dow) {
    int current_minutes = hour * 60 + minute;
    int best_ahead      = 10081; // > 7 days in minutes
    int best_h = -1, best_m = -1;

    for (int i = 0; i < ALARM_COUNT; i++) {
        if (!s_alarms[i].enabled) continue;

        int alarm_dow  = i + 1; // 1=Monday..7=Sunday
        int alarm_mins = s_alarms[i].hour * 60 + s_alarms[i].minute;
        int days_ahead = (alarm_dow - dow + 7) % 7;
        int mins_ahead = days_ahead * 1440 + alarm_mins - current_minutes;
        if (mins_ahead <= 0) mins_ahead += 7 * 1440;

        if (mins_ahead < best_ahead) {
            best_ahead = mins_ahead;
            best_h = s_alarms[i].hour;
            best_m = s_alarms[i].minute;
        }
    }

    if (best_h >= 0) {
        snprintf(buf, 6, "%02d:%02d", best_h, best_m);
    } else {
        snprintf(buf, 6, "--:--");
    }
}

void alarm_load_from_nvs(void) {
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        // No saved data - use defaults (all disabled, 07:00)
        for (int i = 0; i < ALARM_COUNT; i++) {
            s_alarms[i].hour    = 7;
            s_alarms[i].minute  = 0;
            s_alarms[i].enabled = false;
        }
        return;
    }

    for (int i = 0; i < ALARM_COUNT; i++) {
        char key[16];

        snprintf(key, sizeof(key), "alarm_%d_h", i);
        uint8_t h = 7;
        nvs_get_u8(handle, key, &h);
        s_alarms[i].hour = h;

        snprintf(key, sizeof(key), "alarm_%d_m", i);
        uint8_t m = 0;
        nvs_get_u8(handle, key, &m);
        s_alarms[i].minute = m;

        snprintf(key, sizeof(key), "alarm_%d_en", i);
        uint8_t en = 0;
        nvs_get_u8(handle, key, &en);
        s_alarms[i].enabled = (en != 0);
    }

    nvs_close(handle);
}

void alarm_save_to_nvs(void) {
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle) != ESP_OK) return;

    for (int i = 0; i < ALARM_COUNT; i++) {
        char key[16];

        snprintf(key, sizeof(key), "alarm_%d_h", i);
        nvs_set_u8(handle, key, s_alarms[i].hour);

        snprintf(key, sizeof(key), "alarm_%d_m", i);
        nvs_set_u8(handle, key, s_alarms[i].minute);

        snprintf(key, sizeof(key), "alarm_%d_en", i);
        nvs_set_u8(handle, key, s_alarms[i].enabled ? 1 : 0);
    }

    nvs_commit(handle);
    nvs_close(handle);
}
