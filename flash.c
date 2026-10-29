//----------------------------------------------------------------------
// Titel	:	flash.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/ARMCM7_STM32F7/flash.c
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

// Plausibilitaet pruefen
//----------------------------------------------------------------------
#if (BOOT_FLASH_VECTOR_TABLE_CS_OFFSET >= FLASH_WRITE_BLOCK_SIZE)
	#error "BOOT_FLASH_VECTOR_TABLE_CS_OFFSET is set too high. It must be located in the first writable block."
#endif
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------
// Ist BOOT_FLASH_CUSTOM_LAYOUT_ENABLE in blt_conf.h > 0, kann eine eigene Tabelle
// flashLayout[] in der Datei flash_layout.c angelegt werden. Diese wird dann hier
// eingebunden und muss nicht separat uebersetzt werden.
#if (BOOT_FLASH_CUSTOM_LAYOUT_ENABLE == 0)
	// Aufteilung des Flash-Speichers. Legt auch fest, welcher Teil fuer den Bootloader
	// reserviert ist. Bei Groessenaenderung des Bootloaders ggf. anpassen.
	static const tFlashSector flashLayout[] =
	{
		// Platz ist fuer einen Bootloader mit allen Schnittstellen reserviert. Wird z. B.
		// nur UART benoetigt, kann der Bereich des Bootloaders deutlich kleiner sein.
		// { 0x08000000, 0x08000, 0 },	Flash-Sektor 0 - reserviert fuer den Bootloader
		{ 0x08008000, 0x08000, 1},											// Flash-Sektor 1 - 32kB
#if (BOOT_NVM_SIZE_KB > 64)
		{ 0x08010000, 0x08000, 2},											// Flash-Sektor 2 - 32kB
		{ 0x08018000, 0x08000, 3},											// Flash-Sektor 3 - 32kB
#endif
#if (BOOT_NVM_SIZE_KB > 128)
		{ 0x08020000, 0x20000, 4},											// Flash-Sektor 4 - 128kB
#endif
#if (BOOT_NVM_SIZE_KB > 256)
		{ 0x08040000, 0x40000, 5},											// Flash-Sektor 5 - 256kB
#endif
#if (BOOT_NVM_SIZE_KB > 512)
		{ 0x08080000, 0x40000, 6},											// Flash-Sektor 6 - 256kB
		{ 0x080C0000, 0x40000, 7},											// Flash-Sektor 7 - 256kB
#endif
#if (BOOT_NVM_SIZE_KB > 1024)
		{ 0x08100000, 0x40000, 8},											// Flash-Sektor 8 - 256kB
		{ 0x08140000, 0x40000, 9},											// Flash-Sektor 9 - 256kB
		{ 0x08180000, 0x40000, 10},											// Flash-Sektor 10 - 256kB
		{ 0x081C0000, 0x40000, 11},											// Flash-Sektor 11 - 256kB
#endif
#if (BOOT_NVM_SIZE_KB > 2048)
	#error "BOOT_NVM_SIZE_KB > 2048 is currently not supported."
#endif
	};
#else
	#error "Kein Flash-Layout vorhanden."
#endif

// Flash-Block, der aktuell bearbeitet wird. Neue Daten werden zuerst in diesem RAM-Puffer
// gesammelt. Erst wenn der Puffer voll ist oder ein anderer Block beschrieben werden soll,
// wird der Inhalt in den Flash programmiert. Das beschleunigt die Programmierung.
static tFlashBlockInfo blockInfo;

// Boot-Block des Anwenderprogramms. Der erste Block enthaelt die Vektortabelle und die
// Checksumme. Da zweimaliges Beschreiben des Flash nicht auf allen Controllern
// zuverlaessig funktioniert, wird dieser Block separat verwaltet und erst am Ende der
// Programmierung einmalig geschrieben.
static tFlashBlockInfo bootBlockInfo;
//----------------------------------------------------------------------

// Hook-Funktionen definieren
//----------------------------------------------------------------------
#if (BOOT_FLASH_CRYPTO_HOOKS_ENABLE > 0)
	extern blt_bool FlashCryptoDecryptDataHook (blt_addr address, blt_int8u * data, blt_int32u size);	// Daten vor dem Schreiben entschluesseln
#endif
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
static blt_bool FlashInitBlock (tFlashBlockInfo *block, blt_addr address);	// Block mit Flash-Inhalt initialisieren
static tFlashBlockInfo *FlashSwitchBlock (tFlashBlockInfo *block, blt_addr base_addr);	// Auf naechsten Block wechseln
static blt_bool FlashAddToBlock (tFlashBlockInfo *block, blt_addr address, blt_int8u *data, blt_int32u len);	// Daten zum Block hinzufuegen
static blt_bool FlashWriteBlock (tFlashBlockInfo *block);					// Block in den Flash schreiben
static blt_bool FlashEraseSectors (blt_int8u first_sector, blt_int8u last_sector);	// Sektoren loeschen
static blt_bool FlashEmptyCheckSector (blt_int8u sector_num);				// Pruefen ob Sektor geloescht ist
static blt_int8u FlashGetSector (blt_addr address);							// Sektor zu einer Adresse bestimmen
static blt_bool FlashIsSingleBankMode (void);								// Pruefen ob Single-Bank-Modus aktiv ist
//----------------------------------------------------------------------

// Flash-Treiber initialisieren
//----------------------------------------------------------------------
void FlashInit (void)
{
	// Block-Strukturen mit ungueltiger Adresse initialisieren
	blockInfo.base_addr = FLASH_INVALID_ADDRESS;
	bootBlockInfo.base_addr = FLASH_INVALID_ADDRESS;
}
//----------------------------------------------------------------------

// Daten ueber den Block-Manager in den Flash schreiben. Es wird geprueft, dass nur
// innerhalb des Flash-Bereichs geschrieben wird, damit der Bootloader nie ueberschrieben wird.
// addr = Startadresse, len = Laenge in Bytes, data = Zeiger auf die Daten
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
blt_bool FlashWrite (blt_addr addr, blt_int32u len, blt_int8u *data)
{
	blt_addr base_addr;

	// Laenge pruefen
	if ((len - 1) > (FLASH_END_ADDRESS - addr))
		return BLT_FALSE;

	// Pruefen ob die Adressen im Flash liegen
	if ((FlashGetSector(addr) == FLASH_INVALID_SECTOR) || (FlashGetSector(addr+len-1) == FLASH_INVALID_SECTOR))
		return BLT_FALSE;

	// Boot-Block wird vom Boot-Block-Manager verwaltet
	base_addr = (addr/FLASH_WRITE_BLOCK_SIZE)*FLASH_WRITE_BLOCK_SIZE;
	if (base_addr == flashLayout[0].sector_start)
		return FlashAddToBlock(&bootBlockInfo, addr, data, len);

	// Alle anderen Bloecke ueber den Block-Manager
	return FlashAddToBlock(&blockInfo, addr, data, len);
}
//----------------------------------------------------------------------

// Flash-Bereich loeschen. Es wird geprueft, dass nur innerhalb des Flash-Bereichs
// geloescht wird, damit der Bootloader nie geloescht wird.
// addr = Startadresse, len = Laenge in Bytes
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
blt_bool FlashErase (blt_addr addr, blt_int32u len)
{
	blt_int8u first_sector;
	blt_int8u last_sector;

	// Laenge pruefen
	if ((len - 1) > (FLASH_END_ADDRESS - addr))
		return BLT_FALSE;

	// Ersten und letzten Sektor bestimmen
	first_sector = FlashGetSector(addr);
	last_sector = FlashGetSector(addr+len-1);

	// Sektoren pruefen
	if ((first_sector == FLASH_INVALID_SECTOR) || (last_sector == FLASH_INVALID_SECTOR))
		return BLT_FALSE;

	// Sektoren loeschen
	return FlashEraseSectors(first_sector, last_sector);
}
//----------------------------------------------------------------------

// Checksumme des Anwenderprogramms schreiben, nachdem es vollstaendig programmiert wurde.
// Ueber die Checksumme erkennt der Bootloader ein gueltiges Anwenderprogramm.
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
blt_bool FlashWriteChecksum (void)
{
	blt_int32u signature_checksum = 0;

	// Die Checksumme ist das Zweierkomplement der Summe der ersten 7 Eintraege der
	// Vektortabelle:
	//    0x08000000 Initialer Stackpointer
	//    0x08000004 Reset Handler
	//    0x08000008 NMI Handler
	//    0x0800000C Hard Fault Handler
	//    0x08000010 MPU Fault Handler
	//    0x08000014 Bus Fault Handler
	//    0x08000018 Usage Fault Handler
	//
	// Der Bootloader schreibt die 32-Bit Checksumme an BOOT_FLASH_VECTOR_TABLE_CS_OFFSET.
	// Im Anwenderprogramm muss dafuer ein zusaetzlicher Platzhalter reserviert sein.

	// Enthaelt der Boot-Block keine gueltigen Daten, wurde er nicht neu programmiert und
	// es muss keine neue Checksumme geschrieben werden
	if (bootBlockInfo.base_addr == FLASH_INVALID_ADDRESS)
		return BLT_TRUE;

#if (BOOT_FLASH_CRYPTO_HOOKS_ENABLE > 0)
	// Boot-Block vor der Berechnung der Checksumme entschluesseln
	if (FlashCryptoDecryptDataHook(bootBlockInfo.base_addr, bootBlockInfo.data, FLASH_WRITE_BLOCK_SIZE) == BLT_FALSE)
		return BLT_FALSE;
#endif

	// Checksumme berechnen. Die Vektoren liegen zu diesem Zeitpunkt noch im Boot-Block
	// und sind noch nicht in den Flash geschrieben.
	signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x00]));
	signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x04]));
	signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x08]));
	signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x0C]));
	signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x10]));
	signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x14]));
	signature_checksum += *((blt_int32u *)(&bootBlockInfo.data[0+0x18]));
	signature_checksum = ~signature_checksum;								// Einerkomplement
	signature_checksum += 1;												// Zweierkomplement

	// Checksumme schreiben
	return FlashWrite(flashLayout[0].sector_start+BOOT_FLASH_VECTOR_TABLE_CS_OFFSET, sizeof(blt_addr), (blt_int8u *)&signature_checksum);
}
//----------------------------------------------------------------------

// Checksumme pruefen, zeigt an ob ein gueltiges Anwenderprogramm vorhanden ist
// Rueckgabe BLT_TRUE bei gueltiger Checksumme, sonst BLT_FALSE
//----------------------------------------------------------------------
blt_bool FlashVerifyChecksum (void)
{
	blt_int32u signature_checksum = 0;

	// Checksumme wie in FlashWriteChecksum() berechnen
	signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start));
	signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x04));
	signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x08));
	signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x0C));
	signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x10));
	signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x14));
	signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+0x18));
	signature_checksum += *((blt_int32u *)(flashLayout[0].sector_start+BOOT_FLASH_VECTOR_TABLE_CS_OFFSET));

	// Summe muss als 32-Bit Wert 0 ergeben
	if (signature_checksum == 0)
		return BLT_TRUE;

	// Checksumme fehlerhaft
	return BLT_FALSE;
}
//----------------------------------------------------------------------

// Flash-Operationen abschliessen. Im aktiven Block koennen noch Daten liegen, die
// programmiert werden muessen.
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
blt_bool FlashDone (void)
{
	blt_bool result = BLT_TRUE;
	
	// Pruefen ob im Boot-Block noch Daten auf die Programmierung warten
	if (bootBlockInfo.base_addr != FLASH_INVALID_ADDRESS)
	{
		if (FlashWriteBlock(&bootInfo) == BLT_FALSE)
		{
			result = BLT_FALSE;
		}
	}

	if (result == BLT_TRUE)
	{
		// Pruefen ob noch Daten auf die Programmierung warten
		if (blockInfo.base_addr != FLASH_INVALID_ADDRESS)
		{
			if (FlashWriteBlock(&bootBlockInfo) == BLT_FALSE)
			{
				result = BLT_FALSE;
			}
		}
	}

	// Alles in Ordnung
	return result;
}
//----------------------------------------------------------------------

// Startadresse des Flash-Bereichs fuer das Anwenderprogramm (erster Eintrag in flashLayout)
//----------------------------------------------------------------------
blt_addr FlashGetUserProgBaseAddress (void)
{
	return flashLayout[0].sector_start;
}
//----------------------------------------------------------------------

// Aktuellen Flash-Inhalt in block->data kopieren und die Startadresse setzen
// block = Zeiger auf den Block, address = Startadresse des Blocks
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
static blt_bool FlashInitBlock (tFlashBlockInfo *block, blt_addr address)
{
	// Ausrichtung der Adresse pruefen
	if ((address % FLASH_WRITE_BLOCK_SIZE) != 0)
		return BLT_FALSE;

	// Block bereits initialisiert, nichts zu tun
	if (block->base_addr == address)
		return BLT_TRUE;

	// Startadresse setzen und aktuellen Flash-Inhalt kopieren
	block->base_addr = address;
	CpuMemCopy((blt_addr)block->data, address, FLASH_WRITE_BLOCK_SIZE);
	return BLT_TRUE;
}
//----------------------------------------------------------------------

// Block wechseln: aktuellen Block programmieren und naechsten initialisieren
// block = Zeiger auf den Block, base_addr = Startadresse des naechsten Blocks
// Rueckgabe Zeiger auf den nun verwendeten Block oder BLT_NULL bei Fehler
//----------------------------------------------------------------------
static tFlashBlockInfo *FlashSwitchBlock (tFlashBlockInfo *block, blt_addr base_addr)
{
	// Wechsel weg vom Boot-Block. Dieser wird erst am Ende der Programmierung durch
	// FlashDone() geschrieben, direkt nach der Checksumme.
	if (block == &bootBlockInfo)
	{
		// Vom Boot-Block auf den allgemeinen Block wechseln
		block = &blockInfo;
	}
	// Wechsel zurueck in den Boot-Block. Der allgemeine Block muss hier noch nicht
	// geschrieben werden.
	else if (base_addr == flashLayout[0].sector_start)
	{
		// Vom allgemeinen Block auf den Boot-Block wechseln
		block = &bootBlockInfo;
		base_addr = flashLayout[0].sector_start;
	}
	// Wechsel auf einen neuen Block
	else
	{
		// Aktuellen Block programmieren
		if (FlashWriteBlock(block) == BLT_FALSE)
			return BLT_NULL;
	}

	// Neuen Block bei Bedarf initialisieren
	if (FlashInitBlock(block, base_addr) == BLT_FALSE)
		return BLT_NULL;

	// Alles in Ordnung
	return block;
}
//----------------------------------------------------------------------

// Daten zum Block hinzufuegen, der aktuell Daten fuer den Flash sammelt. Liegt die Adresse
// ausserhalb des Blocks, wird dieser geschrieben und ein neuer Block initialisiert.
// block = Zeiger auf den Block, address = Zieladresse im Flash, data = Zeiger auf die
// Daten, len = Anzahl Bytes
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
static blt_bool FlashAddToBlock (tFlashBlockInfo *block, blt_addr address, blt_int8u *data, blt_int32u len)
{
	blt_addr current_base_addr;
	blt_int8u *dst;
	blt_int8u *src;

	// Aktuelle Startadresse bestimmen
	current_base_addr = (address/FLASH_WRITE_BLOCK_SIZE)*FLASH_WRITE_BLOCK_SIZE;

	// Sicherstellen, dass der Block initialisiert ist
	if (block->base_addr == FLASH_INVALID_ADDRESS)
	{
		// Block fuer die aktuelle Adresse initialisieren
		if (FlashInitBlock(block, current_base_addr) == BLT_FALSE)
			return BLT_FALSE;
	}

	// Pruefen ob die Daten in den aktuellen Block passen
	if (block->base_addr != current_base_addr)
	{
		// Aktuellen Block programmieren und naechsten initialisieren
		block = FlashSwitchBlock(block, current_base_addr);
		if (block == BLT_NULL)
			return BLT_FALSE;
	}

	// Daten zum Block hinzufuegen und auf Ueberlauf pruefen
	dst = &(block->data[address - block->base_addr]);
	src = data;
	do
	{
		// Watchdog bedienen
		CopService();

		// Pufferueberlauf?
		if ((blt_addr)(dst-&(block->data[0])) >= FLASH_WRITE_BLOCK_SIZE)
		{
			// Aktuellen Block programmieren und naechsten initialisieren
			block = FlashSwitchBlock(block, block->base_addr+FLASH_WRITE_BLOCK_SIZE);
			if (block == BLT_NULL)
				return BLT_FALSE;

			// Zielzeiger zuruecksetzen
			dst = &(block->data[0]);
		}

		// Daten in den Puffer schreiben
		*dst = *src;

		// Zeiger weitersetzen
		dst++;
		src++;

		// Bytezaehler verringern
		len--;
	}
	while (len > 0);

	// Alles in Ordnung
	return BLT_TRUE;
}
//----------------------------------------------------------------------

// FLASH_WRITE_BLOCK_SIZE Bytes aus block->data in den Flash programmieren
// block = Zeiger auf den Block
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
static blt_bool FlashWriteBlock (tFlashBlockInfo *block)
{
	blt_addr prog_addr;
	blt_int32u prog_data;
	blt_int32u word_cnt;
	blt_bool result = BLT_TRUE;

#if (BOOT_FLASH_CRYPTO_HOOKS_ENABLE > 0)
#if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE == 0)
	// Bei interner Checksumme wurde der Boot-Block bereits in FlashWriteChecksum()
	// entschluesselt, daher nicht erneut entschluesseln
	if (block != &bootBlockInfo)
#endif
	{
		// Programmdaten vor dem Schreiben entschluesseln
		if (FlashCryptoDecryptDataHook(block->base_addr, block->data, FLASH_WRITE_BLOCK_SIZE) == BLT_FALSE)
			return BLT_FALSE;
	}
#endif

	// Der Treiber unterstuetzt nur den Single-Bank-Modus. Im Dual-Bank-Modus Fehler
	// melden, sonst kann ein schwer zu findender Hard Fault auftreten.
	if (FlashIsSingleBankMode() == BLT_FALSE)
		return BLT_FALSE;

	// Flash entsperren
	HAL_FLASH_Unlock();

	// Alle Worte des Blocks einzeln programmieren
	for (word_cnt = 0; word_cnt < (FLASH_WRITE_BLOCK_SIZE/sizeof(blt_int32u)); word_cnt++)
	{
		prog_addr = block->base_addr + (word_cnt * sizeof(blt_int32u));
		prog_data = *(volatile blt_int32u *)(&block->data[word_cnt * sizeof(blt_int32u)]);

		// Watchdog bedienen
		CopService();

		// Wort programmieren
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, prog_addr, prog_data) != HAL_OK)
		{
			result = BLT_FALSE;
			break;
		}

		// Pruefen ob die Daten korrekt geschrieben wurden
		if (*(volatile blt_int32u *)prog_addr != prog_data)
		{
			result = BLT_FALSE;
			break;
		}
	}

	// Flash sperren
	HAL_FLASH_Lock();

	// Ergebnis zurueckgeben
	return result;
}
//----------------------------------------------------------------------

// Pruefen ob ein Flash-Sektor bereits vollstaendig geloescht ist
// sector_num = Sektornummer (Element sector_num aus flashLayout, kein Index)
// Rueckgabe BLT_TRUE wenn der Sektor geloescht ist, sonst BLT_FALSE
//----------------------------------------------------------------------
static blt_bool FlashEmptyCheckSector (blt_int8u sector_num)
{
	blt_bool result = BLT_FALSE;
	blt_addr sectorAddr;
	blt_int32u sectorSize;
	blt_int8u sectorIdx;
	blt_int32u wordCnt;
	blt_int32u volatile const * wordPtr;

	// Index des Sektors in flashLayout suchen
	for (sectorIdx = 0; sectorIdx < FLASH_TOTAL_SECTORS; sectorIdx++)
	{
		// Gehoert der Index zur Sektornummer?
		if (flashLayout[sectorIdx].sector_num == sector_num)
		{
			// Sektorinformationen auslesen
			sectorAddr = flashLayout[sectorIdx].sector_start;
			sectorSize = flashLayout[sectorIdx].sector_size;

			// Startadresse und Groesse muessen auf 32 Bit ausgerichtet sein
			ASSERT_RT(((sectorAddr % sizeof(blt_int32u)) == 0) && ((sectorSize % sizeof(blt_int32u)) == 0));

			// Vorerst Erfolg annehmen
			result = BLT_TRUE;

			// Zeiger auf das erste Wort des Sektors setzen
			wordPtr = (blt_int32u volatile const *)sectorAddr;

			// Sektor wortweise lesen
			for (wordCnt = 0; wordCnt < (sectorSize/sizeof(blt_int32u)); wordCnt++)
			{
				// Watchdog bei jedem 256. Durchlauf bedienen
				if ((wordCnt % 256) == 0)
				{
					CopService();
				}

				// Wort nicht im geloeschten Zustand?
				if (*wordPtr != 0xFFFFFFFFu)
				{
					// Sektor nicht leer, Pruefung abbrechen
					result = BLT_FALSE;
					break;
				}

				// Naechstes Wort
				wordPtr++;
			}

			// Sektor gefunden und geprueft
			break;
		}
	}

	// Ergebnis zurueckgeben
	return result;
}
//----------------------------------------------------------------------

// Flash-Sektoren von first_sector bis last_sector loeschen
// first_sector = erster Sektor, last_sector = letzter Sektor
// Rueckgabe BLT_TRUE bei Erfolg, sonst BLT_FALSE
//----------------------------------------------------------------------
static blt_bool FlashEraseSectors (blt_int8u first_sector, blt_int8u last_sector)
{
	blt_bool result = BLT_TRUE;
	blt_int8u sectorIdx;
	FLASH_EraseInitTypeDef eraseInitStruct;
	blt_int32u eraseSectorError = 0;

	// Sektornummern pruefen
	if (first_sector > last_sector)
	{
		result = BLT_FALSE;
	}

	if ((first_sector < flashLayout[0].sector_num) || (last_sector > flashLayout[FLASH_TOTAL_SECTORS-1].sector_num))
	{
		result = BLT_FALSE;
	}

	// Der Treiber unterstuetzt nur den Single-Bank-Modus. Im Dual-Bank-Modus Fehler
	// melden, sonst kann ein schwer zu findender Hard Fault auftreten.
	if (FlashIsSingleBankMode() == BLT_FALSE)
	{
		result = BLT_FALSE;
	}

	// Nur loeschen, wenn bisher alles in Ordnung ist
	if (result == BLT_TRUE)
	{
		// Struktur fuer das Sektorloeschen initialisieren
		eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
		eraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		eraseInitStruct.NbSectors = 1;

		// Flash entsperren
		HAL_FLASH_Unlock();

		// Alle Sektoren einzeln loeschen
		for (sectorIdx = first_sector; sectorIdx <= last_sector; sectorIdx++)
		{
			// Bereits leere Sektoren nicht loeschen
			if (FlashEmptyCheckSector(sectorIdx) == BLT_FALSE)
			{
				// Watchdog bedienen
				CopService();

				// Zu loeschenden Sektor setzen
				eraseInitStruct.Sector = sectorIdx;

				// Sektor loeschen
				if (HAL_FLASHEx_Erase(&eraseInitStruct, (uint32_t *)&eraseSectorError) != HAL_OK)
				{
					// Loeschen fehlgeschlagen, Schleife abbrechen
					result = BLT_FALSE;
					break;
				}
			}
		}

		// Flash wieder sperren
		HAL_FLASH_Lock();
	}

	// Ergebnis zurueckgeben
	return result;
}
//----------------------------------------------------------------------

// Flash-Sektor bestimmen, in dem die Adresse liegt
// address = Adresse im Flash-Sektor
// Rueckgabe Sektornummer oder FLASH_INVALID_SECTOR
//----------------------------------------------------------------------
static blt_int8u FlashGetSector (blt_addr address)
{
	blt_int8u result = FLASH_INVALID_SECTOR;
	blt_int8u sectorIdx;

	// Passenden Sektor suchen
	for (sectorIdx = 0; sectorIdx < FLASH_TOTAL_SECTORS; sectorIdx++)
	{
		// Watchdog bedienen
		CopService();

		// Liegt die Adresse in diesem Sektor?
		if ((address >= flashLayout[sectorIdx].sector_start) && (address < (flashLayout[sectorIdx].sector_start + flashLayout[sectorIdx].sector_size)))
		{
			// Sektor gefunden, Suche beenden
			result = flashLayout[sectorIdx].sector_num;
			break;
		}
	}

	// Ergebnis zurueckgeben
	return result;
}
//----------------------------------------------------------------------

// Pruefen ob der Flash im Single-Bank-Modus ist, der fuer diesen Treiber noetig ist
// Rueckgabe BLT_TRUE im Single-Bank-Modus, sonst BLT_FALSE
//----------------------------------------------------------------------
static blt_bool FlashIsSingleBankMode (void)
{
	blt_bool result = BLT_TRUE;

	// Dual-Bank-Modus gibt es nur bei bestimmten STM32F7. Fuer diese ist
	// FLASH_OPTCR_nDBANK in der HAL definiert.
#if defined (FLASH_OPTCR_nDBANK)
	// Pruefen ob der Flash NICHT im Single-Bank-Modus ist
	if ((FLASH->OPTCR & FLASH_OPTCR_nDBANK) == 0)
	{
		result = BLT_FALSE;
	}
#endif

	// Ergebnis zurueckgeben
	return result;
}
//----------------------------------------------------------------------
