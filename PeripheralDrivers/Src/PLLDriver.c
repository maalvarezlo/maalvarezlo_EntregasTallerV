/*
 * PLLDriver.c
 *
 *  Created on: 22/05/2023
 *      Author: mateo
 */


#include <stdint.h>
#include "PLLDriver.h"
#include "GPIOxDriver.h"

uint32_t Velocidad = 0;

void configPLL(PLL_Handler_t *ptrHandlerPLL){

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;

	//sintonizando a 16MHz el HSI usando el registro HSITRIM
	RCC->CR &= ~RCC_CR_HSITRIM; //Limpiando el registro
	//Para cargar un #
	RCC->CR = 11 << RCC_CR_HSITRIM_Pos;


	// para configurar la frecuencia del clock en 16MHz
	if(ptrHandlerPLL->FrecuenciaClock == PLL_CLOCK_16MHz){
		// se activa el HSI, el cual es la velocidad por defecto del clock
		RCC->CFGR &= ~RCC_CFGR_SW; // ponemos el SW en HSI (los bits en 0 ambos)
	}

	// para poner el clock del micro a 80MHz
	else if(ptrHandlerPLL->FrecuenciaClock == PLL_CLOCK_80MHz){

		//primero configuramos el power control register para una frecuencia de PLL <= 84MHz
		// activamos el reloj
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		// ponemos los bits de Regulator voltage scaling (VOS), debe ser 10 para <= 84MHz
		PWR->CR &= ~PWR_CR_VOS_0;
		PWR->CR |= PWR_CR_VOS_1;

		//Luego se modifica la latencia de memoria EN 2ws para la frecuencia adecuada
		FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
		FLASH->ACR &= ~FLASH_ACR_LATENCY;  //se limpian los registros
		FLASH->ACR |= FLASH_ACR_LATENCY_2WS; // 2WS

		//sabemos que la velocidad por defecto es 16MHz, por lo tanto debemos usar PLLN, PLLM y PLLP para obtener 80MHz
		// F(PLL general clock output) = 16MHz(PLLN/(PLLM*PLLP))

		// El PLLM es el Factor de división para el reloj de entrada principal del PLL, el resultado será 1.6MHz
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;  // se limpia el registro
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_3; // Ponemos un 10 en el PLLM
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_1;

		// El PLLN es el Factor de multiplicación principal del PLL. el valor debe estar entre 100 y 432MHz
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;  // se limpia el registro
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_2;  // Se pone en 100 el registro
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_5;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_6;
		//El resultado será 160MHz

		// PLLP es el factor de division principal, el resultado no debe exceder los 100 MHz en este dominio.
		// el PLLP se configura en 2 para que el resultado sea 80MHz
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP; // 2 en PLLP


		/*Configuramos el MCO1 para ver la frecuencia del PLL, esto con el fin de probar la frecuencia en el pin PA8*/
//
//		//Seleccionamos la señal PLL en el multiplexor del pin PA8
//		RCC->CFGR |= RCC_CFGR_MCO1_0;
//		RCC->CFGR |= RCC_CFGR_MCO1_1;
//
//		// Utilizamos un prescaler para poder ver la señal en el osciloscopio 5 de preescaler (se dividen los 80Mhz por 5)
//		RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
//		RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
//		RCC->CFGR |= RCC_CFGR_MCO1PRE_2;

		/*Configuracion del PA8 como función alternativa para usarlo como prueba es mejor poner esto en el main
		handlerPinPrueba.pGPIOx = GPIOA;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;

		Cargamos la configuracion del Pin en el registro
		GPIO_Config(&handlerPinPrueba);*/

		// encendemos el PLL
		RCC->CR |= RCC_CR_PLLON;

		// Esperamos que el PLL se estabilice
		while(!(RCC->CR & RCC_CR_PLLRDY)){
			__NOP();
		}

		// Activando el PLL en el multiplexor SW, para que el clock del micro sea el PLL
		RCC->CFGR &= ~RCC_CFGR_SW; // limpiamos el registro
		RCC->CFGR |= RCC_CFGR_SW_1; // ponemos un 1 en el bit 1 para quedar con 10 (MODE PLL) en el SW

		/* el prescaler del AHB en division por 1 */
		RCC -> CFGR &= ~RCC_CFGR_HPRE;
		RCC -> CFGR |= RCC_CFGR_HPRE_DIV1;

		/* el preescaler de APB1 en division por 2 para los perifericos que trabajan maximo en 40MHz */
		RCC -> CFGR &= ~RCC_CFGR_PPRE1;
		RCC -> CFGR |= RCC_CFGR_PPRE1_DIV2;

		/* el preescaler de APB1 en division por 1, queda en 80MHz */
		RCC -> CFGR &= ~RCC_CFGR_PPRE2;
		RCC -> CFGR |= RCC_CFGR_PPRE2_DIV1;

	}

	// para poner el clock del micro a 100MHz
	else if(ptrHandlerPLL->FrecuenciaClock == PLL_CLOCK_100MHz){

		//primero configuramos el power control register para una frecuencia de PLL <= 100MHz
		// activamos el reloj
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		// ponemos los bits de Regulator voltage scaling (VOS), debe ser 11 para <= 100MHz
		PWR->CR |= PWR_CR_VOS_0;
		PWR->CR |= PWR_CR_VOS_1;

		//Luego se modifica la latencia de memoria EN 2ws para la frecuencia adecuada
		FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
		FLASH->ACR &= ~FLASH_ACR_LATENCY;  //se limpian los registros
		FLASH->ACR |= FLASH_ACR_LATENCY_3WS; // 3WS

		//sabemos que la velocidad por defecto es 16MHz, por lo tanto debemos usar PLLN, PLLM y PLLP para obtener 100MHz
		// F(PLL general clock output) = 16MHz(PLLN/(PLLM*PLLP))

		// El PLLM es el Factor de división para el reloj de entrada principal del PLL, el resultado será 2MHz
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;  // se limpia el registro
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_3; // Ponemos un 8 en el PLLM

		// El PLLN es el Factor de multiplicación principal del PLL. el valor debe estar entre 100 y 432MHz
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;  // se limpia el registro
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_2;  // Se pone en 100 el registro
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_5;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_6;
		//El resultado será 200MHz

		// PLLP es el factor de division principal, el resultado no debe exceder los 100 MHz en este dominio.
		// el PLLP se configura en 2 para que el resultado sea 100MHz
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP; // 2 en PLLP

		/*Configuramos el MCO1 para ver la frecuencia del PLL, esto con el fin de probar la frecuencia en el pin PA8*/

		/*Configuracion del PA8 como función alternativa para usarlo como prueba es mejor poner esto en el main
		handlerPinPrueba.pGPIOx = GPIOA;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		handlerPinPrueba.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;

		Cargamos la configuracion del Pin en el registro
		GPIO_Config(&handlerPinPrueba);*/

		// encendemos el PLL
		RCC->CR |= RCC_CR_PLLON;

		// Esperamos que el PLL se estabilice
		while(!(RCC->CR & RCC_CR_PLLRDY)){
			__NOP();
		}

		// Activando el PLL en el multiplexor SW, para que el clock del micro sea el PLL
		RCC->CFGR &= ~RCC_CFGR_SW; // limpiamos el registro
		RCC->CFGR |= RCC_CFGR_SW_1; // ponemos un 1 en el bit 1 para quedar con 10 (MODE PLL) en el SW

		/* el prescaler del AHB en division por 1 */
		RCC -> CFGR &= ~RCC_CFGR_HPRE;
		RCC -> CFGR |= RCC_CFGR_HPRE_DIV1;

		/* el preescaler de APB1 en division por 2 para los perifericos que trabajan maximo en 50MHz */
		RCC -> CFGR &= ~RCC_CFGR_PPRE1;
		RCC -> CFGR |= RCC_CFGR_PPRE1_DIV2;

		/* el preescaler de APB2 en division por 1, queda en 100MHz */
		RCC -> CFGR &= ~RCC_CFGR_PPRE2;
		RCC -> CFGR |= RCC_CFGR_PPRE2_DIV1;

	}
	else{
		// se activa el HSI, el cual es la velocidad por defecto del clock
		RCC->CFGR &= ~RCC_CFGR_SW; // ponemos el SW en HSI (los bits en 0 ambos)
	}
} // FIN configPLL

int getConfigPLL(void) {
    // Se leen y se extraen los valores de configuracion
    uint32_t PLLM = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos;
    uint32_t PLLN = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN_Msk) >> RCC_PLLCFGR_PLLN_Pos;

    // Se calcula el valor actual del clock
    if((RCC->CFGR) |= RCC_CFGR_SW == 0b10  ){
    	Velocidad = ((16000000 / PLLM) * PLLN) / 2;
    }
    else{
    	Velocidad = 16000000;
    }
    return Velocidad;
}

void configMCO1(MCO1_Handler_t *ptrHandlerMCO1){
	if(ptrHandlerMCO1->Reloj == Reloj_HSI){
		RCC->CFGR &= ~RCC_CFGR_MCO1_0;
		RCC->CFGR &= ~RCC_CFGR_MCO1_1;
		if(ptrHandlerMCO1->Preescaler == Preescaler_2){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->Preescaler == Preescaler_3){
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->Preescaler == Preescaler_4){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->Preescaler == Preescaler_5){
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->Preescaler == Preescaler_1){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_2;
		}
	}
	else if(ptrHandlerMCO1->Reloj == Reloj_LSE){
		RCC->CFGR |= RCC_CFGR_MCO1_0;
		RCC->CFGR &= ~RCC_CFGR_MCO1_1;
		if(ptrHandlerMCO1->Preescaler == Preescaler_2){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->Preescaler == Preescaler_3){
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->Preescaler == Preescaler_4){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->Preescaler == Preescaler_5){
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->Preescaler == Preescaler_1){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_2;
		}
	}
	else if(ptrHandlerMCO1->Reloj == Reloj_PLL){
		RCC->CFGR |= RCC_CFGR_MCO1_0;
		RCC->CFGR |= RCC_CFGR_MCO1_1;
		if (ptrHandlerMCO1->Preescaler == Preescaler_2) {
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if (ptrHandlerMCO1->Preescaler == Preescaler_3) {
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if (ptrHandlerMCO1->Preescaler == Preescaler_4) {
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if (ptrHandlerMCO1->Preescaler == Preescaler_5) {
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(ptrHandlerMCO1->Preescaler == Preescaler_1){
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE_2;
		}
	}
}

