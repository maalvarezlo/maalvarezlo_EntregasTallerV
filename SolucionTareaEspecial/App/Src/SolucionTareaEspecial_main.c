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
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "DriverLCD.h"



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
BasicTimer_Handler_t handler1sTimer      = {0};
uint8_t banderaMuestreo                  = 1;
uint16_t numeroMuestreo                  = 0;
uint16_t infinito                        = 0;
uint8_t banderaLCD                       = 0;


// Extis
EXTI_Config_t Exti                       = {0};

// PWM
GPIO_Handler_t handlerPinPWMX               = {0};
GPIO_Handler_t handlerPinPWMY               = {0};
GPIO_Handler_t handlerPinPWMZ               = {0};
PWM_Handler_t handerPWMX                    = {0};
PWM_Handler_t handerPWMY                    = {0};
PWM_Handler_t handerPWMZ                    = {0};
uint16_t DuttyX                             = 0;
uint16_t DuttyY                             = 0;
uint16_t DuttyZ                             = 0;

// Comunicacion USART
USART_Handler_t UsartComm                = {0};
GPIO_Handler_t handlerPinTX              = {0};
GPIO_Handler_t handlerPinRX              = {0};
uint16_t printMSJ                        = 0;
uint8_t usartDataReceived                = 0;
char bufferMsj[64]                       = {0};
char bufferMsj2[64]                      = {0};
float ArregloX[256]                     = {0};
float ArregloY[256]                     = {0};
float ArregloZ[256]                     = {0};
uint8_t numeroArreglo                    = 0;


//Configuracion para el I2C
GPIO_Handler_t handleI2cSDA             = {0};
GPIO_Handler_t handleI2cSCL             = {0};
I2C_Handler_t handlerAccelerometer      = {0};
GPIO_Handler_t handlerI2cSDA2           = {0};
GPIO_Handler_t handlerI2cSCL2           = {0};
I2C_Handler_t handlerLCD                = {0};
uint8_t i2cBuffer                       = 0;
char bufferx[64]                        = {0};
char buffery[64]                        = {0};
char bufferz[64]                        = {0};


//acelerometro
#define ACCEL_ADDRESS          	 0x1D
#define ACCEL_XOUT_L             50
#define ACCEL_XOUT_H             51
#define ACCEL_YOUT_L             52
#define ACCEL_YOUT_H             53
#define ACCEL_ZOUT_L             54
#define ACCEL_ZOUT_H             55
#define BW_RATE                  44
#define POWER_CTL                45
#define WHO_AM_I                 0

//LCD
#define LCD_ADDRESS	        	0x21


//Definiendo las Funciones
void init_hardware (void);
void LCDfijo(void);
void Loop_Principal(void);
void Menu(void);
void Press_w(void);
void Press_p(void);
void Press_r(void);
void Press_x(void);
void Press_y(void);
void Press_z(void);
void Press_d(void);
void Press_l(void);



int main(void){
//Activamos el coprocesador Matematico
	SCB->CPACR |= (0xF <<20);
/*Inicialización de todos los elementos del sistema*/
	init_hardware();
	Menu();
	LCDfijo();

	while(1){

		//aca se activa un muestreo constante a 1KHz, no se imprimen los valores para que no estorben
		//en el coolterm, sin embargo tambien se varia el dutty del PWM asique cuando se mueve el
		//acelerometro se pueden ver comovarian de intensidad 3 leds colocados en la protoboard
		Loop_Principal();

		// cuando el usuario presione una de las siguientes teclas se activara su funcion respectiva
		if(usartDataReceived != '\0'){
			if(usartDataReceived == 'm'){
				Menu();
			}
			if(usartDataReceived == 'w'){
				Press_w();
			}
			else if (usartDataReceived == 'p'){
				Press_p();
			}
			else if (usartDataReceived == 'r'){
				Press_r();
			}
			else if (usartDataReceived == 'x'){
				Press_x();
			}
			else if(usartDataReceived == 'y'){
				Press_y();
			}
			else if(usartDataReceived == 'z'){
				Press_z();
			}
			if (usartDataReceived == 'd'){
				Press_d();
			}
			if (usartDataReceived == 'l'){
				Press_l();
			}
		}

	} // Fin while
	return(0);
} //Fin funcion main


/*aca se inicializan todos los elemntos*/
void init_hardware (void){

/*Configuramos la velocidad del micro*/
	handlerPLL80MHz.FrecuenciaClock  = PLL_CLOCK_80MHz;
	configPLL(&handlerPLL80MHz);
	config_SysTick_ms(SYSTICK_LOAD_VALUE_80MHz_1ms);


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
	handler1HzTimer.ptrTIMx                              = TIM4;
	handler1HzTimer.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP ;
	handler1HzTimer.TIMx_Config.TIMx_speed               = BTIMER_SPEED_PLL80_100us;
	handler1HzTimer.TIMx_Config.TIMx_period              = 10;
	handler1HzTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handler1HzTimer);

	/* Configuracion del TIM5 */
		//Si el timer esta a 80MHz deben usarse la Speed que termina en _PLL80_100us
	handler1sTimer.ptrTIMx                                    = TIM5;
	handler1sTimer.TIMx_Config.TIMx_mode                      = BTIMER_MODE_UP;
	handler1sTimer.TIMx_Config.TIMx_speed                     = BTIMER_SPEED_PLL80_100us;
	handler1sTimer.TIMx_Config.TIMx_period                    = 10000;
	handler1sTimer.TIMx_Config.TIMx_interruptEnable           = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handler1sTimer);


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
	UsartComm.ptrUSARTx                                     = USART6;
	UsartComm.USART_Config.USART_baudrate                   = USART_BAUDRATE_115200_80MHz;
	UsartComm.USART_Config.USART_datasize                   = USART_DATASIZE_8BIT;
	UsartComm.USART_Config.USART_mode                       = USART_MODE_RXTX;
	UsartComm.USART_Config.USART_parity                     = USART_PARITY_NONE;
	UsartComm.USART_Config.USART_stopbits                   = USART_STOPBIT_1;
	UsartComm.USART_Config.USART_enableIntTX                = USART_TX_INTERRUPT_DISABLE;
	UsartComm.USART_Config.USART_enableIntRX                = USART_RX_INTERRUPT_ENABLE;
	USART_Config(&UsartComm);

	/*Probando el PLL*/
	handlerPinPrueba.pGPIOx = GPIOA;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinNumber          = PIN_8;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_PUSHPULL;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_FAST;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	handlerPinPrueba.GPIO_PinConfig.GPIO_PinAltFunMode      = AF0;

	//Cargamos la configuracion del Pin en el registro
	GPIO_Config(&handlerPinPrueba);

	/*-------------------------------------------------Configuracion de I2c----------------------------------------------------------*/

	//ACELEROMETRO
	handleI2cSCL.pGPIOx                                  = GPIOB;
	handleI2cSCL.GPIO_PinConfig.GPIO_PinNumber           = PIN_8;
	handleI2cSCL.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handleI2cSCL.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_OPENDRAIN;
	handleI2cSCL.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handleI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_PULLUP;
	handleI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode       = AF4;
	GPIO_Config(&handleI2cSCL);

	handleI2cSDA.pGPIOx                                  = GPIOB;
	handleI2cSDA.GPIO_PinConfig.GPIO_PinNumber           = PIN_9;
	handleI2cSDA.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handleI2cSDA.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_OPENDRAIN;
	handleI2cSDA.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handleI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_PULLUP;
	handleI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode       = AF4;
	GPIO_Config(&handleI2cSDA);

	handlerAccelerometer.ptrI2Cx            = I2C1;
	handlerAccelerometer.modeI2C            = I2C_MODE_FM;
	handlerAccelerometer.slaveAddress       = ACCEL_ADDRESS;
	i2c_config(&handlerAccelerometer);
	//CAmbiando el muestreo del acelerometro
//	i2c_writeSingleRegister(&handlerAccelerometer, BW_RATE, 0xE);

	//LCD
	// Configurando los pines sobre los que funciona el I2C1
	handlerI2cSCL2.pGPIOx								= GPIOB;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinNumber		= PIN_10;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinOPType	    = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF4;
	GPIO_Config(&handlerI2cSCL2);

	handlerI2cSDA2.pGPIOx								= GPIOB;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF9;
	GPIO_Config(&handlerI2cSDA2);

	// Configuramos el protocolo I2C y cargamos dicha configuración
	handlerLCD.ptrI2Cx			= I2C2;
	handlerLCD.modeI2C			= I2C_MODE_SM;
	handlerLCD.slaveAddress		= LCD_ADDRESS;

	i2c_config(&handlerLCD);

/*-------------------------------------------------Configuracion de PWM----------------------------------------------------------*/

	/*Configuracion PIN para el PWMX (debe ser un pin como Funcion)*/
	handlerPinPWMX.pGPIOx = GPIOB;
	handlerPinPWMX.GPIO_PinConfig.GPIO_PinNumber           = PIN_4;
	handlerPinPWMX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinPWMX.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerPinPWMX.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerPinPWMX.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	handlerPinPWMX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF2;
	GPIO_Config(&handlerPinPWMX);
	//Configuracion PWMX
	handerPWMX.ptrTIMx                                     = TIM3;
	handerPWMX.config.channel                              = PWM_CHANNEL_1;
	handerPWMX.config.duttyCicle                           = DuttyX;
	handerPWMX.config.periodo                              = 20000;
	handerPWMX.config.prescaler                            = 80;
	pwm_Config(&handerPWMX);
	enableOutput(&handerPWMX);
	startPwmSignal(&handerPWMX);

	/*Configuracion PIN para el PWMY (debe ser un pin como Funcion)*/
	handlerPinPWMY.pGPIOx                                 = GPIOB;
	handlerPinPWMY.GPIO_PinConfig.GPIO_PinNumber          = PIN_5;
	handlerPinPWMY.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN;
	handlerPinPWMY.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_PUSHPULL;
	handlerPinPWMY.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_FAST;
	handlerPinPWMY.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	handlerPinPWMY.GPIO_PinConfig.GPIO_PinAltFunMode      = AF2;
	GPIO_Config(&handlerPinPWMY);
	//Configuracion PWMY
	handerPWMY.ptrTIMx                                    = TIM3;
	handerPWMY.config.channel                             = PWM_CHANNEL_2;
	handerPWMY.config.duttyCicle                          = DuttyY;
	handerPWMY.config.periodo                             = 20000;
	handerPWMY.config.prescaler                           = 80;
	pwm_Config(&handerPWMY);
	enableOutput(&handerPWMY);
	startPwmSignal(&handerPWMY);

	/*Configuracion PIN para el PWMZ (debe ser un pin como Funcion)*/
	handlerPinPWMZ.pGPIOx                                 = GPIOB;
	handlerPinPWMZ.GPIO_PinConfig.GPIO_PinNumber          = PIN_0;
	handlerPinPWMZ.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN;
	handlerPinPWMZ.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_PUSHPULL;
	handlerPinPWMZ.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_FAST;
	handlerPinPWMZ.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	handlerPinPWMZ.GPIO_PinConfig.GPIO_PinAltFunMode      = AF2;
	GPIO_Config(&handlerPinPWMZ);
	//Configuracion PWMZ
	handerPWMZ.ptrTIMx                                    = TIM3;
	handerPWMZ.config.channel                             = PWM_CHANNEL_3;
	handerPWMZ.config.duttyCicle                          = DuttyZ;
	handerPWMZ.config.periodo                             = 20000;
	handerPWMZ.config.prescaler                           = 80;

	pwm_Config(&handerPWMZ);
	// Activando señal
	enableOutput(&handerPWMZ);
	startPwmSignal(&handerPWMZ);

} // Termina el int_Hardware

// aca se inicializa la LCD y se agregan los caracteres fijos
void LCDfijo(void){

	ResetScreenLCD(&handlerLCD);
	InicioLCD(&handlerLCD);
	delay_ms(10);
	LimpiarLCD(&handlerLCD);
	delay_ms(10);
	moverCursorLCD(&handlerLCD, 0, 1);
	sendMSJCD(&handlerLCD, "Tarea Especial");
	moverCursorLCD(&handlerLCD, 2, 1);
	sendMSJCD(&handlerLCD, "Mateo Alvarez L.");
	delay_ms(10000);
	LimpiarLCD(&handlerLCD);
	moverCursorLCD(&handlerLCD, 0, 1);
	sendMSJCD(&handlerLCD, "Ax =");
	moverCursorLCD(&handlerLCD, 1, 1);
	sendMSJCD(&handlerLCD, "Ay = ");
	moverCursorLCD(&handlerLCD, 2, 1);
	sendMSJCD(&handlerLCD, "Az = ");
	moverCursorLCD(&handlerLCD, 3, 1);
	sendMSJCD(&handlerLCD, "Sensi = 0.153 m/s2");

}

//En esta funcion se realizauna toma de datos constante para controlar el PWM siempre y la LCD siempre, independientemente si
// se presiona una tecla o no
void Loop_Principal(void){
	banderaMuestreo = 1;
	while (infinito < 2000) {
		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
		int16_t AccelX = AccelX_high << 8 | AccelX_low;

		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
		int16_t AccelY = AccelY_high << 8 | AccelY_low;

		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
		int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;

		infinito = 0; // para que nunca salga del while hasta que se presione otra tecla

		//si se levanta la bandera del timmer 5 (cada 1s) la lcd muetsra los nuevos valores
		if(banderaLCD == 1){
			sprintf(bufferx,"%.2f m/s2  ",(AccelX / 210.f) * 9.78);
			sprintf(buffery,"%.2f m/s2  ",(AccelY / 210.f) * 9.78);
			sprintf(bufferz,"%.2f m/s2  ",(AccelZ / 210.f) * 9.78);

			moverCursorLCD(&handlerLCD, 0, 8);
			sendMSJCD(&handlerLCD, bufferx);
			moverCursorLCD(&handlerLCD, 1, 8);
			sendMSJCD(&handlerLCD, buffery);
			moverCursorLCD(&handlerLCD, 2, 8);
			sendMSJCD(&handlerLCD, bufferz);

			banderaLCD =0;
		}

		// linealizamos el dutty para que cada vez que varien los valores que detecta el acelerometro tambien varie el dutty
		DuttyX = 830 * ((AccelX / 210.f) * 9.78) + 10000;
		updateDuttyCycle(&handerPWMX, DuttyX);
		DuttyY = 830 * ((AccelY / 210.f) * 9.78) + 10000;
		updateDuttyCycle(&handerPWMY, DuttyY);
		DuttyZ = 830 * ((AccelZ / 210.f) * 9.78) + 10000;
		updateDuttyCycle(&handerPWMZ, DuttyZ);

		if (usartDataReceived != '\0') {
			infinito = 2000;
		}
	}
	infinito = 0;
	banderaMuestreo = 0;
} //FIN LOOP PRINCIPAL

void Menu(void){
	sprintf(bufferMsj, "Hola, soy el MCU de Mateo y tengo una frecuencia de %d Hz \n ", getConfigPLL());
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "- Presiona m para mostrar este menu cuando desees. \n ");
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "- Presiona w para imprimir WHO I AM?. \n ");
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "- Presiona r para resetear los valores del acelerometro. \n ");
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "- Presiona p para ver el PWR. \n ");
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "- Presiona x para ver el valor en el eje x. \n ");
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "- Presiona y para ver el valor en el eje y. \n ");
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "- Presiona z para ver el valor en el eje z. \n ");
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "- Presiona d para recoger 6000 datos en ");
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "2s y mostrarlos como una tabla. \n ");
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "- Presiona l para ver un muestreo de datos constante a 1KHz. ");
	writeMsg(&UsartComm, bufferMsj);
	sprintf(bufferMsj, "Presiona cualquier tecla para detener el muestreo. \n ");
	writeMsg(&UsartComm, bufferMsj);
	usartDataReceived = '\0';
}//FIN MENU


void Press_w(void){
	sprintf(bufferMsj, "WHO_AM_I? (r)\n");
	writeMsg(&UsartComm, bufferMsj);

	i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
	sprintf(bufferMsj, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
	writeMsg(&UsartComm, bufferMsj);
	usartDataReceived = '\0';

}//FIN PRESS W

void Press_p(void){
	sprintf(bufferMsj, "PWR_MGMT_1 state (r)\n");
	writeMsg(&UsartComm, bufferMsj);

	i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, POWER_CTL);
	sprintf(bufferMsj, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
	writeMsg(&UsartComm, bufferMsj);
	usartDataReceived = '\0';
}//FIN PRESS P

void Press_r(void){
	sprintf(bufferMsj, "PWR_MGMT_1 reset (w)\n");
	writeMsg(&UsartComm, bufferMsj);

	i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL , 0x2D);
	usartDataReceived = '\0';
}//FIN PRESS R

void Press_x(void){
	sprintf(bufferMsj, "Axis X data (r) \n");
	writeMsg(&UsartComm, bufferMsj);

	uint8_t AccelX_low =  i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
	uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
	int16_t AccelX = AccelX_high << 8 | AccelX_low;
	sprintf(bufferMsj, "AccelX = %.2f m/s² \n", (AccelX/210.f)*9.78);
	writeMsg(&UsartComm, bufferMsj);
	usartDataReceived = '\0';
}//FIN PRESS X

void Press_y(void){
	sprintf(bufferMsj, "Axis Y data (r)\n");
	writeMsg(&UsartComm, bufferMsj);

	uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
	uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer,ACCEL_YOUT_H);
	int16_t AccelY = AccelY_high << 8 | AccelY_low;
	sprintf(bufferMsj, "AccelY = %.2f m/s² \n", (AccelY/210.f)*9.78);
	writeMsg(&UsartComm, bufferMsj);
	usartDataReceived = '\0';
}//FIN PRESS Y

void Press_z(void){
	sprintf(bufferMsj, "Axis Z data (r)\n");
	writeMsg(&UsartComm, bufferMsj);

	uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
	uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
	sprintf(bufferMsj, "AccelZ = %.2f m/s² \n", (AccelZ/210.f)*9.78);
	writeMsg(&UsartComm, bufferMsj);
	usartDataReceived = '\0';
}//FIN PRESS Z

// al presionar la Z el acelerometro mandara 6000 datos (2mil por cada eje) en 2 segundos y los almacenará en un arreglo
// luego el arreglo se mostrará en la terminal. la toma de datosse realiza en 2 segundos pero se tarda mas en mosntar, esto debido
// a la velocidad del coolterm
void Press_d(void){
	banderaMuestreo = 1;
	numeroMuestreo = 0;
	while (numeroMuestreo < 256) {
		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
		int16_t AccelX = AccelX_high << 8 | AccelX_low;

		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
		int16_t AccelY = AccelY_high << 8 | AccelY_low;

		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
		int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;

		ArregloX[numeroMuestreo] = (AccelX / 210.f) * 9.78;
		ArregloY[numeroMuestreo] = (AccelY / 210.f) * 9.78;
		ArregloZ[numeroMuestreo] = (AccelZ / 210.f) * 9.78;
	}
	banderaMuestreo = 0;
	numeroMuestreo = 0;
	for (int i = 0; i < 256; i++) {
		sprintf(bufferMsj, "X = %.2f m/s² ;  Y = %.2f m/s² ;  Z = %.2f m/s²  | %u \n", ArregloX[i], ArregloY[i], ArregloZ[i], i);
		writeMsg(&UsartComm, bufferMsj);
	}
	usartDataReceived = '\0';
}//FIN PRESS D

// Se mostrara de forma continua los datos a 1KHz, los datos se ven en cascada y se presiona cualquier tecla para dejar de mostrarlos
void Press_l(void){
	banderaMuestreo = 1;
	while(numeroMuestreo < 2000){
		uint8_t AccelX_low =  i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
		int16_t AccelX = AccelX_high << 8 | AccelX_low;
		sprintf(bufferMsj, "AccelX = %.2f m/s² \n", (AccelX/210.f)*9.78);
		writeMsg(&UsartComm, bufferMsj);

		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer,ACCEL_YOUT_H);
		int16_t AccelY = AccelY_high << 8 | AccelY_low;
		sprintf(bufferMsj, "AccelY = %.2f m/s² \n", (AccelY/210.f)*9.78);
		writeMsg(&UsartComm, bufferMsj);


		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
		int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
		sprintf(bufferMsj, "AccelZ = %.2f m/s² \n", (AccelZ/210.f)*9.78);
		writeMsg(&UsartComm, bufferMsj);
		numeroMuestreo = 0; // para que nunca salga del while hasta que se precione otra tecla

		DuttyX = 830 * ((AccelX/210.f)*9.78) + 10000;
		updateDuttyCycle(&handerPWMX, DuttyX);
		DuttyY = 830 * ((AccelY/210.f)*9.78) + 10000;
		updateDuttyCycle(&handerPWMY, DuttyY);
		DuttyZ = 830 * ((AccelZ/210.f)*9.78) + 10000;
		updateDuttyCycle(&handerPWMZ, DuttyZ);

		if(banderaLCD == 1){
			sprintf(bufferx,"%.2f m/s2  ",(AccelX / 210.f) * 9.78);
			sprintf(buffery,"%.2f m/s2  ",(AccelY / 210.f) * 9.78);
			sprintf(bufferz,"%.2f m/s2  ",(AccelZ / 210.f) * 9.78);

			moverCursorLCD(&handlerLCD, 0, 8);
			sendMSJCD(&handlerLCD, bufferx);
			moverCursorLCD(&handlerLCD, 1, 8);
			sendMSJCD(&handlerLCD, buffery);
			moverCursorLCD(&handlerLCD, 2, 8);
			sendMSJCD(&handlerLCD, bufferz);

			banderaLCD =0;
		}


		if(usartDataReceived != 'l'){
			numeroMuestreo = 2000;
		}
	}
	numeroMuestreo = 0;
}//FIN PRESS L


// aca se ejecuta el Blinky
void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerLEDBlinky);
}
//Timmer a 1KHz para tomar datos del acelerometro
void BasicTimer4_Callback(void){
	if(banderaMuestreo == 1){
		numeroMuestreo++;
		infinito++;
	}
}
// timer de 1 segundo para la LCD
void BasicTimer5_Callback(void){

	banderaLCD = 1;
}

void callback_extInt3 (void){
 // aca se debe levantar una bandera
}

//recibir datos con el usart
void usart6Rx_Callback(void){
	usartDataReceived = getRxData();
}





