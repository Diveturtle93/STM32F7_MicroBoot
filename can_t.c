//----------------------------------------------------------------------
// Titel	:	can_t.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/ARMCM7_STM32F7/can.c
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

//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------
#include "boot.h"
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------
#if (BOOT_COM_CAN_ENABLE > 0)
	// Bittiming-Tabelle zur Berechnung der Bittiming-Einstellungen. Ein Bit besteht aus
	// 8..25 Zeitquanten (TQ). TQ = SYNC + TSEG1 + TSEG2, SYNC ist immer 1. Der Abtastpunkt
	// liegt bei (SYNC + TSEG1) / (SYNC + TSEG1 + TSEG2) * 100%. Die Tabelle enthaelt
	// gueltige Konfigurationen mit einem Abtastpunkt zwischen 68..78%.
	static const tCanBusTiming canTiming[] =
	{
		// TQ | TSEG1 | TSEG2 | SP
		{  5, 2 },															// 8 | 5 | 2 | 75%
		{  6, 2 },															// 9 | 6 | 2 | 78%
		{  6, 3 },															// 10 | 6 | 3 | 70%
		{  7, 3 },															// 11 | 7 | 3 | 73%
		{  8, 3 },															// 12 | 8 | 3 | 75%
		{  9, 3 },															// 13 | 9 | 3 | 77%
		{  9, 4 },															// 14 | 9 | 4 | 71%
		{ 10, 4 },															// 15 | 10 | 4 | 73%
		{ 11, 4 },															// 16 | 11 | 4 | 75%
		{ 12, 4 },															// 17 | 12 | 4 | 76%
		{ 12, 5 },															// 18 | 12 | 5 | 72%
		{ 13, 5 },															// 19 | 13 | 5 | 74%
		{ 14, 5 },															// 20 | 14 | 5 | 75%
		{ 15, 5 },															// 21 | 15 | 5 | 76%
		{ 15, 6 },															// 22 | 15 | 6 | 73%
		{ 16, 6 },															// 23 | 16 | 6 | 74%
		{ 16, 7 },															// 24 | 16 | 7 | 71%
		{ 16, 8 }															// 25 | 16 | 8 | 68%
	};

	static CAN_HandleTypeDef canHandle;										// CAN-Handle fuer die HAL-Aufrufe
#endif
//----------------------------------------------------------------------

// Passende Bustiming-Konfiguration zur gewuenschten Baudrate suchen
// baud = Baudrate in kbps (10..1000), prescaler, tseg1, tseg2 = Ergebniswerte
// Rueckgabe BLT_TRUE wenn eine Konfiguration gefunden wurde, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_COM_CAN_ENABLE > 0)
static blt_bool CanGetSpeedConfig (blt_int16u baud, blt_int16u *prescaler, blt_int8u *tseg1, blt_int8u *tseg2)
{
	blt_int8u cnt;
	blt_int32u canClockFreqkHz;
	LL_RCC_ClocksTypeDef rccClocks;

	// Taktfrequenzen auslesen
	LL_RCC_GetSystemClocksFreq(&rccClocks);

	// Takt des CAN-Peripheriemoduls in kHz speichern
	canClockFreqkHz = rccClocks.PCLK1_Frequency / 1000u;

	// Alle moeglichen Zeitquanten-Konfigurationen durchsuchen
	for (cnt = 0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
	{
		if ((canClockFreqkHz % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
		{
			// Zugehoerigen Prescaler berechnen
			*prescaler = canClockFreqkHz/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1));

			// Pruefen ob der Prescaler gueltig ist
			if ((*prescaler > 0) && (*prescaler <= 1024))
			{
				// Bustiming-Konfiguration speichern
				*tseg1 = canTiming[cnt].tseg1;
				*tseg2 = canTiming[cnt].tseg2;

				// Passende Konfiguration gefunden
				return BLT_TRUE;
			}
		}
	}

	// Keine passende Konfiguration gefunden
	return BLT_FALSE;
}
#endif
//----------------------------------------------------------------------

// CAN-Controller initialisieren und mit dem CAN-Bus synchronisieren
//----------------------------------------------------------------------
#if (BOOT_COM_CAN_ENABLE > 0)
void CanInit (void)
{
	blt_int16u prescaler = 0;
	blt_int8u tseg1 = 0, tseg2 = 0;
	CAN_FilterTypeDef filterConfig;
	blt_int32u rxMsgId = BOOT_COM_CAN_RX_MSG_ID;
	blt_int32u rxFilterId, rxFilterMask;

	// Bittiming-Konfiguration ermitteln
	if (CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE/1000, &prescaler, &tseg1, &tseg2) == BLT_FALSE)
	{
		// Fehlerhafte Konfiguration, die Baudrate ist mit dem Takt nicht moeglich.
		// Einstellungen in blt_conf.h pruefen: BOOT_COM_CAN_BAUDRATE,
		// BOOT_CPU_XTAL_SPEED_KHZ und BOOT_CPU_SYSTEM_SPEED_KHZ
		ASSERT_RT(BLT_FALSE);
	}

	// Konfiguration des CAN-Controllers setzen
	canHandle.Instance = CAN_CHANNEL;
	canHandle.Init.TimeTriggeredMode = DISABLE;
	canHandle.Init.AutoBusOff = DISABLE;
	canHandle.Init.AutoWakeUp = DISABLE;
	canHandle.Init.AutoRetransmission = ENABLE;
	canHandle.Init.ReceiveFifoLocked = DISABLE;
	canHandle.Init.TransmitFifoPriority = DISABLE;
	canHandle.Init.Mode = CAN_MODE_NORMAL;
	canHandle.Init.SyncJumpWidth = CAN_SJW_1TQ;
	canHandle.Init.TimeSeg1 = ((blt_int32u)tseg1 - 1) << CAN_BTR_TS1_Pos;
	canHandle.Init.TimeSeg2 = ((blt_int32u)tseg2 - 1) << CAN_BTR_TS2_Pos;
	canHandle.Init.Prescaler = prescaler;

	// CAN-Controller initialisieren. Schlaegt nur bei defekter Hardware fehl, daher
	// wird der Rueckgabewert nicht ausgewertet.
	(void)HAL_CAN_Init(&canHandle);

	// Filtermaske und ID so bestimmen, dass nur BOOT_COM_CAN_RX_MSG_ID durchgelassen wird
	// CAN-Nachricht hat standard ID
	if ((rxMsgId & 0x80000000) == 0)
	{
		rxFilterId = rxMsgId << CAN_RI0R_STID_Pos;
		rxFilterMask = (CAN_RI0R_STID_Msk) | CAN_RI0R_IDE;
	}
	// CAN-Nachricht hat extended ID
	else
	{
		// ID-Typ-Bit entfernen
		rxMsgId &= ~0x80000000;
		rxFilterId = (rxMsgId << CAN_RI0R_EXID_Pos) | CAN_RI0R_IDE;
		rxFilterMask = (CAN_RI0R_EXID_Msk) | CAN_RI0R_IDE;
	}

	// Empfangsfilter konfigurieren. Die Funktion liefert immer HAL_OK, daher wird der
	// Rueckgabewert nicht ausgewertet.
	filterConfig.FilterBank = 0;
	filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	filterConfig.FilterIdHigh = (rxFilterId >> 16) & 0x0000FFFFu;
	filterConfig.FilterIdLow = rxFilterId & 0x0000FFFFu;
	filterConfig.FilterMaskIdHigh = (rxFilterMask >> 16) & 0x0000FFFFu;
	filterConfig.FilterMaskIdLow = rxFilterMask & 0x0000FFFFu;
	filterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	filterConfig.FilterActivation = ENABLE;
	(void)HAL_CAN_ConfigFilter(&canHandle, &filterConfig);

	// CAN-Peripherie starten. Bei defekter Hardware kann nichts getan werden, daher wird
	// der Rueckgabewert nicht ausgewertet.
	(void)HAL_CAN_Start(&canHandle);
}
#endif
//----------------------------------------------------------------------

// Paket der Kommunikationsschnittstelle senden
// data = Zeiger auf die zu sendenden Daten, len = Anzahl Bytes
//----------------------------------------------------------------------
#if (BOOT_COM_CAN_ENABLE > 0)
void CanTransmitPacket (blt_int8u *data, blt_int8u len)
{
	blt_int32u txMsgId = BOOT_COM_CAN_TX_MSG_ID;
	CAN_TxHeaderTypeDef txMsgHeader;
	blt_int32u txMsgMailbox;
	blt_int32u timeout;
	HAL_StatusTypeDef txStatus;

	// Zu sendende Nachricht konfigurieren
	// CAN-Nachricht hat standard ID
	if ((txMsgId & 0x80000000) == 0)
	{
		// 11-Bit CAN-Identifier setzen
		txMsgHeader.StdId = txMsgId;
		txMsgHeader.IDE = CAN_ID_STD;
	}
	// CAN-Nachricht hat extended ID
	else
	{
		// ID-Typ-Bit entfernen
		txMsgId &= ~0x80000000;

		// 29-Bit CAN-Identifier setzen
		txMsgHeader.ExtId = txMsgId;
		txMsgHeader.IDE = CAN_ID_EXT;
	}
	txMsgHeader.RTR = CAN_RTR_DATA;
	txMsgHeader.DLC = len;

	// Nachricht zum Senden uebergeben
	txStatus = HAL_CAN_AddTxMessage(&canHandle, &txMsgHeader, data, (uint32_t *)&txMsgMailbox);
	if (txStatus == HAL_OK)
	{
		// Timeout fuer den Abschluss des Sendevorgangs bestimmen
		timeout = TimerGet() + CAN_MSG_TX_TIMEOUT_MS;

		// Auf Abschluss des Sendevorgangs warten
		while (HAL_CAN_IsTxMessagePending(&canHandle, txMsgMailbox) != 0)
		{
			// Watchdog bedienen
			CopService();

			// Schleife bei Timeout verlassen. Deutet auf einen Hardwarefehler oder einen
			// Bus ohne weitere Teilnehmer hin.
			if (TimerGet() > timeout)
			{
				break;
			}
		}
	}
}
#endif
//----------------------------------------------------------------------

// Paket der Kommunikationsschnittstelle empfangen, falls vorhanden
// data = Zeiger auf den Datenspeicher, len = Zeiger auf die Paketlaenge
// Rueckgabe BLT_TRUE wenn ein Paket empfangen wurde, sonst BLT_FALSE
//----------------------------------------------------------------------
#if (BOOT_COM_CAN_ENABLE > 0)
blt_bool CanReceivePacket (blt_int8u *data, blt_int8u *len)
{
	blt_int32u rxMsgId = BOOT_COM_CAN_RX_MSG_ID;
	blt_bool result = BLT_FALSE;
	CAN_RxHeaderTypeDef rxMsgHeader;

	if (HAL_CAN_GetRxMessage(&canHandle, CAN_RX_FIFO0, &rxMsgHeader, data) == HAL_OK)
	{
		// Pruefen ob die Nachricht den konfigurierten CAN-Identifier hat
		// CAN-Nachricht hat standard ID
		if ((rxMsgId & 0x80000000) == 0)
		{
			// Passende 11-Bit CAN-Nachricht empfangen?
			if ((rxMsgHeader.StdId == rxMsgId) && (rxMsgHeader.IDE == CAN_ID_STD))
			{
				// Paket mit passendem CAN-Identifier empfangen
				result = BLT_TRUE;
			}
		}
		// CAN-Nachricht hat extended ID
		else
		{
			// ID-Typ-Bit entfernen
			rxMsgId &= ~0x80000000;

			// Passende 29-Bit CAN-Nachricht empfangen?
			if ((rxMsgHeader.ExtId == rxMsgId) && (rxMsgHeader.IDE == CAN_ID_EXT))
			{
				// Paket mit passendem CAN-Identifier empfangen
				result = BLT_TRUE;
			}
		}
	}

	// Datenlaenge speichern
	if (result == BLT_TRUE)
	{
		*len = rxMsgHeader.DLC;
	}

	// Ergebnis zurueckgeben
	return result;
}
#endif
//----------------------------------------------------------------------
