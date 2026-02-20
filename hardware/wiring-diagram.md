# Aansluitschema - Zigbee Wekker

## Overzicht

Alle verbindingen zijn 3.3V logica. De ESP32-C6 wordt gevoed via USB-C (5V).
De interne 3.3V regelaar op het development board voorziet alle componenten.

## ⚠️ Belangrijk: D-nummers vs GPIO-nummers

De Seeed Studio XIAO ESP32-C6 heeft labels op de pinnen (D0-D10) die NIET overeenkomen met de GPIO nummers!

**Voorbeeld:**
- **D4** op het board = **GPIO22** in de code
- **D5** op het board = **GPIO23** in de code
- **D8** op het board = **GPIO19** in de code
- **D9** op het board = **GPIO20** in de code

In ESPHome configuratie gebruik je altijd de **GPIO nummers**, niet de D-nummers!

## Aansluittabel

| Component | Pin | XIAO ESP32-C6 Pin | GPIO | Notities |
|-----------|-----|-------------------|------|----------|
| OLED Display | VCC | 3.3V | - | Voeding |
| OLED Display | GND | GND | - | Ground |
| OLED Display | SDA | D4 | GPIO22 | I2C Data |
| OLED Display | SCL | D5 | GPIO23 | I2C Clock |
| Buzzer | + | D8 | GPIO19 | PWM output |
| Buzzer | - | GND | - | Ground |
| Button | Pin 1 | D9 | GPIO20 | Met interne pull-up |
| Button | Pin 2 | GND | - | Ground |
| ESP32-C6 | USB-C | 5V adapter | - | Voeding |

## ASCII Aansluitdiagram

```
                    ESP32-C6 (Seeed Studio XIAO C6)
                    ┌─────────────────────────────┐
               3.3V ┤ 3V3                     GND ├─── GND (gemeenschappelijk)
               GND  ┤ GND                     5V  ├─── USB-C ingang
             GPIO22 ┤ D4/GPIO22           GPIO0   ├
             GPIO23 ┤ D5/GPIO23           GPIO1   ├
             GPIO19 ┤ D8/GPIO19           GPIO2   ├
             GPIO20 ┤ D9/GPIO20           GPIO3   ├
                    │                             │
                    └─────────────────────────────┘
                           │     │    │    │
                         USB-C   │    │    │
                         5V IN   │    │    │
                                 │    │    │
                    ┌────────────┘    │    └─────────────────┐
                    │                │                       │
                    ▼                ▼                       ▼
             ┌──────────┐    ┌──────────────┐        ┌──────────┐
             │   OLED   │    │    Buzzer    │        │   Knop   │
             │ SSD1306  │    │  (Actief)    │        │ (Momenteel│
             │ 128x64   │    │              │        │ normaal   │
             │ I2C 0x3C │    │   + aan D8   │        │  open)    │
             │          │    │   (GPIO19)   │        │           │
             │VCC → 3.3V│    │   - aan GND  │        │Pin1→D9    │
             │GND → GND │    └──────────────┘        │  (GPIO20) │
             │SDA→ D4   │                            │Pin2→GND   │
             │  (GPIO22)│                            └──────────┘
             │SCL→ D5   │
             │  (GPIO23)│
             └──────────┘
```

## Gedetailleerde Stap-voor-Stap Aansluiting

### 1. OLED Display (SSD1306 1.3" I2C)

Het OLED display heeft 4 aansluitingen. Let op de volgorde op jouw module
(kan verschillen per fabrikant):

```
Typische pinout 1.3" OLED:
┌─────────────────────────┐
│  GND  VCC  SCL  SDA     │  ← pins aan onderzijde
└─────────────────────────┘
```

| Display Pin | ESP32-C6 | Kleur |
|-------------|----------|-------|
| GND | GND | Zwart |
| VCC | **3.3V** (niet 5V!) | Rood |
| SCL | D5 (GPIO23) | Geel |
| SDA | D4 (GPIO22) | Blauw |

⚠️ **Waarschuwing:** Sluit nooit 5V aan op de OLED VCC. Dit beschadigt het
display permanent!

### 2. Buzzer (Actieve Piezo Buzzer)

Een **actieve** buzzer maakt een toon zolang spanning aanwezig is.
Een **passieve** buzzer heeft PWM signaal nodig (ook ondersteund).

```
Buzzer aansluiting:
┌──────────┐
│  BUZZER  │
│   + (lang│───────── D8 (GPIO19)
│   - (kort│───────── GND
└──────────┘
```

| Buzzer Pin | ESP32-C6 |
|------------|----------|
| + (positief, langere pin) | D8 (GPIO19) |
| - (negatief, kortere pin) | GND |

💡 **Tip:** De meeste buzzer modules hebben een `+` markering. Verbind die
met D8 (GPIO19). Test zonder behuizing voor je alles samenbouwt.

### 3. Drukknop

Een momenteel normaal-open (NO) drukknop met 2 aansluitingen:

```
Knop aansluiting (2-draads):
┌──────────┐
│   KNOP   │
│  Pin 1   │───── D9 (GPIO20, interne pull-up)
│  Pin 2   │───── GND
└──────────┘
```

De interne pull-up weerstand is ingeschakeld in software. De knop trekt GPIO20
naar GND bij indrukken.

### 4. Voeding

Gebruik een betrouwbare USB-C adapter van minimaal 1A (5V).

```
USB-C Adapter
     │
     ▼
ESP32-C6 USB-C poort
     │
     ├─── 3.3V intern geregeld → OLED VCC
     └─── GND gemeenschappelijk → OLED GND, Buzzer -, Knop Pin 2
```

## Componentspecificaties

### ESP32-C6 (Seeed Studio XIAO ESP32C6)
- Spanning: 3.3V logica, 5V voeding via USB-C
- WiFi: 802.11 b/g/n (2.4 GHz)
- Zigbee: IEEE 802.15.4 (ingebouwd)
- GPIO: 3.3V logica, max 12 mA per pin
- Afmetingen: 21 x 17.5 mm

### OLED Display (SSD1306)
- Voedingsspanning: 3.3V (sommige modules 3.3-5V)
- Communicatie: I2C, adres 0x3C (standaard)
- Resolutie: 128x64 pixels
- Afmetingen display: ~35 x 28 mm (module ~38 x 38 mm)
- Stroomverbruik: ~20 mA bij vol wit scherm

### Buzzer (Actieve Piezo)
- Type: Actieve buzzer (ingebouwde oscillator)
- Spanning: 3.3V - 5V
- Stroom: ~30 mA
- Frequentie: 2300 Hz (typisch voor actieve buzzers)
- Of: Passieve piezo - gecontroleerd via PWM

### Drukknop (DE-GQ19H-10)
- Type: Momenteel normaal-open (NO)
- Diameter: 19 mm
- Schakelspanning: max 250V AC / 30V DC
- Stroom: max 3A
- Merk: ook compatibele 12 mm knoppen werken

## Veiligheidsnotities

> ⚠️ **Controleer polariteit** voor je de voeding aansluit!

1. **OLED op 3.3V**: Sluit nooit 5V aan op de OLED VCC-pin.
2. **GPIO max stroom**: Max 12 mA per GPIO-pin. Buzzer ~30 mA kan direct worden
   aangesloten via D8/GPIO19 (LEDC PWM regelt de duty cycle).
3. **ESD bescherming**: Raak de GPIO-pinnen niet aan met statisch geladen vingers.
4. **Test voor behuizing**: Test alle verbindingen voor je alles in de behuizing bouwt.
5. **Voedingspolariteit**: USB-C is polariteitsbestendig.
6. **Kortsluiting voorkomen**: Gebruik behuizing of isolatietape voor onbedekte aansluitingen.

## Verificatie

### I2C Scan (controleer OLED adres)
Gebruik de ESPHome logs of deze seriële commando's om te verifiëren:

```
# In ESPHome: zet scan: true in i2c sectie
# Controleer log output voor: Found i2c device at address 0x3C
```

### LED Test
De ESP32-C6 XIAO heeft een ingebouwde LED op GPIO15. Als de LED knippert
na het flashen, werkt de firmware.

### Button Test  
Druk de knop in. In de ESPHome logs zie je:
```
[D] [binary_sensor:036] 'Wekker Knop': Sending state ON
```
