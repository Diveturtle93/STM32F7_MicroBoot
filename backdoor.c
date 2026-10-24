//----------------------------------------------------------------------
// Titel	:	backdoor.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/backdoor.c
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

// Konstanten definieren
//----------------------------------------------------------------------
// Zeit in ms, die die Backdoor geoeffnet ist. Kann durch Definition von
// BOOT_BACKDOOR_ENTRY_TIMEOUT_MS in blt_conf.h ueberschrieben werden.
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
	#ifndef BOOT_BACKDOOR_ENTRY_TIMEOUT_MS
		#define BOOT_BACKDOOR_ENTRY_TIMEOUT_MS	(500)
	#endif
#endif
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
	static blt_bool backdoorOpen;											// Backdoor geoeffnet oder geschlossen
	static blt_int32u backdoorOpenTime;										// Zeitpunkt, zu dem die Backdoor geoeffnet wurde

	// Verlaengerung der Backdoor-Zeit in ms. Muss hier und nicht in BackDoorInit()
	// mit 0 initialisiert werden, da BackDoorInit() erst am Ende von BootInit()
	// aufgerufen wird. So kann BackDoorSetExtension() schon vorher aufgerufen werden.
	static blt_int32u backdoorExtensionTime = 0;
#endif
//----------------------------------------------------------------------

// Hook-Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
	extern void BackDoorInitHook (void);									// Backdoor der Anwendung initialisieren
	extern blt_bool BackDoorEntryHook (void);								// Abfrage, ob im Bootloader geblieben werden soll
#endif
//----------------------------------------------------------------------

// Backdoor-Eintritt initialisieren
//----------------------------------------------------------------------
void BackDoorInit (void)
{
#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
	// Backdoor-Funktion der Anwendung initialisieren
	BackDoorInitHook();

	// Anwenderprogramm starten, wenn kein Backdoor-Eintritt angefordert ist
	if (BackDoorEntryHook() == BLT_FALSE)
	{
		// Kehrt nicht zurueck, wenn ein gueltiges Anwenderprogramm vorhanden ist
		CpuStartUserProgram();
	}
#else
	// Backdoor nach einem Reset oeffnen
	backdoorOpen = BLT_TRUE;
	BackDoorRestartTimer();
#endif

	// Erste Pruefung, die die Backdoor oeffnet bzw. schliesst
	BackDoorCheck();
}
//----------------------------------------------------------------------

// Backdoor oeffnen bzw. schliessen. Der Bootloader bleibt nach einem Reset fuer eine
// bestimmte Zeit aktiv, damit der Host eine Verbindung aufbauen kann.
//----------------------------------------------------------------------
void BackDoorCheck (void)
{
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
#if (BOOT_COM_ENABLE > 0)
	// Besteht bereits eine Verbindung zum Host, bleibt die Backdoor ohnehin offen
	if (ComIsConnected() == BLT_TRUE)
		return;
#endif

	// Ist die Backdoor noch offen, pruefen ob sie geschlossen werden muss
	if (backdoorOpen == BLT_TRUE)
	{
		// Pruefen ob das Zeitfenster fuer den Backdoor-Eintritt abgelaufen ist
		if (TimerGet() >= (BOOT_BACKDOOR_ENTRY_TIMEOUT_MS + backdoorExtensionTime + backdoorOpenTime))
		{
			// Backdoor schliessen
			backdoorOpen = BLT_FALSE;

			// Keine Update-Anfrage erkannt, daher Anwenderprogramm starten. Kehrt nicht
			// zurueck, wenn ein gueltiges Anwenderprogramm vorhanden ist.
			CpuStartUserProgram();
		}
	}
#endif
}
//----------------------------------------------------------------------

// Verlaengerung der Backdoor-Zeit (BOOT_BACKDOOR_ENTRY_TIMEOUT_MS) in ms setzen
//----------------------------------------------------------------------
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
void BackDoorSetExtension (blt_int32u extension_ms)
{
	// Verlaengerung uebernehmen
	backdoorExtensionTime = extension_ms;
}
#endif
//----------------------------------------------------------------------

// Verlaengerung der Backdoor-Zeit (BOOT_BACKDOOR_ENTRY_TIMEOUT_MS) in ms zurueckgeben
//----------------------------------------------------------------------
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
blt_int32u BackDoorGetExtension (void)
{
	// Eingestellte Verlaengerung zurueckgeben
	return backdoorExtensionTime;
}
#endif
//----------------------------------------------------------------------

// Backdoor-Timer mit der aktuellen Systemzeit neu starten. Die Backdoor bleibt danach
// fuer BOOT_BACKDOOR_ENTRY_TIMEOUT_MS (plus Verlaengerung) geoeffnet.
//----------------------------------------------------------------------
#if (BOOT_BACKDOOR_HOOKS_ENABLE == 0)
void BackDoorRestartTimer (void)
{
	// Timer nur neu starten, wenn die Backdoor noch offen ist
	if (backdoorOpen == BLT_TRUE)
	{
		backdoorOpenTime = TimerGet();
	}
}
#endif
//----------------------------------------------------------------------
