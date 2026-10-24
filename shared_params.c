//----------------------------------------------------------------------
// Titel	:	shared_params.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Demo/ARMCM7_STM32F7_Nucleo_F767ZI_CubeIDE/Boot/App/shared_params.c
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
#include <stddef.h>
//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------
#include "shared_params.h"
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------
// Parameterpuffer, der von diesem Modul verwaltet wird. Damit Bootloader und
// Anwenderprogramm Daten austauschen koennen, muss die Variable in beiden an derselben
// RAM-Adresse liegen und darf vom C-Startup-Code (vor main()) nicht genullt werden.
// Bei GCC wird die Variable dafuer in die Sektion ".shared" gelegt. Im Linkerskript
// wird unter SECTIONS folgendes ergaenzt:
//
//   .shared (NOLOAD) :
//   {
//     . = ALIGN(4);
//     _sshared = .;
//     __shared_start__ = _sshared;
//     *(.shared)
//     *(.shared.*)
//     KEEP(*(.shared))
//     . = ALIGN(4);
//     _eshared = .;
//     __shared_end__ = _eshared;
//   } >SHARED
//
// Zusaetzlich unter MEMORY einen Eintrag SHARED am RAM-Anfang anlegen und den
// restlichen RAM verkleinern:
//
//   SHARED (xrw)   : ORIGIN = 0x200000C0, LENGTH = 64
//   RAM (xrw)      : ORIGIN = 0x20000100, LENGTH = 32K - 192 - 64
//
// Das Beispiel gilt fuer einen STM32F0, bei dem die ersten 192 (0xC0) Bytes im RAM
// fuer die Vektortabelle des Anwenderprogramms reserviert sind. Fuer andere Toolchains
// (Keil MDK, IAR EWARM) siehe deren Handbuecher:
//   IAR EWARM: https://www.iar.com/support/tech-notes/compiler/linker-error-for-absolute-located-variable/
//   Keil MDK:  http://www.keil.com/support/docs/3480.htm
static tSharedParamsBuffer sharedParamsBuffer __attribute__ ((section (".shared")));
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
static bool SharedParamsValidateBuffer (void);								// Pufferinhalt pruefen
static void SharedParamsWriteChecksum (void);								// Checksumme berechnen und schreiben
static bool SharedParamsVerifyChecksum (void);								// Checksumme pruefen
static uint16_t SharedParamsCalculateChecksum (void);						// Checksumme berechnen
//----------------------------------------------------------------------

// Modul fuer gemeinsame RAM-Parameter initialisieren
//----------------------------------------------------------------------
void SharedParamsInit (void)
{
	uint32_t byteIdx;

	// Der Puffer wird nicht vom C-Startup-Code initialisiert. Hat ein zuvor laufendes
	// Programm ihn bereits initialisiert, ist nichts weiter zu tun.
	if (!SharedParamsValidateBuffer())
	{
		// Puffer noch nicht initialisiert, typischerweise nach einem Kaltstart. Kennung
		// setzen, Parameterdaten nullen und Checksumme aktualisieren.
		sharedParamsBuffer.identifier = SHARED_PARAMS_BUFFER_ID;
		for (byteIdx = 0; byteIdx < SHARED_PARAMS_CFG_BUFFER_DATA_LEN; byteIdx++)
		{
			sharedParamsBuffer.data[byteIdx] = 0;
		}
		SharedParamsWriteChecksum();
	}
}
//----------------------------------------------------------------------

// Datenbyte am angegebenen Index aus dem Parameterpuffer lesen
// idx = Index (0 bis SHARED_PARAMS_CFG_BUFFER_DATA_LEN - 1), value = Zeiger auf das Ergebnis
// Rueckgabe true bei Erfolg, sonst false
//----------------------------------------------------------------------
bool SharedParamsReadByIndex (uint32_t idx, uint8_t * value)
{
	bool result = false;

	// Nur fortfahren, wenn Puffer und Parameter gueltig sind
	if ((SharedParamsValidateBuffer()) && (idx < SHARED_PARAMS_CFG_BUFFER_DATA_LEN) && (value != NULL))
	{
		// Wert lesen
		*value = sharedParamsBuffer.data[idx];
		result = true;
	}

	// Ergebnis zurueckgeben
	return result;
}
//----------------------------------------------------------------------

// Datenbyte am angegebenen Index in den Parameterpuffer schreiben
// idx = Index (0 bis SHARED_PARAMS_CFG_BUFFER_DATA_LEN - 1), value = zu schreibender Wert
// Rueckgabe true bei Erfolg, sonst false
//----------------------------------------------------------------------
bool SharedParamsWriteByIndex (uint32_t idx, uint8_t value)
{
	bool result = false;

	// Nur fortfahren, wenn Puffer und Parameter gueltig sind
	if ((SharedParamsValidateBuffer()) && (idx < SHARED_PARAMS_CFG_BUFFER_DATA_LEN))
	{
		// Wert schreiben
		sharedParamsBuffer.data[idx] = value;

		// Checksumme aktualisieren, da sich der Inhalt geaendert hat
		SharedParamsWriteChecksum();

		// Ergebnis aktualisieren
		result = true;
	}

	// Ergebnis zurueckgeben
	return result;
}
//----------------------------------------------------------------------

// Pufferinhalt anhand von Kennung und Checksumme pruefen
// Rueckgabe true wenn gueltig, sonst false
//----------------------------------------------------------------------
static bool SharedParamsValidateBuffer (void)
{
	bool result = false;

	// Pruefung durchfuehren
	if ((sharedParamsBuffer.identifier == SHARED_PARAMS_BUFFER_ID) && (SharedParamsVerifyChecksum()))
	{
		// Puffer ist gueltig
		result = true;
	}

	// Ergebnis zurueckgeben
	return result;
}
//----------------------------------------------------------------------

// Checksumme berechnen und in den Puffer schreiben
//----------------------------------------------------------------------
static void SharedParamsWriteChecksum (void)
{
	// Checksumme berechnen und schreiben
	sharedParamsBuffer.checksum = SharedParamsCalculateChecksum();
}
//----------------------------------------------------------------------

// Checksumme im Puffer berechnen und pruefen
// Rueckgabe true wenn die Checksumme korrekt ist, sonst false
//----------------------------------------------------------------------
static bool SharedParamsVerifyChecksum (void)
{
	bool result = false;

	// Checksumme berechnen und vergleichen
	if (SharedParamsCalculateChecksum() == sharedParamsBuffer.checksum)
	{
		// Checksumme korrekt
		result = true;
	}

	// Ergebnis zurueckgeben
	return result;
}
//----------------------------------------------------------------------

// Checksumme ueber den aktuellen Pufferinhalt berechnen. Summe aller Bytes (ohne die
// Checksumme selbst), davon das Zweierkomplement.
//----------------------------------------------------------------------
static uint16_t SharedParamsCalculateChecksum (void)
{
	uint16_t result = 0;
	uint32_t byteIdx;

	// Bytes der Kennung addieren
	result += (uint8_t)sharedParamsBuffer.identifier;
	result += (uint8_t)(sharedParamsBuffer.identifier >> 8u);
	result += (uint8_t)(sharedParamsBuffer.identifier >> 16u);
	result += (uint8_t)(sharedParamsBuffer.identifier >> 24u);

	// Alle Parameterdaten addieren
	for (byteIdx = 0; byteIdx < SHARED_PARAMS_CFG_BUFFER_DATA_LEN; byteIdx++)
	{
		result += (uint8_t)sharedParamsBuffer.data[byteIdx];
	}

	// Einerkomplement bilden
	result = ~result;

	// Zweierkomplement bilden
	result += 1;

	// Ergebnis zurueckgeben
	return result;
}
//----------------------------------------------------------------------
