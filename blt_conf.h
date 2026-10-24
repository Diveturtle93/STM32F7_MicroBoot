//----------------------------------------------------------------------
// Titel	:	blt_conf.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Demo/ARMCM7_STM32F7_Nucleo_F767ZI_CubeIDE/Boot/App/blt_conf.h
//----------------------------------------------------------------------

// Lizenz
//----------------------------------------------------------------------
// Copyright (c) 2020  by Feaser    http://www.feaser.com    All rights reserved
//
// This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You have received a copy of the GNU General Public License along with OpenBLT. It
// should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
//----------------------------------------------------------------------

// Sicherheitssymbol
//----------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------

// Dateiheader definieren
//----------------------------------------------------------------------
#ifndef INC_BLT_CONF_H_
#define INC_BLT_CONF_H_
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// CPU-Treiber konfigurieren
//----------------------------------------------------------------------
// Fuer die Baudrate der Kommunikationsschnittstelle werden Quarz- und Systemtakt
// benoetigt (BOOT_CPU_XTAL_SPEED_KHZ, BOOT_CPU_SYSTEM_SPEED_KHZ). Fuer den
// Datenaustausch mit dem Host muss die Byte-Reihenfolge bekannt sein:
// BOOT_CPU_BYTE_ORDER_MOTOROLA = 1 fuer Big Endian, 0 fuer Little Endian.
// Mit BOOT_CPU_USER_PROGRAM_START_HOOK = 1 wird kurz vor dem Start des
// Anwenderprogramms eine Hook-Funktion aufgerufen, z. B. um eine LED abzuschalten.
#define BOOT_CPU_XTAL_SPEED_KHZ					(25000)						// Frequenz des externen Quarzes in kHz
#define BOOT_CPU_SYSTEM_SPEED_KHZ				(216000)					// Gewuenschter Systemtakt in kHz
#define BOOT_CPU_BYTE_ORDER_MOTOROLA			(0)							// Byte-Reihenfolge Motorola (1) oder Intel (0)
#define BOOT_CPU_USER_PROGRAM_START_HOOK		(1)							// Hook-Funktion vor dem Start des Anwenderprogramms
//----------------------------------------------------------------------

// Kommunikationsschnittstelle konfigurieren
//----------------------------------------------------------------------
// Die CAN-Schnittstelle wird mit BOOT_COM_CAN_ENABLE = 1 ausgewaehlt.
// BOOT_COM_CAN_BAUDRATE legt die Geschwindigkeit in Bit/s fest. Zwei CAN-Nachrichten
// sind fuer die Kommunikation mit dem Host reserviert: BOOT_COM_CAN_TX_MSG_ID vom
// Steuergeraet zum Host und BOOT_COM_CAN_RX_MSG_ID vom Host zum Steuergeraet. Eine
// 29-Bit Extended-ID wird durch Verodern mit 0x80000000 eingestellt.
// BOOT_COM_CAN_CHANNEL_INDEX waehlt den CAN-Controller (beginnend bei 0).
#define BOOT_COM_CAN_ENABLE						(1)							// CAN-Transportschicht ein/aus
#define BOOT_COM_CAN_BAUDRATE					(500000)					// CAN-Baudrate in Bit/s
#define BOOT_COM_CAN_TX_MSG_ID					(0x7E1)						// CAN-ID Steuergeraet -> Host (Extended: | 0x80000000)
#define BOOT_COM_CAN_RX_MSG_ID					(0x7E0)						// CAN-ID Host -> Steuergeraet (Extended: | 0x80000000)
#define BOOT_COM_CAN_CHANNEL_INDEX				(2)							// CAN-Peripherie, Index beginnend bei 0 (2 = CAN3)
//----------------------------------------------------------------------

// Backdoor-Eintritt konfigurieren
//----------------------------------------------------------------------
// Ueber Hook-Funktionen kann ein anwendungsspezifischer Weg implementiert werden, um den
// Bootloader nach einem Reset aktiv zu halten. Das ist hilfreich, wenn das
// Anwenderprogramm nicht korrekt laeuft und den Bootloader nicht selbst aktivieren kann.
// Die Hook-Funktionen ersetzen den Standard-Backdoor und koennen aus Sicherheitsgruenden
// den Backdoor-Eintritt auch komplett abschalten.
#define BOOT_BACKDOOR_HOOKS_ENABLE				(0)							// Hook-Funktionen fuer den Backdoor ein/aus
//----------------------------------------------------------------------

// NVM-Treiber konfigurieren
//----------------------------------------------------------------------
// Der NVM-Treiber loescht und programmiert den internen Speicher des Controllers. Ueber
// Hook-Funktionen kann er um weitere Speicher (externer Flash, EEPROM) erweitert werden.
// BOOT_NVM_SIZE_KB gibt die Groesse des internen Speichers in kB an. Mit
// BOOT_NVM_CHECKSUM_HOOKS_ENABLE kann die interne Checksummen-Methode durch eine
// anwendungsspezifische ersetzt werden.
#define BOOT_NVM_HOOKS_ENABLE					(0)							// Hook-Funktionen fuer zusaetzliche Speicher ein/aus
#define BOOT_NVM_SIZE_KB						(2048)						// Groesse des internen Flash in kB
#define BOOT_NVM_CHECKSUM_HOOKS_ENABLE			(0)							// Hook-Funktionen fuer die Checksumme ein/aus
//----------------------------------------------------------------------

// Flash-Treiber konfigurieren
//----------------------------------------------------------------------
// Der Controller hat eine groessere Vektortabelle als im Bootloader-Kern angenommen.
// Die Checksumme liegt am Ende der Vektortabelle des Anwenderprogramms.
#define BOOT_FLASH_VECTOR_TABLE_CS_OFFSET		(0x1F8)						// Offset der Checksumme in der Vektortabelle
//----------------------------------------------------------------------

// Watchdog-Treiber konfigurieren
//----------------------------------------------------------------------
// Der Watchdog ist anwendungsspezifisch und wird daher nicht im Bootloader konfiguriert.
// Wird ein Watchdog verwendet, kann er ueber Hook-Funktionen initialisiert und bedient
// werden.
#define BOOT_COP_HOOKS_ENABLE					(1)							// Hook-Funktionen fuer den Watchdog ein/aus
//----------------------------------------------------------------------

// Info-Tabelle konfigurieren
//----------------------------------------------------------------------
// Die Info-Tabelle ist eine konstante Struktur an fester Adresse in der eigenen Firmware,
// z. B. mit Produkttyp, Firmware-Version, Hardware-Revision oder Seriennummer.
// BOOT_INFO_TABLE_ADDR und BOOT_INFO_TABLE_LEN geben Adresse und Laenge an.
// Ist die Funktion aktiv, wird die Info-Tabelle zu Beginn des Updates aus der neuen
// Firmware-Datei in einen RAM-Puffer kopiert und InfoTableCheckHook() aufgerufen. Dort
// wird sie mit der Tabelle der aktuell programmierten Firmware verglichen. Rueckgabe
// BLT_TRUE setzt das Update fort, BLT_FALSE bricht es ab. So wird z. B. verhindert,
// dass versehentlich Firmware fuer ein anderes Produkt programmiert wird.
// Dies ist eine Komfort- und keine Sicherheitsfunktion, dafuer ist Seed/Key gedacht.
#define BOOT_INFO_TABLE_ENABLE					(1)							// Info-Tabelle ein (1) / aus (0)
#define BOOT_INFO_TABLE_LEN						(12)						// Laenge der Info-Tabelle in Bytes

// Feste Adresse der Info-Tabelle. Sie liegt direkt hinter dem "Boot-Block", den der
// Flash-Treiber als die ersten FLASH_WRITE_BLOCK_SIZE Bytes des Anwenderprogramms festlegt.
#define BOOT_INFO_TABLE_ADDR					(0x08008200)				// Adresse der Info-Tabelle
//----------------------------------------------------------------------

// Seed/Key-Sicherheit konfigurieren
//----------------------------------------------------------------------
// Mit BOOT_XCP_SEED_KEY_ENABLE = 1 wird ein Sicherheitsmechanismus im XCP-Modul
// aktiviert. Vor dem Loeschen oder Programmieren muss die Ressource entsperrt werden.
// In MicroBoot wird dafuer unter "XCP Protection" eine DLL mit dem Entsperr-Algorithmus
// angegeben (Beispiel: "libseednkey.dll"). MicroBoot fordert einen Seed vom Bootloader
// an, berechnet daraus einen Schluessel und sendet ihn zurueck. Der Bootloader prueft
// den Schluessel ueber XcpGetSeedHook() und XcpVerifyKeyHook().
#define BOOT_XCP_SEED_KEY_ENABLE				(0)							// Seed/Key-Mechanismus ein/aus
//----------------------------------------------------------------------

#endif /* INC_BLT_CONF_H_ */
//----------------------------------------------------------------------
