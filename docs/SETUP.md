# Installatie Handleiding - Zigbee Wekker

> **Niveau:** Beginner (geen eerdere ESPHome ervaring vereist)
> **Tijdsduur:** 2-3 uur eerste keer, 30-60 minuten voor ervaren gebruikers

## Overzicht

Na deze handleiding heb je:
- ✅ Hardware aangesloten
- ✅ ESPHome firmware geflasht op de ESP32-C6
- ✅ Wekker verbonden met Home Assistant
- ✅ Alarm tijden instelbaar via dashboard

---

## Stap 1: Hardware Aansluiten

> 📸 [Foto placeholder: overzicht bedrading]

Sluit alle componenten aan **voor** je de voeding aansluit.

### 1.1 Wat je nodig hebt
- ESP32-C6 module (Seeed Studio XIAO ESP32-C6)
- OLED display 1.3" SSD1306
- Actieve buzzer
- Drukknop 19mm
- Jumperwires (Dupont female-female)
- USB-C kabel voor flashen

### 1.2 Verbindingen maken

Volg de tabel exact op. **Let op de 3.3V voor het OLED display - gebruik nooit 5V!**

| OLED Pin | ESP32-C6 Pin |
|----------|--------------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO6 |
| SCL | GPIO7 |

| Buzzer Pin | ESP32-C6 Pin |
|------------|--------------|
| + (lang) | GPIO8 |
| - (kort) | GND |

| Knop Pin | ESP32-C6 Pin |
|----------|--------------|
| Pin 1 | GPIO9 |
| Pin 2 | GND |

> 📸 [Foto placeholder: aansluitingen close-up]

### 1.3 Verbindingen controleren

Controleer voor je de voeding aansluit:
- [ ] OLED VCC verbonden met 3.3V (niet 5V!)
- [ ] Alle GND draden verbonden
- [ ] SDA op GPIO6, SCL op GPIO7
- [ ] Buzzer + op GPIO8
- [ ] Knop op GPIO9 en GND

---

## Stap 2: ESPHome Installeren in Home Assistant

> 📸 [Screenshot placeholder: HA instellingen menu]

### 2.1 ESPHome add-on installeren

1. Open **Home Assistant** in je browser
2. Ga naar **Instellingen** (linker menu, tandwiel icoon)
3. Klik **Add-ons**
4. Klik de blauwe knop **Add-on Store** (rechtsonder)
5. Zoek naar **ESPHome**
6. Klik op **ESPHome Device Compiler**
7. Klik **Installeren** en wacht (~2 minuten)
8. Schakel in: **Start bij opstarten** en **Tonen in zijbalk**
9. Klik **Starten**

> 📸 [Screenshot placeholder: ESPHome add-on pagina]

---

## Stap 3: Configuratie Bestand Aanmaken

### 3.1 secrets.yaml aanmaken

Maak een kopie van `esphome/secrets.yaml.example` en hernoem naar `secrets.yaml`:

```yaml
wifi_ssid: "NaamVanJeWifi"        # jouw WiFi naam
wifi_password: "WifiWachtwoord"   # jouw WiFi wachtwoord
api_encryption_key: ""            # zie stap 3.2
ota_password: "kies_een_wachtwoord"
```

### 3.2 API sleutel genereren

1. Ga naar de ESPHome add-on (via zijbalk)
2. Klik **Nieuw apparaat** of gebruik de dashboard
3. Of genereer handmatig via de terminal:
   ```bash
   openssl rand -base64 32
   ```
4. Plak de gegenereerde sleutel als `api_encryption_key` in secrets.yaml

### 3.3 Configuratie uploaden

De bestanden staan al klaar in de `esphome/` map van dit project.
Kopieer ze naar de ESPHome configuratiemap:

**Via Samba add-on (aanbevolen):**
1. Installeer de **Samba share** add-on in HA
2. Verbind met `\\homeassistant\config\esphome\`
3. Kopieer `zigbee-alarm-clock.yaml` en `secrets.yaml` naar die map

**Via de ESPHome add-on editor:**
1. Open ESPHome in de zijbalk
2. Klik **Nieuw Apparaat** → **Nee, ga handmatig verder**
3. Voer in als naam: `zigbee-alarm-clock`
4. Klik **ESP32-C6**
5. Vervang de gehele configuratie met de inhoud van `esphome/zigbee-alarm-clock.yaml`

---

## Stap 4: ESP32-C6 Flashen

### 4.1 ESP32-C6 verbinden met computer

1. Verbind de ESP32-C6 met een **USB-C kabel** met je computer
   (zorg dat het een datakabel is, niet alleen een oplaadkabel!)
2. De computer herkent het als een USB serieel apparaat

### 4.2 Eerste keer flashen (via browser - makkelijkst)

1. Ga naar de ESPHome add-on in Home Assistant
2. Je ziet het apparaat `zigbee-alarm-clock` in de lijst
3. Klik **Installeren** (of de 3 puntjes → Installeren)
4. Kies **Verbinden via USB**
5. Selecteer de COM-poort / tty-poort van de ESP32-C6

> 📸 [Screenshot placeholder: ESPHome apparaat installeren]

**Als de COM-poort niet zichtbaar is:**
- Windows: installeer [CP210x driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- Mac/Linux: poort verschijnt als `/dev/ttyUSB0` of `/dev/ttyACM0`

### 4.3 Flashen via ESPHome web interface

Alternatief zonder HA add-on:
1. Ga naar [web.esphome.io](https://web.esphome.io) in Chrome
2. Klik **Connect**
3. Selecteer de poort
4. Klik **Install** → upload de gecompileerde firmware

### 4.4 Wachten op voltooiing

Het flashen duurt 2-5 minuten. Je ziet:
```
INFO Compiling project...
INFO Linking...
INFO Flashing...
INFO Done!
```

Na het flashen herstart de ESP32-C6 automatisch.

---

## Stap 5: Verbinding met Home Assistant

### 5.1 Apparaat toevoegen

Na het flashen en opstarten:
1. HA toont een melding: **Nieuw apparaat ontdekt: Zigbee Wekker**
2. Klik de melding
3. Klik **Configureren**
4. Voer de API sleutel in (dezelfde als in secrets.yaml)
5. Klik **Indienen**

> 📸 [Screenshot placeholder: HA apparaat toevoegen melding]

**Geen melding?**
- Ga naar Instellingen → Apparaten & Services → **Integraties toevoegen**
- Zoek **ESPHome** en klik erop
- Voer het IP-adres in van de wekker (zie je router DHCP tabel)

### 5.2 Verbinding verifiëren

1. Ga naar Instellingen → Apparaten & Services → ESPHome
2. Je ziet **Zigbee Wekker** als verbonden apparaat
3. Klik erop voor alle entiteiten

---

## Stap 6: Home Assistant Configureren

### 6.1 Helpers aanmaken

Voeg de helpers toe aan Home Assistant (voor het dashboard):

**Via de UI (aanbevolen):**
1. Instellingen → Apparaten & Services → **Helpers** tab
2. Klik **+ Maak Helper**
3. Kies **Datum/tijd** → naam: "Alarm Maandag", type: Tijd
4. Herhaal voor alle 7 dagen (Di, Wo, Do, Vr, Za, Zo)
5. Maak ook **Schakelaar** helpers aan voor "Alarm Maandag Aan" etc.

**Via YAML (gevorderd):**
Voeg de inhoud van `home-assistant/configuration.yaml` toe aan je HA configuration.yaml

### 6.2 Automatiseringen toevoegen

1. Instellingen → Automatiseringen
2. Klik **Importeren** (of maak handmatig aan)
3. Kopieer inhoud van `home-assistant/automations.yaml`
4. **Pas aan:** verander `light.slaapkamer` naar jouw lamp entiteit

### 6.3 Dashboard toevoegen

1. Ga naar je gewenste dashboard
2. Klik het **potlood icoon** (bewerken)
3. Klik **+ Kaart toevoegen** → **Handmatig**
4. Plak inhoud van `home-assistant/dashboard.yaml`
5. Klik **Opslaan**

> 📸 [Screenshot placeholder: dashboard voorbeeld]

---

## Stap 7: Testen

### 7.1 Basisfuncties controleren

- [ ] **Display:** Toont de huidige tijd (gesynchroniseerd met HA)
- [ ] **WiFi icoon:** "W" zichtbaar in de rechterbovenhoek van het display
- [ ] **Alarm icoon:** "ON" zichtbaar als hoofdschakelaar aan staat

### 7.2 Alarm instellen en testen

1. Ga naar het wekker dashboard in Home Assistant
2. Stel een alarm in op 2 minuten in de toekomst
3. Schakel de dag schakelaar aan
4. Wacht tot het alarm afgaat

**Alarm test via HA:**
1. Ga naar Ontwikkeltools → Services
2. Zoek `esphome.zigbee_wekker_trigger_alarm`
3. Klik **Service aanroepen** - het alarm gaat direct af!

### 7.3 Knop testen

Druk de fysieke knop in:
- Als alarm actief: alarm stopt direct
- Altijd: HA event `esphome.button_pressed` wordt gestuurd

Controleer via: Ontwikkeltools → Evenementen → luisteren naar `esphome.button_pressed`

### 7.4 OTA Update testen

Maak een kleine wijziging in de ESPHome configuratie en installeer via:
ESPHome add-on → Zigbee Wekker → Installeren → **Installeren via WiFi**

---

## Stap 8: Behuizing Monteren (optioneel)

1. Print `3d-models/case-v1.stl` (body en deksel apart)
2. Volg de assemblage instructies in `3d-models/README.md`
3. Monteer alle componenten in de behuizing
4. Klik het deksel op de body

---

## Problemen?

Zie [TROUBLESHOOTING.md](TROUBLESHOOTING.md) voor oplossingen bij veelvoorkomende problemen.

## Vragen?

Open een [GitHub Issue](https://github.com/natuurwaarnemer/Zigbee-alarm-clock/issues)
met een beschrijving van het probleem en de ESPHome logs.
