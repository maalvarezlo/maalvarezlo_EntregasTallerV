/**
 ******************************************************************************
 * @file           : main.c
 * @author         : maalvarezlo by STM32CubeIDE
 * @brief          : caonfiguracion basica del proyecto
 ******************************************************************************

 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f411xx_hal.h"


int main(void)
{
	// configuracion inicial del MCU
	RCC->AHB1ENR &= ~(1<<0); //limpiando la posicion 0 del registro
	RCC->AHB1ENR |= (1<<0); // activamos la señal del reloj (poniendo 1 en la posicion 0 del registro)

	//configurando el pin PA5 como salida
	GPIOA->MODER &= ~(0b11 << 10); // limpiando las posiciones 11:10 del MODER
	GPIOA->MODER |= (0b01 << 10); // configurando pin A5 como salida

	// configuracion otyper
	GPIOA->OTYPER &= ~(1 << 5); // limpiando la posicion(configurando push-pull)

	// config OSPEED
	GPIOA->OSPEEDR &= ~(0b11 <<10); // limpiando posiciones 10:11
	GPIOA->OSPEEDR |= (0b10 <<10); // velocidad de salida en fast

	// configuracion de la resistencia de PU-PD
	GPIOA->PUPDR &= ~(0b11 << 10); //limpiando las posiciones 11:10. no PUPD

	GPIOA->ODR &= ~(1 << 5); // Limpimos la salida PA5, Apaga el LED
	GPIOA->ODR |= (1 << 5); // Enciende el LED




    /* Loop forever */
	while(1);{

	}
}
