# Zigbee Wekker voor Home Assistant

Een slimme wekker gemaakt met ESP32-C6 die integreert met Home Assistant via WiFi (met Zigbee upgrade pad).

## Features

- 🕐 7 instelbare alarmen (één per dag van de week)
- 📱 Besturing via Home Assistant dashboard
- 🔊 Piëzo buzzer voor alarm met geleidelijk toenemend volume
- 🔘 Fysieke knop om alarm uit te zetten
- 💡 Triggert automatisch acties in Home Assistant (bijv. licht aan)
- 📟 OLED display (1.3", 128x64) toont tijd, datum en volgend alarm
- 📶 WiFi verbinding via ESPHome (Zigbee upgrade mogelijk)
- 🌙 Weergavehelderheid aanpasbaar (dim 's nachts)
- 🗓️ Weekend/werkdag modus

## Benodigdheden

| Component | Type | Opmerkingen |
|-----------|------|-------------|
| Microcontroller | ESP32-C6 (Seeed Studio) | Met ingebouwde WiFi + Zigbee |
| Display | OLED 1.3" 128x64 I2C | SSD1306 driver, blauw |
| Buzzer | Piezo buzzer (Grove/Actief) | 3-5V |
| Knop | Drukknop 19mm (DE-GQ19H-10) | Normaal open |
| Voeding | USB-C 5V adapter | Minimaal 1A |

Zie [hardware/components.md](hardware/components.md) voor de volledige lijst met bestelnummers.

## Snelstart

1. Sluit hardware aan volgens [hardware/wiring-diagram.md](hardware/wiring-diagram.md)
2. Volg [docs/SETUP.md](docs/SETUP.md) voor installatie en configuratie
3. Voeg Home Assistant helpers toe via [home-assistant/configuration.yaml](home-assistant/configuration.yaml)
4. Importeer het dashboard via [home-assistant/dashboard.yaml](home-assistant/dashboard.yaml)
5. (Optioneel) Print de behuizing via [3d-models/](3d-models/)

## Repository Structuur

```
Zigbee-alarm-clock/
├── esphome/
│   ├── zigbee-alarm-clock.yaml     # Primaire ESPHome configuratie (WiFi)
│   └── secrets.yaml.example        # Sjabloon voor WiFi/API gegevens
├── platformio/                     # Alternatief: native Zigbee (gevorderd)
│   ├── platformio.ini
│   ├── src/
│   │   ├── main.cpp
│   │   ├── display.cpp/h
│   │   ├── zigbee.cpp/h
│   │   └── alarm.cpp/h
│   └── README.md
├── home-assistant/
│   ├── configuration.yaml          # Helpers definitie
│   ├── automations.yaml            # Voorbeeldautomatiseringen
│   ├── dashboard.yaml              # Dashboard kaart
│   └── README.md
├── hardware/
│   ├── wiring-diagram.md           # Gedetailleerde aansluitgids
│   ├── components.md               # Onderdelen lijst (BOM)
│   └── photos/
├── 3d-models/
│   ├── case-v1.scad                # OpenSCAD bronbestand
│   └── README.md
├── docs/
│   ├── SETUP.md                    # Stap-voor-stap installatie
│   ├── TROUBLESHOOTING.md          # Veelvoorkomende problemen
│   └── ARCHITECTURE.md             # Systeemontwerp (Engels)
├── .gitignore
└── LICENSE
```

## Schermafdrukken / Foto's

> 📸 Foto's worden toegevoegd zodra de hardware is geassembleerd.
> Zie [hardware/photos/](hardware/photos/) voor placeholders.

## Technische Details

### Pinnen Configuratie

| Functie | GPIO |
|---------|------|
| OLED SDA | GPIO6 |
| OLED SCL | GPIO7 |
| Buzzer (+) | GPIO8 |
| Knop | GPIO9 |

### Stroomverbruik

| Toestand | Stroom |
|----------|--------|
| Standby | ~80 mA |
| Display actief | ~100 mA |
| Alarm actief | ~120 mA |
| WiFi piek | ~150 mA |

## Zigbee Upgrade

Dit project gebruikt momenteel WiFi via ESPHome. De ESP32-C6 heeft ingebouwde
Zigbee hardware. Zodra ESPHome native Zigbee ondersteunt, is een upgrade mogelijk
zonder hardware wijzigingen. De `platformio/` map bevat een Zigbee implementatie
als referentie voor gevorderde gebruikers.

Zie [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) voor het migratiepad.

## Bekende Beperkingen

- ESPHome heeft nog geen native Zigbee ondersteuning voor ESP32-C6
- De buzzer vereist een actieve buzzer of passieve piëzo (zie BOM)
- OTA updates vereisen WiFi verbinding
- NTP tijd synchronisatie vereist internetverbinding

## Bijdragen

Pull requests zijn welkom! Zie [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) voor
de systeemarchitectuur en bijdragenrichtlijnen.

## Changelog

### v1.0.0 (2026)
- Initiële release
- ESPHome WiFi implementatie
- 7 instelbare alarmen
- OLED display
- Home Assistant integratie
- PlatformIO Zigbee referentie implementatie
- 3D printbare behuizing

## Licentie

MIT License - zie [LICENSE](LICENSE) voor details.

Copyright (c) 2026 Ruud Hogenberg
