//----------------------------------------------------------------------
// Titel	:	can_t.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/can.h
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
#ifndef INC_CAN_T_H_
#define INC_CAN_T_H_
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
//	#define DEBUG_CAN_T
#endif
//----------------------------------------------------------------------

// Typedefines definieren
//----------------------------------------------------------------------
#if (BOOT_COM_CAN_ENABLE > 0)
	typedef struct t_can_bus_timing
	{
		blt_int8u tseg1;													// CAN Zeitsegment 1
		blt_int8u tseg2;													// CAN Zeitsegment 2
	} tCanBusTiming;
#endif
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#if (BOOT_COM_CAN_ENABLE > 0)
	#define CAN_MSG_TX_TIMEOUT_MS				(50u)						// Timeout fuer das Senden einer CAN-Nachricht in ms
	#define CAN_CHANNEL							CAN3						// Verwendeter CAN-Kanal des STM32
#endif
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_COM_CAN_ENABLE > 0)
	void CanInit (void);													// CAN-Controller initialisieren
	void CanTransmitPacket (blt_int8u *data, blt_int8u len);				// Paket senden
	blt_bool CanReceivePacket (blt_int8u *data, blt_int8u *len);			// Paket empfangen, falls vorhanden
#endif
//----------------------------------------------------------------------

#endif /* INC_CAN_T_H_ */
//----------------------------------------------------------------------
