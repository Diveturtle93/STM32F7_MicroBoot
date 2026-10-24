# STM32F7 MicroBoot

Eine Bibliothek mit dem angepassten [OpenBLT](https://www.feaser.com/openblt/)-Bootloader für den
**STM32F767ZI** (Nucleo-F767ZI Board). Der Firmware-Upload erfolgt über **CAN-Bus** mit dem
**XCP-Protokoll**, gesteuert über das Host-Tool **MicroBoot** von Feaser. Die Bibliothek wird als
Submodul in ein STM32CubeIDE-Projekt eingebunden.

1. [Beschreibung](#beschreibung)
2. [Funktionsumfang](#funktionsumfang)
3. [Dateien](#dateien)
4. [Konfigurationsparameter](#konfigurationsparameter)
5. [Flash-Aufteilung](#flash-aufteilung)
6. [Info-Tabelle](#info-tabelle)
7. [Shared Parameters](#shared-parameters)
8. [Versionierung](#versionierung)
9. [Fehlerbilder](#fehlerbilder)
10. [API](#api)
11. [Verwendung](#verwendung)
12. [Hinweise](#hinweise)
13. [Quellen](#quellen)
14. [Abhängigkeiten](#abhängigkeiten)
15. [Lizenz](#lizenz)

## Beschreibung

OpenBLT ist ein quelloffener Bootloader für Embedded-Systeme. Diese Bibliothek enthält den
OpenBLT-Kern (Stand 1.21.0), die STM32F7-Treiber und die targetspezifischen Anpassungen aus dem
Demo-Projekt für das Nucleo-F767ZI. Alle Dateien sind auf den eigenen Coding Style umgestellt.
Die Konfiguration erfolgt zentral über `blt_conf.h`. Anwendungsspezifisches Verhalten (Checksumme,
Info-Tabelle, Watchdog, Start des Anwenderprogramms) ist in `hooks.c` als Hook-Funktionen
implementiert, die OpenBLT an definierten Stellen aufruft.

Der Bootloader arbeitet ohne Interrupts im Polling-Betrieb. Nach einem Reset bleibt er 500 ms
aktiv (Backdoor). Meldet sich in dieser Zeit kein Host über CAN, startet er das
Anwenderprogramm. Ist der Taster an `PC13` am Ende dieser 500 ms gedrückt, bleibt der Bootloader
aktiv (`CpuUserProgramStartHook()` in `hooks.c`). Während der Bootloader läuft, blinkt die LED an
`PB7` im 100-ms-Takt.

Wie die Module zusammenspielen, steht in der [Architektur](ARCHITECTURE.md).

## Funktionsumfang

- Firmware-Update über CAN3 mit 500 kBit/s und XCP
- Host-Tools MicroBoot und BootCommander von Feaser
- Zeitgesteuerter Backdoor-Eintritt (500 ms) und Verbleib im Bootloader per Taster
- Prüfung der Info-Tabelle: Update nur für Firmware mit gleicher Produktkennung
- Gemeinsame RAM-Parameter zwischen Bootloader und Application
- Optional: Seed/Key-Schutz, eigene Checksumme, Hardware-Watchdog über Hook-Funktionen

## Dateien

| Datei                | Beschreibung                                                                 |
|----------------------|------------------------------------------------------------------------------|
| `blt_conf.h`         | Zentrale Bootloader-Konfiguration (CPU, CAN, Flash, Watchdog, Info-Tabelle)  |
| `blt_version.h`      | Projektversion                                                               |
| `boot.c/.h`          | Bootloader-Kern: Initialisierung, Task und Version des OpenBLT-Kerns         |
| `app.c/.h`           | Einstieg aus `main()`: `AppInit()` und `AppTask()`                           |
| `can_t.c/.h`         | CAN-Treiber für die STM32F7-HAL (CAN3, Bittiming aus dem APB1-Takt)          |
| `flash.c/.h`         | Flash-Treiber: Flash-Aufteilung, Löschen und Programmieren des internen Flashs |
| `nvm.c/.h`           | NVM-Abstraktion über den Flash-Treiber, Checksumme über Hook-Funktionen      |
| `timer.c/.h`         | Millisekunden-Timer auf Basis von TIM1, ersetzt `HAL_GetTick()`              |
| `cpu.c/.h`           | CPU-Treiber: Interrupts sperren, Sprung in das Anwenderprogramm, Speicherfunktionen |
| `com.c/.h`           | Kommunikationsschicht: Auswahl des Transportkanals, Verbindungs-Timeout      |
| `cop.c/.h`           | Watchdog-Treiber (COP), ruft die Watchdog-Hook-Funktionen auf                |
| `xcp.c/.h`           | XCP-Protokollschicht (Slave), optional mit Seed/Key-Schutz                   |
| `backdoor.c/.h`      | Zeitgesteuerter Backdoor-Eintritt nach einem Reset                           |
| `hooks.c`            | Hook-Funktionen (Programmstart, Watchdog/LED, Checksumme, Info-Tabelle, Seed/Key) |
| `led.c/.h`           | LED-Blinktreiber während des Bootloader-Betriebs                             |
| `infotable.c/.h`     | Info-Tabellen-Prüfung: Validierung der neuen Firmware vor dem Flashen        |
| `shared_params.c/.h` | Gemeinsame Parameter zwischen Bootloader und Application (RAM-basiert)       |
| `asserts.c/.h`       | Assert-Handler für Laufzeit- und Compile-Zeit-Prüfungen                      |
| `plausibility.h`     | Plausibilitätsprüfungen der Bootloader-Konfiguration zur Compile-Zeit        |
| `types.h`            | Grundlegende Typdefinitionen (`blt_int8u`, `blt_bool`, …)                    |

## Konfigurationsparameter

Alle Parameter stehen in `blt_conf.h`.

### CPU

| Parameter                          | Wert      | Beschreibung                                      |
|------------------------------------|-----------|---------------------------------------------------|
| `BOOT_CPU_XTAL_SPEED_KHZ`          | 25000     | Quarzoszillator: 25 MHz                           |
| `BOOT_CPU_SYSTEM_SPEED_KHZ`        | 216000    | Systemtakt: 216 MHz                               |
| `BOOT_CPU_BYTE_ORDER_MOTOROLA`     | 0         | Little-Endian (Intel-Format)                      |
| `BOOT_CPU_USER_PROGRAM_START_HOOK` | 1         | Hook-Funktion vor Application-Start aktiviert     |

### CAN-Kommunikation

| Parameter                    | Wert    | Beschreibung                                          |
|------------------------------|---------|-------------------------------------------------------|
| `BOOT_COM_CAN_ENABLE`        | 1       | CAN-Transportschicht aktiviert                        |
| `BOOT_COM_CAN_BAUDRATE`      | 500000  | CAN-Baudrate: 500 kBit/s                              |
| `BOOT_COM_CAN_TX_MSG_ID`     | 0x7E1   | CAN-ID für Daten vom Steuergerät zum Host             |
| `BOOT_COM_CAN_RX_MSG_ID`     | 0x7E0   | CAN-ID für Daten vom Host zum Steuergerät             |
| `BOOT_COM_CAN_CHANNEL_INDEX` | 2       | CAN3 (zero-basiert: Index 2)                          |

### Flash und NVM

| Parameter                           | Wert  | Beschreibung                                            |
|-------------------------------------|-------|---------------------------------------------------------|
| `BOOT_NVM_SIZE_KB`                  | 2048  | Gesamter Flash-Speicher: 2048 kB                        |
| `BOOT_NVM_HOOKS_ENABLE`             | 0     | Kein zusätzlicher Speicher (externer Flash, EEPROM)     |
| `BOOT_NVM_CHECKSUM_HOOKS_ENABLE`    | 1     | Eigene Checksum-Überprüfung über Hook-Funktion          |
| `BOOT_FLASH_VECTOR_TABLE_CS_OFFSET` | 0x1F8 | Offset der Checksum-Adresse in der Vektortabelle        |

### Info-Tabelle

| Parameter                | Wert         | Beschreibung                                              |
|--------------------------|--------------|-----------------------------------------------------------|
| `BOOT_INFO_TABLE_ENABLE` | 1            | Info-Tabellen-Prüfung aktiviert                           |
| `BOOT_INFO_TABLE_LEN`    | 12           | Länge der Info-Tabelle: 12 Bytes                          |
| `BOOT_INFO_TABLE_ADDR`   | `0x08008200` | Adresse der Info-Tabelle in der User-Application          |

### Watchdog und Backdoor

| Parameter                    | Wert | Beschreibung                                                    |
|------------------------------|------|-----------------------------------------------------------------|
| `BOOT_COP_HOOKS_ENABLE`      | 1    | Watchdog-Hooks aktiviert, bedienen hier nur die LED (kein IWDG) |
| `BOOT_BACKDOOR_HOOKS_ENABLE` | 0    | Standard-Backdoor mit 500 ms Zeitfenster (kein eigener Hook)    |
| `BOOT_XCP_SEED_KEY_ENABLE`   | 0    | Seed/Key-Sicherheitsmechanismus deaktiviert                     |

## Flash-Aufteilung

Die Aufteilung steht in `flashLayout[]` in `flash.c`. Sektor 0 ist für den Bootloader
reserviert, die Application beginnt mit Sektor 1.

| Bereich     | Startadresse | Endadresse   | Größe   | Sektoren |
|-------------|--------------|--------------|---------|----------|
| Bootloader  | `0x08000000` | `0x08007FFF` | 32 kB   | 0        |
| Application | `0x08008000` | `0x081FFFFF` | 2016 kB | 1–11     |

Der Flash-Treiber verwaltet die ersten `FLASH_WRITE_BLOCK_SIZE` Bytes (512 Byte = `0x200`) der
Application als Boot-Block gesondert und schreibt ihn erst am Ende des Updates. Darin liegen die
Vektortabelle des STM32F767 (126 Einträge, `0x1F8` Byte) und direkt dahinter die Checksumme
(`BOOT_FLASH_VECTOR_TABLE_CS_OFFSET`). Die Info-Tabelle liegt unmittelbar nach dem Boot-Block,
also an `Startadresse der Application + 0x200`. Wird die Startadresse der Application in
`flash.c` verschoben, muss `BOOT_INFO_TABLE_ADDR` entsprechend angepasst werden.

## Info-Tabelle

Die Info-Tabelle ermöglicht es, die Ziel-Firmware vor dem Flashen zu validieren. Der Host liest
sie aus der Firmware-Datei und sendet sie vor dem Löschen an den Bootloader. Dieser vergleicht
sie in `InfoTableCheckHook()` mit der Tabelle der aktuell programmierten Firmware.

| Offset | Feld              | Typ        | Geprüft |
|--------|-------------------|------------|---------|
| 0      | `tableId`         | `uint32_t` | ja, muss gleich sein |
| 4      | `productId`       | `uint32_t` | ja, muss gleich sein |
| 8      | `firmwareVersion` | `uint32_t` | nein    |

Weichen `tableId` oder `productId` ab, wird das Update abgebrochen, bevor etwas gelöscht wird.

## Shared Parameters

Über `shared_params` können Parameter aus dem RAM zwischen Bootloader und Application
ausgetauscht werden, ohne dass ein Flash-Schreibvorgang notwendig ist. Der Puffer liegt in der
Linker-Sektion `.shared` und enthält eine feste Kennung, 56 Datenbytes und eine Checksumme.
Typische Verwendung: Die Application setzt ein Flag im RAM, um den Bootloader beim nächsten Reset
zum Verbleib im Bootloader-Modus zu veranlassen. Die Auswertung eines solchen Flags ist im
Bootloader derzeit nicht umgesetzt, `AppInit()` initialisiert nur den Puffer.

## Versionierung

Das Projekt führt zwei Versionsnummern:

- **Projektversion** in `blt_version.h` (`BLT_VERSION_MAJOR`, `_MINOR`, `_PATCH`, `_DEV`): die
eigene Versionsnummer nach dem [Documentation Style](https://github.com/Diveturtle93/Coding_Styleguide/blob/main/docs/DOCUMENTATION_STYLE.md#6-versionierung).
Sie wird bei jedem Release angepasst und gehört zum Tag und zum [CHANGELOG](CHANGELOG.md).
- **OpenBLT-Version** in `boot.h` (`BOOT_VERSION_CORE_MAIN`, `_MINOR`, `_PATCH`): die
Original-Versionsnummer des OpenBLT-Kerns von Feaser, auf dem die Bibliothek beruht (1.21.0).
Sie ändert sich nur, wenn ein neuer OpenBLT-Stand übernommen wird.

## Fehlerbilder

| Symptom | Ursache | Abhilfe |
|---------|---------|---------|
| Linker-Fehler `multiple definition of HAL_GetTick` | HAL-Zeitbasis auf einen Timer gestellt (`stm32f7xx_hal_timebase_tim.c`) | In CubeMX unter *SYS → Timebase Source* SysTick wählen |
| Bootloader hängt nach dem Start, LED blinkt nicht | Systemtakt weicht von 216 MHz ab, `CanGetSpeedConfig()` findet kein Bittiming und `ASSERT_RT` greift | Takt in CubeMX auf HSE 25 MHz / 216 MHz stellen oder `blt_conf.h` anpassen |
| Keine CAN-Verbindung, LED blinkt | CAN3 in CubeMX nicht aktiviert, `HAL_CAN_MspInit()` setzt Pins und Takt nicht | CAN3 in CubeMX aktivieren und Code erzeugen |
| Update bricht beim Programmieren unter `0x08008000` ab | Application für `0x08000000` gelinkt | Linker-Skript der Application auf `ORIGIN = 0x8008000` ändern |
| Application stürzt nach dem Sprung sofort ab | Vektortabelle nicht verschoben | In der Application `VECT_TAB_OFFSET` auf `0x00008000U` setzen |
| Update wird abgelehnt, bevor gelöscht wird | Info-Tabelle fehlt in der Application, liegt nicht an `0x08008200` oder hat eine andere `productId` | Info-Tabelle wie unter [Verwendung](#4-application-vorbereiten) anlegen |
| Shared Parameters nach dem Sprung verloren | Sektion `.shared` liegt in Bootloader und Application an unterschiedlichen Adressen oder wird initialisiert | `.shared` in beiden Linker-Skripten an dieselbe Adresse legen, als `NOLOAD` |

## API

Aufgerufen wird nur `app.h`, der Rest wird über `blt_conf.h` und `hooks.c` angepasst.

```c
// app.h
void AppInit (void);										// Bootloader initialisieren, einmalig nach der CubeMX-Initialisierung
void AppTask (void);										// Bootloader zyklisch in der Hauptschleife ausfuehren

// shared_params.h
void SharedParamsInit (void);								// Puffer initialisieren (in AppInit() enthalten)
bool SharedParamsReadByIndex (uint32_t idx, uint8_t * value);	// Datenbyte am Index lesen
bool SharedParamsWriteByIndex (uint32_t idx, uint8_t value);	// Datenbyte am Index schreiben
```

### Hook-Funktionen in `hooks.c`

| Funktion | Aktiv | Aufgabe |
|----------|-------|---------|
| `CpuUserProgramStartHook()` | ja | Taster `PC13` abfragen, LED beenden, Start der Application freigeben |
| `CopInitHook()` / `CopServiceHook()` | ja | LED initialisieren bzw. blinken lassen |
| `NvmVerifyChecksumHook()` / `NvmWriteChecksumHook()` | ja | Checksumme der Application prüfen bzw. schreiben, derzeit ohne Funktion |
| `InfoTableCheckHook()` | ja | `tableId` und `productId` vergleichen |
| `BackDoorInitHook()` / `BackDoorEntryHook()` | nein | eigener Backdoor-Eintritt |
| `NvmInitHook()`, `NvmWriteHook()`, `NvmEraseHook()`, `NvmDoneHook()` | nein | zusätzlicher Speicher |
| `XcpGetSeedHook()` / `XcpVerifyKeyHook()` | nein | Seed/Key-Schutz |

## Verwendung

### 1. Dateien einbinden

Die Bibliothek als Submodul in das STM32CubeIDE-Projekt des Bootloaders einbinden, z. B. unter
`Lib/STM32F7_MicroBoot`, und den Ordner als Include- und Quellpfad eintragen:

```sh
git submodule add https://github.com/Diveturtle93/STM32F7_MicroBoot.git Lib/STM32F7_MicroBoot
```

### 2. CubeMX konfigurieren

| Einstellung | Wert |
|-------------|------|
| Takt | HSE 25 MHz, Systemtakt 216 MHz |
| CAN3 | aktiviert, Parameter werden von `can_t.c` überschrieben, keine Interrupts |
| `PB7` | GPIO Output (LED) |
| `PC13` | GPIO Input (Taster) |
| SYS → Timebase Source | SysTick |
| Linker-Skript | `FLASH (rx) : ORIGIN = 0x8000000, LENGTH = 32K` |

### 3. Initialisieren und Task aufrufen

```c
#include "app.h"

int main (void)
{
	// HAL, Takt und Peripherie aus CubeMX initialisieren
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();

	// Bootloader initialisieren
	AppInit();

	while (1)
	{
		// Bootloader-Task ausfuehren
		AppTask();
	}
}
```

### 4. Application vorbereiten

Die Application, die über den Bootloader aufgespielt wird, braucht:

1. **Linker-Skript:** `FLASH (rx) : ORIGIN = 0x8008000, LENGTH = 2016K`.
2. **Vektortabelle:** In `system_stm32f7xx.c` `USER_VECT_TAB_ADDRESS` aktivieren und
   `VECT_TAB_OFFSET` auf `0x00008000U` setzen.
3. **Platz für die Checksumme:** Am Ende von `g_pfnVectors` in der Startup-Datei ein zusätzliches
   Wort eintragen (`.word 0x55AA11EE`), damit an Offset `0x1F8` kein Code liegt.
4. **Info-Tabelle** an `0x08008200`, mit derselben `tableId` und `productId` wie die Firmware im
   Gerät:
   ```c
   __attribute__ ((section (".info_table"), used)) const uint32_t infoTable[3] = {0x11223344, 1234, 10000};
   ```
   Im Linker-Skript direkt nach `.isr_vector`:
   ```ld
   .info_table 0x08008200 : { KEEP(*(.info_table)) } >FLASH
   ```
5. **Ausgabedatei:** Unter *MCU Post build outputs* „Convert to Motorola S-record file
   (-O srec)“ aktivieren. Übertragen wird die `.srec`.

Die Handgriffe am Gerät stehen im [Runbook](RUNBOOK.md).

## Hinweise

- Die Checksum-Hooks in `hooks.c` geben immer `BLT_TRUE` zurück. Es wird keine Checksumme
  geschrieben oder geprüft. Der Bootloader springt deshalb auch in eine leere oder halb
  geschriebene Application, und die Info-Tabelle wird auch bei leerem Flash verglichen. Ein
  leeres Gerät nimmt dadurch keine Firmware über CAN an.
- TIM1 und CAN3 sind vom Bootloader belegt. `TimerReset()` und `HAL_DeInit()` setzen sie vor dem
  Sprung zurück, die Application kann sie danach frei nutzen.
- Host-Tools: [MicroBoot](https://www.feaser.com/openblt/doku.php?id=manual:microboot) oder
  [BootCommander](https://www.feaser.com/openblt/doku.php?id=manual:bootcommander) und ein
  CAN-Adapter, den diese unterstützen (z. B. PCAN-USB).

## Quellen

- [Feaser/OpenBLT – STM32F7 Nucleo Demo](https://github.com/feaser/openblt/tree/master/Target/Demo/ARMCM7_STM32F7_Nucleo_F767ZI_CubeIDE)
- [OpenBLT-Handbuch](https://www.feaser.com/openblt/doku.php?id=manual:start)

## Abhängigkeiten

- `stm32f7xx.h` – STM32F7 CMSIS und HAL (CAN, Flash, GPIO)
- `stm32f7xx_ll_rcc.h`, `stm32f7xx_ll_bus.h` – STM32F7 LL-Treiber für Takt und TIM1

## Lizenz

Dieses Projekt steht unter der [GPL-3.0 Lizenz](LICENSE).
