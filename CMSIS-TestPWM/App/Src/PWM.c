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
#include "PwmDriver.h"

#include <math.h>

#include "arm_math.h"


/* Definicion de los elementos del sistema */

// Handlers de los pines
GPIO_Handler_t handlerLEDBlinky          = {0};
GPIO_Handler_t handlerpinEXTI            = {0};
GPIO_Handler_t handlerPinPWM3               = {0};


// Handlers de los timers
BasicTimer_Handler_t handlerBlinkyTimer  = {0};
BasicTimer_Handler_t handlerMensaje      = {0};
// PWM
PWM_Handler_t handerPWM3    ={0};
uint16_t Dutty               = 1500;


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

		if(usart2DataReceived |= '\0'){

			if(usart2DataReceived == 'D'){

				Dutty -= 10;
				updateDuttyCycle(&handerPWM3, Dutty);
			}
			else if(usart2DataReceived == 'U'){

				Dutty += 10;
				updateDuttyCycle(&handerPWM3, Dutty);
			}

			sprintf(bufferMsj, "El Valor del Dutty es %u \n", (unsigned int)Dutty);
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

/*Configuracion PIN para el PWM (debe ser un pin como Funcion)*/
	handlerPinPWM3.pGPIOx = GPIOC;
	handlerPinPWM3.GPIO_PinConfig.GPIO_PinNumber           = PIN_6;
	handlerPinPWM3.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinPWM3.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerPinPWM3.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerPinPWM3.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	handlerPinPWM3.GPIO_PinConfig.GPIO_PinAltFunMode       = AF2;


	GPIO_Config(&handlerPinPWM3);

/* Configuracion del TIM2 para que haga un blinky cada 250ms*/
	handlerBlinkyTimer.ptrTIMx                              = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP ;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed               = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period              = 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;

	BasicTimer_Config(&handlerBlinkyTimer);


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

	//Configuracion PWM
	handerPWM3.ptrTIMx = TIM3;
	handerPWM3.config.channel = PWM_CHANNEL_1;
	handerPWM3.config.duttyCicle = Dutty;
	handerPWM3.config.periodo = 20000;
	handerPWM3.config.prescaler = 16;

	pwm_Config(&handerPWM3);
	// Activando señal
	enableOutput(&handerPWM3);
	startPwmSignal(&handerPWM3);


} // Termina el int_Hardware



// aca se ejecuta el Blinky
void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerLEDBlinky);
}


void callback_extInt3 (void){
 // aca se debe levantar una bandera
}

void usart2Rx_Callback(void){
	usart2DataReceived = getRxData();

}





