/*
 * PLLDriver.h
 *
 *  Created on: 22/05/2023
 *      Author: mateo
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include "stm32f4xx.h"

#define PLL_CLOCK_16MHz   0
#define PLL_CLOCK_80MHz   1

typedef struct
{
	uint8_t         FrecuenciaClock;

}PLL_Handler_t;

//prototipo de las funciones publicas
void configPLL(PLL_Handler_t *ptrHandlerPLL);
void getConfigPLL(void);

#endif /* PLLDRIVER_H_ */
