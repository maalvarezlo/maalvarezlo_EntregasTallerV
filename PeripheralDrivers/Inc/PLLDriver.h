/*
 * PLLDriver.h
 *
 *  Created on: 22/05/2023
 *      Author: mateo
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include "stm32f4xx.h"

#define PLL_CLOCK_16MHz         0
#define PLL_CLOCK_80MHz         1
#define PLL_CLOCK_100MHz        2

#define Reloj_HSI               0
#define Reloj_LSE               1
#define Reloj_PLL               2

#define Preescaler_2            2
#define Preescaler_3            3
#define Preescaler_4            4
#define Preescaler_5            5

typedef struct
{
	uint8_t         FrecuenciaClock;

}PLL_Handler_t;

typedef struct
{
	uint8_t         Reloj;    // Que reloj se usara en el pin PA8
	uint8_t         Preescaler; // cuanto será el preescaler

}MCO1_Handler_t;

//prototipo de las funciones publicas
void configPLL(PLL_Handler_t *ptrHandlerPLL);
void configMCO1(MCO1_Handler_t *ptrHandlerMCO1);
int getConfigPLL(void);

#endif /* PLLDRIVER_H_ */
