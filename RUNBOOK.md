# Runbook STM32F7 MicroBoot

Handgriffe am Gerät für alle Projekte, die diesen Bootloader einbinden. Die Prozeduren sind für
jedes Gerät gleich. Was vom Gerät abhängt, steht hier als Platzhalter `‹…›` und wird im Runbook
der jeweiligen Anwendung festgelegt.

| Platzhalter     | Bedeutung                                                              |
|-----------------|------------------------------------------------------------------------|
| `‹CAN-Adapter›` | CAN-Adapter am PC, z. B. PEAK PCAN-USB                                 |
| `‹CAN-Stecker›` | Stecker am Gerät, an dem CAN3 anliegt                                  |
| `‹Reset›`       | Wie das Gerät neu gestartet wird (Reset-Taste, Versorgung)             |
| `‹Taster›`      | Taster am Pin `PC13`, auf dem Nucleo-F767ZI der blaue User-Taster      |
| `‹LED›`         | LED am Pin `PB7`, auf dem Nucleo-F767ZI die blaue LED LD2              |
| `‹Firmware›`    | Name und Ablage der `.srec`-Datei der Application                      |

Feste Werte des Bootloaders: CAN 500 kBit/s, Host → Gerät `0x7E0`, Gerät → Host `0x7E1`,
Standard-IDs (11 Bit). Wie der Bootloader arbeitet, steht in der [Architektur](ARCHITECTURE.md).

## Firmware mit MicroBoot aktualisieren

### Voraussetzungen
- PC mit MicroBoot (aus dem OpenBLT-Paket) und Treiber für `‹CAN-Adapter›`
- Firmware als Motorola-S-Record: `‹Firmware›`. Keine `.bin`-, `.hex`- oder `.elf`-Datei.
- CAN-Bus mit 120 Ω an beiden Enden

### Schritte
1. `‹CAN-Adapter›` an `‹CAN-Stecker›` und an den PC anschließen.
2. MicroBoot starten, **Settings** klicken.
3. Reiter **Session**: **XCP version 1.0** wählen.
4. Reiter **Communication** (Transport): **XCP on CAN**, Gerät `‹CAN-Adapter›`, Kanal 0,
   Baudrate **500 kbits/sec**, **Transmit CAN identifier** `7E0`, **Receive CAN identifier**
   `7E1`, **29-bit CAN identifiers** nicht anhaken. **OK** klicken.
5. **Browse** klicken und `‹Firmware›` wählen. MicroBoot meldet „Connecting to target“.
6. Gerät neu starten: `‹Reset›`. MicroBoot muss sich innerhalb von 500 ms verbinden, das
   geschieht automatisch, solange MicroBoot „Connecting“ anzeigt.
7. Warten, bis Löschen und Programmieren abgeschlossen sind. Gerät dabei nicht ausschalten und
   den Adapter nicht abziehen.

### Prüfung
- MicroBoot meldet den Abschluss und schließt das Fenster.
- `‹LED›` hört auf zu blinken, die Application läuft.

### Fehlerbilder

| Symptom | Ursache | Abhilfe |
|---------|---------|---------|
| MicroBoot bleibt bei „Connecting to target“ | Gerät nicht im Bootloader, Application wurde schon gestartet | Schritt 6 wiederholen, solange MicroBoot „Connecting“ anzeigt |
| MicroBoot bleibt bei „Connecting“, `‹LED›` blinkt | falsche Baudrate oder vertauschte IDs, fehlender Abschluss | Schritt 4 prüfen, Abschlusswiderstände prüfen |
| MicroBoot bleibt bei „Connecting“, `‹LED›` blinkt nie | Gerät ohne Versorgung oder kein Bootloader im Flash | Versorgung prüfen. Hilft das nicht, Bootloader per ST-Link neu aufspielen (Entwickler) |
| Update bricht direkt nach dem Verbinden ab, nichts wird gelöscht | Info-Tabelle der neuen Firmware passt nicht (`tableId` oder `productId` abweichend), oder im Gerät steht noch keine Firmware mit Info-Tabelle | Richtige `‹Firmware›` für dieses Gerät wählen. Ist das Gerät leer, Application einmal per ST-Link aufspielen |
| Fehler beim Programmieren einer Adresse unter `0x08008000` | Firmware nicht für den Bootloader gelinkt | Nicht am Gerät lösbar, Entwickler informieren (siehe [README, Fehlerbilder](README.md#fehlerbilder)) |
| Update bricht mitten im Programmieren ab | Verbindung unterbrochen, Versorgung weg | [Gerät mit defekter Application wiederherstellen](#gerät-mit-defekter-application-wiederherstellen) |

## Firmware mit BootCommander aktualisieren

### Voraussetzungen
- wie bei [Firmware mit MicroBoot aktualisieren](#firmware-mit-microboot-aktualisieren)
- BootCommander (aus dem OpenBLT-Paket) im Pfad

### Schritte
1. `‹CAN-Adapter›` an `‹CAN-Stecker›` und an den PC anschließen.
2. In der Eingabeaufforderung starten, für einen PCAN-USB:
   ```sh
   BootCommander -s=xcp -t=xcp_can -d=peak_pcanusb -b=500000 -tid=7e0 -rid=7e1 ‹Firmware›
   ```
3. Sobald „Connecting to target bootloader“ erscheint, das Gerät neu starten: `‹Reset›`.
4. Warten, bis die Ausgabe das Ende des Updates meldet.

### Prüfung
- Jede Zeile der Ausgabe endet mit `[OK]`.
- `‹LED›` hört auf zu blinken, die Application läuft.

### Fehlerbilder

Wie bei [Firmware mit MicroBoot aktualisieren](#fehlerbilder). Zusätzlich:

| Symptom | Ursache | Abhilfe |
|---------|---------|---------|
| Fehler beim Öffnen des Geräts | falscher Wert für `-d` oder Treiber fehlt | `-d` passend zu `‹CAN-Adapter›` setzen, Treiber installieren |

## Gerät mit defekter Application wiederherstellen

Die Application startet nicht, hängt sofort oder hat das Update abgebrochen.

### Voraussetzungen
- wie bei [Firmware mit MicroBoot aktualisieren](#firmware-mit-microboot-aktualisieren)
- Zugang zu `‹Taster›`

### Schritte
1. MicroBoot wie in [Firmware mit MicroBoot aktualisieren](#firmware-mit-microboot-aktualisieren),
   Schritte 1–5, vorbereiten.
2. `‹Taster›` drücken und gedrückt halten.
3. Gerät neu starten: `‹Reset›`.
4. `‹Taster›` erst loslassen, wenn mindestens 1 s vergangen ist. `‹LED›` blinkt weiter, der
   Bootloader bleibt aktiv.
5. Weiter mit Schritt 7 der Aktualisierung.

### Prüfung
- wie bei [Firmware mit MicroBoot aktualisieren](#firmware-mit-microboot-aktualisieren)

### Fehlerbilder

| Symptom | Ursache | Abhilfe |
|---------|---------|---------|
| `‹LED›` hört nach dem Reset auf zu blinken | `‹Taster›` zu früh losgelassen | Schritte 2–4 wiederholen, Taster länger halten |
| Update wird abgelehnt, bevor gelöscht wird | Info-Tabelle im Flash ist gelöscht oder beschädigt | Application per ST-Link aufspielen (Entwickler) |
