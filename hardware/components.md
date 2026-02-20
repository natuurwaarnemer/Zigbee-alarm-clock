# Materiaallijst (BOM) - Zigbee Wekker

## Benodigde Onderdelen

| # | Component | Specificatie | Hoeveelheid | Prijs (ca.) | Bestelcode (voorbeeld) |
|---|-----------|--------------|-------------|-------------|------------------------|
| 1 | Microcontroller | Seeed Studio XIAO ESP32-C6 | 1 | €8,00 | Seeed 113991054 |
| 2 | OLED Display | 1.3" 128x64 I2C SSD1306 (blauw) | 1 | €7,00 | AliExpress/Amazon |
| 3 | Buzzer | Actieve piezo buzzer 3.3V-5V | 1 | €1,50 | Grove 107020000 |
| 4 | Drukknop | 19mm momenteel normaal-open | 1 | €3,00 | DE-GQ19H-10 |
| 5 | USB-C kabel | USB-C naar USB-A of USB-C naar USB-C | 1 | €4,00 | - |
| 6 | USB-C adapter | 5V / 1A minimum | 1 | €8,00 | Diverse merken |
| 7 | Prototype PCB | 7x9 cm soldeereiland PCB (optioneel) | 1 | €2,00 | - |
| 8 | Jumperwires | Dupont male-female jumpers | ~10 | €3,00 | Set van 40 |
| 9 | Pin headers | 2.54 mm male headers | 2 rijen | €1,00 | - |
| 10 | 3D print filament | PLA 1.75mm (zwart) | ~50 gram | €1,50 | Diverse merken |

**Totaal (geschat): €39,00**

## Alternatieve Componenten

### Microcontroller Alternatieven
| Variant | Opmerking |
|---------|-----------|
| ESP32-C6 DevKit (Espressif) | Groter formaat, meer GPIO's beschikbaar |
| ESP32-C6 (andere fabrikant) | Let op pinout verschil! |

### Display Alternatieven
| Variant | Opmerking |
|---------|-----------|
| SSD1306 0.96" 128x64 | Kleiner display, zelfde driver |
| SH1106 1.3" 128x64 | Andere driver - aanpassen in ESPHome: `model: "SH1106 128x64"` |
| SSD1306 wit (i.p.v. blauw) | Zelfde aansluitingen |

### Buzzer Alternatieven
| Variant | Opmerking |
|---------|-----------|
| Passieve piezo buzzer | Ondersteund via LEDC PWM - behoudt melodie mogelijkheid |
| Grove buzzer module | Heeft eigen transistor driver |
| KY-012 buzzer module | Directe GPIO aansluiting |

### Knop Alternatieven
| Variant | Opmerking |
|---------|-----------|
| 12mm drukknop | Kleiner, past in compactere behuizing |
| Arcade knop 30mm | Groter, makkelijker te bedienen half-wakker |
| Captatieve touchknop | Geen mechanische slijtage |

## Gereedschap

| Item | Gebruik |
|------|---------|
| Soldeerbout + soldeertin | Solderen van pinnen/wires |
| Kniptang | Draadjes knippen |
| Striptang | Isolatie verwijderen |
| Multimeter | Verbindingen testen |
| USB naar serieel adapter | Eerste keer flashen (of via USB direct) |
| 3D printer | Behuizing afdrukken |

## Waar Bestellen?

### Nederland/België
- **Kiwi Electronics** (kiwi-electronics.com) - ESP32-C6, displays, Arduino componenten
- **AliExpress** - Voordelig, langere levertijd (2-4 weken)
- **Amazon.nl** - Snelle levering, iets duurder
- **Mouser** / **DigiKey** - Professionele componenten, vaak duurder

### Directe Links (controleer actualiteit)
- XIAO ESP32-C6: https://www.seeedstudio.com/Seeed-Studio-XIAO-ESP32C6-p-5884.html
- Grove Buzzer: https://www.seeedstudio.com/Grove-Buzzer.html

## Opmerkingen bij de Onderdelen

**ESP32-C6:** De XIAO variant van Seeed Studio is compact (21x17.5mm) en heeft
een ingebouwde RGB LED. Ideaal voor kleine behuizingen. Let op: de pinout wijkt
af van standaard ESP32 dev boards.

**OLED Display:** Koop een module met 4-pins I2C aansluiting (VCC, GND, SCL, SDA).
Sommige modules hebben een RST-pin - die laat je onverbonden of verbindt met een
GPIO (niet nodig voor SSD1306).

**Buzzer:** Een **actieve** buzzer heeft een ingebouwde oscillator en maakt geluid
bij vaste spanning. Een **passieve** buzzer heeft PWM nodig maar kan melodieën
spelen. Beide worden ondersteund door de ESPHome configuratie.

**Drukknop:** De DE-GQ19H-10 is een robuuste 19mm panelknop. Zorg dat je de
normaal-open (NO) variant hebt, niet normaal-gesloten (NC).
