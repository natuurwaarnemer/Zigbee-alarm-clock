# Veelvoorkomende Problemen - Zigbee Wekker

## Display Problemen

### Display blijft zwart / I2C device niet gevonden

### Symptoom
- ESPHome log toont: "I2C device not found at address 0x3C"
- Display blijft compleet zwart
- Geen foutmeldingen over display driver

### Oorzaak
Waarschijnlijk zijn de I2C pinnen verkeerd aangesloten. De XIAO ESP32-C6 heeft verwarrende pin labels!

### Oplossing
1. Controleer dat je de OLED hebt aangesloten op:
   - SDA → **D4** (staat GPIO22 op het schema, NIET D6!)
   - SCL → **D5** (staat GPIO23 op het schema, NIET D7!)
2. De D-nummers op de pins komen NIET overeen met GPIO nummers
3. Zie hardware/wiring-diagram.md voor de correcte aansluitingen

### Test
In ESPHome logs (na herstarten), moet je zien:
```
[I][i2c.arduino:071]: Results from i2c bus scan:
[I][i2c.arduino:077]: Found i2c device at address 0x3C
```

Als je dit NIET ziet, zijn de I2C pinnen verkeerd aangesloten.

---

### Display blijft zwart / toont niets

**Mogelijke oorzaken en oplossingen:**

1. **Verkeerde voedingsspanning**
   - Controleer of VCC verbonden is met **3.3V**, niet 5V
   - Meet met een multimeter: 3.3V tussen VCC en GND op het display

2. **Verkeerd I2C adres**
   - Standaard adres is `0x3C`, maar sommige modules gebruiken `0x3D`
   - Zet `scan: true` in de i2c sectie van de ESPHome configuratie
   - Herstart en controleer de logs: je ziet `Found i2c device at address 0x3C`
   - Als je `0x3D` ziet: verander het adres in de yaml

3. **SDA/SCL verwisseld**
   - D4 (GPIO22) = SDA, D5 (GPIO23) = SCL
   - Controleer de kabels nog eens

4. **Slechte verbinding**
   - Controleer of alle jumper wires stevig zijn
   - Test met een I2C scanner sketch in Arduino IDE

5. **Defect display of module**
   - Test met een andere I2C scanner op een andere microcontroller

---

### Display toont ruis / rare tekens

- **Slechte I2C verbinding:** Gebruik kortere kabels (max 30 cm)
- **Interferentie:** Houd display kabels uit de buurt van de buzzer bedrading
- **Verkeerd display model:** Controleer of het `SSD1306 128x64` is, niet SH1106
  - Als je een SH1106 display hebt: verander `model: "SSD1306 128x64"` naar `model: "SH1106 128x64"` in de yaml

---

## Alarm Problemen

### Alarm gaat niet af op de ingestelde tijd

1. **Tijdsynchronisatie controleren**
   - Ga naar Ontwikkeltools → Sjablonen in Home Assistant
   - Typ: `{{ now() }}` - dit toont de HA systeemtijd
   - Controleer of de wekker dezelfde tijd toont als HA

2. **Dag schakelaar controleren**
   - Is de schakelaar voor die dag AAN?
   - Is de **hoofdschakelaar** AAN?

3. **Tijdzone controleren**
   - De configuratie gebruikt `Europe/Amsterdam`
   - Als je in een andere tijdzone bent: pas aan in de yaml

4. **ESPHome logs controleren**
   - Open ESPHome → Logs van het apparaat
   - Zoek naar `[I] [script:] check_alarms`
   - Je ziet elke minuut een check

5. **Alarm minuten controleren**
   - Alarm controle gebeurt op seconde 0 van elke minuut
   - Er kan 1 minuut vertraging zijn

---

### Buzzer maakt geen geluid

1. **D8 (GPIO19) controleren**
   - Verbinding buzzer + naar D8 (GPIO19), - naar GND

2. **Actieve vs passieve buzzer**
   - Actieve buzzer: reageert op vaste spanning (werkt direct)
   - Passieve buzzer: heeft PWM nodig (ook ondersteund)
   - Test: sluit de buzzer direct aan op 3.3V en GND - als hij piept, is het een actieve buzzer

3. **PWM frequentie**
   - De firmware gebruikt 1000 Hz PWM
   - Sommige passieve buzzers reageren beter op andere frequenties
   - Pas `frequency: 1000 Hz` aan in de yaml

4. **Buzzer defect**
   - Test met een multimeter: meet de weerstand van de buzzer (typisch 8-32 Ohm)

---

### Alarm stopt niet na knop indrukken

1. **Knop verbinding controleren**
   - D9 (GPIO20) naar knop Pin 1, GND naar knop Pin 2
   - Meet met multimeter: kortsluiting wanneer knop ingedrukt?

2. **Pull-up controleren**
   - GPIO20 moet `HIGH` zijn in rust en `LOW` bij indrukken
   - Controleer met: Ontwikkeltools → Staten → zoek `binary_sensor.zigbee_wekker_wekker_knop`

3. **Debounce problemen**
   - De debounce is 20ms. Bij bouncy knoppen: verhoog naar 50ms
   - Zoek in yaml: `delayed_on: 20ms` en vergroot de waarde

---

## WiFi / Verbindingsproblemen

### Wekker verbindt niet met WiFi

1. **Credentials controleren**
   - Controleer `wifi_ssid` en `wifi_password` in `secrets.yaml`
   - WiFi naam is hoofdlettergevoelig: `MijnWifi` ≠ `mijnwifi`

2. **2.4 GHz WiFi**
   - ESP32-C6 ondersteunt **alleen 2.4 GHz** WiFi
   - Zorg dat je 2.4 GHz netwerk actief is (niet alleen 5 GHz)

3. **Fallback hotspot**
   - Als WiFi mislukt: de wekker maakt een eigen hotspot "Wekker Fallback"
   - Verbind hiermee en configureer WiFi via het captive portal

4. **WiFi signaal te zwak**
   - Controleer `sensor.zigbee_wekker_wifi_signaalsterkte` in HA
   - Onder -80 dBm: problemen verwacht
   - Gebruik een WiFi extender of verplaats de router dichter bij

5. **ESP32-C6 herstarten**
   - Haal de USB stekker er uit en steek hem er weer in
   - Of via HA: Ontwikkeltools → Services → `button.zigbee_wekker_restart`

---

### Home Assistant ziet het apparaat niet

1. **Zelfde netwerk**
   - HA en wekker moeten op **hetzelfde** WiFi netwerk zitten

2. **mDNS / Discovery**
   - HA gebruikt mDNS (`.local` adressen) voor ESPHome discovery
   - Sommige routers blokkeren mDNS - probeer het IP-adres direct in te voeren
   - Vind het IP via je router DHCP tabel of scan met een app als "Fing"

3. **API sleutel**
   - Zorg dat de `api_encryption_key` in secrets.yaml overeenkomt
   - Als je opnieuw flasht, gebruik dezelfde sleutel

4. **Firewall**
   - Port 6053 moet open zijn tussen HA en de wekker

---

## ESPHome Compileerfouten

### "Font not found" fout

De configuratie gebruikt Google Fonts. Dit vereist internetverbinding bij compileren.
Als je geen internet hebt:

1. Download het font handmatig
2. Sla op als `esphome/fonts/Roboto-Regular.ttf`
3. Verander in yaml:
   ```yaml
   font:
     - file: "fonts/Roboto-Regular.ttf"
   ```

### "Unknown platform" fout

- Controleer of je de laatste versie van ESPHome hebt
- Update via: Instellingen → Add-ons → ESPHome → Update

### Compileren duurt erg lang of mislukt

- Eerste compilatie kan 10-15 minuten duren (ESP-IDF framework)
- Zorg voor voldoende vrije schijfruimte (>500 MB) op je HA systeem
- Herstart de ESPHome add-on en probeer opnieuw

---

## Display Helderheid

### Display te helder 's nachts

De automatische dimmer is actief tussen 22:00 en 07:00.
Pas de uren aan in de yaml als gewenst:

```yaml
// Zoek in de display lambda:
bool night_mode = (now.hour >= 22 || now.hour < 7);
// Verander 22 en 7 naar gewenste uren
```

---

## Overige Problemen

### Wekker toont verkeerde tijd

1. Verbinding met HA verbroken - wekker synchroniseert elke keer opnieuw bij verbinding
2. Verkeerde tijdzone - controleer `timezone: "Europe/Amsterdam"` in de yaml
3. NTP synchronisatie: de sntp fallback time server is geconfigureerd, maar vereist internet

### OTA Update mislukt

1. Zorg dat wekker en HA op hetzelfde netwerk zitten
2. Probeer via USB als WiFi OTA niet werkt
3. Controleer het OTA wachtwoord in secrets.yaml

---

## Nog steeds problemen?

1. Bekijk de ESPHome **logs** voor foutmeldingen:
   ESPHome add-on → klik op apparaat → Logs

2. Open een **GitHub Issue** met:
   - Beschrijving van het probleem
   - Inhoud van de ESPHome logs (geen secrets!)
   - Foto van de bedrading

3. Bezoek het **ESPHome forum**: [community.home-assistant.io](https://community.home-assistant.io/c/esphome/)
