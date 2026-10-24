//----------------------------------------------------------------------
// Titel	:	com.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/com.h
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
#ifndef INC_COM_H_
#define INC_COM_H_
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	#include "xcp.h"														// XCP-Kommunikationsschicht
#endif
//----------------------------------------------------------------------

// Definiere Debug-Symbole
//----------------------------------------------------------------------
#ifdef DEBUG
//	#define DEBUG_COM
#endif
//----------------------------------------------------------------------

// Typedefines definieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	typedef enum
	{
		COM_IF_RS232,														// 0 RS232-Schnittstelle
		COM_IF_MBRTU,														// 1 Modbus-RTU-Schnittstelle
		COM_IF_CAN,															// 2 CAN-Schnittstelle
		COM_IF_USB,															// 3 USB-Schnittstelle
		COM_IF_CUSTOM,														// 4 Eigene Schnittstelle
		COM_IF_NET,															// 5 Netzwerk-Schnittstelle
		COM_IF_OTHER														// 6 Andere Schnittstelle
	} tComInterfaceId;
#endif
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	// Maximale Anzahl Bytes fuer den Empfang, abhaengig von den aktiven Schnittstellen
	#define BOOT_COM_RX_MAX_DATA				(1)

	// Anpassen, falls die CAN-Schnittstelle mehr verwendet
	#if (BOOT_COM_CAN_ENABLE > 0)
		#if (BOOT_COM_CAN_RX_MAX_DATA > BOOT_COM_RX_MAX_DATA)
			#undef BOOT_COM_RX_MAX_DATA
			#define BOOT_COM_RX_MAX_DATA		(BOOT_COM_CAN_RX_MAX_DATA)
		#endif
	#endif

	// Maximale Anzahl Bytes fuer das Senden, abhaengig von den aktiven Schnittstellen
	#define BOOT_COM_TX_MAX_DATA				(1)

	// Anpassen, falls die CAN-Schnittstelle mehr verwendet
	#if (BOOT_COM_CAN_ENABLE > 0)
		#if (BOOT_COM_CAN_TX_MAX_DATA > BOOT_COM_TX_MAX_DATA)
			#undef BOOT_COM_TX_MAX_DATA
			#define BOOT_COM_TX_MAX_DATA		(BOOT_COM_CAN_TX_MAX_DATA)
		#endif
	#endif
#endif
//----------------------------------------------------------------------

// Plausibilitaet pruefen
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	#if (BOOT_COM_TX_MAX_DATA < 1)
		#undef BOOT_COM_TX_MAX_DATA
		#define BOOT_COM_TX_MAX_DATA			(8)
	#endif

	#if (BOOT_COM_TX_MAX_DATA > 256)
		#error "COM.H, BOOT_COM_TX_MAX_DATA cannot be larger than 256."
	#endif

	#if (BOOT_COM_RX_MAX_DATA < 1)
		#undef BOOT_COM_RX_MAX_DATA
		#define BOOT_COM_RX_MAX_DATA			(8)
	#endif

	#if (BOOT_COM_RX_MAX_DATA > 65536)
		#error "COM.H, BOOT_COM_RX_MAX_DATA cannot be larger than 65536."
	#endif
#endif
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	void ComInit (void);													// Kommunikationsmodul initialisieren
	void ComTask (void);													// Empfangene Daten verarbeiten
	void ComFree (void);													// Kommunikationsmodul freigeben
	blt_int16u ComGetActiveInterfaceMaxRxLen (void);						// Maximale Empfangslaenge der aktiven Schnittstelle
	blt_int16u ComGetActiveInterfaceMaxTxLen (void);						// Maximale Sendelaenge der aktiven Schnittstelle
	void ComTransmitPacket (blt_int8u *data, blt_int16u len);				// Paket ueber die XCP-Transportschicht senden
	blt_bool ComIsConnected (void);											// XCP-Verbindungszustand abfragen
#endif
//----------------------------------------------------------------------

#endif /* INC_COM_H_ */
//----------------------------------------------------------------------
