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
#include "PLLDriver.h"
#include "I2CDriver.h"
#include "arm_math.h"
#include <math.h>
#include "LCDxDriver.h"
#include "SysTickDriver.h"




/* Definicion de los elementos del sistema */
/*Handler PLL para la velocidad del micro*/
PLL_Handler_t handlerPLL80MHz            = {0};


// Handlers de los pines
GPIO_Handler_t handlerLEDBlinky          = {0};
GPIO_Handler_t handlerpinEXTI            = {0};
GPIO_Handler_t handlerPinPrueba          = {0};


// Handlers y banderas de los timers
BasicTimer_Handler_t handlerBlinkyTimer  = {0};
BasicTimer_Handler_t handler1HzTimer     = {0};
uint8_t banderaMuestreo                  = 0;
uint16_t numeroMuestreo                   = 0;




// Extis
EXTI_Config_t Exti                       = {0};


// Comunicacion USART
USART_Handler_t Usart2Comm               = {0};
GPIO_Handler_t handlerPinTX              = {0};
GPIO_Handler_t handlerPinRX              = {0};
uint16_t printMSJ                        = 0;
uint8_t usart2DataReceived               = 0;
char bufferMsj[64]                       = {0};
int8_t ArregloX[]                          ={0};
int8_t ArregloY[]                          ={0};
int8_t ArregloZ[]                          ={0};
uint8_t numeroArreglo = 0;
uint8_t i = 0;


/* Configuración para el I2C */
GPIO_Handler_t			handlerSCL				= {0};
GPIO_Handler_t			handlerSDA				= {0};
I2C_Handler_t handlerLCD 						= {0};
char dataLCD[64] 				= {0};



// direccion esclavo de la LCD
#define LCD_ADRESS		0b0100111
#define ACCEL_ADDRESS          	 0x1D
#define ACCEL_XOUT_L             50
#define ACCEL_XOUT_H             51
#define ACCEL_YOUT_L             52
#define ACCEL_YOUT_H             53
#define ACCEL_ZOUT_L             54
#define ACCEL_ZOUT_H             55

#define POWER_CTL                45
#define WHO_AM_I                 0





//Definiendo las Funciones
void init_hardware (void);


int main(void){

	init_hardware();

	LCD_Clear(&handlerLCD);
	delay_10();
	LCD_Init(&handlerLCD);
	delay_10();
	sprintf(dataLCD, "Examen Taller V");
	LCD_setCursor(&handlerLCD,0,0);
	LCD_sendSTR(&handlerLCD,dataLCD);
	sprintf(dataLCD, "Hecho por: JFGC");
	LCD_setCursor(&handlerLCD,0,1);
	LCD_sendSTR(&handlerLCD,dataLCD);

	/* Loop forever */

	while(1){
		/*if(usart2DataReceived != '\0'){
				sprintf(bufferMsj, "%c", usart2DataReceived);
				writeMsg(&Usart2Comm, bufferMsj);
				usart2DataReceived = '\0';
		}*/


	} // Fin while
	return(0);
} //Fin funcion main

/**/
void init_hardware (void){

/*Configuramos la velocidad del micro*/
	handlerPLL80MHz.FrecuenciaClock  = PLL_CLOCK_80MHz;
	configPLL(&handlerPLL80MHz);


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
	//Si el timer esta a 80MHz deben usarse la Speed que termina en _PLL80_100us
	handlerBlinkyTimer.ptrTIMx                              = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP ;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed               = BTIMER_SPEED_PLL80_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period              = 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	//Si el timer esta a 80MHz deben usarse la Speed que termina en _PLL80_100us
	handler1HzTimer.ptrTIMx                              = TIM3;
	handler1HzTimer.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP ;
	handler1HzTimer.TIMx_Config.TIMx_speed               = BTIMER_SPEED_PLL80_100us;
	handler1HzTimer.TIMx_Config.TIMx_period              = 2;
	handler1HzTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handler1HzTimer);


// Configurando la interrucion del EXTI
	Exti.edgeType                                           = EXTERNAL_INTERRUPT_FALLING_EDGE;
	Exti.pGPIOHandler                                       = &handlerpinEXTI;
	extInt_Config(&Exti);


/* Configuracion de la comunicacion serial USART*/
	//PINTX del usart para Usart6(PA11 AF8) Para Usart2(PA2 AF7)
	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber           = PIN_11;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF8;
	GPIO_Config(&handlerPinTX);
	//PINRX del usart para Usart6(PA12 AF8) Para Usart2(PA3 AF7)
	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber           = PIN_12;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF8;
	GPIO_Config(&handlerPinRX);
	//cuando el micro este a 80MHz deben usarse los baudrate terminados en _80MHz
	Usart2Comm.ptrUSARTx                                     = USART6;
	Usart2Comm.USART_Config.USART_baudrate                   = USART_BAUDRATE_19200_80MHz;
	Usart2Comm.USART_Config.USART_datasize                   = USART_DATASIZE_8BIT;
	Usart2Comm.USART_Config.USART_mode                       = USART_MODE_RXTX;
	Usart2Comm.USART_Config.USART_parity                     = USART_PARITY_NONE;
	Usart2Comm.USART_Config.USART_stopbits                   = USART_STOPBIT_1;
	Usart2Comm.USART_Config.USART_enableIntTX                = USART_TX_INTERRUPT_DISABLE;
	Usart2Comm.USART_Config.USART_enableIntRX                = USART_RX_INTERRUPT_ENABLE;
	USART_Config(&Usart2Comm);

	/*Probando el PLL*/
	handlerPinPrueba.pGPIOx = GPIOA;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinNumber = PIN_8;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;

	//Cargamos la configuracion del Pin en el registro
	GPIO_Config(&handlerPinPrueba);

	/*Configuracion para I2C*/

	handlerSCL.pGPIOx 											= GPIOB;
		handlerSCL.GPIO_PinConfig.GPIO_PinNumber 					= PIN_8;
		handlerSCL.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_ALTFN;
		handlerSCL.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_OPENDRAIN;
		handlerSCL.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
		handlerSCL.GPIO_PinConfig.GPIO_PinPuPdControl 				= GPIO_PUPDR_PULLUP;
		handlerSCL.GPIO_PinConfig.GPIO_PinAltFunMode				= AF4;
		GPIO_Config(&handlerSCL);

		handlerSDA.pGPIOx 											= GPIOB;
		handlerSDA.GPIO_PinConfig.GPIO_PinNumber 					= PIN_9;
		handlerSDA.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_ALTFN;
		handlerSDA.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_OPENDRAIN;
		handlerSDA.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
		handlerSDA.GPIO_PinConfig.GPIO_PinPuPdControl 				= GPIO_PUPDR_PULLUP;
		handlerSDA.GPIO_PinConfig.GPIO_PinAltFunMode				= AF4;
		GPIO_Config(&handlerSDA);

		// configuracion de la LCD con la direccion del esclavo
		handlerLCD.ptrI2Cx											= I2C1;
		handlerLCD.modeI2C											= I2C_MODE_SM;
		handlerLCD.slaveAddress										= LCD_ADDRESS;
		i2c_config(&handlerLCD);



} // Termina el int_Hardware



// aca se ejecuta el Blinky
void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerLEDBlinky);
}
// aca se ejecuta el Blinky
void BasicTimer3_Callback(void){
	banderaMuestreo = 1;
}


void callback_extInt3 (void){
 // aca se debe levantar una bandera
}

//recibir datos con el usart
void usart6Rx_Callback(void){
	usart2DataReceived = getRxData();
}





