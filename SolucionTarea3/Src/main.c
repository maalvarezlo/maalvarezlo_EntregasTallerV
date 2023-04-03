/**
 ******************************************************************************
 * @file           : main.c
 * @author         : maalvarezlo by STM32CubeIDE
 * @brief          : configuracion basica del proyecto
 ******************************************************************************

 ******************************************************************************
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

/* Definicion de los elementos del sistema */
//Handlers
GPIO_Handler_t handlerLEDBlinky = {0};
GPIO_Handler_t handlerencoderCLK = {0};
GPIO_Handler_t handlerencoderDT = {0};
GPIO_Handler_t handlerencoderSW = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};

EXTI_Config_t Exti_encoderCLK ={0};
EXTI_Config_t Exti_encoderDT ={0};
EXTI_Config_t Exti_encoderSW ={0};


uint8_t counterExti1 = 0;
uint8_t contador = 0;
uint8_t contador2 = 0;
uint8_t banderasw = 0;
uint8_t banderadata = 0;
uint8_t banderaclock = 0;
uint8_t x = 0;
uint8_t y = 0;



/* Prototipos de las funciones del main*/
void init_hardware (void);
/**/

int main(void){

/*Inicialización de todos los elementos del sistema*/
	init_hardware();

	while(1){

		/* El programa empezará en el modo contador, la banderaSW cambiara de 0 a 1 y viceversa cada vez que
		se presiona el boton del encoder (esto se configura en la funcion callback_extInt5) */
		if(banderasw == 0){
			if(GPIO_ReadPin(&handlerencoderDT) == 1 && banderaclock){
				if(contador2 < 99){
					contador2++;
				}
				banderaclock = 0;
			}
			else if(GPIO_ReadPin(&handlerencoderDT) == 0 && banderaclock ){
				if(contador2 > 0){
					contador2--;
				}
				banderaclock = 0;
			}
		}
		// Aca se cambiara al modo culebrita cuando se presiona el boton del encoder
		else{
			if(GPIO_ReadPin(&handlerencoderDT) == 1 && banderaclock){
				if(contador2 > 0){
					contador2--;
				}
				banderaclock = 0;
			}
			else if(GPIO_ReadPin(&handlerencoderDT) == 0 && banderaclock ){
				if(contador2 < 99){
					contador2++;
				}
				banderaclock = 0;
			}
		}


	} // fin while
} //FIN FUNCION main

/**/
void init_hardware (void){

/*Configuracion del LED del blinky en el puesto PB10 */
	handlerLEDBlinky.pGPIOx = GPIOB;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

/* Cargar la configuracion del LED2 en los registros*/
	GPIO_Config(&handlerLEDBlinky);

	GPIO_WritePin(&handlerLEDBlinky, SET);

/* Configuracion del TIM2 para que haga un blinky cada 300ms*/
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP ;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 250;  // Lanza una interrupción cada 300ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;

/* Cargando la configuracion del TIM2 en los registros*/
	BasicTimer_Config(&handlerBlinkyTimer);


	// configurando el pin del clock
	handlerencoderCLK.pGPIOx = GPIOB;
	handlerencoderCLK.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerencoderCLK.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerencoderCLK.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerencoderCLK.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerencoderCLK.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerencoderCLK.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	GPIO_Config(&handlerencoderCLK);


	// configurando el pin del Data
	handlerencoderDT.pGPIOx = GPIOB;
	handlerencoderDT.GPIO_PinConfig.GPIO_PinNumber			= PIN_4;
	handlerencoderDT.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerencoderDT.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerencoderDT.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerencoderDT.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerencoderDT.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	GPIO_Config(&handlerencoderDT);

	//configurando el pin para el SW (cuando se presiona el boton del encoder)
	handlerencoderSW.pGPIOx = GPIOB;
	handlerencoderSW.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerencoderSW.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerencoderSW.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerencoderSW.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerencoderSW.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerencoderSW.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	GPIO_Config(&handlerencoderSW);


	// Configurando la interrucion del encoder al girar
	Exti_encoderCLK.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	Exti_encoderCLK.pGPIOHandler = &handlerencoderCLK;

	extInt_Config(&Exti_encoderCLK);


	// Configurando la interrucion del encoder
	Exti_encoderSW.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	Exti_encoderSW.pGPIOHandler = &handlerencoderSW;

	extInt_Config(&Exti_encoderSW);

} // Termina el int_Hardware



void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerLEDBlinky);

}

void callback_extInt3 (void){
	banderaclock = 1;

}


void callback_extInt5 (void){
	if(banderasw == 0){
		banderasw = 1;
	}
	else{
		banderasw = 0;
	}
	counterExti1++;
}




