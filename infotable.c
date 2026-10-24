//----------------------------------------------------------------------
// Titel	:	infotable.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/infotable.c
//----------------------------------------------------------------------

// Lizenz
//----------------------------------------------------------------------
// Copyright (c) 2025  by Feaser    http://www.feaser.com    All rights reserved
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

//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------
#include "boot.h"
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
	// RAM-Puffer fuer die Info-Tabelle, die zu Beginn des Updates aus der Firmware-Datei
	// gelesen wird. Bei Remote-Updates wird sie hierhin heruntergeladen, bei Updates von
	// lokalem Speicher direkt hierhin kopiert.
	static blt_int8u infoTableBuffer[BOOT_INFO_TABLE_LEN];

	static blt_int8u * infoTableWritePtr;									// Aktueller Schreibzeiger in den RAM-Puffer
#endif
//----------------------------------------------------------------------

// Hook-Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
	extern blt_bool InfoTableCheckHook (blt_addr newInfoTable, blt_addr currentInfoTable);	// Info-Tabellen durch die Anwendung vergleichen
#endif
//----------------------------------------------------------------------

// Info-Tabellen-Modul initialisieren
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
void InfoTableInit (void)
{
	// Modulvariablen initialisieren
	infoTableWritePtr = &infoTableBuffer[0];

	// Info-Tabelle im RAM-Puffer loeschen
	InfoTableClear(INFO_TABLE_ID_INTERNAL_RAM);
}
#endif
//----------------------------------------------------------------------

// Hook-Funktion zum Vergleich der Info-Tabellen aufrufen: neue Firmware (RAM) gegen
// aktuell programmierte Firmware (NVM). Damit kann die Anwendung entscheiden, ob das
// Update fortgesetzt werden darf, z. B. nur bei Firmware fuer den richtigen Produkttyp.
// Rueckgabe BLT_TRUE wenn das Update fortgesetzt werden darf, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
blt_bool InfoTableCheck (void)
{
	blt_bool result;
	blt_addr infoTableNewAddr;
	blt_addr infoTableCurrentAddr;

	// Adressen der Info-Tabellen ermitteln
	infoTableNewAddr = InfoTableGetPtr(INFO_TABLE_ID_INTERNAL_RAM);
	infoTableCurrentAddr = InfoTableGetPtr(INFO_TABLE_ID_FIRMWARE_NVM);

	// Eine Pruefung ist nur mit gueltiger Info-Tabelle moeglich, daher zuerst die
	// Checksumme der aktuellen Firmware pruefen
	// Keine gueltige Firmware vorhanden (noch nie programmiert oder letztes Update nicht
	// abgeschlossen), Update ohne weitere Pruefung erlauben
	if (NvmVerifyChecksum() == BLT_FALSE)
	{
		result = BLT_TRUE;
	}
	// Checksumme der aktuellen Firmware gueltig, Info-Tabelle sollte vorhanden sein
	else
	{
		// Pruefung durch die Bootloader-Anwendung durchfuehren lassen
		result = InfoTableCheckHook(infoTableNewAddr, infoTableCurrentAddr);
	}

	// Ergebnis zurueckgeben
	return result;
}
#endif
//----------------------------------------------------------------------

// Speicheradresse der gewaehlten Info-Tabelle zurueckgeben
// infoTableId = gewuenschte Info-Tabelle
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
blt_addr InfoTableGetPtr (tInfoTableId infoTableId)
{
	blt_addr result;

	// RAM-Puffer, hierhin wird die Info-Tabelle der neuen Firmware geschrieben
	if (infoTableId == INFO_TABLE_ID_INTERNAL_RAM)
	{
		result = (blt_addr)(&infoTableBuffer[0]);
	}
	// Info-Tabelle der aktuell programmierten Firmware. Eventuell ist keine Firmware
	// programmiert.
	else
	{
		result = (blt_addr)BOOT_INFO_TABLE_ADDR;
	}

	// Ergebnis zurueckgeben
	return result;
}
#endif
//----------------------------------------------------------------------

// RAM-Puffer der Info-Tabelle loeschen und Schreibzeiger auf den Anfang setzen
// infoTableId = muss INFO_TABLE_ID_INTERNAL_RAM sein, die NVM-Tabelle ist nicht aenderbar
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
void InfoTableClear (tInfoTableId infoTableId)
{
	blt_int16u idx;

	// Parameter pruefen
	ASSERT_RT(infoTableId == INFO_TABLE_ID_INTERNAL_RAM);

	// Nur mit gueltigem Parameter fortfahren
	if (infoTableId == INFO_TABLE_ID_INTERNAL_RAM)
	{
		// RAM-Puffer der Info-Tabelle loeschen
		for (idx = 0U; idx < BOOT_INFO_TABLE_LEN; idx++)
		{
			infoTableBuffer[idx] = 0U;
		}

		// Schreibzeiger zuruecksetzen
		infoTableWritePtr = &infoTableBuffer[0];
	}
}
#endif
//----------------------------------------------------------------------

// Anzahl aktuell gespeicherter Bytes einer Info-Tabelle bestimmen. Fuer
// INFO_TABLE_ID_FIRMWARE_NVM immer gleich, fuer INFO_TABLE_ID_INTERNAL_RAM die Anzahl
// bisher geschriebener Bytes.
// infoTableId = gewuenschte Info-Tabelle
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
blt_int16u InfoTableCurrentSize (tInfoTableId infoTableId)
{
	blt_int16u result = BOOT_INFO_TABLE_LEN + 1;							// Ungueltige Laenge (zu voll)

	// Anzahl gespeicherter Bytes ueber den Schreibzeiger berechnen, vorher pruefen
	if (infoTableId == INFO_TABLE_ID_INTERNAL_RAM)
	{
		if (infoTableWritePtr >= &infoTableBuffer[0])
		{
			result = (blt_int16u)((blt_addr)infoTableWritePtr-(blt_addr)(&infoTableBuffer[0]));
		}
	}
	// Info-Tabelle im NVM hat eine feste Groesse
	else
	{
		result = BOOT_INFO_TABLE_LEN;
	}

	// Ergebnis zurueckgeben
	return result;
}
#endif
//----------------------------------------------------------------------

// Daten zur RAM-Info-Tabelle hinzufuegen und Schreibzeiger weitersetzen
// infoTableId = muss INFO_TABLE_ID_INTERNAL_RAM sein, data = Zeiger auf die Daten,
// len = Anzahl Bytes
// Rueckgabe BLT_TRUE wenn die Daten hinzugefuegt wurden, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
blt_bool InfoTableAddData (tInfoTableId infoTableId, blt_int8u const * data, blt_int16u len)
{
	blt_bool result = BLT_FALSE;

	// Nur zur RAM-Info-Tabelle und mit gueltigem Datenzeiger hinzufuegen
	if ((infoTableId == INFO_TABLE_ID_INTERNAL_RAM) && (data != BLT_NULL) && (len > 0))
	{
		// Nur fortfahren, wenn die Daten noch in die Info-Tabelle passen
		if ((InfoTableCurrentSize(INFO_TABLE_ID_INTERNAL_RAM) + len) <= BOOT_INFO_TABLE_LEN)
		{
			// Daten in der RAM-Info-Tabelle speichern
			CpuMemCopy((blt_addr)infoTableWritePtr, (blt_addr)((blt_int32u)&data[0]), len);

			// Schreibzeiger weitersetzen
			infoTableWritePtr += len;

			// Ergebnis aktualisieren
			result = BLT_TRUE;
		}
	}

	// Ergebnis zurueckgeben
	return result;
}
#endif
//----------------------------------------------------------------------
