/*
 * SysTickDriver.c
 *
 *  Created on: 2/05/2023
 *      Author: mateo
 */

#include <stm32f4xx.h>
#include "SysTickDriver.h"


uint64_t ticks = 0;
uint64_t ticks_start = 0;
uint64_t ticks_counting = 0;

/**/
void config_SysTick_ms(uint32_t systemClock){
    // Reiniciamos el valor de la variable que cuenta tiempo
    ticks = 0;

    // Cargando el valor del limite de incrementos que representan 1ms.
    switch(systemClock){
    //caso para el reloj HSI-> 16MHz
    case 0:
    	SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
    	break;
    //caso del reloj HSE
    case 1:
    	SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
    	break;
    //caso del reloj PLL 80MHz
    case 2:
    	SysTick->LOAD = SYSTICK_LOAD_VALUE_80MHz_1ms;
    	break;
    //en caso de un valor diferente
    default:
    	SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
    	break;

    }

    // Limpiamos el valor actual del SysTick
    SysTick->VAL = 0;

    // Configuramos el reloj interno como el reloj para el Timer
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

    //Desactivamos las interrupciones globales
    __disable_irq();

    // Matriculamos la interrupción en el NVIC
    NVIC_EnableIRQ(SysTick_IRQn);

    // Activamos la interrupción debida al conteo a cero del SysTick
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

    // Activamos el Timer
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    // Activamos de nuevo las interrupciones globales
    __enable_irq();

}

uint64_t getTicks_ms(void){
    return ticks;
}

void delay_ms(uint32_t wait_time_ms){

	//captura el primer valor del tiempo a comparar
    ticks_start    = getTicks_ms();

	//captura el segundo valor del tiempo a comparar
    ticks_counting = getTicks_ms();

    while (ticks_counting < (ticks_start + (uint64_t)wait_time_ms)){

    	ticks_counting = getTicks_ms();

    }
}

/**/
void SysTick_Handler(void){
    // Verificamos que la interrupción se lanzo
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){

        // Limpiamos la bandera
        SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;

        // Incrementamos en 1 el contador.
        ticks++;
    }

}


