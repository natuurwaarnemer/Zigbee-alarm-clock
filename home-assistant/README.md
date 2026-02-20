# Home Assistant Configuratie - Zigbee Wekker

## Overzicht

Deze map bevat de Home Assistant configuratie voor de Zigbee Wekker:

- `configuration.yaml` - Helpers (input_datetime, input_boolean)
- `automations.yaml` - Automatiseringen voor alarm sync en acties
- `dashboard.yaml` - Lovelace dashboard kaart

## Installatie

### Stap 1: Helpers toevoegen

Voeg de inhoud van `configuration.yaml` toe aan je Home Assistant
`configuration.yaml` (of aan een apart bestand via `!include`).

Herstart Home Assistant na het toevoegen van de helpers.

**Via de UI (aanbevolen):**
1. Ga naar Instellingen → Apparaten & Services → Helpers
2. Klik "+ Maak Helper"
3. Maak alle `input_datetime` en `input_boolean` helpers aan zoals beschreven

### Stap 2: Apparaat toevoegen (ESPHome)

1. Zorg dat de wekker is geflasht en verbonden met WiFi
2. Home Assistant detecteert het apparaat automatisch via ESPHome integratie
3. Ga naar Instellingen → Apparaten & Services → ESPHome
4. Accepteer het nieuwe apparaat "Zigbee Wekker"

### Stap 3: Automatiseringen toevoegen

**Via de UI:**
1. Ga naar Instellingen → Automatiseringen
2. Importeer `automations.yaml` of maak handmatig aan

**Via YAML:**
Voeg inhoud van `automations.yaml` toe aan je `automations.yaml`

### Stap 4: Dashboard toevoegen

1. Ga naar je gewenste dashboard
2. Klik het bewerkingspictogram (potlood)
3. Klik "Kaart toevoegen" → "Handmatig"
4. Plak de inhoud van `dashboard.yaml`

## Entiteiten

Na installatie zijn deze entiteiten beschikbaar:

### Van de wekker (ESPHome)
| Entiteit | Type | Beschrijving |
|----------|------|--------------|
| `binary_sensor.zigbee_wekker_wifi_verbonden` | Sensor | WiFi status |
| `sensor.zigbee_wekker_volgend_alarm` | Sensor | Volgende alarm tijd |
| `sensor.zigbee_wekker_wifi_signaalsterkte` | Sensor | WiFi signaal (dBm) |
| `switch.zigbee_wekker_alarm_hoofdschakelaar` | Schakelaar | Master aan/uit |
| `switch.zigbee_wekker_alarm_maandag_aan` | Schakelaar | Maandag alarm |
| ... | ... | Herhaal voor alle 7 dagen |
| `number.zigbee_wekker_alarm_maandag_uur` | Nummer | Alarm uur |
| `number.zigbee_wekker_alarm_maandag_minuut` | Nummer | Alarm minuut |
| ... | ... | Herhaal voor alle 7 dagen |

### HA Helpers (aangemaakt via configuration.yaml)
| Entiteit | Type | Beschrijving |
|----------|------|--------------|
| `input_datetime.alarm_maandag` | Tijd invoer | Alternatief tijdsinvoer |
| `input_boolean.alarm_maandag_aan` | Schakelaar | Alternatief schakelaar |
| `input_boolean.alarm_hoofdschakelaar` | Schakelaar | Hoofd schakelaar HA kant |

## Troubleshooting

**Apparaat verschijnt niet:**
- Controleer WiFi verbinding
- Herstart de ESPHome integratie
- Check de logs: Instellingen → Systeem → Logboek

**Alarm synchroniseert niet:**
- Check dat de automatisering actief is
- Verifieer entiteitsnamen in de automatisering
- Bekijk trace in automatiseringseditor
