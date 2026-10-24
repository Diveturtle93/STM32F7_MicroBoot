//----------------------------------------------------------------------
// Titel	:	flash.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/ARMCM7_STM32F7/flash.h
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

// Sicherheitssymbol
//----------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------

// Dateiheader definieren
//----------------------------------------------------------------------
#ifndef INC_FLASH_H_
#define INC_FLASH_H_
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
//	#define DEBUG_FLASH
#endif
//----------------------------------------------------------------------

// Typedefines definieren
//----------------------------------------------------------------------
typedef struct
{
	blt_addr sector_start;													// Startadresse des Sektors
	blt_int32u sector_size;													// Groesse des Sektors in Bytes
	blt_int8u sector_num;													// Sektornummer
} tFlashSector;
//----------------------------------------------------------------------
// Informationen eines Flash-Blocks. Programmiert wird blockweise mit maximal
// FLASH_WRITE_BLOCK_SIZE Bytes ueber einen Block-Manager. base_addr muss ein Vielfaches
// von FLASH_WRITE_BLOCK_SIZE sein.
typedef struct
{
	blt_addr base_addr;														// Startadresse des Blocks
	blt_int8u data[FLASH_WRITE_BLOCK_SIZE];									// Zu programmierende Daten
} tFlashBlockInfo;
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#define FLASH_INVALID_SECTOR					(0xff)						// Wert fuer einen ungueltigen Flash-Sektor
#define FLASH_INVALID_ADDRESS					(0xffffffff)				// Wert fuer eine ungueltige Flash-Adresse
#define FLASH_WRITE_BLOCK_SIZE					(512)						// Groesse eines Flash-Blocks zum Schreiben
#define FLASH_TOTAL_SECTORS						(sizeof(flashLayout)/sizeof(flashLayout[0]))	// Anzahl Sektoren in flashLayout[]
#define FLASH_END_ADDRESS						(flashLayout[FLASH_TOTAL_SECTORS-1].sector_start + flashLayout[FLASH_TOTAL_SECTORS-1].sector_size - 1)	// Endadresse des programmierbaren Flash

// Offset der Checksumme in der Vektortabelle des Anwenderprogramms. Standardmaessig das
// Ende der Vektortabelle. Kann in blt_conf.h ueberschrieben werden, da die Groesse der
// Vektortabelle variieren kann. Bei Aenderung muss die Position der Checksumme im
// Anwenderprogramm ebenfalls angepasst werden, sonst schlaegt die Pruefung immer fehl.
#ifndef BOOT_FLASH_VECTOR_TABLE_CS_OFFSET
	#define BOOT_FLASH_VECTOR_TABLE_CS_OFFSET	(0x1C8)
#endif

#ifndef BOOT_FLASH_CUSTOM_LAYOUT_ENABLE
	#define BOOT_FLASH_CUSTOM_LAYOUT_ENABLE		(0u)
#endif
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
void FlashInit (void);														// Flash-Treiber initialisieren
blt_bool FlashWrite (blt_addr addr, blt_int32u len, blt_int8u *data);		// Daten in den Flash schreiben
blt_bool FlashErase (blt_addr addr, blt_int32u len);						// Flash-Bereich loeschen
blt_bool FlashWriteChecksum (void);											// Checksumme des Anwenderprogramms schreiben
blt_bool FlashVerifyChecksum (void);										// Checksumme des Anwenderprogramms pruefen
blt_bool FlashDone (void);													// Flash-Operationen abschliessen
blt_addr FlashGetUserProgBaseAddress (void);								// Startadresse des Anwenderprogramms
//----------------------------------------------------------------------

#endif /* INC_FLASH_H_ */
//----------------------------------------------------------------------
