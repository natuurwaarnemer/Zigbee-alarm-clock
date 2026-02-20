# 3D-geprinte Behuizing - Zigbee Wekker

## Overzicht

De behuizing is ontworpen in OpenSCAD (parametrisch, aanpasbaar) en is 
printbaar zonder supports.

## Benodigdheden

| Item | Specificatie |
|------|--------------|
| 3D Printer | FDM printer, minimaal 100x80 mm printoppervlak |
| Filament | PLA 1.75 mm, ~50 gram |
| Kleur | Naar keuze (zwart aanbevolen) |

## Printinstellingen

| Instelling | Waarde |
|------------|--------|
| Laagdikte | 0.2 mm |
| Vulling | 20% (gyroid of grid) |
| Omtreklijnen | 3 |
| Printtemperatuur PLA | 200-210°C |
| Bed temperatuur | 60°C |
| Supports | NIET nodig |
| Brim | Aanbevolen (3-5 mm) voor het deksel |

## Onderdelen

Het model bestaat uit 2 onderdelen die apart worden afgedrukt:

### 1. Body (Onderstuk) - `case_body()`
- Buitenafmetingen: 100 x 75 x 40 mm
- Bevat: montage palen voor ESP32-C6 en OLED, USB-C poort uitsparing,
  snap-fit clips, kabelgeleiding
- Printoriëntatie: **Platte onderkant naar beneden** (geen supports nodig)

### 2. Deksel (Bovenstuk) - `case_lid()`
- Buitenafmetingen: 100 x 75 x 6 mm  
- Bevat: display venster uitsparing (35x28 mm), knop gat (19.5 mm Ø),
  snap-fit groeven
- Printoriëntatie: **Platte binnenkant naar beneden** (geen supports nodig)

## STL Exporteren vanuit OpenSCAD

### Body exporteren:
1. Open `case-v1.scad` in OpenSCAD
2. Zoek de render regels onderaan het bestand
3. Verwijder `//` voor `case_body();` en voeg `//` toe voor andere render regels
4. Klik F6 (Render)
5. Klik File → Export → Export as STL

### Deksel exporteren:
1. Zelfde procedure, maar activeer `case_lid();`

## Parameters Aanpassen

Open `case-v1.scad` en pas de parameters bovenaan het bestand aan:

```openscad
// Buitenafmetingen - vergroot voor meer ruimte binnenin
case_width  = 100;   // breedte in mm
case_depth  = 75;    // diepte in mm
case_height = 40;    // hoogte in mm
wall_thickness = 2.5; // wanddikte in mm
```

**Let op toleranties:** Bij grote printers of andere filamenttypes kan het nodig
zijn `fit_tol` aan te passen (standaard 0.2 mm).

## Assemblage

### Benodigdheden voor assemblage
- 4x M2x4mm schroeven (voor ESP32-C6)
- 4x M2x4mm schroeven (voor OLED module, optioneel)
- Dubbelzijdig tape of hot glue (alternatief voor schroeven)
- Kleine platte schroevendraaier

### Stappen
1. Druk beide onderdelen af
2. Verwijder brim/rand voorzichtig
3. Test of de snap-fit past (body + deksel klikken samen)
4. Monteer ESP32-C6 module op de 4 montage palen (M2 schroeven)
5. Monteer OLED display (gericht naar het display venster in het deksel)
6. Verleg kabels netjes via de kabelgeleiding
7. Klik het deksel op de body
8. Test alle functies voor definitieve montage

## Maatvoering

```
Bovenaanzicht deksel:
┌────────────────────────────────────────────┐
│                                            │
│    ┌──────────────────────┐    ○           │
│    │   Display venster    │  (knop)        │
│    │    35 x 28 mm        │   19.5mm Ø     │
│    └──────────────────────┘                │
│                                            │
└────────────────────────────────────────────┘
          100 mm breed x 75 mm diep

Zijaanzicht:
┌────────────────────────────────────────────┐  ← Deksel (6mm)
│                                            │
│                                            │
│                                            │  ← Body (40mm)
│  [USB-C]                                   │
└────────────────────────────────────────────┘
```

## Problemen Oplossen

**Deksel past niet:**
- Vergroot `fit_tol` van 0.2 naar 0.3 of 0.4 mm en herprint
- Schuur de binnenkant van de snap-fit groeven lichtjes bij

**Display venster te klein:**
- Meet je OLED module nauwkeurig na
- Pas `display_w` en `display_h` aan in het SCAD bestand

**Knop gat te nauw:**
- Pas `button_dia` aan (verhoog met 0.2-0.5 mm)

**Montagepalen te smal/breed:**
- Pas `post_hole_dia` aan voor M2 schroeven (standaard 2.1 mm)
