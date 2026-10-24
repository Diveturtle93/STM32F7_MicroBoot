//----------------------------------------------------------------------
// Titel	:	infotable.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/infotable.h
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

// Sicherheitssymbol
//----------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------

// Dateiheader definieren
//----------------------------------------------------------------------
#ifndef INC_INFOTABLE_H_
#define INC_INFOTABLE_H_
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
//	#define DEBUG_INFOTABLE
#endif
//----------------------------------------------------------------------

// Typedefines definieren
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
	typedef enum
	{
		INFO_TABLE_ID_INTERNAL_RAM,											// 0 RAM-Puffer der zu programmierenden Firmware
		INFO_TABLE_ID_FIRMWARE_NVM											// 1 Aktuell programmierte Firmware im NVM
	} tInfoTableId;
#endif
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_INFO_TABLE_ENABLE > 0)
	void InfoTableInit (void);												// Info-Tabellen-Modul initialisieren
	blt_bool InfoTableCheck (void);											// Info-Tabellen vergleichen
	blt_addr InfoTableGetPtr (tInfoTableId infoTableId);					// Adresse einer Info-Tabelle
	void InfoTableClear (tInfoTableId infoTableId);							// RAM-Info-Tabelle loeschen
	blt_int16u InfoTableCurrentSize (tInfoTableId infoTableId);				// Aktuelle Groesse einer Info-Tabelle
	blt_bool InfoTableAddData (tInfoTableId infoTableId, blt_int8u const * data, blt_int16u len);	// Daten zur RAM-Info-Tabelle hinzufuegen
#endif
//----------------------------------------------------------------------

#endif /* INC_INFOTABLE_H_ */
//----------------------------------------------------------------------
