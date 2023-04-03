/**
 ******************************************************************************
 * @file           : main.c
 * @author         : maalvarezlo by STM32CubeIDE
 * @brief          : caonfiguracion basica del proyecto
 ******************************************************************************

 ******************************************************************************
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"

/* Definicion de los elementos del sistema */
GPIO_Handler_t handlerLED2 = {0};
GPIO_Handler_t handlerUserButton = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};



/* Prototipos de las funciones del main*/
void init_hardware (void);
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

} // Termina el int_Hardware





void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);

}
