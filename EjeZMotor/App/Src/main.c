/**
 ******************************************************************************
 * @file           : main.c
 * @author         : maalvarezlo by STM32CubeIDE
 * @Nombre          : Mateo Alvarez Lopera
 * @brief          : main configiguracion basica Lib externas
 ******************************************************************************

 ******************************************************************************
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "SysTickDriver.h"


/* Definicion de los elementos del sistema */
// Handlers de los pines
GPIO_Handler_t handlerLEDBlinky          = {0};
GPIO_Handler_t handlerSTEEP              = {0};
GPIO_Handler_t handlerDIR                = {0};
GPIO_Handler_t handlerUV                = {0};



// Handlers de los timers
BasicTimer_Handler_t handlerBlinkyTimer  = {0};
BasicTimer_Handler_t handlerMotor  = {0};



// Extis
EXTI_Config_t Exti                       = {0};

// Variables
uint16_t PasosTotales = 0;
double AlturaCapamm = 0;
double alturaActualmm = 0;
uint16_t AlturaCapaPasos = 0;
double alturaActualPasos = 0;
uint8_t pasosDadosSubiendo = 0;
uint8_t pasosDadosBajando = 0;
uint8_t banderaMotor = 0;
uint8_t DIR = 0;

//Definiendo las Funciones
void init_hardware (void);
void SecuenciaMotor (uint16_t);


int main(void){

/*Inicialización de todos los elementos del sistema*/
	init_hardware();

/* definiendo la altura de capa en pasos*/
	/* Teniendo en cuenta que cada paso del motor mueve la cama una altura de 0.4mm, es necesario que la altura de capa deseada
	 * sea multiplo de 0.04mm*/
	AlturaCapamm = 0.04;
	/* Para calcular los pasos de está altura de capa (teniendo en cuenta el motor y la varilla roscada*/
	AlturaCapaPasos = (AlturaCapamm*200)/8;

	config_SysTick_ms(SYSTICK_LOAD_VALUE_16MHz_1ms);

	while(1){

		DIR = GPIO_ReadPin(&handlerDIR);

		if(banderaMotor == 1){
			SecuenciaMotor(AlturaCapaPasos);
			banderaMotor = 0;
		}



	} // Fin while
} //Fin funcion main

/**/
void init_hardware (void){

/*Configuracion del LED del blinky en el puesto PA5 */
	handlerLEDBlinky.pGPIOx = GPIOA;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinNumber           = PIN_5;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLEDBlinky);
	GPIO_WritePin(&handlerLEDBlinky, SET);

	/*Configuracion del PIN que controla los pasos del motor en el puesto PB5 */
	handlerSTEEP.pGPIOx = GPIOB;
	handlerSTEEP.GPIO_PinConfig.GPIO_PinNumber           = PIN_5;
	handlerSTEEP.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerSTEEP.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerSTEEP.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerSTEEP.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerSTEEP);

	/*Configuracion del PIN que controla la direccion del motor en el puesto PC5 */
	handlerDIR.pGPIOx = GPIOC;
	handlerDIR.GPIO_PinConfig.GPIO_PinNumber           = PIN_5;
	handlerDIR.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerDIR.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerDIR.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerDIR.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDIR);
	GPIO_WritePin(&handlerDIR, RESET);

	/*Configuracion del PIN que controla la iluminacion UV en el puesto PA8 */
	handlerUV.pGPIOx = GPIOA;
	handlerUV.GPIO_PinConfig.GPIO_PinNumber           = PIN_8;
	handlerUV.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerUV.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerUV.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerUV.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUV);


/* Configuracion del TIM2 para que haga un blinky cada 250ms*/
	handlerBlinkyTimer.ptrTIMx                              = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP ;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed               = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period              = 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBlinkyTimer);


/* Configuracion del TIM3 configurar el STEEP del Motor cada 1ms*/
	handlerMotor.ptrTIMx                              = TIM3;
	handlerMotor.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP ;
	handlerMotor.TIMx_Config.TIMx_speed               = BTIMER_SPEED_1ms;
	handlerMotor.TIMx_Config.TIMx_period              = 5;
	handlerMotor.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerMotor);


} // Termina el int_Hardware

// El motor subirá 200 pasos (8mm) y bajará el numero de pasos determiando para que quede la diferencia de la altura de capa
void SecuenciaMotor(uint16_t AlturadeCapa){
	if(GPIO_ReadPin(&handlerDIR) == RESET){
		//mover hacia arriba un numeros de 200 pasos para una altura total de 8mm
		if (pasosDadosSubiendo < 200) {
			GPIOxTooglePin(&handlerSTEEP);
			pasosDadosSubiendo++;
		}
		else {
			GPIO_WritePin(&handlerDIR, SET);
			pasosDadosSubiendo = 0;
		}
	}
	else if (GPIO_ReadPin(&handlerDIR) == SET){

		if (pasosDadosBajando < (200-AlturadeCapa)){
			GPIOxTooglePin(&handlerSTEEP);
			pasosDadosBajando++;
		}
		else {
			GPIO_WritePin(&handlerDIR, RESET);
			pasosDadosBajando = 0;
			alturaActualPasos = alturaActualPasos+AlturadeCapa;
			GPIO_WritePin(&handlerUV, SET);
			delay_ms(5000);
			GPIO_WritePin(&handlerUV, RESET);
		}
	}
	PasosTotales++;
	alturaActualmm = (alturaActualPasos*8)/200;
}

// aca se ejecuta el Blinky
void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerLEDBlinky);

}


void BasicTimer3_Callback(void){
	banderaMotor = 1;
}





