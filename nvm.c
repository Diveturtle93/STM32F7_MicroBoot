//----------------------------------------------------------------------
// Titel	:	nvm.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/ARMCM7_STM32F7/nvm.c
//----------------------------------------------------------------------

// Lizenz
//----------------------------------------------------------------------
// Copyright (c) 2018  by Feaser    http://www.feaser.com    All rights reserved
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
#include "flash.h"
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Hook-Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_NVM_HOOKS_ENABLE > 0)
	extern void NvmInitHook (void);											// NVM-Treiber der Anwendung initialisieren
	extern blt_int8u NvmWriteHook (blt_addr addr, blt_int32u len, blt_int8u *data);	// Zusaetzlichen Speicher programmieren
	extern blt_int8u NvmEraseHook (blt_addr addr, blt_int32u len);			// Zusaetzlichen Speicher loeschen
	extern blt_bool NvmDoneHook (void);										// NVM-Treiber der Anwendung abschliessen
#endif

#if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 0)
	extern blt_bool NvmWriteChecksumHook (void);							// Checksumme schreiben (anwendungsspezifisch)
	extern blt_bool NvmVerifyChecksumHook (void);							// Checksumme pruefen (anwendungsspezifisch)
#endif
//----------------------------------------------------------------------

// NVM-Treiber initialisieren
//----------------------------------------------------------------------
void NvmInit (void)
{
#if (BOOT_NVM_HOOKS_ENABLE > 0)
	// Anwendung kann einen Treiber fuer zusaetzlichen Speicher initialisieren
	NvmInitHook();
#endif

	// Internen Treiber initialisieren
	FlashInit();
}
//----------------------------------------------------------------------

// Nichtfluechtigen Speicher programmieren
// addr = Startadresse, len = Laenge in Bytes, data = Zeiger auf die Daten
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
blt_bool NvmWrite (blt_addr addr, blt_int32u len, blt_int8u *data)
{
#if (BOOT_NVM_HOOKS_ENABLE > 0)
	blt_int8u result = BLT_NVM_NOT_IN_RANGE;

	// Anwendung kann Speicher bearbeiten, der von diesem Treiber nicht unterstuetzt wird
	result = NvmWriteHook(addr, len, data);

	// Daten lagen im zusaetzlichen Speicher und wurden erfolgreich programmiert
	if (result == BLT_NVM_OKAY)
		return BLT_TRUE;
	// Daten lagen im zusaetzlichen Speicher, beim Programmieren trat ein Fehler auf
	else if (result == BLT_NVM_ERROR)
		return BLT_FALSE;
#endif

	// Programmierung ueber den internen Treiber
	return FlashWrite(addr, len, data);
}
//----------------------------------------------------------------------

// Nichtfluechtigen Speicher loeschen
// addr = Startadresse, len = Laenge in Bytes
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
blt_bool NvmErase (blt_addr addr, blt_int32u len)
{
#if (BOOT_NVM_HOOKS_ENABLE > 0)
	blt_int8u result = BLT_NVM_NOT_IN_RANGE;

	// Anwendung kann Speicher bearbeiten, der von diesem Treiber nicht unterstuetzt wird
	result = NvmEraseHook(addr, len);

	// Adresse lag im zusaetzlichen Speicher und wurde erfolgreich geloescht
	if (result == BLT_NVM_OKAY)
		return BLT_TRUE;
	// Adresse lag im zusaetzlichen Speicher, beim Loeschen trat ein Fehler auf
	else if (result == BLT_NVM_ERROR)
		return BLT_FALSE;
#endif

	// Loeschen ueber den internen Treiber
	return FlashErase(addr, len);
}
//----------------------------------------------------------------------

// Checksumme pruefen, zeigt an ob ein gueltiges Anwenderprogramm vorhanden ist
// Rueckgabe BLT_TRUE bei gueltiger Checksumme, sonst BLT_FALSE
//----------------------------------------------------------------------
blt_bool NvmVerifyChecksum (void)
{
#if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 0)
	// Checksumme ueber die anwendungsspezifische Methode pruefen
	return NvmVerifyChecksumHook();
#else
	// Checksumme ueber die interne Methode pruefen
	return FlashVerifyChecksum();
#endif
}
//----------------------------------------------------------------------

// Startadresse des Speichers fuer das Anwenderprogramm (Beginn der Vektortabelle)
//----------------------------------------------------------------------
blt_addr NvmGetUserProgBaseAddress (void)
{
	return FlashGetUserProgBaseAddress();
}
//----------------------------------------------------------------------

// Programmierung abschliessen. Wird nach allen Loesch- und Schreibvorgaengen direkt vor
// dem Software-Reset aufgerufen. Berechnet die Checksumme und schreibt sie in den Flash.
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
blt_bool NvmDone (void)
{
#if (BOOT_NVM_HOOKS_ENABLE > 0)
	// NVM-Treiber der Anwendung abschliessen
	if (NvmDoneHook() == BLT_FALSE)
		return BLT_FALSE;
#endif

#if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 0)
	// Checksumme ueber die anwendungsspezifische Methode berechnen und schreiben
	if (NvmWriteChecksumHook() == BLT_FALSE)
		return BLT_FALSE;
#else
	// Checksumme ueber den internen Treiber berechnen und schreiben
	if (FlashWriteChecksum() == BLT_FALSE)
		return BLT_FALSE;
#endif

	// Operationen des internen Treibers abschliessen
	return FlashDone();
}
//----------------------------------------------------------------------
