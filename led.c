//----------------------------------------------------------------------
// Titel	:	led.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Demo/ARMCM7_STM32F7_Nucleo_F767ZI_CubeIDE/Boot/App/led.c
//----------------------------------------------------------------------

// Lizenz
//----------------------------------------------------------------------
// Copyright (c) 2020  by Feaser    http://www.feaser.com    All rights reserved
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
#include "led.h"
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------
static blt_int16u ledBlinkIntervalMs;										// Blinkintervall der LED in ms
//----------------------------------------------------------------------

// LED-Blinktreiber initialisieren
// interval_ms = Blinkintervall in ms
//----------------------------------------------------------------------
void LedBlinkInit (blt_int16u interval_ms)
{
	// Intervall zwischen den Umschaltvorgaengen speichern
	ledBlinkIntervalMs = interval_ms;
}
//----------------------------------------------------------------------

// LED im festen Zeitintervall blinken lassen
//----------------------------------------------------------------------
void LedBlinkTask (void)
{
	static blt_bool ledOn = BLT_FALSE;
	static blt_int32u nextBlinkEvent = 0;

	// Pruefen ob die LED umgeschaltet werden muss
	if (TimerGet() >= nextBlinkEvent)
	{
		// LED ist aus, einschalten
		if (ledOn == BLT_FALSE)
		{
			ledOn = BLT_TRUE;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		}
		// LED ist an, ausschalten
		else
		{
			ledOn = BLT_FALSE;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		}

		// Naechsten Umschaltzeitpunkt festlegen
		nextBlinkEvent = TimerGet() + ledBlinkIntervalMs;
	}
}
//----------------------------------------------------------------------

// LED-Blinktreiber beenden, beim Verlassen des Programms aufrufen
//----------------------------------------------------------------------
void LedBlinkExit (void)
{
	// LED ausschalten
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
}
//----------------------------------------------------------------------
