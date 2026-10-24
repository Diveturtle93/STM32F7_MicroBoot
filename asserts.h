//----------------------------------------------------------------------
// Titel	:	asserts.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/asserts.h
//----------------------------------------------------------------------

// Lizenz
//----------------------------------------------------------------------
// Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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
#ifndef INC_ASSERTS_H_
#define INC_ASSERTS_H_
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
//	#define DEBUG_ASSERTS
#endif
//----------------------------------------------------------------------

// Makros definieren
//----------------------------------------------------------------------
// ASSERT_CT fuer Pruefungen zur Compile-Zeit, ASSERT_RT fuer Pruefungen zur Laufzeit
#ifdef NDEBUG
	#define ASSERT_CT(cond)						((void)0)
	#define ASSERT_RT(cond)						((void)0)
#else
	#define ASSERT_CONCAT_(a, b)				a##b
	#define ASSERT_CONCAT(a, b)					ASSERT_CONCAT_(a, b)
	#define ASSERT_CT(cond)						enum { ASSERT_CONCAT(assert_error_on_line_, __LINE__) = 1/(!!(cond)) }
	#define ASSERT_RT(cond)						\
		if (cond) \
		{ ; } \
		else \
			AssertFailure(__FILE__, __LINE__)
#endif
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
#ifndef NDEBUG
	void AssertFailure (blt_char *file, blt_int32u line);					// Fehlgeschlagene Laufzeitpruefung behandeln
#endif
//----------------------------------------------------------------------

#endif /* INC_ASSERTS_H_ */
//----------------------------------------------------------------------
