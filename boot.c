//----------------------------------------------------------------------
// Titel	:	boot.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/boot.c
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

//----------------------------------------------------------------------

// Bootloader-Kern initialisieren
//----------------------------------------------------------------------
void BootInit (void)
{
	// CPU initialisieren
	CpuInit();

	// Watchdog initialisieren
	CopInit();

	// Millisekunden-Timer initialisieren
	TimerInit();

	// Treiber fuer nichtfluechtigen Speicher initialisieren
	NvmInit();

#if (BOOT_COM_ENABLE > 0)
	// Kommunikationsmodul initialisieren
	ComInit();
#endif

	// Backdoor-Eintritt initialisieren
	BackDoorInit();
}
//----------------------------------------------------------------------

// Task des Bootloader-Kerns, treibt das Programm an
//----------------------------------------------------------------------
void BootTask (void)
{
	// Watchdog bedienen
	CopService();

	// Millisekunden-Timer aktualisieren
	TimerUpdate();

#if (BOOT_COM_ENABLE > 0)
	// Eventuell anstehende Kommunikationsdaten verarbeiten
	ComTask();
#endif

	// Backdoor steuern
	BackDoorCheck();
}
//----------------------------------------------------------------------
