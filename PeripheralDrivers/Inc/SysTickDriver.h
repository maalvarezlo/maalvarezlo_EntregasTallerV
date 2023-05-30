/*
 * SysTickDriver.h
 *
 *  Created on: 2/05/2023
 *      Author: mateo
 */

#ifndef SYSTICKDRIVER_H_
#define SYSTICKDRIVER_H_

#include <stm32f4xx.h>

#define SYSTICK_LOAD_VALUE_16MHz_1ms         16000  //Numero de ciclos es 1ms
#define SYSTICK_LOAD_VALUE_80MHz_1ms         80000  //Numero de ciclos es 1ms
#define SYSTICK_LOAD_VALUE_100MHz_1ms       100000  //Numero de ciclos es 1ms

void config_SysTick_ms(uint32_t systemClock);
uint64_t getTicks_ms(void);
void delay_ms(uint32_t wait_time_ms);

#endif /* SYSTICKDRIVER_H_ */
