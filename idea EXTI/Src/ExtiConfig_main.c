/*
 * ExtiConfig_main.c
 *
 *  Created on: 24/03/2023
 *      Author: mateo
 */


#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"

/* Definicion de los elementos del sistema */
GPIO_Handler_t handlerLED2 = {0};
GPIO_Handler_t handlerUserButton = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};
uint32_t counterExti13= 0;


/* Prototipos de las funciones del main*/
void init_hardware (void);
void callback_exti13 (void);
/**/

int main(void){

/*Inicialización de todos los elementos del sistema*/
init_hardware ();

while(1){

}


return 0;
}

/**/
void init_hardware (void){

/*Configuracion del LED2 - PA5*/
	handlerLED2.pGPIOx = GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

/* Cargar la configuracion del LED2 en los registros*/
	GPIO_Config(&handlerLED2);

	GPIO_WritePin(&handlerLED2, SET);

/* Configuracion del TIM2 para que haga un blinky cada 300ms*/
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP ;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 300;  // Lanza una interrupción cada 300ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

/* Cargando la configuracion del TIM2 en los registros*/
	BasicTimer_Config(&handlerBlinkyTimer);

/* Configuracion del exti - user button-PC13*/
/* 1. Activamos la señal digital */

	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

/* Cargando la configuración en los registros del MCU*/
	GPIO_Config(&handlerUserButton);

/* 2. Activando la señal de reloj del SYSCONFIG */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

/* 3. Configurando el Mux 13 para que utilice el puerto C 0xF -> 0b1111*/
	SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI13_Pos);
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC; //Seleccionando el puerto C para el Mux13

/* 4. Configurar EXTI, Mirar diagrama donde se ve que hay que conf cada caracteristica*/
/*4.a Configurar tipo de flanco*/

	EXTI->FTSR = 0; // Desactivamos todos los posibles flancosde bajada
	EXTI->RTSR = 0; // Llevando el registro a un valor conocido
	EXTI->RTSR |= EXTI_RTSR_TR13;

/*4.b  Activar interrupción */

	EXTI->IMR = 0;
	EXTI->IMR |= EXTI_IMR_IM13; // Activamos la interrupcion 13

/* 5.a Desactivar TODAS las interrupciones */
	__disable_irq();

/*5.b Matricular la interrupción en el NVIC */
	NVIC_EnableIRQ(EXTI15_10_IRQn);

/*5.c Crear ISR */
/*5.d Crear el callback */
/*5.e Activas las interrupciones */
	__enable_irq();


} // Termina el int_Hardware

void callback_exti13 (void){
	counterExti13 ++;
}

/**/
void EXTI15_10_IRQHandler (void){

	if((EXTI->PR & EXTI_PR_PR13) !=0){           // Da másde 1 (hay un 1 movido 13 veces en la exti_pr_pr13
		EXTI->PR |= EXTI_PR_PR13; // Limpiar la bandera de exti 13, en el RefManual dice que se limpia poniendo
                         // 1 en la posicion que hicimos
		}
}


void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);

}
