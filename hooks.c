//----------------------------------------------------------------------
// Titel	:	hooks.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Demo/ARMCM7_STM32F7_Nucleo_F767ZI_CubeIDE/Boot/App/hooks.c
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

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------
#include "stm32f7xx.h"
//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------
#include "boot.h"
#include "led.h"
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// CPU-Treiber: Wird aufgerufen, bevor der Bootloader an das Anwenderprogramm uebergibt.
// Letzte Moeglichkeit fuer Pruefungen, um den Start des Anwenderprogramms zu verhindern.
// Rueckgabe BLT_TRUE wenn das Anwenderprogramm starten darf, BLT_FALSE um im Bootloader
// zu bleiben
//----------------------------------------------------------------------
#if (BOOT_CPU_USER_PROGRAM_START_HOOK > 0)
blt_bool CpuUserProgramStartHook (void)
{
	// Zusaetzlicher Backdoor-Eintritt ueber den Taster auf dem Board. Wird der Taster
	// waehrend des Resets gedrueckt gehalten, bleibt der Bootloader aktiv.
	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET)
		return BLT_FALSE;

	// LED-Treiber beenden
	LedBlinkExit();

	// Anwenderprogramm darf gestartet werden
	return BLT_TRUE;
}
#endif
//----------------------------------------------------------------------

// Watchdog-Treiber: Wird am Ende der COP-Initialisierung aufgerufen, um den Watchdog
// zu konfigurieren und einzuschalten
//----------------------------------------------------------------------
#if (BOOT_COP_HOOKS_ENABLE > 0)
void CopInitHook (void)
{
	// Wird bei der Initialisierung aufgerufen, daher hier auch den LED-Treiber
	// initialisieren. Die LED dient als optischer Watchdog.
	LedBlinkInit(100);
}
#endif
//----------------------------------------------------------------------

// Watchdog-Treiber: Wird am Ende der COP-Service-Routine aufgerufen, auch waehrend
// langer Schleifen. Dient zum Bedienen des Watchdogs, um einen Reset zu verhindern.
//----------------------------------------------------------------------
#if (BOOT_COP_HOOKS_ENABLE > 0)
void CopServiceHook (void)
{
	// LED-Blinktask hier statt in main() ausfuehren. Lange Operationen wie das Loeschen
	// des Flash rufen diese Funktion ebenfalls auf, so wird kein Blinkintervall ausgelassen.
	LedBlinkTask();
}
#endif
//----------------------------------------------------------------------

// Backdoor: Backdoor-Eintritt initialisieren
//----------------------------------------------------------------------
#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
void BackDoorInitHook (void)
{
	// Keine Initialisierung noetig
}
#endif
//----------------------------------------------------------------------

// Backdoor: Pruefen ob ein Backdoor-Eintritt angefordert ist
// Rueckgabe BLT_TRUE wenn der Backdoor-Eintritt angefordert ist, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
blt_bool BackDoorEntryHook (void)
{
	// Standard: Bootloader nach einem Reset immer aktivieren
	return BLT_TRUE;
}
#endif
//----------------------------------------------------------------------

// NVM-Treiber: Wird zu Beginn der NVM-Initialisierung aufgerufen
//----------------------------------------------------------------------
#if (BOOT_NVM_HOOKS_ENABLE > 0)
void NvmInitHook (void)
{
	// Keine Initialisierung noetig
}
#endif
//----------------------------------------------------------------------

// NVM-Treiber: Wird zu Beginn des Schreibvorgangs aufgerufen, um zusaetzlichen Speicher
// zu beschreiben. Liegt die Adresse nicht im zusaetzlichen Speicher, muss
// BLT_NVM_NOT_IN_RANGE zurueckgegeben werden.
// addr = Startadresse, len = Laenge in Bytes, data = Zeiger auf die Daten
// Rueckgabe BLT_NVM_OKAY, BLT_NVM_NOT_IN_RANGE oder BLT_NVM_ERROR
//----------------------------------------------------------------------
#if (BOOT_NVM_HOOKS_ENABLE > 0)
blt_int8u NvmWriteHook (blt_addr addr, blt_int32u len, blt_int8u *data)
{
	return BLT_NVM_NOT_IN_RANGE;
}
#endif
//----------------------------------------------------------------------

// NVM-Treiber: Wird zu Beginn des Loeschvorgangs aufgerufen, um zusaetzlichen Speicher
// zu loeschen. Liegt die Adresse nicht im zusaetzlichen Speicher, muss
// BLT_NVM_NOT_IN_RANGE zurueckgegeben werden.
// addr = Startadresse, len = Laenge in Bytes
// Rueckgabe BLT_NVM_OKAY, BLT_NVM_NOT_IN_RANGE oder BLT_NVM_ERROR
//----------------------------------------------------------------------
#if (BOOT_NVM_HOOKS_ENABLE > 0)
blt_int8u NvmEraseHook (blt_addr addr, blt_int32u len)
{
	return BLT_NVM_NOT_IN_RANGE;
}
#endif
//----------------------------------------------------------------------

// NVM-Treiber: Wird am Ende der Programmierung aufgerufen
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_NVM_HOOKS_ENABLE > 0)
blt_bool NvmDoneHook (void)
{
	return BLT_TRUE;
}
#endif
//----------------------------------------------------------------------

// NVM-Treiber: Checksumme pruefen, zeigt an ob ein gueltiges Anwenderprogramm vorhanden ist
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 0)
blt_bool NvmVerifyChecksumHook (void)
{
	return BLT_TRUE;
}
#endif
//----------------------------------------------------------------------

// NVM-Treiber: Checksumme des Anwenderprogramms schreiben, nachdem es vollstaendig
// programmiert wurde
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 0)
blt_bool NvmWriteChecksumHook (void)
{
	return BLT_TRUE;
}
#endif
//----------------------------------------------------------------------

// Info-Tabelle: Wird zu Beginn des Updates vor dem Loeschen und Programmieren aufgerufen.
// Vergleicht die Info-Tabellen, um zu entscheiden ob das Update fortgesetzt werden darf,
// z. B. nur bei Firmware fuer den richtigen Produkttyp.
// newInfoTable = Adresse der Info-Tabelle aus der neuen Firmware-Datei
// currentInfoTable = Adresse der Info-Tabelle der aktuell programmierten Firmware
// Rueckgabe BLT_TRUE wenn das Update fortgesetzt werden darf, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
blt_bool InfoTableCheckHook (blt_addr newInfoTable, blt_addr currentInfoTable)
{
	blt_bool result = BLT_FALSE;

	// Wichtig: Die Struktur muss den gleichen Aufbau wie die Info-Tabelle in der
	// Firmware haben
	struct firmwareInfoTable
	{
		blt_int32u tableId;													// Fester Wert zur Erkennung als Info-Tabelle
		blt_int32u productId;												// Produktkennung, z. B. 1234 = Luftpumpe
		blt_int32u firmwareVersion;											// Firmware-Version, z. B. 10429 = v1.4.29
	};

	// Adressen in Zeiger auf Info-Tabellen umwandeln
	struct firmwareInfoTable const * newInfoTablePtr = (void const *)newInfoTable;
	struct firmwareInfoTable const * currentInfoTablePtr = (void const *)currentInfoTable;

	// Konfigurierte Laenge der Info-Tabelle zur Compile-Zeit pruefen
	ASSERT_CT(BOOT_INFO_TABLE_LEN == sizeof(struct firmwareInfoTable));

	// Stimmen die Tabellenkennungen ueberein?
	if (currentInfoTablePtr->tableId == newInfoTablePtr->tableId)
	{
		// Update nur fuer Firmware des gleichen Produkttyps erlauben
		if (currentInfoTablePtr->productId == newInfoTablePtr->productId)
		{
			result = BLT_TRUE;
		}
	}

	// Ergebnis zurueckgeben
	return result;
}
#endif
//----------------------------------------------------------------------

// Seed/Key: Seed an den XCP-Master liefern, mit dem dieser den Schluessel zum Entsperren
// der Ressource erzeugt. Wird vom Befehl GET_SEED aufgerufen.
// resource = angefragte Ressource (XCP_RES_XXX), seed = Zeiger auf den Seed-Puffer
// Rueckgabe Laenge des Seeds in Bytes
//----------------------------------------------------------------------
#if (BOOT_XCP_SEED_KEY_ENABLE > 0)
blt_int8u XcpGetSeedHook (blt_int8u resource, blt_int8u *seed)
{
	// Seed zum Entsperren der Programmier-Ressource (PGM)
	if ((resource & XCP_RES_PGM) != 0)
	{
		seed[0] = 0x55;
	}

	// Laenge des Seeds zurueckgeben
	return 1;
}
#endif
//----------------------------------------------------------------------

// Seed/Key: Wird vom Befehl UNLOCK aufgerufen und prueft den Schluessel. Ist er korrekt,
// wird der Schutz der Ressource aufgehoben.
// resource = zu entsperrende Ressource (XCP_RES_XXX), key = Zeiger auf den Schluessel,
// len = Laenge des Schluessels in Bytes
// Rueckgabe 1 bei korrektem Schluessel, sonst 0
//----------------------------------------------------------------------
#if (BOOT_XCP_SEED_KEY_ENABLE > 0)
blt_int8u XcpVerifyKeyHook (blt_int8u resource, blt_int8u *key, blt_int8u len)
{
	// Compiler-Warnung fuer unbenutzten Parameter vermeiden
	len = len;

	// Beispiel-Algorithmus aus "libseednkey.dll": PGM wird entsperrt, wenn key = seed - 1
	// Schluessel fuer die Programmier-Ressource pruefen
	if ((resource == XCP_RES_PGM) && (key[0] == (0x55-1)))
		return 1;

	// Schluessel falsch
	return 0;
}
#endif
//----------------------------------------------------------------------
