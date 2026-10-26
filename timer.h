//----------------------------------------------------------------------
// Titel	:	timer.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/timer.h
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
#ifndef INC_TIMER_H_
#define INC_TIMER_H_
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
//	#define DEBUG_TIMER
#endif
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#define TIMER_COUNTER_FREQ_HZ					(100000U)					// Frequenz des freilaufenden Zaehlers in Hz (100 kHz)
#define TIMER_COUNTS_PER_MS						(TIMER_COUNTER_FREQ_HZ / 1000U)	// Zaehlschritte pro Millisekunde
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
void TimerInit (void);														// Millisekunden-Timer initialisieren
void TimerUpdate (void);													// Millisekunden-Timer aktualisieren
blt_int32u TimerGet (void);													// Millisekunden seit dem Start
void TimerReset (void);														// Timer in den Reset-Zustand versetzen
//----------------------------------------------------------------------

#endif /* INC_TIMER_H_ */
//----------------------------------------------------------------------
