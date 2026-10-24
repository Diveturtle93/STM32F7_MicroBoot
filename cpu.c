//----------------------------------------------------------------------
// Titel	:	cpu.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/ARMCM7_STM32F7/cpu.c
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

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------
#include "stm32f7xx.h"
//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------
#include "boot.h"
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#define CPU_USER_PROGRAM_STARTADDR_PTR			((blt_addr)(NvmGetUserProgBaseAddress() + 0x00000004))	// Zeiger auf den Reset-Vektor des Anwenderprogramms
#define CPU_USER_PROGRAM_VECTABLE_OFFSET		((blt_int32u)NvmGetUserProgBaseAddress())	// Zeiger auf die Vektortabelle des Anwenderprogramms
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Hook-Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_CPU_USER_PROGRAM_START_HOOK > 0)
	extern blt_bool CpuUserProgramStartHook (void);							// Abfrage vor dem Start des Anwenderprogramms
#endif
//----------------------------------------------------------------------

// CPU-Modul initialisieren
//----------------------------------------------------------------------
void CpuInit (void)
{
	// Der Bootloader arbeitet im Polling-Betrieb, daher globale Interrupts sperren. Wurde
	// der Bootloader aus einem laufenden Anwenderprogramm gestartet, koennten dessen
	// Peripherie-Interrupts noch aktiv sein.
	CpuIrqDisable();
}
//----------------------------------------------------------------------

// Anwenderprogramm starten, falls vorhanden. In diesem Fall kehrt die Funktion nicht zurueck.
//----------------------------------------------------------------------
void CpuStartUserProgram (void)
{
	void (*pProgResetHandler)(void);

	// Pruefen ob ein gueltiges Anwenderprogramm vorhanden ist (Checksumme)
	if (NvmVerifyChecksum() == BLT_FALSE)
		return;

#if (BOOT_CPU_USER_PROGRAM_START_HOOK > 0)
	// Callback aufrufen, dieser kann den Start des Anwenderprogramms verhindern
	if (CpuUserProgramStartHook() == BLT_FALSE)
		return;
#endif

#if (BOOT_COM_ENABLE > 0)
	// Kommunikationsschnittstelle freigeben
	ComFree();
#endif

	// HAL zuruecksetzen
	HAL_DeInit();

	// Timer zuruecksetzen
	TimerReset();

	// Vektortabelle des Anwenderprogramms einstellen
	SCB->VTOR = CPU_USER_PROGRAM_VECTABLE_OFFSET & (blt_int32u)0x1FFFFF80;

	// Sprungadresse setzen. Das ist der zweite Eintrag der Vektortabelle des
	// Anwenderprogramms und zeigt auf dessen Reset-Handler.
	pProgResetHandler = (void(*)(void))(*((blt_addr *)CPU_USER_PROGRAM_STARTADDR_PTR));

	// Der Bootloader hat die Interrupts gesperrt. Hier wieder freigeben, damit das
	// Anwenderprogramm dies nicht selbst tun muss.
	CpuIrqEnable();

	// Anwenderprogramm ueber dessen Reset-Handler starten
	pProgResetHandler();
}
//----------------------------------------------------------------------

// Daten von der Quell- zur Zieladresse kopieren
// dest = Zieladresse, src = Quelladresse, len = Laenge in Bytes
//----------------------------------------------------------------------
void CpuMemCopy (blt_addr dest, blt_addr src, blt_int16u len)
{
	blt_int8u *from, *to;

	// Zeiger setzen
	from = (blt_int8u *)src;
	to = (blt_int8u *)dest;

	// Alle Bytes von der Quell- zur Zieladresse kopieren
	while (len-- > 0)
	{
		// Byte kopieren
		*to++ = *from++;

		// Watchdog bedienen
		CopService();
	}
}
//----------------------------------------------------------------------

// Bytes ab der Zieladresse auf einen Wert setzen
// dest = Zieladresse, value = Wert, len = Anzahl Bytes
//----------------------------------------------------------------------
void CpuMemSet (blt_addr dest, blt_int8u value, blt_int16u len)
{
	blt_int8u *to;

	// Zeiger setzen
	to = (blt_int8u *)dest;

	// Alle Bytes ab der Zieladresse auf den Wert setzen
	while (len-- > 0)
	{
		// Byte setzen
		*to++ = value;

		// Watchdog bedienen
		CopService();
	}
}
//----------------------------------------------------------------------

// Globale Interrupts sperren
//----------------------------------------------------------------------
void CpuIrqDisable (void)
{
	__asm volatile ("cpsid i");
}
//----------------------------------------------------------------------

// Globale Interrupts freigeben
//----------------------------------------------------------------------
void CpuIrqEnable (void)
{
	__asm volatile ("cpsie i");
}
//----------------------------------------------------------------------
