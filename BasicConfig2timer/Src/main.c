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
#include <GPIOxDriver.h>


GPIO_Handler_t handlerUserLedPin = {0};


int main(void){
    /* configurar el pin */

	handlerUserLedPin.pGPIOx                                = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber        = PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_FAST;

	GPIO_Config(&handlerUserLedPin);

	// configuracion del timer



}
