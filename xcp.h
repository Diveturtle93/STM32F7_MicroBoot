//----------------------------------------------------------------------
// Titel	:	xcp.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/xcp.h
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
#ifndef INC_XCP_H_
#define INC_XCP_H_
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
//	#define DEBUG_XCP
#endif
//----------------------------------------------------------------------

// XCP konfigurieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	#define XCP_CTO_PACKET_LEN					(ComGetActiveInterfaceMaxRxLen())	// Maximale Laenge eines CTO-Pakets (Befehl)
	#define XCP_DTO_PACKET_LEN					(ComGetActiveInterfaceMaxTxLen())	// Maximale Laenge eines DTO-Pakets (Daten)
	#define XCP_STATION_ID_STRING				"OpenBLT"					// Kennung des Steuergeraets fuer den Befehl GET_ID

	// Byte-Reihenfolge Motorola (Big Endian) oder Intel (Little Endian)
	#if (BOOT_CPU_BYTE_ORDER_MOTOROLA > 0)
		#define XCP_MOTOROLA_FORMAT				(0x01)
	#else
		#define XCP_MOTOROLA_FORMAT				(0x00)
	#endif

	// Kalibrier-Ressource, noetig wenn waehrend der XCP-Sitzung Daten ins RAM geschrieben werden
	#define XCP_RES_CALIBRATION_EN				(0)

	// Paging-Ressource, noetig zum Umschalten von Kalibrierseiten. Dann muessen
	// AppCalSetPage und AppCalGetPage bereitgestellt werden.
	#define XCP_RES_PAGING_EN					(0)

	// Programmier-Ressource, noetig zum Loeschen und Programmieren des NVM. Dann muessen
	// NvmWrite, NvmErase und CpuStartUserProgram bereitgestellt werden.
	#define XCP_RES_PROGRAMMING_EN				(1)

	// Datenerfassungs-Ressource, wird vom XCP-Treiber aktuell nicht unterstuetzt
	#define XCP_RES_DATA_ACQUISITION_EN			(0)

	// Datenstimulations-Ressource, wird vom XCP-Treiber aktuell nicht unterstuetzt
	#define XCP_RES_DATA_STIMULATION_EN			(0)

	// Seed/Key-Schutz. Der XCP-Master muss mit GET_SEED/UNLOCK den Zugriff auf eine
	// Ressource freischalten. Der Algorithmus liegt in XcpGetSeed und XcpVerifyKey.
	#if (BOOT_XCP_SEED_KEY_ENABLE > 0)
		#define XCP_SEED_KEY_PROTECTION_EN		(1)
	#else
		#define XCP_SEED_KEY_PROTECTION_EN		(0)
	#endif

	// Upload freigeben. Standardmaessig erlaubt XCP das Lesen des Speichers mit UPLOAD
	// und SHORT_UPLOAD. Ist dies abgeschaltet, liefert das Lesen immer 0.
	#if (BOOT_XCP_UPLOAD_ENABLE > 0)
		#define XCP_UPLOAD_EN					(1)
	#else
		#define XCP_UPLOAD_EN					(0)
	#endif

	// Hook-Funktion bei jedem empfangenen XCP-Paket. Damit laesst sich ein
	// Master-Slave-Bootloader umsetzen: Der Parameter mode im Befehl CONNECT dient als
	// Knoten-ID, ein Gateway leitet die Pakete an den Slave weiter und dessen Antwort
	// ueber ComTransmitPacket() an den Host. PROGRAM_RESET deaktiviert das Gateway am
	// Ende. Liefert die Hook-Funktion BLT_TRUE, wird das Paket nicht weiter vom XCP-Modul
	// verarbeitet, bei BLT_FALSE wie gewohnt.
	#if (BOOT_XCP_PACKET_RECEIVED_HOOK > 0)
		#define XCP_PACKET_RECEIVED_HOOK_EN		(1)
	#else
		#define XCP_PACKET_RECEIVED_HOOK_EN		(0)
	#endif
#endif
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	#define XCP_VERSION_PROTOCOL_LAYER			(0x0100)					// Version der XCP-Protokollschicht (16 Bit)
	#define XCP_VERSION_TRANSPORT_LAYER			(0x0100)					// Version der XCP-Transportschicht (16 Bit)

	// XCP-Paketkennungen
	#define XCP_PID_RES							(0xff)						// Antwortpaket
	#define XCP_PID_ERR							(0xfe)						// Fehlerpaket

	// XCP-Fehlercodes
	#define XCP_ERR_CMD_SYNCH					(0x00)						// Synchronisation des Befehlsprozessors
	#define XCP_ERR_CMD_BUSY					(0x10)						// Befehl wurde nicht ausgefuehrt
	#define XCP_ERR_CMD_UNKNOWN					(0x20)						// Unbekannter oder nicht unterstuetzter Befehl
	#define XCP_ERR_OUT_OF_RANGE				(0x22)						// Parameter ausserhalb des Bereichs
	#define XCP_ERR_ACCESS_LOCKED				(0x25)						// Geschuetzt, Seed/Key erforderlich
	#define XCP_ERR_PAGE_NOT_VALID				(0x26)						// Kalibrierseite ungueltig
	#define XCP_ERR_SEQUENCE					(0x29)						// Fehler in der Befehlsfolge
	#define XCP_ERR_GENERIC						(0x31)						// Allgemeiner Fehler

	// XCP-Befehlscodes
	#define XCP_CMD_CONNECT						(0xff)						// CONNECT
	#define XCP_CMD_DISCONNECT					(0xfe)						// DISCONNECT
	#define XCP_CMD_GET_STATUS					(0xfd)						// GET_STATUS
	#define XCP_CMD_SYNCH						(0xfc)						// SYNCH
	#define XCP_CMD_GET_ID						(0xfa)						// GET_ID
	#define XCP_CMD_GET_SEED					(0xf8)						// GET_SEED
	#define XCP_CMD_UNLOCK						(0xf7)						// UNLOCK
	#define XCP_CMD_SET_MTA						(0xf6)						// SET_MTA
	#define XCP_CMD_UPLOAD						(0xf5)						// UPLOAD
	#define XCP_CMD_SHORT_UPLOAD				(0xf4)						// SHORT_UPLOAD
	#define XCP_CMD_BUILD_CHECKSUM				(0xf3)						// BUILD_CHECKSUM
	#define XCP_CMD_USER						(0xf1)						// USER
	#define XCP_CMD_DOWNLOAD					(0xf0)						// DOWNLOAD
	#define XCP_CMD_DOWLOAD_MAX					(0xee)						// DOWNLOAD_MAX
	#define XCP_CMD_SET_CAL_PAGE				(0xeb)						// SET_CAL_PAGE
	#define XCP_CMD_GET_CAL_PAGE				(0xea)						// GET_CAL_PAGE
	#define XCP_CMD_PROGRAM_START				(0xd2)						// PROGRAM_START
	#define XCP_CMD_PROGRAM_CLEAR				(0xd1)						// PROGRAM_CLEAR
	#define XCP_CMD_PROGRAM						(0xd0)						// PROGRAM
	#define XCP_CMD_PROGRAM_RESET				(0xcf)						// PROGRAM_RESET
	#define XCP_CMD_PROGRAM_PREPARE				(0xcc)						// PROGRAM_PREPARE
	#define XCP_CMD_PROGRAM_MAX					(0xc9)						// PROGRAM_MAX

	// XCP-USER-Unterbefehle
	#define XCP_CMD_USER_SUB_INFOTABLE			(0x17)						// Unterbefehl fuer die Info-Tabelle

	// Befehls-IDs des Info-Tabellen-Unterbefehls
	#define XCP_CMD_IT_CID_GETINFO				(0x04)						// GET_INFO
	#define XCP_CMD_IT_CID_DOWNLOAD				(0x06)						// DOWNLOAD
	#define XCP_CMD_IT_CID_CHECK				(0x08)						// CHECK

	// Unterstuetzte XCP-Ressourcen
	#define XCP_RES_PGM							(0x10)						// Programmierung (PGM)
	#define XCP_RES_STIM						(0x08)						// Datenstimulation (STIM)
	#define XCP_RES_DAQ							(0x04)						// Datenerfassung (DAQ)
	#define XCP_RES_CALPAG						(0x01)						// Kalibrierung und Paging (CALPAG)

	// XCP-Checksummentypen
	#define XCP_CS_ADD11						(0x01)						// Byte in Byte addieren
	#define XCP_CS_ADD12						(0x02)						// Byte in Wort addieren
	#define XCP_CS_ADD14						(0x03)						// Byte in Doppelwort addieren
	#define XCP_CS_ADD22						(0x04)						// Wort in Wort addieren
	#define XCP_CS_ADD24						(0x05)						// Wort in Doppelwort addieren
	#define XCP_CS_ADD44						(0x06)						// Doppelwort in Doppelwort addieren
	#define XCP_CS_CRC16						(0x07)						// 16-Bit CRC
	#define XCP_CS_CRC16CITT					(0x08)						// 16-Bit CRC CITT
	#define XCP_CS_CRC32						(0x09)						// 32-Bit CRC
	#define XCP_CS_USER							(0xff)						// Benutzerdefinierter Algorithmus

	#define XCP_SEED_MAX_LEN					(BOOT_XCP_SEED_MAX_LEN)		// Maximale Laenge des Seeds in Bytes
	#define XCP_KEY_MAX_LEN						(BOOT_XCP_KEY_MAX_LEN)		// Maximale Laenge des Schluessels in Bytes
#endif
//----------------------------------------------------------------------

// Typedefines definieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	typedef struct
	{
		blt_int8u ctoData[BOOT_COM_RX_MAX_DATA];							// Datenpuffer fuer CTO-Pakete
		blt_int8u connected;												// Verbindung aufgebaut
		blt_int8u protection;												// Schutzstatus der Ressourcen
		blt_int8u s_n_k_resource;											// Ressource fuer die Seed/Key-Sequenz
		blt_int8u ctoPending;												// CTO-Sendung ausstehend
		blt_int16s ctoLen;													// Aktuelle Laenge des CTO-Pakets
		blt_int32u mta;														// Speicher-Transferadresse (MTA)
		blt_int8u node_id;													// Knoten-ID aus dem Verbindungsmodus
	} tXcpInfo;
#endif
//----------------------------------------------------------------------

// Konfiguration pruefen
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	#if (XCP_RES_DATA_ACQUISITION_EN == 1)
		#error "XCP.H, Current XCP driver does not support Data AcQuisition resource."
	#endif

	#if (XCP_RES_DATA_STIMULATION_EN == 1)
		#error "XCP.H, Current XCP driver does not support Data StIMulation resource."
	#endif

	#ifndef XCP_CTO_PACKET_LEN
		#error "XCP.H, Configuration macro XCP_CTO_PACKET_LEN is missing."
	#endif

	#ifndef XCP_DTO_PACKET_LEN
		#error "XCP.H, Configuration macro XCP_DTO_PACKET_LEN is missing."
	#endif

	#ifndef XCP_STATION_ID_STRING
		#error "XCP.H, Configuration macro XCP_STATION_ID_STRING is missing."
	#endif

	#ifndef XCP_RES_CALIBRATION_EN
		#error "XCP.H, Configuration macro XCP_RES_CALIBRATION_EN is missing."
	#endif

	#if (XCP_RES_CALIBRATION_EN < 0) || (XCP_RES_CALIBRATION_EN > 1)
		#error "XCP.H, XCP_RES_CALIBRATION_EN must be 0 or 1."
	#endif

	#ifndef XCP_RES_PAGING_EN
		#error "XCP.H, Configuration macro XCP_RES_PAGING_EN is missing."
	#endif

	#if (XCP_RES_PAGING_EN < 0) || (XCP_RES_PAGING_EN > 1)
		#error "XCP.H, XCP_RES_PAGING_EN must be 0 or 1."
	#endif

	#ifndef XCP_RES_PROGRAMMING_EN
		#error "XCP.H, Configuration macro XCP_RES_PAGING_EN is missing."
	#endif

	#if (XCP_RES_PROGRAMMING_EN < 0) || (XCP_RES_PROGRAMMING_EN > 1)
		#error "XCP.H, XCP_RES_PROGRAMMING_EN must be 0 or 1."
	#endif

	#ifndef XCP_RES_DATA_ACQUISITION_EN
		#error "XCP.H, Configuration macro XCP_RES_DATA_ACQUISITION_EN is missing."
	#endif

	#if (XCP_RES_DATA_ACQUISITION_EN < 0) || (XCP_RES_DATA_ACQUISITION_EN > 1)
		#error "XCP.H, XCP_RES_DATA_ACQUISITION_EN must be 0 or 1."
	#endif

	#ifndef XCP_RES_DATA_STIMULATION_EN
		#error "XCP.H, Configuration macro XCP_RES_DATA_STIMULATION_EN is missing."
	#endif

	#if (XCP_RES_DATA_STIMULATION_EN < 0) || (XCP_RES_DATA_STIMULATION_EN > 1)
		#error "XCP.H, XCP_RES_DATA_STIMULATION_EN must be 0 or 1."
	#endif

	#ifndef XCP_SEED_KEY_PROTECTION_EN
		#error "XCP.H, Configuration macro XCP_SEED_KEY_PROTECTION_EN is missing."
	#endif

	#if (XCP_SEED_KEY_PROTECTION_EN < 0) || (XCP_SEED_KEY_PROTECTION_EN > 1)
		#error "XCP.H, XCP_SEED_KEY_PROTECTION_EN must be 0 or 1."
	#endif

	#ifndef XCP_UPLOAD_EN
		#error "XCP.H, Configuration macro XCP_UPLOAD_EN is missing."
	#endif

	#if (XCP_UPLOAD_EN < 0) || (XCP_UPLOAD_EN > 1)
		#error "XCP.H, XCP_UPLOAD_EN must be 0 or 1."
	#endif

	#ifndef XCP_PACKET_RECEIVED_HOOK_EN
		#error "XCP.H, Configuration macro XCP_PACKET_RECEIVED_HOOK_EN is missing."
	#endif

	#if (XCP_PACKET_RECEIVED_HOOK_EN < 0) || (XCP_PACKET_RECEIVED_HOOK_EN > 1)
		#error "XCP.H, XCP_PACKET_RECEIVED_HOOK_EN must be 0 or 1."
	#endif
#endif
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_COM_ENABLE > 0)
	void XcpInit (void);													// XCP-Modul initialisieren
	blt_bool XcpIsConnected (void);											// XCP-Verbindungszustand abfragen
	void XcpPacketTransmitted (void);										// Meldung, dass ein Paket gesendet wurde
	void XcpPacketReceived (blt_int8u *data, blt_int8u len);				// Empfangenes Paket verarbeiten
	blt_int32u XcpGetOrderedLong (blt_int8u const * data);					// 32-Bit Wert in Byte-Reihenfolge lesen
	void XcpSetOrderedLong (blt_int32u value, blt_int8u *data);				// 32-Bit Wert in Byte-Reihenfolge schreiben
#endif
//----------------------------------------------------------------------

#endif /* INC_XCP_H_ */
//----------------------------------------------------------------------
