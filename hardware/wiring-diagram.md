# Aansluitschema - Zigbee Wekker

## Overzicht

Alle verbindingen zijn 3.3V logica. De ESP32-C6 wordt gevoed via USB-C (5V).
De interne 3.3V regelaar op het development board voorziet alle componenten.

## Aansluittabel

| Component | Pin | ESP32-C6 Pin | Draadkleur (advies) | Notities |
|-----------|-----|--------------|---------------------|----------|
| OLED Display | VCC | 3.3V | Rood | Nooit 5V! |
| OLED Display | GND | GND | Zwart | |
| OLED Display | SDA | GPIO6 | Blauw | I2C Data |
| OLED Display | SCL | GPIO7 | Geel | I2C Clock |
| Buzzer | + (VCC) | GPIO8 | Oranje | PWM output |
| Buzzer | - (GND) | GND | Zwart | |
| Knop | Pin 1 | GPIO9 | Groen | Interne pull-up actief |
| Knop | Pin 2 | GND | Zwart | |
| ESP32-C6 | USB-C | 5V adapter | - | Min. 1A voeding |

## ASCII Aansluitdiagram

```
                    ESP32-C6 (Seeed Studio XIAO C6)
                    ┌─────────────────────────────┐
               3.3V ┤ 3V3                     GND ├─── GND (gemeenschappelijk)
               GND  ┤ GND                     5V  ├─── USB-C ingang
             GPIO6  ┤ D4/GPIO6            GPIO0   ├
             GPIO7  ┤ D5/GPIO7            GPIO1   ├
             GPIO8  ┤ D6/GPIO8            GPIO2   ├
             GPIO9  ┤ D7/GPIO9            GPIO3   ├
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
             │ I2C 0x3C │    │   + aan GPIO8│        │  open)    │
             │          │    │   - aan GND  │        │           │
             │VCC → 3.3V│    └──────────────┘        │Pin1→GPIO9 │
             │GND → GND │                            │Pin2→GND   │
             │SDA→ GPIO6│                            └──────────┘
             │SCL→ GPIO7│
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
| SCL | GPIO7 | Geel |
| SDA | GPIO6 | Blauw |

⚠️ **Waarschuwing:** Sluit nooit 5V aan op de OLED VCC. Dit beschadigt het
display permanent!

### 2. Buzzer (Actieve Piezo Buzzer)

Een **actieve** buzzer maakt een toon zolang spanning aanwezig is.
Een **passieve** buzzer heeft PWM signaal nodig (ook ondersteund).

```
Buzzer aansluiting:
┌──────────┐
│  BUZZER  │
│   + (lang│───────── GPIO8
│   - (kort│───────── GND
└──────────┘
```

| Buzzer Pin | ESP32-C6 |
|------------|----------|
| + (positief, langere pin) | GPIO8 |
| - (negatief, kortere pin) | GND |

💡 **Tip:** De meeste buzzer modules hebben een `+` markering. Verbind die
met GPIO8. Test zonder behuizing voor je alles samenbouwt.

### 3. Drukknop

Een momenteel normaal-open (NO) drukknop met 2 aansluitingen:

```
Knop aansluiting (2-draads):
┌──────────┐
│   KNOP   │
│  Pin 1   │───── GPIO9 (interne pull-up)
│  Pin 2   │───── GND
└──────────┘
```

De interne pull-up weerstand is ingeschakeld in software. De knop trekt GPIO9
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
   aangesloten via GPIO8 (LEDC PWM regelt de duty cycle).
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
