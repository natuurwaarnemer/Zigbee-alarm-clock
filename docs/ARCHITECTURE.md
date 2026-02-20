# System Architecture - Zigbee Alarm Clock

## Overview

The Zigbee Alarm Clock is a WiFi/Zigbee-enabled smart alarm clock built on the
ESP32-C6 microcontroller. It integrates with Home Assistant for alarm management
and automation.

```
┌─────────────────────────────────────────────────────────────────┐
│                      Home Assistant                             │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  ESPHome Integration   │  Input Helpers  │  Dashboard   │   │
│  └─────────────────────────────────────────────────────────┘   │
└──────────────────────────────┬──────────────────────────────────┘
                               │ WiFi (ESPHome native API)
                               │ or Zigbee (future / platformio/)
┌──────────────────────────────▼──────────────────────────────────┐
│                       ESP32-C6                                  │
│                                                                 │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────────┐  │
│  │  ESPHome     │  │  Alarm Logic │  │   Display Driver     │  │
│  │  Framework   │  │  (Globals +  │  │   (SSD1306 I2C)      │  │
│  │  (WiFi/API)  │  │   Scripts)   │  │   GPIO6/7            │  │
│  └──────────────┘  └──────────────┘  └──────────────────────┘  │
│         │                │                                      │
│  ┌──────▼──────┐  ┌──────▼──────┐  ┌──────────────────────┐   │
│  │  NTP / HA   │  │  PWM Buzzer │  │   Button Handler     │   │
│  │  Time Sync  │  │  (LEDC)     │  │   GPIO9 (pull-up)    │   │
│  └─────────────┘  │  GPIO8      │  └──────────────────────┘   │
│                   └─────────────┘                               │
└─────────────────────────────────────────────────────────────────┘
        │                    │                    │
   ┌────▼────┐          ┌────▼────┐          ┌────▼────┐
   │  OLED   │          │  Buzzer │          │  Button │
   │ 128x64  │          │ GPIO8   │          │ GPIO9   │
   └─────────┘          └─────────┘          └─────────┘
```

## Components

### 1. ESPHome Firmware (`esphome/`)

**Primary implementation** - recommended for most users.

| Component | Implementation |
|-----------|----------------|
| Time sync | `homeassistant` platform + SNTP fallback |
| Display | `ssd1306_i2c` platform, lambda rendering |
| Alarm logic | `script` components + `globals` |
| Button | `gpio` binary_sensor with debounce |
| Buzzer | `ledc` PWM output + manual control in scripts |
| WiFi | Native ESP32 WiFi + captive portal fallback |
| HA API | Native ESPHome API (encrypted) |

**Alarm checking:** A `script` runs every minute triggered by `on_time`.
It checks each day's alarm against current time and day-of-week. If a match
is found, the `alarm_start` script is invoked.

**Display rendering:** The display lambda runs every second. It reads globals
(`alarm_active`, `next_alarm_str`) and the HA time component to render the UI.
During an active alarm, the display blinks to attract attention.

**Fade-in buzzer:** The `alarm_beep_loop` script uses a global `buzzer_step`
(0-10) to gradually increase the PWM duty cycle over the first 10 beep cycles,
creating a gentle wake-up experience.

### 2. Home Assistant

**Alarm configuration storage:**
- `number` entities (hour/minute per day) - stored on the ESP32 in flash
- `switch` entities (enabled per day) - stored on the ESP32 in flash
- `input_datetime` / `input_boolean` helpers - optional HA-side storage

**Automations:**
- Sync helpers → device number/switch entities
- Trigger lights/scenes on button press event
- Send mobile notification on alarm trigger
- Vacation mode (disable all alarms)

**Dashboard:** Lovelace vertical-stack card with entities, buttons for test/stop.

### 3. PlatformIO / Native Zigbee (`platformio/`)

**Alternative implementation** for advanced users wanting native Zigbee.

| Module | File | Responsibility |
|--------|------|----------------|
| Main loop | `main.cpp` | Setup, loop, button, time integration |
| Display | `display.cpp/h` | SSD1306 Adafruit driver, all rendering |
| Alarm | `alarm.cpp/h` | Alarm logic, buzzer PWM, NVS storage |
| Zigbee | `zigbee.cpp/h` | ESP-Zigbee-SDK, clusters, HA communication |

**Zigbee device profile:** End Device (ZED) on HA profile.
Custom cluster `0xFF00` for alarm configuration attributes.
On/Off cluster for button events and master switch.

## Data Flow

### WiFi Version (ESPHome) - Normal Operation

```
HA Dashboard          ESPHome Device (ESP32-C6)         Hardware
────────────          ──────────────────────────         ────────
User sets alarm  ───► number.set_value (hour/min)
User enables day ───► switch.turn_on (day_en)
                       │
                       ▼ (every minute, on_time trigger)
                      check_alarms()
                       │ match found
                       ▼
                      alarm_start()  ──────────────────► Buzzer ON
                      alarm_active=true ──────────────► Display blinks
                       │
                       │ ◄────────────────────────────── Button pressed
                      alarm_stop()
                      alarm_active=false
                       │
                       ▼
                      homeassistant.event(button_pressed) ─► HA Automation
                                                              └─► Light ON
```

### Time Synchronization

```
internet/NTP                Home Assistant              ESP32-C6
────────────                ──────────────              ────────
NTP servers ─────────────► HA system time ────────────► homeassistant_time
                                                          (syncs on connect)
                                                          + SNTP fallback
```

## Storage

### On-Device (ESP32-C6 Flash)
- 7x alarm hour (0-23) - ESPHome: `restore_value: true` on number entities
- 7x alarm minute (0-59) - ESPHome: `restore_value: true`
- 7x alarm enabled - ESPHome: switch with `RESTORE_DEFAULT_OFF`
- Master switch state - ESPHome: switch with `RESTORE_DEFAULT_ON`

ESPHome uses the NVS (Non-Volatile Storage) partition automatically for
entities with `restore_value: true` or switches with `RESTORE_DEFAULT_*`.

### In Home Assistant
- `input_datetime` helpers (optional, for HA-side scheduling)
- `input_boolean` helpers (optional)
- Automation state

## Security

- ESPHome API uses AES-256-CBC encryption (`api_encryption_key`)
- OTA updates protected by password (`ota_password`)
- WiFi credentials stored in `secrets.yaml` (not committed to git)
- No external services used (all local)

## Future: Zigbee Migration Path

The ESP32-C6 has built-in Zigbee hardware (IEEE 802.15.4).
Currently ESPHome does not support native Zigbee for ESP32-C6.

**Migration steps when ESPHome Zigbee support matures:**
1. Replace `wifi:` block with `zigbee:` configuration
2. Replace `api:` (ESPHome API) with Zigbee cluster definitions
3. Update HA integration from ESPHome to ZHA/Zigbee2MQTT
4. No hardware changes required

**Current Zigbee implementation** (PlatformIO):
- Uses ESP-Zigbee-SDK directly (ESP-IDF component)
- Custom Zigbee cluster 0xFF00 for alarm configuration
- On/Off cluster for button events
- See `platformio/` for full implementation

## Known Limitations

1. **ESPHome Zigbee:** Not yet supported for ESP32-C6 in stable ESPHome
2. **OTA requires WiFi:** No air-gap updates possible in WiFi mode
3. **Single endpoint:** One alarm clock instance per ESPHome device
4. **Time zone:** Hardcoded to `Europe/Amsterdam` - change in yaml if needed
5. **Font rendering:** Requires internet connection at compile time for Google Fonts

## Performance

| Metric | Value | Notes |
|--------|-------|-------|
| Boot time | ~3s | ESP-IDF framework |
| Display update | 1s | ESPHome timer |
| Alarm check latency | ≤60s | Checked every minute at :00 |
| Button response | <100ms | GPIO interrupt + 20ms debounce |
| WiFi reconnect | ~5s | Auto reconnect |
| Memory usage | ~180KB | ESP-IDF heap |

## Contributing

1. Fork the repository
2. Create a feature branch
3. Follow existing code style (English comments, Dutch UI text)
4. Test on real hardware when possible
5. Submit a pull request with description of changes

### Code Style
- ESPHome YAML: comments in English, all user-facing text in Dutch
- C++ (PlatformIO): Google C++ style, 4-space indent, English comments
- Markdown docs: Dutch for user documentation, English for ARCHITECTURE.md
