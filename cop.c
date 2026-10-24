//----------------------------------------------------------------------
// Titel	:	cop.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/cop.c
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

// Hook-Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_COP_HOOKS_ENABLE > 0)
	extern void CopInitHook (void);											// Watchdog der Anwendung initialisieren
	extern void CopServiceHook (void);										// Watchdog der Anwendung bedienen
#endif
//----------------------------------------------------------------------

// Watchdog initialisieren
//----------------------------------------------------------------------
void CopInit (void)
{
#if (BOOT_COP_HOOKS_ENABLE > 0)
	CopInitHook();
#endif
}
//----------------------------------------------------------------------

// Watchdog bedienen, damit dieser nicht ablaeuft
//----------------------------------------------------------------------
void CopService (void)
{
#if (BOOT_COP_HOOKS_ENABLE > 0)
	CopServiceHook();
#endif
}
//----------------------------------------------------------------------
