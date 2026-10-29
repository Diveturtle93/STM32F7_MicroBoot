//----------------------------------------------------------------------
// Titel	:	types.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/ARMCM7_STM32F7/types.h
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

// Sicherheitssymbol
//----------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------

// Dateiheader definieren
//----------------------------------------------------------------------
#ifndef INC_TYPES_H_
#define INC_TYPES_H_
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

// Konstanten definieren
//----------------------------------------------------------------------
#define BLT_TRUE								(1)							// Boolescher Wert wahr
#define BLT_FALSE								(0)							// Boolescher Wert falsch
#define BLT_NULL								((void *)0)					// Null-Zeiger
//----------------------------------------------------------------------

// Typedefines definieren
//----------------------------------------------------------------------
typedef unsigned char blt_bool;												// Boolescher Typ
typedef char blt_char;														// Zeichentyp
typedef unsigned long blt_addr;												// Speicheradresse
typedef unsigned char blt_int8u;											// 8-Bit vorzeichenlos
typedef signed char blt_int8s;												// 8-Bit vorzeichenbehaftet
typedef unsigned short blt_int16u;											// 16-Bit vorzeichenlos
typedef signed short blt_int16s;											// 16-Bit vorzeichenbehaftet
typedef unsigned int blt_int32u;											// 32-Bit vorzeichenlos
typedef signed int blt_int32s;												// 32-Bit vorzeichenbehaftet
//----------------------------------------------------------------------

#endif /* INC_TYPES_H_ */
//----------------------------------------------------------------------
