//----------------------------------------------------------------------
// Titel	:	timer.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.06.2026
// Autor	:	Diveturtle93
// Projekt	:	STM32F7_MicroBoot
// Quelle	:	https://github.com/feaser/openblt/blob/master/Target/Source/ARMCM7_STM32F7/timer.c
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
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_rcc.h"
//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------
#include "boot.h"
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------
static blt_int32u millisecond_counter;										// Millisekunden seit dem Start
static blt_int16u free_running_counter_last;								// Letzter Wert des freilaufenden Zaehlers
//----------------------------------------------------------------------

// Millisekunden-Timer im Polling-Betrieb initialisieren. Fuer geringen ROM-Bedarf werden
// nur Inline-Funktionen des LL-Treibers verwendet.
//----------------------------------------------------------------------
void TimerInit (void)
{
	blt_int8u tim_multiplier;
	blt_int32u pclk_frequency;
	blt_int32u pclk_tim_frequency;

	// Alle STM32F7 besitzen TIM1. Dessen freilaufender Zaehler dient als Zeitbasis.
	// Zuerst den Takt der Peripherie einschalten.
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

	// Der Takt von TIM1 wird von PCLK abgeleitet, PCLK-Frequenz bestimmen
	pclk_frequency = __LL_RCC_CALC_PCLK2_FREQ(SystemCoreClock, LL_RCC_GetAPB2Prescaler());

	// Laut Taktbaum im Referenzhandbuch gilt TPCLK = PCLK * 1, wenn der APB-Prescaler 1
	// ist, sonst TPCLK = PCLK * 2
	tim_multiplier = (LL_RCC_GetAPB2Prescaler() == LL_RCC_APB2_DIV_1) ? 1U : 2U;

	// TPCLK-Frequenz bestimmen
	pclk_tim_frequency = pclk_frequency * tim_multiplier;

	// Freilaufenden 16-Bit Aufwaertszaehler mit gewuenschter Frequenz einstellen und starten
	TIM1->CR1 = TIM_CR1_CEN;
	TIM1->ARR = 65535U;
	TIM1->PSC = (pclk_tim_frequency / TIMER_COUNTER_FREQ_HZ) - 1U;

	// Update-Event erzeugen, damit der Prescaler sofort uebernommen wird
	TIM1->EGR |= TIM_EGR_UG;

	// Lokale Variablen initialisieren
	millisecond_counter = 0;
	free_running_counter_last = (blt_int16u)TIM1->CNT;
}
//----------------------------------------------------------------------

// Timer in die Standard-Reset-Konfiguration zuruecksetzen
//----------------------------------------------------------------------
void TimerReset (void)
{
	// TIM1 in den Reset-Zustand versetzen und dessen Takt abschalten
	LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_TIM1);
	LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_TIM1);
	LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM1);

	// SysTick-Register auf Reset-Werte setzen. Das Modul nutzt den SysTick nicht, aber
	// HAL_Init() hat ihn initialisiert.
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;
	SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
}
//----------------------------------------------------------------------

// Millisekunden-Timer aktualisieren
//----------------------------------------------------------------------
void TimerUpdate (void)
{
	blt_int16u free_running_counter_now;
	blt_int16u delta_counts;
	blt_int16u ms_counts;

	// Aktuellen Wert des freilaufenden Zaehlers lesen
	free_running_counter_now = (blt_int16u)TIM1->CNT;

	// Zaehlschritte seit dem letzten Millisekunden-Ereignis berechnen. Funktioniert durch
	// die Ganzzahlarithmetik auch bei einem Ueberlauf des Zaehlers.
	delta_counts = free_running_counter_now - free_running_counter_last;

	// Sind eine oder mehrere Millisekunden vergangen?
	if (delta_counts >= TIMER_COUNTS_PER_MS)
	{
		// Vergangene Millisekunden berechnen
		ms_counts = delta_counts / TIMER_COUNTS_PER_MS;

		// Millisekundenzaehler aktualisieren
		millisecond_counter += ms_counts;

		// Zaehlerwert des letzten Ereignisses speichern, um das naechste zu erkennen
		free_running_counter_last += (ms_counts * TIMER_COUNTS_PER_MS);
	}
}
//----------------------------------------------------------------------

// Aktuellen Wert des Millisekunden-Timers zurueckgeben
//----------------------------------------------------------------------
blt_int32u TimerGet (void)
{
	// Timer hier aktualisieren, damit die Funktion in Schleifen mit Timeout nutzbar ist
	TimerUpdate();

	// Millisekunden seit der Initialisierung zurueckgeben
	return millisecond_counter;
}
//----------------------------------------------------------------------

// Ueberschreibt HAL_GetTick() der HAL. Der Bootloader nutzt keine Interrupts, die HAL
// erwartet aber einen Interrupt-basierten Tick. So funktioniert der Tick im Polling-Betrieb.
//----------------------------------------------------------------------
uint32_t HAL_GetTick (void)
{
	// Mit dem 1-ms-Timer des Bootloaders verknuepfen
	return TimerGet();
}
//----------------------------------------------------------------------
