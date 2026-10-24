//----------------------------------------------------------------------
// Titel	:	shared_params.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Demo/ARMCM7_STM32F7_Nucleo_F767ZI_CubeIDE/Boot/App/shared_params.h
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
#ifndef INC_SHARED_PARAMS_H_
#define INC_SHARED_PARAMS_H_
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
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
//	#define DEBUG_SHARED_PARAMS
#endif
//----------------------------------------------------------------------

// Typedefines definieren
//----------------------------------------------------------------------
typedef struct t_shared_params_buffer
{
	uint32_t identifier;													// Feste Kennung, zeigt gueltige Parameter im RAM an
	uint8_t data[SHARED_PARAMS_CFG_BUFFER_DATA_LEN];						// Eigentliche Parameterdaten
	uint16_t checksum;														// Checksumme (Zweierkomplement der Summe aller Bytes ohne Checksumme)
} tSharedParamsBuffer;
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#define SHARED_PARAMS_CFG_BUFFER_DATA_LEN		(56u)						// Laenge der Parameterdaten in Bytes (ohne Kennung und Checksumme)
#define SHARED_PARAMS_BUFFER_ID					(0xCE42E7A2u)				// Feste Kennung am Pufferanfang zur Pruefung auf gueltige Parameter
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
void SharedParamsInit (void);												// Modul fuer gemeinsame Parameter initialisieren
bool SharedParamsReadByIndex (uint32_t idx, uint8_t * value);				// Datenbyte am Index lesen
bool SharedParamsWriteByIndex (uint32_t idx, uint8_t value);				// Datenbyte am Index schreiben
//----------------------------------------------------------------------

#endif /* INC_SHARED_PARAMS_H_ */
//----------------------------------------------------------------------
