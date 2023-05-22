/**
 ******************************************************************************
 * @file           : main.c
 * @author         : maalvarezlo by STM32CubeIDE
 * @Nombre          : Mateo Alvarez Lopera
 * @brief          : seno ARM Funcion
 ******************************************************************************

 ******************************************************************************
 */

#include <stdint.h>
#include <stm32f4xx.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"

#include <math.h>

#include "arm_math.h"


/* Definicion de los elementos del sistema */

// Handlers de los pines
GPIO_Handler_t handlerLEDBlinky          = {0};
GPIO_Handler_t handlerpinEXTI            = {0};



// Handlers de los timers
BasicTimer_Handler_t handlerBlinkyTimer  = {0};
BasicTimer_Handler_t handlerMensaje      = {0};



// Extis
EXTI_Config_t Exti                       = {0};


// Comunicacion serial
USART_Handler_t Usart2Comm               = {0};
GPIO_Handler_t handlerPinTX              = {0};
GPIO_Handler_t handlerPinRX              = {0};
uint16_t printMSJ                        = 0;
uint8_t usart2DataReceived               = 0;

char bufferMsj[64]                       = {0};

/*Pruebas de libreria CMSIS*/
float32_t srcNumber[4] = {-0.212, 45.444, -45.23, -23234.423423};
float32_t destNumber[4] = {0};
uint32_t dataSize = 0;

//Variables para usar sen
float32_t sineValue = 0;
float32_t sineArgValue = 0;


//Definiendo las Funciones
void init_hardware (void);


int main(void){

/*Activamos el coprocesador matematicos*/
	SCB->CPACR |= (0xF << 20);
/*Inicialización de todos los elementos del sistema*/
	init_hardware();

	while(1){

		if(usart2DataReceived == 'A'){
			//dataSize = sizeof(srcNumber); no funciona
			dataSize = 4;
			arm_abs_f32(srcNumber, destNumber, dataSize);

			sprintf(bufferMsj, "Valor Absoluto de %#.2f = %#.2f \n", srcNumber[0], destNumber[0]);
			writeMsg(&Usart2Comm, bufferMsj);

			sprintf(bufferMsj, "Valor Absoluto de %#.2f = %#.2f \n", srcNumber[1], destNumber[1]);
			writeMsg(&Usart2Comm, bufferMsj);

			sprintf(bufferMsj, "Valor Absoluto de %#.2f = %#.2f \n", srcNumber[2], destNumber[2]);
			writeMsg(&Usart2Comm, bufferMsj);

			sprintf(bufferMsj, "Valor Absoluto de %#.2f = %#.2f \n", srcNumber[3], destNumber[3]);
			writeMsg(&Usart2Comm, bufferMsj);

			usart2DataReceived = '\0';
		}

		if(usart2DataReceived == 'B'){
			sineArgValue = M_PI/4;
			sineValue  = arm_sin_f32(sineArgValue);

			sprintf(bufferMsj, "El Seno de %#.2f = %#.6f \n", sineArgValue, sineValue);
			writeMsg(&Usart2Comm, bufferMsj);

			usart2DataReceived = '\0';
		}

		if(usart2DataReceived == 'C'){
			sineArgValue = M_PI/8;
			sineValue  = arm_sin_f32(sineArgValue);

			sprintf(bufferMsj, "El Seno de %#.2f = %#.6f \n", sineArgValue, sineValue);
			writeMsg(&Usart2Comm, bufferMsj);

			usart2DataReceived = '\0';
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

/*Configuracion PIN para el exti (debe ser un pin como entrada)*/
	handlerpinEXTI.pGPIOx = GPIOB;
	handlerpinEXTI.GPIO_PinConfig.GPIO_PinNumber           = PIN_3;
	handlerpinEXTI.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_IN;
	handlerpinEXTI.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerpinEXTI.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerpinEXTI.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerpinEXTI);

/* Configuracion del TIM2 para que haga un blinky cada 250ms*/
	handlerBlinkyTimer.ptrTIMx                              = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP ;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed               = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period              = 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBlinkyTimer);

/* Configuracion del TIM3 para definir cuando se envia un mensaje*/
	handlerMensaje.ptrTIMx                              = TIM3;
	handlerMensaje.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP ;
	handlerMensaje.TIMx_Config.TIMx_speed               = BTIMER_SPEED_1ms;
	handlerMensaje.TIMx_Config.TIMx_period              = 1250;
	handlerMensaje.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerMensaje);

// Configurando la interrucion del EXTI
	Exti.edgeType                                           = EXTERNAL_INTERRUPT_FALLING_EDGE;
	Exti.pGPIOHandler                                       = &handlerpinEXTI;
	extInt_Config(&Exti);


	/* Configuracion de la comunicacion serial USART*/
	//PINTX del usart
	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber           = PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF7;
	GPIO_Config(&handlerPinTX);

	//PINRX del usart
	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber           = PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF7;
	GPIO_Config(&handlerPinRX);

	Usart2Comm.ptrUSARTx                                     = USART2;
	Usart2Comm.USART_Config.USART_baudrate                   = USART_BAUDRATE_115200;
	Usart2Comm.USART_Config.USART_datasize                   = USART_DATASIZE_8BIT;
	Usart2Comm.USART_Config.USART_mode                       = USART_MODE_RXTX;
	Usart2Comm.USART_Config.USART_parity                     = USART_PARITY_NONE;
	Usart2Comm.USART_Config.USART_stopbits                   = USART_STOPBIT_1;
	Usart2Comm.USART_Config.USART_enableIntTX                = USART_TX_INTERRUPT_DISABLE;
	Usart2Comm.USART_Config.USART_enableIntRX                = USART_RX_INTERRUPT_ENABLE;



	USART_Config(&Usart2Comm);


} // Termina el int_Hardware



// aca se ejecuta el Blinky
void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerLEDBlinky);
}

void BasicTimer3_Callback(void){

	//printMSJ++;
}


void callback_extInt3 (void){
 // aca se debe levantar una bandera
}

void usart2Rx_Callback(void){
	usart2DataReceived = getRxData();

}





