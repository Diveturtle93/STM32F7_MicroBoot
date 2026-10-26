//----------------------------------------------------------------------
// Titel	:	boot.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/boot.h
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
#ifndef INC_BOOT_H_
#define INC_BOOT_H_
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------
// Die Reihenfolge der Include-Dateien ist relevant, da die Module aufeinander aufbauen
#include "types.h"															// Variablentypen
#include "asserts.h"														// Pruefungen (Assertions)
#include "blt_conf.h"														// Bootloader-Konfiguration
#include "plausibility.h"													// Plausibilitaetspruefungen der Konfiguration
#include "cpu.h"															// CPU-Treiber
#include "can_t.h"															// CAN-Treiber
#include "cop.h"															// Watchdog-Treiber
#include "nvm.h"															// Speichertreiber
#include "timer.h"															// Timer-Treiber
#include "backdoor.h"														// Backdoor-Eintritt
#include "com.h"															// Kommunikationsschnittstelle
#include "infotable.h"														// Pruefung der Info-Tabelle
//----------------------------------------------------------------------

// Definiere Debug-Symbole
//----------------------------------------------------------------------
#ifdef DEBUG
//	#define DEBUG_BOOT
#endif
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#define BOOT_VERSION_CORE_MAIN					(1u)						// Hauptversion des Bootloader-Kerns
#define BOOT_VERSION_CORE_MINOR					(22u)						// Nebenversion des Bootloader-Kerns
#define BOOT_VERSION_CORE_PATCH					(0u)						// Patch-Stand des Bootloader-Kerns
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
void BootInit (void);														// Bootloader-Kern initialisieren
void BootTask (void);														// Bootloader-Kern zyklisch ausfuehren
//----------------------------------------------------------------------

#endif /* INC_BOOT_H_ */
//----------------------------------------------------------------------
