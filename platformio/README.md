# PlatformIO - Native Zigbee Implementatie (Gevorderd)

Dit is de **alternatieve** implementatie van de Zigbee Wekker met native Zigbee
via de ESP-Zigbee-SDK. De primaire implementatie (via ESPHome + WiFi) staat in
de `esphome/` map.

## Wanneer deze versie gebruiken?

Gebruik de PlatformIO versie als je:
- Native Zigbee communicatie wilt (geen WiFi vereist)
- Meer controle wilt over de Zigbee device definitie
- Gevorderd bent met embedded programmeren
- Een Zigbee coordinator hebt (bijv. SMLIGHT SLZB-MR3 of Sonoff Zigbee Dongle)

## Vereisten

### Software
- [PlatformIO IDE](https://platformio.org/) (VS Code extensie aanbevolen)
- ESP-IDF v5.x (automatisch geïnstalleerd door PlatformIO)
- ESP-Zigbee-SDK (via ESP-IDF component manager)

### Hardware
- ESP32-C6 development board (Seeed Studio XIAO ESP32C6 of vergelijkbaar)
- Zigbee coordinator op hetzelfde netwerk als Home Assistant

## Installatie

### Stap 1: PlatformIO installeren

1. Installeer [VS Code](https://code.visualstudio.com/)
2. Installeer de PlatformIO extensie
3. Open de `platformio/` map als project

### Stap 2: ESP-Zigbee-SDK component toevoegen

Maak een `idf_component.yml` aan in de `platformio/` map:

```yaml
dependencies:
  espressif/esp-zigbee-sdk: ">=1.1.0"
```

Of voeg toe via ESP-IDF component manager:
```bash
cd platformio
idf.py add-dependency "espressif/esp-zigbee-sdk"
```

### Stap 3: sdkconfig aanpassen

Maak een `sdkconfig.defaults` bestand aan in de `platformio/` map:

```
CONFIG_ZB_ENABLED=y
CONFIG_ZB_ZED=y
CONFIG_ZB_RADIO_NATIVE=y
CONFIG_ZB_GP_ENABLED=n
CONFIG_FREERTOS_HZ=1000
```

### Stap 4: Compileren en uploaden

```bash
cd platformio
pio run --target upload
pio device monitor --baud 115200
```

## Zigbee Configuratie in Home Assistant

### MQTT/Zigbee2MQTT

Als je Zigbee2MQTT gebruikt, wordt het apparaat automatisch gedetecteerd als
een "Alarm Clock" device na pairen.

**Pairen:**
1. Zet het apparaat in pairmodus (eerste keer automatisch, daarna via serieel commando)
2. Open Zigbee2MQTT dashboard
3. Klik "Permit join"
4. Het apparaat verschijnt als "Zigbee Alarm Clock"

### ZHA (Zigbee Home Automation)

1. Ga naar Instellingen → Apparaten & Services → ZHA
2. Klik "Apparaat toevoegen"
3. Druk de resetknop op het ESP32-C6 board

## Codestructuur

```
platformio/src/
├── main.cpp      # Arduino setup/loop, button handling, time management
├── display.cpp   # SSD1306 OLED rendering
├── display.h     # Display API
├── alarm.cpp     # Alarm logic, NVS storage, buzzer PWM
├── alarm.h       # Alarm API
├── zigbee.cpp    # Zigbee stack, clusters, attribute handling
└── zigbee.h      # Zigbee API
```

## Bekende Beperkingen

- ESP-Zigbee-SDK is nog in actieve ontwikkeling - API kan veranderen
- Arduino framework heeft beperkte compatibiliteit met ESP-Zigbee-SDK
  (gebruik `espidf` framework voor productie)
- Geen OTA via Zigbee (gebruik seriële verbinding voor updates)
- Zigbee2MQTT heeft mogelijk custom converter nodig voor het custom cluster

## Referenties

- [ESP-Zigbee-SDK GitHub](https://github.com/espressif/esp-zigbee-sdk)
- [ESP-IDF Zigbee Examples](https://github.com/espressif/esp-idf/tree/master/examples/zigbee)
- [Zigbee2MQTT Custom Devices](https://www.zigbee2mqtt.io/advanced/support-new-devices/01_support_new_devices.html)
