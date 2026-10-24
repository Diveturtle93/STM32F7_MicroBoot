//----------------------------------------------------------------------
// Titel	:	com.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/com.c
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

#if (BOOT_COM_CAN_ENABLE > 0)
	#include "can_t.h"
#endif
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
// Timeout der Kommunikation in ms. Zeit zwischen dem Senden der letzten Antwort und
// dem Empfang der naechsten Anfrage. Wird die Zeit ueberschritten, wird ein Timeout
// ausgeloest. Kann in blt_conf.h ueberschrieben werden.
#if (BOOT_COM_ENABLE > 0)
	#ifndef BOOT_COM_TIMEOUT_MS
		#define BOOT_COM_TIMEOUT_MS				(5000U)
	#endif
#endif
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	static tComInterfaceId comActiveInterface = COM_IF_OTHER;				// Aktuell aktive Kommunikationsschnittstelle
	static blt_bool comTimeoutMonitoringActive;								// Timeout-Ueberwachung aktiv
	static blt_int32u comLastResponseTransmitTime;							// Zeitstempel der letzten gesendeten Antwort in ms
#endif
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	static void ComTimeoutEnable (blt_bool enable);							// Timeout-Ueberwachung ein- oder ausschalten
	static blt_bool ComTimeoutDetected (void);								// Pruefen ob ein Timeout aufgetreten ist
	static void ComTimeoutReset (void);										// Timeout-Zeit zuruecksetzen
#endif
//----------------------------------------------------------------------

// Kommunikationsmodul inklusive benoetigter Hardware initialisieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
void ComInit (void)
{
	// Timeout-Ueberwachung standardmaessig ausschalten
	ComTimeoutEnable(BLT_FALSE);

	// XCP-Protokoll initialisieren
	XcpInit();

	// CAN-Controller initialisieren
	CanInit();

	// CAN als aktive Schnittstelle setzen
	comActiveInterface = COM_IF_CAN;
}
#endif
//----------------------------------------------------------------------

// Auf neu empfangene Daten pruefen und deren Verarbeitung anstossen
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
void ComTask (void)
{
	blt_int8u xcpPacketLen;

	// Puffer static fuer bessere Laufzeit
	static blt_int8u xcpCtoReqPacket[BOOT_COM_RX_MAX_DATA];

	if (CanReceivePacket(&xcpCtoReqPacket[0], &xcpPacketLen) == BLT_TRUE)
	{
		// CAN als aktive Schnittstelle setzen
		comActiveInterface = COM_IF_CAN;

		// Timeout-Ueberwachung einschalten
		ComTimeoutEnable(BLT_TRUE);

		// Paket verarbeiten
		XcpPacketReceived(&xcpCtoReqPacket[0], xcpPacketLen);
	}

	// Pruefen ob ein Timeout der Kommunikation aufgetreten ist
	if (ComTimeoutDetected() == BLT_TRUE)
	{
		// Aktuell keine Reaktion auf einen Timeout
	}
}
#endif
//----------------------------------------------------------------------

// Kommunikationsmodul freigeben
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
void ComFree (void)
{
	// Timeout-Ueberwachung ausschalten
	ComTimeoutEnable(BLT_FALSE);
}
#endif
//----------------------------------------------------------------------

// Paket ueber die XCP-Transportschicht senden
// data = Zeiger auf die Paketdaten, len = Anzahl Bytes
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
void ComTransmitPacket (blt_int8u *data, blt_int16u len)
{
	// Paket senden. len ist durch die Plausibilitaetspruefung auf 8 begrenzt, daher ist
	// der Cast unkritisch.
	if (comActiveInterface == COM_IF_CAN)
	{
		CanTransmitPacket(data, (blt_int8u)len);
	}

	// Melden, dass das Paket gesendet wurde
	XcpPacketTransmitted();

	// Timeout-Ueberwachung zuruecksetzen
	ComTimeoutReset();
}
#endif
//----------------------------------------------------------------------

// Maximale Anzahl Bytes, die auf der aktiven Schnittstelle empfangen werden koennen
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
blt_int16u ComGetActiveInterfaceMaxRxLen (void)
{
	blt_int16u result;

	// Nach Kommunikationsschnittstelle unterscheiden
	switch (comActiveInterface)
	{
		case COM_IF_CAN:
		{
			result = BOOT_COM_CAN_RX_MAX_DATA;
			break;
		}
		default:
		{
			result = BOOT_COM_RX_MAX_DATA;
			break;
		}
	}

	return result;
}
#endif
//----------------------------------------------------------------------

// Maximale Anzahl Bytes, die auf der aktiven Schnittstelle gesendet werden koennen
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
blt_int16u ComGetActiveInterfaceMaxTxLen (void)
{
	blt_int16u result;

	// Nach Kommunikationsschnittstelle unterscheiden
	switch (comActiveInterface)
	{
		case COM_IF_CAN:
		{
			result = BOOT_COM_CAN_TX_MAX_DATA;
			break;
		}
		default:
		{
			result = BOOT_COM_TX_MAX_DATA;
			break;
		}
	}

	return result;
}
#endif
//----------------------------------------------------------------------

// XCP-Verbindungszustand abfragen
// Rueckgabe BLT_TRUE wenn eine XCP-Verbindung besteht, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
blt_bool ComIsConnected (void)
{
	blt_bool result = BLT_FALSE;

	// Besteht eine aktive XCP-Verbindung, ist die Schnittstelle verbunden
	if (XcpIsConnected())
	{
		result = BLT_TRUE;
	}

	// Ergebnis zurueckgeben
	return result;
}
#endif
//----------------------------------------------------------------------

// Timeout-Ueberwachung der Kommunikation ein- oder ausschalten
// enable = BLT_TRUE zum Einschalten, BLT_FALSE zum Ausschalten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void ComTimeoutEnable (blt_bool enable)
{
	// Flag setzen und Timeout zuruecksetzen
	comTimeoutMonitoringActive = enable;
	ComTimeoutReset();
}
#endif
//----------------------------------------------------------------------

// Pruefen ob ein Timeout der Kommunikation aufgetreten ist
// Rueckgabe BLT_TRUE bei Timeout, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static blt_bool ComTimeoutDetected (void)
{
	blt_bool result = BLT_FALSE;
	blt_int32u currentTime;
	blt_int32u deltaTime;

	// Nur ueberwachen, wenn eine Verbindung besteht und die Ueberwachung aktiv ist
	if ((ComIsConnected() == BLT_TRUE) && (comTimeoutMonitoringActive == BLT_TRUE))
	{
		// Zeit seit dem letzten gesendeten Paket bestimmen. Funktioniert durch die
		// Ganzzahlarithmetik auch bei einem Timer-Ueberlauf.
		currentTime = TimerGet();
		deltaTime = currentTime - comLastResponseTransmitTime;

		// Ist ein Timeout aufgetreten?
		if (deltaTime >= BOOT_COM_TIMEOUT_MS)
		{
			// Ueberwachung ausschalten, sonst wird der Timeout staendig erneut ausgeloest.
			// Mit dem naechsten empfangenen Paket wird sie wieder eingeschaltet.
			ComTimeoutEnable(BLT_FALSE);

			// Timeout erkannt
			result = BLT_TRUE;
		}
	}

	// Ergebnis zurueckgeben
	return result;
}
#endif
//----------------------------------------------------------------------

// Timeout-Zeit zuruecksetzen, nach jedem gesendeten Antwortpaket aufrufen
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void ComTimeoutReset (void)
{
	// Zeitstempel der letzten gesendeten Antwort aktualisieren
	comLastResponseTransmitTime = TimerGet();
}
#endif
//----------------------------------------------------------------------
