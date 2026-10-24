//----------------------------------------------------------------------
// Titel	:	plausibility.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/plausibility.h
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
#ifndef INC_PLAUSIBILITY_H_
#define INC_PLAUSIBILITY_H_
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

// Konfiguration des CPU-Treibers pruefen
//----------------------------------------------------------------------
#ifndef BOOT_CPU_XTAL_SPEED_KHZ
	#error "BOOT_CPU_XTAL_SPEED_KHZ is missing in blt_conf.h"
#endif

#if (BOOT_CPU_XTAL_SPEED_KHZ <= 0)
	#error "BOOT_CPU_XTAL_SPEED_KHZ must be > 0"
#endif

#ifndef BOOT_CPU_SYSTEM_SPEED_KHZ
	#error "BOOT_CPU_SYSTEM_SPEED_KHZ is missing in blt_conf.h"
#endif

#if (BOOT_CPU_SYSTEM_SPEED_KHZ <= 0)
	#error "BOOT_CPU_SYSTEM_SPEED_KHZ must be > 0"
#endif

#ifndef BOOT_CPU_BYTE_ORDER_MOTOROLA
	#error "BOOT_CPU_BYTE_ORDER_MOTOROLA is missing in blt_conf.h"
#endif

#if (BOOT_CPU_BYTE_ORDER_MOTOROLA < 0) || (BOOT_CPU_BYTE_ORDER_MOTOROLA > 1)
	#error "BOOT_CPU_BYTE_ORDER_MOTOROLA must be 0 or 1"
#endif

#ifndef BOOT_CPU_USER_PROGRAM_START_HOOK
	#define BOOT_CPU_USER_PROGRAM_START_HOOK	(0)
#endif

#if (BOOT_CPU_USER_PROGRAM_START_HOOK < 0) || (BOOT_CPU_USER_PROGRAM_START_HOOK > 1)
	#error "BOOT_CPU_USER_PROGRAM_START_HOOK must be 0 or 1"
#endif
//----------------------------------------------------------------------

// Konfiguration der Kommunikationsschnittstelle pruefen
//----------------------------------------------------------------------
// Muss vor BOOT_COM_CAN_TX_MAX_DATA und BOOT_COM_CAN_RX_MAX_DATA stehen
#ifndef BOOT_COM_CAN_FD_ENABLE
	#define BOOT_COM_CAN_FD_ENABLE				(0)
#endif

#ifndef BOOT_COM_CAN_TX_MAX_DATA
	#if (BOOT_COM_CAN_FD_ENABLE > 0)
		#define BOOT_COM_CAN_TX_MAX_DATA		(64)
	#else
		#define BOOT_COM_CAN_TX_MAX_DATA		(8)
	#endif
#endif

#ifndef BOOT_COM_CAN_RX_MAX_DATA
	#if (BOOT_COM_CAN_FD_ENABLE > 0)
		#define BOOT_COM_CAN_RX_MAX_DATA		(64)
	#else
		#define BOOT_COM_CAN_RX_MAX_DATA		(8)
	#endif
#endif

#ifndef BOOT_COM_CAN_ENABLE
	#define BOOT_COM_CAN_ENABLE					(0)
#endif

// Fuer BOOT_COM_CAN_BAUDRATE, BOOT_COM_CAN_TX_MSG_ID, BOOT_COM_CAN_RX_MSG_ID und
// BOOT_COM_CAN_FD_BRS_BAUDRATE ist die Wertepruefung abgeschaltet. So koennen sie auch
// mit einer Funktion verknuepft werden (dynamische Konfiguration).
#if (BOOT_COM_CAN_ENABLE > 0)
	#ifndef BOOT_COM_CAN_BAUDRATE
		#error "BOOT_COM_CAN_BAUDRATE is missing in blt_conf.h"
	#endif

	#ifndef BOOT_COM_CAN_TX_MSG_ID
		#error "BOOT_COM_CAN_TX_MSG_ID is missing in blt_conf.h"
	#endif

	#ifndef BOOT_COM_CAN_TX_MAX_DATA
		#error "BOOT_COM_CAN_TX_MAX_DATA is missing in blt_conf.h"
	#endif

	#if (BOOT_COM_CAN_TX_MAX_DATA <= 0)
		#error "BOOT_COM_CAN_TX_MAX_DATA must be > 0"
	#endif

	#if (BOOT_COM_CAN_FD_ENABLE > 0)
		#if (BOOT_COM_CAN_TX_MAX_DATA > 64)
			#error "BOOT_COM_CAN_TX_MAX_DATA must be <= 64"
		#endif
	#else
		#if (BOOT_COM_CAN_TX_MAX_DATA > 8)
			#error "BOOT_COM_CAN_TX_MAX_DATA must be <= 8"
		#endif
	#endif

	#ifndef BOOT_COM_CAN_RX_MSG_ID
		#error "BOOT_COM_CAN_RX_MSG_ID is missing in blt_conf.h"
	#endif

	#ifndef BOOT_COM_CAN_RX_MAX_DATA
		#error "BOOT_COM_CAN_RX_MAX_DATA is missing in blt_conf.h"
	#endif

	#if (BOOT_COM_CAN_RX_MAX_DATA <= 0)
		#error "BOOT_COM_CAN_RX_MAX_DATA must be > 0"
	#endif

	#if (BOOT_COM_CAN_FD_ENABLE > 0)
		#if (BOOT_COM_CAN_RX_MAX_DATA > 64)
			#error "BOOT_COM_CAN_RX_MAX_DATA must be <= 64"
		#endif
	#else
		#if (BOOT_COM_CAN_RX_MAX_DATA > 8)
			#error "BOOT_COM_CAN_RX_MAX_DATA must be <= 8"
		#endif
	#endif

	#ifndef BOOT_COM_CAN_CHANNEL_INDEX
		#error "BOOT_COM_CAN_CHANNEL_INDEX is missing in blt_conf.h"
	#endif

	#if (BOOT_COM_CAN_CHANNEL_INDEX < 0)
		#error "BOOT_COM_CAN_CHANNEL_INDEX must be >= 0"
	#endif

	#if (BOOT_COM_CAN_FD_ENABLE < 0) || (BOOT_COM_CAN_FD_ENABLE > 1)
		#error "BOOT_COM_CAN_FD_ENABLE must be 0 (disabled) or 1 (enabled)"
	#endif

	// Optional, nur fuer die Bitraten-Umschaltung (BRS) noetig
	#ifndef BOOT_COM_CAN_FD_BRS_BAUDRATE
		#define BOOT_COM_CAN_FD_BRS_BAUDRATE	(0)
	#endif
#endif

#if (BOOT_COM_CAN_ENABLE == 1)
	#define BOOT_COM_ENABLE						(1)
#else
	#define BOOT_COM_ENABLE						(0)
#endif
//----------------------------------------------------------------------

// Konfiguration des Backdoor-Eintritts pruefen
//----------------------------------------------------------------------
#ifndef BOOT_BACKDOOR_HOOKS_ENABLE
	#define BOOT_BACKDOOR_HOOKS_ENABLE			(0)
#endif

#if (BOOT_BACKDOOR_HOOKS_ENABLE < 0) || (BOOT_BACKDOOR_HOOKS_ENABLE > 1)
	#error "BOOT_BACKDOOR_HOOKS_ENABLE must be 0 or 1"
#endif
//----------------------------------------------------------------------

// Konfiguration des Flash-Treibers pruefen
//----------------------------------------------------------------------
#ifndef BOOT_FLASH_CRYPTO_HOOKS_ENABLE
	#define BOOT_FLASH_CRYPTO_HOOKS_ENABLE		(0)
#endif

#if (BOOT_FLASH_CRYPTO_HOOKS_ENABLE < 0) || (BOOT_FLASH_CRYPTO_HOOKS_ENABLE > 1)
	#error "BOOT_FLASH_CRYPTO_HOOKS_ENABLE must be 0 or 1"
#endif
//----------------------------------------------------------------------

// Konfiguration des NVM-Treibers pruefen
//----------------------------------------------------------------------
#ifndef BOOT_NVM_HOOKS_ENABLE
	#define BOOT_NVM_HOOKS_ENABLE				(0)
#endif

#if (BOOT_NVM_HOOKS_ENABLE < 0) || (BOOT_NVM_HOOKS_ENABLE > 1)
	#error "BOOT_NVM_HOOKS_ENABLE must be 0 or 1"
#endif

#ifndef BOOT_NVM_SIZE_KB
	#error "BOOT_NVM_SIZE_KB is missing in blt_conf.h"
#endif

#if (BOOT_NVM_SIZE_KB <= 0)
	#error "BOOT_NVM_SIZE_KB must be > 0"
#endif

#ifndef BOOT_NVM_CHECKSUM_HOOKS_ENABLE
	#define BOOT_NVM_CHECKSUM_HOOKS_ENABLE		(0)
#endif

#if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE < 0) || (BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 1)
	#error "BOOT_NVM_CHECKSUM_HOOKS_ENABLE must be 0 or 1"
#endif
//----------------------------------------------------------------------

// Konfiguration des Watchdog-Treibers pruefen
//----------------------------------------------------------------------
#ifndef BOOT_COP_HOOKS_ENABLE
	#define BOOT_COP_HOOKS_ENABLE				(0)
#endif

#if (BOOT_COP_HOOKS_ENABLE < 0) || (BOOT_COP_HOOKS_ENABLE > 1)
	#error "BOOT_COP_HOOKS_ENABLE must be 0 or 1"
#endif
//----------------------------------------------------------------------

// Konfiguration des XCP-Moduls pruefen
//----------------------------------------------------------------------
#ifndef BOOT_XCP_SEED_KEY_ENABLE
	#define BOOT_XCP_SEED_KEY_ENABLE			(0)
#endif

#if (BOOT_XCP_SEED_KEY_ENABLE < 0) || (BOOT_XCP_SEED_KEY_ENABLE > 1)
	#error "BOOT_XCP_SEED_KEY_ENABLE must be 0 or 1"
#endif

#ifndef BOOT_XCP_UPLOAD_ENABLE
	#define BOOT_XCP_UPLOAD_ENABLE				(1)
#endif

#if (BOOT_XCP_UPLOAD_ENABLE < 0) || (BOOT_XCP_UPLOAD_ENABLE > 1)
	#error "BOOT_XCP_UPLOAD_ENABLE must be 0 or 1"
#endif

#ifndef BOOT_XCP_PACKET_RECEIVED_HOOK
	#define BOOT_XCP_PACKET_RECEIVED_HOOK		(0)
#endif

#if (BOOT_XCP_PACKET_RECEIVED_HOOK < 0) || (BOOT_XCP_PACKET_RECEIVED_HOOK > 1)
	#error "BOOT_XCP_PACKET_RECEIVED_HOOK must be 0 or 1"
#endif

#ifndef BOOT_XCP_SEED_MAX_LEN
	#define BOOT_XCP_SEED_MAX_LEN				(64)
#endif

#if (BOOT_XCP_SEED_MAX_LEN <= 0)
	#error "BOOT_XCP_SEED_MAX_LEN must be > 0"
#endif

#ifndef BOOT_XCP_KEY_MAX_LEN
	#define BOOT_XCP_KEY_MAX_LEN				(64)
#endif

#if (BOOT_XCP_KEY_MAX_LEN <= 0)
	#error "BOOT_XCP_KEY_MAX_LEN must be > 0"
#endif
//----------------------------------------------------------------------

#endif /* INC_PLAUSIBILITY_H_ */
//----------------------------------------------------------------------
