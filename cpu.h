//----------------------------------------------------------------------
// Titel	:	cpu.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/cpu.h
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
#ifndef INC_CPU_H_
#define INC_CPU_H_
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
//	#define DEBUG_CPU
#endif
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
void CpuInit (void);														// CPU-Modul initialisieren
void CpuStartUserProgram (void);											// Anwenderprogramm starten
void CpuMemCopy (blt_addr dest, blt_addr src, blt_int16u len);				// Speicher kopieren
void CpuMemSet (blt_addr dest, blt_int8u value, blt_int16u len);			// Speicher mit Wert fuellen
void CpuIrqDisable (void);													// Globale Interrupts sperren
void CpuIrqEnable (void);													// Globale Interrupts freigeben
//----------------------------------------------------------------------

#endif /* INC_CPU_H_ */
//----------------------------------------------------------------------
