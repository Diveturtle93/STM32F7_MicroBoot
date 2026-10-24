//----------------------------------------------------------------------
// Titel	:	nvm.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/nvm.h
//----------------------------------------------------------------------

// Lizenz
//----------------------------------------------------------------------
// Copyright (c) 2016  by Feaser    http://www.feaser.com    All rights reserved
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
#ifndef INC_NVM_H_
#define INC_NVM_H_
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

// Definiere Debug-Symbole
//----------------------------------------------------------------------
#ifdef DEBUG
//	#define DEBUG_NVM
#endif
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
// Rueckgabewerte der Hook-Funktionen NvmWriteHook/NvmEraseHook
#define BLT_NVM_ERROR							(0x00)						// Fehler
#define BLT_NVM_OKAY							(0x01)						// Erfolgreich
#define BLT_NVM_NOT_IN_RANGE					(0x02)						// Adresse nicht im Bereich
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
void NvmInit (void);														// NVM-Treiber initialisieren
blt_bool NvmWrite (blt_addr addr, blt_int32u len, blt_int8u *data);			// NVM programmieren
blt_bool NvmErase (blt_addr addr, blt_int32u len);							// NVM loeschen
blt_bool NvmVerifyChecksum (void);											// Checksumme des Anwenderprogramms pruefen
blt_addr NvmGetUserProgBaseAddress (void);									// Startadresse des Anwenderprogramms
blt_bool NvmDone (void);													// Programmierung abschliessen
//----------------------------------------------------------------------

#endif /* INC_NVM_H_ */
//----------------------------------------------------------------------
