# Changelog

Alle nennenswerten Änderungen an diesem Projekt werden hier dokumentiert.
Format nach [Keep a Changelog](https://keepachangelog.com/de/1.1.0/),
Versionierung nach [Semantic Versioning](https://semver.org/lang/de/).

## [Unveröffentlicht]

### Hinzugefügt
- OpenBLT-Bootloader (Kern 1.21.0) für den STM32F767ZI auf Basis des Nucleo-F767ZI-Demos von Feaser.
- Firmware-Update über CAN3 mit 500 kBit/s und XCP, CAN-IDs `0x7E0` (Host → Steuergerät) und `0x7E1` (Steuergerät → Host).
- Zentrale Konfiguration des Bootloaders in `blt_conf.h`.
- Flash-Aufteilung mit 32 kB für den Bootloader in Sektor 0 und der Application ab `0x08008000`.
- Zeitgesteuerter Backdoor-Eintritt: Der Bootloader bleibt nach einem Reset 500 ms aktiv, bevor er die Application startet.
- Verbleib im Bootloader durch Halten des User-Tasters `PC13` während des Resets.
- LED an `PB7` blinkt während des Bootloader-Betriebs im 100-ms-Takt.
- Info-Tabelle an `0x08008200`, die ein Update nur bei gleicher Tabellen- und Produktkennung zulässt.
- Hook-Funktionen für Checksumme, Watchdog, Programmstart und Seed/Key in `hooks.c`.
- Gemeinsame Parameter zwischen Bootloader und Application im RAM über `shared_params`.
- Projektversion in `blt_version.h`.
- Quelltexte auf den C Coding Style umgestellt, Kommentare auf Deutsch.
- README mit Konfiguration, Flash-Aufteilung, Einbindung und Versionierung.
- ARCHITECTURE mit Modulen, Start- und Update-Ablauf sowie Speicheraufteilung.
- RUNBOOK zum Aktualisieren mit MicroBoot oder BootCommander und zum Wiederherstellen per Taster.
