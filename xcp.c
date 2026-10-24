//----------------------------------------------------------------------
// Titel	:	xcp.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/xcp.c
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
#if (BOOT_COM_ENABLE > 0)
	static const blt_int8s xcpStationId[] = XCP_STATION_ID_STRING;			// Kennung des Steuergeraets
	static tXcpInfo xcpInfo;												// Interne Informationen des XCP-Moduls
#endif
//----------------------------------------------------------------------

// Hook-Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	#if (XCP_PACKET_RECEIVED_HOOK_EN == 1)
		extern blt_bool XcpPacketReceivedHook (blt_int8u *data, blt_int8u len);	// Empfangenes Paket an die Anwendung geben
	#endif

	#if (XCP_RES_PAGING_EN == 1)
		extern blt_int8u XcpCalSetPageHook (blt_int8u segment, blt_int8u page);	// Kalibrierseite setzen
		extern blt_int8u XcpCalGetPageHook (blt_int8u segment);				// Kalibrierseite lesen
	#endif

	#if (XCP_SEED_KEY_PROTECTION_EN == 1)
		extern blt_int8u XcpGetSeedHook (blt_int8u resource, blt_int8u *seed);	// Seed bereitstellen
		extern blt_int8u XcpVerifyKeyHook (blt_int8u resource, blt_int8u *key, blt_int8u len);	// Schluessel pruefen
	#endif
#endif
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	// Transportschicht
	static void XcpTransmitPacket (blt_int8u *data, blt_int16s len);		// Paket senden

	// Anwendungsspezifisch
	static blt_int8u XcpComputeChecksum (blt_int32u address, blt_int32u length, blt_int32u *checksum);	// Checksumme berechnen

	#if (XCP_SEED_KEY_PROTECTION_EN == 1)
		static blt_int8u XcpGetSeed (blt_int8u resource, blt_int8u *seed);	// Seed bereitstellen
		static blt_int8u XcpVerifyKey (blt_int8u resource, blt_int8u *key, blt_int8u len);	// Schluessel pruefen
	#endif

	// Hilfsfunktionen
	static void XcpProtectResources (void);									// Alle Ressourcen schuetzen
	static void XcpSetCtoError (blt_int8u cmd, blt_int8u error);			// Fehlerpaket vorbereiten

	// Verarbeitung der XCP-Befehle
	static void XcpCmdConnect (blt_int8u *data);							// CONNECT
	static void XcpCmdDisconnect (blt_int8u *data);							// DISCONNECT
	static void XcpCmdGetStatus (blt_int8u *data);							// GET_STATUS
	static void XcpCmdSynch (blt_int8u *data);								// SYNCH
	static void XcpCmdGetId (blt_int8u *data);								// GET_ID
	static void XcpCmdSetMta (blt_int8u *data);								// SET_MTA
	static void XcpCmdUpload (blt_int8u *data);								// UPLOAD
	static void XcpCmdShortUpload (blt_int8u *data);						// SHORT_UPLOAD
	static void XcpCmdBuildCheckSum (blt_int8u *data);						// BUILD_CHECKSUM

	#if (XCP_SEED_KEY_PROTECTION_EN == 1)
		static void XcpCmdGetSeed (blt_int8u *data);						// GET_SEED
		static void XcpCmdUnlock (blt_int8u *data);							// UNLOCK
	#endif

	#if (XCP_RES_CALIBRATION_EN == 1)
		static void XcpCmdDownload (blt_int8u *data);						// DOWNLOAD
		static void XcpCmdDownloadMax (blt_int8u *data);					// DOWNLOAD_MAX
	#endif

	#if (XCP_RES_PAGING_EN == 1)
		static void XcpCmdSetCalPage (blt_int8u *data);						// SET_CAL_PAGE
		static void XcpCmdGetCalPage (blt_int8u *data);						// GET_CAL_PAGE
	#endif

	#if (XCP_RES_PROGRAMMING_EN == 1)
		static void XcpCmdProgramMax (blt_int8u *data);						// PROGRAM_MAX
		static void XcpCmdProgram (blt_int8u *data);						// PROGRAM
		static void XcpCmdProgramStart (blt_int8u *data);					// PROGRAM_START
		static void XcpCmdProgramClear (blt_int8u *data);					// PROGRAM_CLEAR
		static void XcpCmdProgramReset (blt_int8u *data);					// PROGRAM_RESET
		static void XcpCmdProgramPrepare (blt_int8u *data);					// PROGRAM_PREPARE
	#endif

	static void XcpCmdUser (blt_int8u *data);								// USER

	#if (BOOT_INFO_TABLE_ENABLE > 0)
		static void XcpCmdUserSubCmdInfoTable (blt_int8u *data);			// USER-Unterbefehl Info-Tabelle
		static void XcpCmdUserSubCmdInfoTableCidGetInfo (blt_int8u *data);	// Info-Tabelle GET_INFO
		static void XcpCmdUserSubCmdInfoTableCidDownload (blt_int8u *data);	// Info-Tabelle DOWNLOAD
		static void XcpCmdUserSubCmdInfoTableCidCheck (blt_int8u *data);	// Info-Tabelle CHECK
	#endif
#endif
//----------------------------------------------------------------------

// XCP-Treiber initialisieren, einmalig beim Systemstart aufrufen
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
void XcpInit (void)
{
	// Modulinformationen zuruecksetzen
	xcpInfo.connected = 0;
	xcpInfo.mta = 0;
	xcpInfo.ctoPending = 0;
	xcpInfo.ctoLen = 0;
	xcpInfo.s_n_k_resource = 0;
	xcpInfo.protection = 0;
}
#endif
//----------------------------------------------------------------------

// XCP-Verbindungszustand abfragen
// Rueckgabe BLT_TRUE wenn eine XCP-Verbindung besteht, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
blt_bool XcpIsConnected (void)
{
	if (xcpInfo.connected == 0)
		return BLT_FALSE;

	return BLT_TRUE;
}
#endif
//----------------------------------------------------------------------

// Meldung der Transportschicht, dass ein ausstehendes Paket gesendet wurde
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
void XcpPacketTransmitted (void)
{
	// Flag fuer ausstehende Sendung zuruecksetzen
	xcpInfo.ctoPending = 0;
}
#endif
//----------------------------------------------------------------------

// Meldung der Transportschicht, dass ein neues Paket empfangen wurde
// data = Zeiger auf die Paketdaten, len = Anzahl Bytes im Paket
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
void XcpPacketReceived (blt_int8u *data, blt_int8u len)
{
#if (XCP_PACKET_RECEIVED_HOOK_EN == 0)
	// Compiler-Warnung fuer unbenutzten Parameter vermeiden
	(void)len;
#else
	// Hook-Funktion kann das Paket verarbeiten. Bei BLT_TRUE ist keine weitere
	// Verarbeitung noetig.
	if (XcpPacketReceivedHook(data, len) == BLT_TRUE)
		return;
#endif

	// Befehl CONNECT verarbeiten
	if (data[0] == XCP_CMD_CONNECT)
	{
		XcpCmdConnect(data);
	}
	// Alle anderen Befehle nur bei bestehender Verbindung
	else if (xcpInfo.connected == 1)
	{
		switch (data[0])
		{
			case XCP_CMD_UPLOAD:
			{
				XcpCmdUpload(data);
				break;
			}
			case XCP_CMD_SHORT_UPLOAD:
			{
				XcpCmdShortUpload(data);
				break;
			}
			case XCP_CMD_SET_MTA:
			{
				XcpCmdSetMta(data);
				break;
			}
			case XCP_CMD_BUILD_CHECKSUM:
			{
				XcpCmdBuildCheckSum(data);
				break;
			}
			case XCP_CMD_GET_ID:
			{
				XcpCmdGetId(data);
				break;
			}
			case XCP_CMD_SYNCH:
			{
				XcpCmdSynch(data);
				break;
			}
			case XCP_CMD_GET_STATUS:
			{
				XcpCmdGetStatus(data);
				break;
			}
			case XCP_CMD_DISCONNECT:
			{
				XcpCmdDisconnect(data);
				break;
			}
#if (XCP_RES_CALIBRATION_EN == 1)
			case XCP_CMD_DOWNLOAD:
			{
				XcpCmdDownload(data);
				break;
			}
			case XCP_CMD_DOWLOAD_MAX:
			{
				XcpCmdDownloadMax(data);
				break;
			}
#endif
#if (XCP_RES_PROGRAMMING_EN == 1)
			case XCP_CMD_PROGRAM_MAX:
			{
				XcpCmdProgramMax(data);
				break;
			}
			case XCP_CMD_PROGRAM:
			{
				XcpCmdProgram(data);
				break;
			}
			case XCP_CMD_PROGRAM_START:
			{
				XcpCmdProgramStart(data);
				break;
			}
			case XCP_CMD_PROGRAM_CLEAR:
			{
				XcpCmdProgramClear(data);
				break;
			}
			case XCP_CMD_PROGRAM_RESET:
			{
				XcpCmdProgramReset(data);
				break;
			}
			case XCP_CMD_PROGRAM_PREPARE:
			{
				XcpCmdProgramPrepare(data);
				break;
			}
#endif
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
			case XCP_CMD_GET_SEED:
			{
				XcpCmdGetSeed(data);
				break;
			}
			case XCP_CMD_UNLOCK:
			{
				XcpCmdUnlock(data);
				break;
			}
#endif
#if (XCP_RES_PAGING_EN == 1)
			case XCP_CMD_SET_CAL_PAGE:
			{
				XcpCmdSetCalPage(data);
				break;
			}
			case XCP_CMD_GET_CAL_PAGE:
			{
				XcpCmdGetCalPage(data);
				break;
			}
#endif
			case XCP_CMD_USER:
			{
				XcpCmdUser(data);
				break;
			}
			default:
			{
				XcpSetCtoError(data[0], XCP_ERR_CMD_UNKNOWN);
				break;
			}
		}
	}
	// Keine Verbindung, daher keine Antwort senden
	else
		return;

	// Pruefen ob der vorherige Befehl abgeschlossen wurde
	if (xcpInfo.ctoPending == 1)
	{
		// Befehlsueberlauf
		XcpSetCtoError(data[0], XCP_ERR_CMD_BUSY);
	}

	// Antwort senden, falls vorhanden
	if (xcpInfo.ctoLen > 0)
	{
		// Flag fuer ausstehende Sendung setzen
		xcpInfo.ctoPending = 1;

		// CTO-Antwortpaket senden
		XcpTransmitPacket(xcpInfo.ctoData, xcpInfo.ctoLen);
	}
}
#endif
//----------------------------------------------------------------------

// Paket ueber die XCP-Transportschicht senden
// data = Zeiger auf die Paketdaten, len = Anzahl Bytes
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpTransmitPacket (blt_int8u *data, blt_int16s len)
{
	// Paket an die Kommunikationsschnittstelle uebergeben
	ComTransmitPacket(data, len);
}
#endif
//----------------------------------------------------------------------

// Checksumme ueber einen Speicherbereich berechnen, aufgerufen von BUILD_CHECKSUM
// address = Startadresse, length = Laenge in Bytes, checksum = Zeiger auf das Ergebnis
// Rueckgabe verwendeter Checksummentyp
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static blt_int8u XcpComputeChecksum (blt_int32u address, blt_int32u length, blt_int32u *checksum)
{
	blt_int8u cs = 0;

	// Checksumme mit dem Algorithmus Byte-in-Byte addieren berechnen
	while (length-- > 0)
	{
		// Naechstes Byte addieren
		cs += *((blt_int8u *)(blt_addr)address);

		// Adresse erhoehen
		address++;

		// Watchdog bedienen
		CopService();
	}

	// Berechnete Checksumme speichern
	*checksum = cs;

	// Verwendeten Algorithmus zurueckgeben
	return XCP_CS_ADD11;
}
#endif
//----------------------------------------------------------------------

// Seed an den XCP-Master liefern, aufgerufen von GET_SEED
// resource = angefragte Ressource (XCP_RES_XXX), seed = Zeiger auf den Seed-Puffer
// Rueckgabe Laenge des Seeds in Bytes
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_SEED_KEY_PROTECTION_EN == 1)
static blt_int8u XcpGetSeed (blt_int8u resource, blt_int8u *seed)
{
	// Anfrage ueber die Hook-Funktion an die Anwendung weitergeben
	return XcpGetSeedHook(resource, seed);
}
#endif
//----------------------------------------------------------------------

// Schluessel pruefen, aufgerufen von UNLOCK. Ist er korrekt, wird der Schutz aufgehoben.
// resource = zu entsperrende Ressource (XCP_RES_XXX), key = Zeiger auf den Schluessel,
// len = Laenge des Schluessels in Bytes
// Rueckgabe 1 bei korrektem Schluessel, sonst 0
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_SEED_KEY_PROTECTION_EN == 1)
static blt_int8u XcpVerifyKey (blt_int8u resource, blt_int8u *key, blt_int8u len)
{
	// Anfrage ueber die Hook-Funktion an die Anwendung weitergeben
	return XcpVerifyKeyHook(resource, key, len);
}
#endif
//----------------------------------------------------------------------

// Alle verfuegbaren Ressourcen schuetzen
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpProtectResources (void)
{
	xcpInfo.protection = 0;

#if (XCP_SEED_KEY_PROTECTION_EN == 1)
#if (XCP_RES_CALIBRATION_EN == 1)
	xcpInfo.protection |= XCP_RES_CALPAG;
#endif

#if (XCP_RES_PAGING_EN == 1)
	xcpInfo.protection |= XCP_RES_CALPAG;
#endif

#if (XCP_RES_PROGRAMMING_EN == 1)
	xcpInfo.protection |= XCP_RES_PGM;
#endif

#if (XCP_RES_DATA_ACQUISITION_EN == 1)
	xcpInfo.protection |= XCP_RES_DAQ;
#endif

#if (XCP_RES_DATA_STIMULATION_EN == 1)
	xcpInfo.protection |= XCP_RES_STIM;
#endif
#endif
}
#endif
//----------------------------------------------------------------------

// CTO-Paket fuer den angegebenen Fehler vorbereiten
// cmd = XCP-Befehl, der den Fehler ausgeloest hat, error = XCP-Fehlercode (XCP_ERR_XXX)
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpSetCtoError (blt_int8u cmd, blt_int8u error)
{
	// Fehlerpaket vorbereiten
	xcpInfo.ctoData[0] = XCP_PID_ERR;
	xcpInfo.ctoData[1] = error;
	xcpInfo.ctoLen = 2;
}
#endif
//----------------------------------------------------------------------

// 32-Bit Wert aus einem Bytepuffer lesen, Byte-Reihenfolge Intel oder Motorola beachten
// data = Zeiger auf den Puffer
// Rueckgabe 32-Bit Wert
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
blt_int32u XcpGetOrderedLong (blt_int8u const * data)
{
	blt_int32u result = 0;

#if (BOOT_CPU_BYTE_ORDER_MOTOROLA == 0)
	result |= (blt_int32u)data[0];
	result |= (blt_int32u)((blt_int32u)data[1] << 8);
	result |= (blt_int32u)((blt_int32u)data[2] << 16);
	result |= (blt_int32u)((blt_int32u)data[3] << 24);
#else
	result |= (blt_int32u)data[3];
	result |= (blt_int32u)((blt_int32u)data[2] << 8);
	result |= (blt_int32u)((blt_int32u)data[1] << 16);
	result |= (blt_int32u)((blt_int32u)data[0] << 24);
#endif

	// Ergebnis zurueckgeben
	return result;
}
#endif
//----------------------------------------------------------------------

// 32-Bit Wert in einen Bytepuffer schreiben, Byte-Reihenfolge Intel oder Motorola beachten
// value = zu speichernder Wert, data = Zeiger auf den Puffer
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
void XcpSetOrderedLong (blt_int32u value, blt_int8u *data)
{
#if (BOOT_CPU_BYTE_ORDER_MOTOROLA == 0)
	data[0] = (blt_int8u) value;
	data[1] = (blt_int8u)(value >>  8);
	data[2] = (blt_int8u)(value >> 16);
	data[3] = (blt_int8u)(value >> 24);
#else
	data[3] = (blt_int8u) value;
	data[2] = (blt_int8u)(value >>  8);
	data[1] = (blt_int8u)(value >> 16);
	data[0] = (blt_int8u)(value >> 24);
#endif
}
#endif
//----------------------------------------------------------------------

// Befehl CONNECT verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpCmdConnect (blt_int8u *data)
{
	// Compiler-Warnung fuer unbenutzten Parameter vermeiden
	data = data;

#if (BOOT_FILE_SYS_ENABLE > 0)
	// Verbindung ablehnen, wenn gerade ein Update von lokalem Speicher laeuft
	if (FileIsIdle() == BLT_FALSE)
	{
		// Befehl nicht verarbeitet, da beschaeftigt
		XcpSetCtoError(data[0], XCP_ERR_CMD_BUSY);
		return;
	}
#endif

	// Ressourcenschutz aktivieren
	XcpProtectResources();

	// Verbindung als aufgebaut markieren
	xcpInfo.connected = 1;

	// Verbindungsmodus als Knoten-ID speichern
	xcpInfo.node_id = data[1];

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Verfuegbare Ressourcen melden
	xcpInfo.ctoData[1] = 0;
#if (XCP_RES_CALIBRATION_EN == 1)
	xcpInfo.ctoData[1] |= XCP_RES_CALPAG;
#endif

#if (XCP_RES_PAGING_EN == 1)
	xcpInfo.ctoData[1] |= XCP_RES_CALPAG;
#endif

#if (XCP_RES_PROGRAMMING_EN == 1)
	xcpInfo.ctoData[1] |= XCP_RES_PGM;
#endif

#if (XCP_RES_DATA_ACQUISITION_EN == 1)
	xcpInfo.ctoData[1] |= XCP_RES_DAQ;
#endif

#if (XCP_RES_DATA_STIMULATION_EN == 1)
	xcpInfo.ctoData[1] |= XCP_RES_STIM;
#endif

	// Kommunikationsmodus melden, nur Byte-Granularitaet unterstuetzt
	xcpInfo.ctoData[2] = 0;

	// Byte-Reihenfolge Motorola oder Intel
	xcpInfo.ctoData[2] |= XCP_MOTOROLA_FORMAT;

	// Maximale CTO-Datenlaenge melden
	xcpInfo.ctoData[3] = (blt_int8u)XCP_CTO_PACKET_LEN;

	// Maximale DTO-Datenlaenge melden
#if (XCP_MOTOROLA_FORMAT == 0)
	xcpInfo.ctoData[4] = (blt_int8u)XCP_DTO_PACKET_LEN;
	xcpInfo.ctoData[5] = (blt_int8u)(XCP_DTO_PACKET_LEN >> 8);
#else
	xcpInfo.ctoData[4] = (blt_int8u)(XCP_DTO_PACKET_LEN >> 8);
	xcpInfo.ctoData[5] = (blt_int8u)XCP_DTO_PACKET_LEN;
#endif

	// Hoeherwertiges Byte der Version der Protokollschicht melden
	xcpInfo.ctoData[6] = XCP_VERSION_PROTOCOL_LAYER >> 8;

	// Hoeherwertiges Byte der Version der Transportschicht melden
	xcpInfo.ctoData[7] = XCP_VERSION_TRANSPORT_LAYER >> 8;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 8;

	// NVM-Treiber neu initialisieren, da ein neues Update beginnt
	NvmInit();
}
#endif
//----------------------------------------------------------------------

// Befehl DISCONNECT verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpCmdDisconnect (blt_int8u *data)
{
	// Compiler-Warnung fuer unbenutzten Parameter vermeiden
	data = data;

	// Verbindung als getrennt markieren
	xcpInfo.connected = 0;

	// Ressourcenschutz aktivieren
	XcpProtectResources();

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 1;
}
#endif
//----------------------------------------------------------------------

// Befehl GET_STATUS verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpCmdGetStatus (blt_int8u *data)
{
	// Compiler-Warnung fuer unbenutzten Parameter vermeiden
	data = data;

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Sitzungsstatus melden
	xcpInfo.ctoData[1] = 0;

	// Aktuellen Schutzstatus der Ressourcen melden
	xcpInfo.ctoData[2] = xcpInfo.protection;

	// Reservierte Werte und Sitzungskonfigurations-ID zuruecksetzen
	xcpInfo.ctoData[3] = 0;
	xcpInfo.ctoData[4] = 0;
	xcpInfo.ctoData[5] = 0;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 6;
}
#endif
//----------------------------------------------------------------------

// Befehl SYNCH verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpCmdSynch (blt_int8u *data)
{
	// SYNCH erfordert eine negative Antwort
	XcpSetCtoError(data[0], XCP_ERR_CMD_SYNCH);
}
#endif
//----------------------------------------------------------------------

// Befehl GET_ID verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpCmdGetId (blt_int8u *data)
{
	blt_int32u stationIdLen;

	// Compiler-Warnung fuer unbenutzten Parameter vermeiden
	data = data;

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// MTA auf den Anfang der Steuergeraete-Kennung setzen
	xcpInfo.mta = (blt_int32u)&xcpStationId[0];

	// Kennungsmodus auf 0 setzen
	xcpInfo.ctoData[1] = 0;

	// Reservierte Werte zuruecksetzen
	xcpInfo.ctoData[2] = 0;
	xcpInfo.ctoData[3] = 0;

	// Laenge der Kennung (ohne Nullterminierung) in die Antwort schreiben
	stationIdLen = (sizeof(xcpStationId)/sizeof(xcpStationId[0])) - 1;
	XcpSetOrderedLong(stationIdLen, &xcpInfo.ctoData[4]);

	// Paketlaenge setzen
	xcpInfo.ctoLen = 8;
}
#endif
//----------------------------------------------------------------------

// Befehl SET_MTA verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpCmdSetMta (blt_int8u *data)
{
	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// MTA aktualisieren, die Adresserweiterung wird ignoriert
	xcpInfo.mta = XcpGetOrderedLong(&data[4]);

	// Paketlaenge setzen
	xcpInfo.ctoLen = 1;
}
#endif
//----------------------------------------------------------------------

// Befehl UPLOAD verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpCmdUpload (blt_int8u *data)
{
	blt_int16u len;
	blt_int8u *destPtr;

	// Laenge der Upload-Anfrage pruefen
	if (data[1] > (XCP_CTO_PACKET_LEN-1))
	{
		// Angeforderte Datenlaenge zu gross
		XcpSetCtoError(data[0], XCP_ERR_OUT_OF_RANGE);
		return;
	}

	// Laenge der angeforderten Daten lesen
	len = data[1];

	// Zielzeiger setzen
	destPtr = (blt_int8u *)((blt_addr)(blt_int32u)&xcpInfo.ctoData[1]);

#if (XCP_UPLOAD_EN == 1)
	// Laut XCP-Spezifikation ist das Lesen des Speichers immer erlaubt. Bei aktivem
	// Seed/Key-Schutz ist das unerwuenscht, daher werden Nullwerte geliefert, solange
	// die Programmier-Ressource noch gesperrt ist.
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Ressource PGM gesperrt, Nullwerte liefern
	if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
	{
		CpuMemSet((blt_addr)destPtr, 0, len);
	}
	// Ressource PGM entsperrt, Daten aus dem Speicher ins Paket kopieren
	else
	{
		CpuMemCopy((blt_addr)destPtr, (blt_addr)xcpInfo.mta, len);
	}
#else
	// Daten aus dem Speicher ins Paket kopieren
	CpuMemCopy((blt_addr)destPtr, (blt_addr)xcpInfo.mta, len);
#endif
#else
	// Upload abgeschaltet, Nullwerte liefern
	CpuMemSet((blt_addr)destPtr, 0, len);
#endif

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// MTA nachtraeglich erhoehen
	xcpInfo.mta += data[1];

	// Paketlaenge setzen
	xcpInfo.ctoLen = data[1]+1;
}
#endif
//----------------------------------------------------------------------

// Befehl SHORT_UPLOAD verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpCmdShortUpload (blt_int8u *data)
{
	blt_int16u len;
	blt_int8u *destPtr;

	// Laenge der Upload-Anfrage pruefen
	if (data[1] > (XCP_CTO_PACKET_LEN-1))
	{
		// Angeforderte Datenlaenge zu gross
		XcpSetCtoError(data[0], XCP_ERR_OUT_OF_RANGE);
		return;
	}

	// MTA aktualisieren, die Adresserweiterung wird ignoriert
	xcpInfo.mta = XcpGetOrderedLong(&data[4]);

	// Laenge der angeforderten Daten lesen
	len = data[1];

	// Zielzeiger setzen
	destPtr = (blt_int8u *)((blt_addr)(blt_int32u)&xcpInfo.ctoData[1]);

#if (XCP_UPLOAD_EN == 1)
	// Laut XCP-Spezifikation ist das Lesen des Speichers immer erlaubt. Bei aktivem
	// Seed/Key-Schutz ist das unerwuenscht, daher werden Nullwerte geliefert, solange
	// die Programmier-Ressource noch gesperrt ist.
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Ressource PGM gesperrt, Nullwerte liefern
	if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
	{
		CpuMemSet((blt_addr)destPtr, 0, len);
	}
	// Ressource PGM entsperrt, Daten aus dem Speicher ins Paket kopieren
	else
	{
		CpuMemCopy((blt_addr)destPtr, (blt_addr)xcpInfo.mta, len);
	}
#else
	// Daten aus dem Speicher ins Paket kopieren
	CpuMemCopy((blt_addr)destPtr, (blt_addr)xcpInfo.mta, len);
#endif
#else
	// Upload abgeschaltet, Nullwerte liefern
	CpuMemSet((blt_addr)destPtr, 0, len);
#endif

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// MTA nachtraeglich erhoehen
	xcpInfo.mta += data[1];

	// Paketlaenge setzen
	xcpInfo.ctoLen = data[1]+1;
}
#endif
//----------------------------------------------------------------------

// Befehl DOWNLOAD verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_RES_CALIBRATION_EN == 1)
static void XcpCmdDownload (blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Pruefen ob die Ressource CAL_PAG entsperrt ist
	if ((xcpInfo.protection & XCP_RES_CALPAG) != 0)
	{
		// Ressource gesperrt, mit Seed/Key entsperren
		XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);
		return;
	}
#endif

	// Laenge der Download-Anfrage pruefen
	if (data[1] > (XCP_CTO_PACKET_LEN-2))
	{
		// Angeforderte Datenlaenge zu gross
		XcpSetCtoError(data[0], XCP_ERR_OUT_OF_RANGE);
		return;
	}

	// Daten aus dem Paket in den Speicher kopieren
	CpuMemCopy((blt_addr)xcpInfo.mta, (blt_addr)((blt_int32u)&data[2]), data[1]);

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// MTA nachtraeglich erhoehen
	xcpInfo.mta += data[1];

	// Paketlaenge setzen
	xcpInfo.ctoLen = 1;
}
#endif
//----------------------------------------------------------------------

// Befehl DOWNLOAD_MAX verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_RES_CALIBRATION_EN == 1)
static void XcpCmdDownloadMax (blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Pruefen ob die Ressource CAL_PAG entsperrt ist
	if ((xcpInfo.protection & XCP_RES_CALPAG) != 0)
	{
		// Ressource gesperrt, mit Seed/Key entsperren
		XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);
		return;
	}
#endif

	// Daten aus dem Paket in den Speicher kopieren
	CpuMemCopy((blt_addr)xcpInfo.mta, (blt_addr)((blt_int32u)&data[1]), XCP_CTO_PACKET_LEN-1);

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// MTA nachtraeglich erhoehen
	xcpInfo.mta += XCP_CTO_PACKET_LEN-1;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 1;
}
#endif
//----------------------------------------------------------------------

// Befehl BUILD_CHECKSUM verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpCmdBuildCheckSum (blt_int8u *data)
{
	blt_int32u checksumLen;
	blt_int32u checksumVal = 0;
	blt_int8u checksumType;

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Checksumme und Checksummentyp bestimmen
	checksumLen = XcpGetOrderedLong(&data[4]);
	checksumType = XcpComputeChecksum(xcpInfo.mta, checksumLen, &checksumVal);
	xcpInfo.ctoData[1] = checksumType;
	XcpSetOrderedLong(checksumVal, &xcpInfo.ctoData[4]);

	// Reservierte Parameter initialisieren
	xcpInfo.ctoData[2] = 0;
	xcpInfo.ctoData[3] = 0;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 8;
}
#endif
//----------------------------------------------------------------------

// Befehl GET_SEED verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_SEED_KEY_PROTECTION_EN == 1)
static void XcpCmdGetSeed (blt_int8u *data)
{
	blt_int8u resourceOK;

	// Seed-Puffer static, um den Stack zu entlasten
	static blt_int8u seedBuffer[XCP_SEED_MAX_LEN];
	static blt_int8u seedRemainderLen = 0;
	static blt_int8u *seedCurrentPtr;
	static blt_bool sequenceInProgress = BLT_FALSE;
	blt_int8u seedCurrentLen;

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Angeforderte Ressource pruefen, wenn das Modus-Flag 0 ist
	if (data[1] == 0)
	{
		// Zunaechst ungueltige Ressource annehmen
		resourceOK = 0;

		// Nur Kalibrierung/Paging angefordert?
		if (((data[2] & XCP_RES_CALPAG) > 0) && ((data[2] & ~XCP_RES_CALPAG) == 0))
		{
			resourceOK = 1;
		}

		// Nur Programmierung angefordert?
		if (((data[2] & XCP_RES_PGM) > 0) && ((data[2] & ~XCP_RES_PGM) == 0))
		{
			resourceOK = 1;
		}

		// Nur Datenerfassung angefordert?
		if (((data[2] & XCP_RES_DAQ) > 0) && ((data[2] & ~XCP_RES_DAQ) == 0))
		{
			resourceOK = 1;
		}

		// Nur Datenstimulation angefordert?
		if (((data[2] & XCP_RES_STIM) > 0) && ((data[2] & ~XCP_RES_STIM) == 0))
		{
			resourceOK = 1;
		}

		// Ergebnis der Ressourcenpruefung auswerten
		if (resourceOK == 0)
		{
			XcpSetCtoError(data[0], XCP_ERR_OUT_OF_RANGE);
			return;
		}

		// Pruefen ob die Ressource bereits entsperrt ist
		if ((xcpInfo.protection & data[2]) == 0)
		{
			// Seed-Laenge 0 meldet eine bereits entsperrte Ressource
			xcpInfo.ctoData[1] = 0;

			// Paketlaenge setzen
			xcpInfo.ctoLen = 2;
			return;
		}

		// Ressource fuer die Seed/Key-Sequenz speichern
		xcpInfo.s_n_k_resource = data[2];
	}

	// Modus-Flag auswerten: 0 = erster Teil des Seeds, 1 = Rest des Seeds
	if (data[1] == 0)
	{
		// Seed-Sequenz laeuft
		sequenceInProgress = BLT_TRUE;

		// Seed holen und im Puffer speichern
		seedRemainderLen = XcpGetSeed(xcpInfo.s_n_k_resource, seedBuffer);

		// Pufferueberlauf verhindern
		ASSERT_RT(seedRemainderLen <= XCP_SEED_MAX_LEN);

		// Seed-Zeiger setzen
		seedCurrentPtr = &seedBuffer[0];
	}
	// Rest des Seeds angefordert
	else
	{
		// Nur erlaubt, wenn eine Sequenz laeuft
		if (sequenceInProgress == BLT_FALSE)
		{
			// Ungueltige Sequenz
			XcpSetCtoError(data[0], XCP_ERR_SEQUENCE);

			// Ressource fuer naechstes Entsperren zuruecksetzen
			xcpInfo.s_n_k_resource = 0;
			return;
		}
	}

	// Anzahl Seed-Bytes bestimmen, die in die Antwort passen
	seedCurrentLen = seedRemainderLen;
	if (seedCurrentLen > (XCP_DTO_PACKET_LEN-2))
	{
		seedCurrentLen = XCP_DTO_PACKET_LEN-2;
	}

	// Teil des Seeds in die Antwort kopieren
	CpuMemCopy((blt_addr)(&xcpInfo.ctoData[2]), (blt_addr)seedCurrentPtr, seedCurrentLen);
	xcpInfo.ctoData[1] = seedRemainderLen;

	// Steuervariablen aktualisieren
	seedRemainderLen -= seedCurrentLen;
	seedCurrentPtr += seedCurrentLen;

	// Am Ende der Sequenz das Flag zuruecksetzen
	if (seedRemainderLen == 0)
	{
		sequenceInProgress = BLT_FALSE;
	}

	// Paketlaenge setzen
	xcpInfo.ctoLen = seedCurrentLen + 2;
}
#endif
//----------------------------------------------------------------------

// Befehl UNLOCK verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_SEED_KEY_PROTECTION_EN == 1)
static void XcpCmdUnlock (blt_int8u *data)
{
	// Schluesselpuffer static, um den Stack zu entlasten
	static blt_int8u keyBuffer[XCP_KEY_MAX_LEN];
	static blt_int8u keyPreviousRemainder = 0;
	static blt_int8u keyTotalLen = 0;
	static blt_int8u *keyCurrentPtr;
	static blt_int8u keyReceivedLen = 0;
	blt_int8u keyCurrentLen;

	// Pruefen ob der Schluessel in den Puffer passt
	if (data[1] > XCP_KEY_MAX_LEN)
	{
		// Vorherigen Rest fuer den naechsten Durchlauf zuruecksetzen
		keyPreviousRemainder = 0;

		// Schluessel zu lang
		XcpSetCtoError(data[0], XCP_ERR_OUT_OF_RANGE);

		// Ressource fuer naechstes Entsperren zuruecksetzen
		xcpInfo.s_n_k_resource = 0;
		return;
	}

	// Beginnt ein neuer Schluessel? Die erste Nachricht enthaelt die Gesamtlaenge, die
	// folgenden die Restlaenge. Ist die Laenge >= dem vorherigen Rest, beginnt ein neuer
	// Schluessel.
	if (data[1] >= keyPreviousRemainder)
	{
		// Gesamtlaenge des Schluessels speichern
		keyTotalLen = data[1];

		// Zeiger auf den Empfangspuffer initialisieren
		keyCurrentPtr = &keyBuffer[0];

		// Anzahl empfangener Bytes zuruecksetzen
		keyReceivedLen = 0;
	}

	// Laenge/Rest fuer die Pruefung im naechsten Durchlauf speichern
	keyPreviousRemainder = data[1];

	// Anzahl empfangener Schluessel-Bytes bestimmen
	keyCurrentLen = data[1];
	if (keyCurrentLen > (XCP_CTO_PACKET_LEN-2))
	{
		keyCurrentLen = XCP_CTO_PACKET_LEN-2;
	}

	// Empfangene Schluessel-Bytes im Puffer speichern
	CpuMemCopy((blt_addr)keyCurrentPtr, (blt_addr)(&data[2]), keyCurrentLen);

	// Steuervariablen aktualisieren
	keyCurrentPtr += keyCurrentLen;
	keyReceivedLen += keyCurrentLen;

	// Pruefen ob der komplette Schluessel empfangen wurde
	if (keyReceivedLen >= keyTotalLen)
	{
		// Vorherigen Rest fuer den naechsten Durchlauf zuruecksetzen
		keyPreviousRemainder = 0;

		// Schluessel pruefen
		if (XcpVerifyKey(xcpInfo.s_n_k_resource, keyBuffer, keyTotalLen) == 0)
		{
			// Ungueltiger Schluessel, Master informieren und Verbindung trennen
			XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);

			// Verbindung als getrennt markieren
			xcpInfo.connected = 0;

			// Ressource fuer naechstes Entsperren zuruecksetzen
			xcpInfo.s_n_k_resource = 0;

			// Ressourcenschutz aktivieren
			XcpProtectResources();
			return;
		}

		// Schluessel korrekt, Ressource entsperren
		xcpInfo.protection &= ~xcpInfo.s_n_k_resource;

		// Ressource fuer naechstes Entsperren zuruecksetzen
		xcpInfo.s_n_k_resource = 0;
	}

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Aktuellen Ressourcenschutz melden
	xcpInfo.ctoData[1] = xcpInfo.protection;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 2;
}
#endif
//----------------------------------------------------------------------

// Befehl SET_CAL_PAGE verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_RES_PAGING_EN == 1)
static void XcpCmdSetCalPage (blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Pruefen ob die Ressource CAL_PAG entsperrt ist
	if ((xcpInfo.protection & XCP_RES_CALPAG) == XCP_RES_CALPAG)
	{
		// Ressource gesperrt, mit Seed/Key entsperren
		XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);
		return;
	}
#endif

	// Seite auswaehlen, der Modus-Parameter wird ignoriert
	if (XcpCalSetPageHook(data[2], data[3]) == 0)
	{
		// Kalibrierseite konnte nicht ausgewaehlt werden
		XcpSetCtoError(data[0], XCP_ERR_PAGE_NOT_VALID);
		return;
	}

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 1;
}
#endif
//----------------------------------------------------------------------

// Befehl GET_CAL_PAGE verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_RES_PAGING_EN == 1)
static void XcpCmdGetCalPage (blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Pruefen ob die Ressource CAL_PAG entsperrt ist
	if ((xcpInfo.protection & XCP_RES_CALPAG) == XCP_RES_CALPAG)
	{
		// Ressource gesperrt, mit Seed/Key entsperren
		XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);
		return;
	}
#endif

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Reservierte Parameter initialisieren
	xcpInfo.ctoData[1] = 0;
	xcpInfo.ctoData[2] = 0;

	// Kalibrierseite speichern
	xcpInfo.ctoData[3] = XcpCalGetPageHook(data[2]);

	// Paketlaenge setzen
	xcpInfo.ctoLen = 4;
}
#endif
//----------------------------------------------------------------------

// Befehl PROGRAM_START verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_RES_PROGRAMMING_EN == 1)
static void XcpCmdProgramStart (blt_int8u *data)
{
	// Compiler-Warnung fuer unbenutzten Parameter vermeiden
	data = data;

#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Pruefen ob die Ressource PGM entsperrt ist
	if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
	{
		// Ressource gesperrt, mit Seed/Key entsperren
		XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);
		return;
	}
#endif

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Reservierten Parameter initialisieren
	xcpInfo.ctoData[1] = 0;

	// Kein besonderer Kommunikationsmodus waehrend der Programmierung
	xcpInfo.ctoData[2] = 0;

	// CTO-Paketlaenge bleibt waehrend der Programmierung gleich
	xcpInfo.ctoData[3] = (blt_int8u)XCP_CTO_PACKET_LEN;

	// Blockgroesse, ST-Min-Zeit und Warteschlangengroesse werden nicht unterstuetzt
	xcpInfo.ctoData[4] = 0;
	xcpInfo.ctoData[5] = 0;
	xcpInfo.ctoData[6] = 0;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 7;
}
#endif
//----------------------------------------------------------------------

// Befehl PROGRAM_MAX verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_RES_PROGRAMMING_EN == 1)
static void XcpCmdProgramMax (blt_int8u *data)
{
	blt_int32u programLen;
	blt_addr programAddr;

#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Pruefen ob die Ressource PGM entsperrt ist
	if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
	{
		// Ressource gesperrt, mit Seed/Key entsperren
		XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);
		return;
	}
#endif

	// Daten programmieren
	programAddr = (blt_addr)xcpInfo.mta;
	programLen = XCP_CTO_PACKET_LEN-1;

	if (NvmWrite(programAddr, programLen, &data[1]) == BLT_FALSE)
	{
		// Fehler beim Programmieren
		XcpSetCtoError(data[0], XCP_ERR_GENERIC);
		return;
	}

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// MTA nachtraeglich erhoehen
	xcpInfo.mta += XCP_CTO_PACKET_LEN-1;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 1;
}
#endif
//----------------------------------------------------------------------

// Befehl PROGRAM verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_RES_PROGRAMMING_EN == 1)
static void XcpCmdProgram (blt_int8u *data)
{
	blt_int32u programLen;
	blt_addr programAddr;

#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Pruefen ob die Ressource PGM entsperrt ist
	if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
	{
		// Ressource gesperrt, mit Seed/Key entsperren
		XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);
		return;
	}
#endif

	// Laenge der Anfrage pruefen
	if (data[1] > (XCP_CTO_PACKET_LEN-2))
	{
		// Angeforderte Datenlaenge zu gross
		XcpSetCtoError(data[0], XCP_ERR_OUT_OF_RANGE);
		return;
	}

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 1;

	// Ende der Programmierung (Datenlaenge 0)?
	if (data[1] == 0)
	{
		// Programmierung abschliessen
		if (NvmDone() == BLT_FALSE)
		{
			// Fehler beim Abschliessen der Programmierung
			XcpSetCtoError(data[0], XCP_ERR_GENERIC);
		}
		
		return;
	}

	// Daten programmieren
	programAddr = (blt_addr)xcpInfo.mta;
	programLen = data[1];

	if (NvmWrite(programAddr, programLen, &data[2]) == BLT_FALSE)
	{
		// Fehler beim Programmieren
		XcpSetCtoError(data[0], XCP_ERR_GENERIC);
		return;
	}

	// MTA nachtraeglich erhoehen
	xcpInfo.mta += data[1];
}
#endif
//----------------------------------------------------------------------

// Befehl PROGRAM_CLEAR verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_RES_PROGRAMMING_EN == 1)
static void XcpCmdProgramClear (blt_int8u *data)
{
	blt_int32u eraseLen;
	blt_addr eraseAddr;

#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Pruefen ob die Ressource PGM entsperrt ist
	if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
	{
		// Ressource gesperrt, mit Seed/Key entsperren
		XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);
		return;
	}
#endif

	// Speicher loeschen
	eraseAddr = xcpInfo.mta;
	eraseLen = XcpGetOrderedLong(&data[4]);

	if (NvmErase(eraseAddr, eraseLen) == BLT_FALSE)
	{
		// Fehler beim Loeschen
		XcpSetCtoError(data[0], XCP_ERR_GENERIC);
		return;
	}

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 1;
}
#endif
//----------------------------------------------------------------------

// Befehl PROGRAM_RESET verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_RES_PROGRAMMING_EN == 1)
static void XcpCmdProgramReset (blt_int8u *data)
{
	// Compiler-Warnung fuer unbenutzten Parameter vermeiden
	data = data;

#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Pruefen ob die Ressource PGM entsperrt ist
	if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
	{
		// Ressource gesperrt, mit Seed/Key entsperren
		XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);
		return;
	}
#endif

	// Nach der Programmierung die neu programmierte Firmware starten. Die Funktion muss
	// nicht zurueckkehren.
	CpuStartUserProgram();

	// Wurde das Anwenderprogramm nicht gestartet, Verbindung trennen und
	// Ressourcenschutz wieder aktivieren, wie in XcpCmdDisconnect()
	xcpInfo.connected = 0;

	// Ressourcenschutz aktivieren
	XcpProtectResources();

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 1;
}
#endif
//----------------------------------------------------------------------

// Befehl PROGRAM_PREPARE verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (XCP_RES_PROGRAMMING_EN == 1)
static void XcpCmdProgramPrepare (blt_int8u *data)
{
#if (XCP_SEED_KEY_PROTECTION_EN == 1)
	// Pruefen ob die Ressource PGM entsperrt ist
	if ((xcpInfo.protection & XCP_RES_PGM) == XCP_RES_PGM)
	{
		// Ressource gesperrt, mit Seed/Key entsperren
		XcpSetCtoError(data[0], XCP_ERR_ACCESS_LOCKED);
		return;
	}
#endif

	// Programmierung mit Kernel wird nicht benoetigt und daher nicht unterstuetzt
	XcpSetCtoError(data[0], XCP_ERR_GENERIC);
	return;
}
#endif
//----------------------------------------------------------------------

// Befehl USER verarbeiten. Erlaubt anwenderspezifische Erweiterungen des XCP-Protokolls.
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
static void XcpCmdUser (blt_int8u *data)
{
	blt_int8u subCommand;

	// Unterbefehl lesen
	subCommand = data[1];

	// Unterbefehl verarbeiten
	switch (subCommand)
	{
#if (BOOT_INFO_TABLE_ENABLE > 0)
		case XCP_CMD_USER_SUB_INFOTABLE:
		{
			XcpCmdUserSubCmdInfoTable(data);
			break;
		}
#endif
		default:
		{
			XcpSetCtoError(data[0], XCP_ERR_CMD_UNKNOWN);
			break;
		}
	}
}
#endif
//----------------------------------------------------------------------

// USER-Unterbefehl fuer die Info-Tabelle verarbeiten (anwenderspezifische Erweiterung)
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (BOOT_INFO_TABLE_ENABLE > 0)
static void XcpCmdUserSubCmdInfoTable (blt_int8u *data)
{
	blt_int8u commandId;

	// Befehls-ID lesen
	commandId = data[2];

	// Befehls-ID der Info-Tabelle verarbeiten
	switch (commandId)
	{
		case XCP_CMD_IT_CID_GETINFO:
		{
			XcpCmdUserSubCmdInfoTableCidGetInfo(data);
			break;
		}
		case XCP_CMD_IT_CID_DOWNLOAD:
		{
			XcpCmdUserSubCmdInfoTableCidDownload(data);
			break;
		}
		case XCP_CMD_IT_CID_CHECK:
		{
			XcpCmdUserSubCmdInfoTableCidCheck(data);
			break;
		}
		default:
		{
			XcpSetCtoError(data[0], XCP_ERR_CMD_UNKNOWN);
			break;
		}
	}
}
#endif
//----------------------------------------------------------------------

// Info-Tabelle: Befehls-ID GET_INFO verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (BOOT_INFO_TABLE_ENABLE > 0)
static void XcpCmdUserSubCmdInfoTableCidGetInfo (blt_int8u *data)
{
	blt_int16u tableLen;
	blt_addr tableAddr;

	// Compiler-Warnung fuer unbenutzten Parameter vermeiden
	data = data;

	// Laenge und Startadresse der Tabelle bestimmen
	tableLen = InfoTableCurrentSize(INFO_TABLE_ID_FIRMWARE_NVM);
	tableAddr = InfoTableGetPtr(INFO_TABLE_ID_FIRMWARE_NVM);

	// RAM-Info-Tabelle loeschen und Schreibzeiger zuruecksetzen
	InfoTableClear(INFO_TABLE_ID_INTERNAL_RAM);

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Befehls-ID der Antwort setzen
	xcpInfo.ctoData[1] = XCP_CMD_IT_CID_GETINFO;

	// Tabellenlaenge speichern
	CpuMemCopy((blt_addr)(&xcpInfo.ctoData[2]), (blt_addr)&tableLen, sizeof(tableLen));

	// Startadresse der Tabelle speichern
	CpuMemCopy((blt_addr)(&xcpInfo.ctoData[4]), (blt_addr)&tableAddr, sizeof(tableAddr));

	// Paketlaenge setzen
	xcpInfo.ctoLen = 8;
}
#endif
//----------------------------------------------------------------------

// Info-Tabelle: Befehls-ID DOWNLOAD verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (BOOT_INFO_TABLE_ENABLE > 0)
static void XcpCmdUserSubCmdInfoTableCidDownload (blt_int8u *data)
{
	// Laenge der Download-Anfrage pruefen
	if (data[3] > (XCP_CTO_PACKET_LEN-4))
	{
		// Angegebene Datenlaenge zu gross
		XcpSetCtoError(data[0], XCP_ERR_OUT_OF_RANGE);
		return;
	}

	// Empfangene Daten in der Info-Tabelle speichern
	if (InfoTableAddData(INFO_TABLE_ID_INTERNAL_RAM, &data[4], data[3]) == BLT_FALSE)
	{
		// Daten passen nicht in den RAM-Puffer der Info-Tabelle
		XcpSetCtoError(data[0], XCP_ERR_OUT_OF_RANGE);
		return;
	}

	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Befehls-ID der Antwort setzen
	xcpInfo.ctoData[1] = XCP_CMD_IT_CID_DOWNLOAD;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 2;
}
#endif
//----------------------------------------------------------------------

// Info-Tabelle: Befehls-ID CHECK verarbeiten
// data = Zeiger auf die Paketdaten
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0) && (BOOT_INFO_TABLE_ENABLE > 0)
static void XcpCmdUserSubCmdInfoTableCidCheck (blt_int8u *data)
{
	// Paketkennung auf Antwortpaket setzen
	xcpInfo.ctoData[0] = XCP_PID_RES;

	// Befehls-ID der Antwort setzen
	xcpInfo.ctoData[1] = XCP_CMD_IT_CID_CHECK;

	// Paketlaenge setzen
	xcpInfo.ctoLen = 3;

	// RAM-Info-Tabelle muss die gleiche Groesse wie die Tabelle im NVM haben
	if (InfoTableCurrentSize(INFO_TABLE_ID_INTERNAL_RAM) != InfoTableCurrentSize(INFO_TABLE_ID_FIRMWARE_NVM))
	{
		// RAM-Puffer wurde noch nicht vollstaendig heruntergeladen
		XcpSetCtoError(data[0], XCP_ERR_SEQUENCE);
		return;
	}

	// Pruefung der Info-Tabelle durch die Bootloader-Anwendung durchfuehren lassen
	// Pruefung bestanden, Update darf fortgesetzt werden
	if (InfoTableCheck() == BLT_TRUE)
	{
		xcpInfo.ctoData[2] = 1;
	}
	// Pruefung fehlgeschlagen, Update soll abgebrochen werden
	else
	{
		xcpInfo.ctoData[2] = 0;
	}
}
#endif
//----------------------------------------------------------------------
