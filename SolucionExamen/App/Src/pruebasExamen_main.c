/**
 ******************************************************************************
 * @file           : main.c
 * @author         : maalvarezlo by STM32CubeIDE
 * @Nombre          : Mateo Alvarez Lopera
 * @brief          : examen main
 ******************************************************************************

 ******************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
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
#include "RTCDriver.h"



/* Definicion de los elementos del sistema */
/*Handler PLL para la velocidad del micro*/
PLL_Handler_t handlerPLL100MHz            = {0};
MCO1_Handler_t handlerMCO1                = {0};

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

// Comunicacion USART
RTC_Handler_t handlerRTC                 = {0};
uint16_t *dato                           = 0;
uint8_t horas                            = 0;
uint8_t minutos                          = 0;
uint8_t segundos                         = 0;
uint8_t dia                            = 0;
uint8_t mes                          = 0;
uint8_t año                         = 0;
USART_Handler_t UsartComm                = {0};
GPIO_Handler_t handlerPinTX              = {0};
GPIO_Handler_t handlerPinRX              = {0};
uint8_t usartDataReceived                = 0;
char bufferMsj[64]                       = {0};
char bufferMsj2[64]                      = {0};
char bufferReception[64]                 = {0};
char userMsg[64]                         = {0};
char cmd[64];
unsigned int firstParameter = 0;
unsigned int secondParameter = 0;
unsigned int thirdParameter = 0;
uint8_t counterReception                 = 0;
bool stringComplete                      = false;

//Configuracion para el I2C
GPIO_Handler_t handleI2cSDA             = {0};
GPIO_Handler_t handleI2cSCL             = {0};
I2C_Handler_t handlerAccelerometer      = {0};
uint8_t i2cBuffer                       = 0;
char bufferx[64]                        = {0};
char buffery[64]                        = {0};
char bufferz[64]                        = {0};
float32_t ArregloX[1024]                = {0};
float32_t ArregloY[1024]               = {0};
float32_t ArregloZ[1024]               = {0};
uint16_t numerodatos                   = 1024;


//FFT
//Elementos para generar una señal
#define ACEL_DATA_SIZE    1024   			//Tamaño del arrlego de datos
float32_t acelSignal[ACEL_DATA_SIZE];
float32_t transformedSignal[ACEL_DATA_SIZE];
float32_t FFTFinal[ACEL_DATA_SIZE];
float32_t* ptrSineSingal;
float32_t dt =	0.0;			//Periodo de muestreo, en este caso sera (1/fs)
float32_t maximo = 0;
uint16_t indiceFFT = 0;
float32_t FrecuenciaF = 0;

uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
arm_rfft_fast_instance_f32 config_Rfft_fast_f32;
arm_cfft_radix4_instance_f32 configRadix4_f32;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;
uint16_t fftSize = 1024;



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


//Definiendo las Funciones
void init_hardware (void);
void Loop_Principal(void);
void parseCommands(char *ptrBufferReception);
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

	while(1){

		// cuando el usuario presione una de las siguientes teclas se activara su funcion respectiva
		if(usartDataReceived != '\0'){

			bufferReception[counterReception] = usartDataReceived;
			counterReception++;
			if(usartDataReceived == '@'){
				stringComplete = true;
				bufferReception[counterReception] ='\0';
				counterReception = 0;
			}
			usartDataReceived = '\0';
		}
		//analizamos la cadena de datos obtenida
		if(stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;
		}

	} // Fin while
	return(0);
} //Fin funcion main


/*aca se inicializan todos los elemntos*/
void init_hardware (void){

/*Configuramos la velocidad del micro*/
	handlerPLL100MHz.FrecuenciaClock  = PLL_CLOCK_100MHz;
	configPLL(&handlerPLL100MHz);
	config_SysTick_ms(SYSTICK_LOAD_VALUE_100MHz_1ms);


/*Configuracion del LED del blinky en el puesto PH1 */
	handlerLEDBlinky.pGPIOx = GPIOH;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinNumber           = PIN_1;
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
	//Si el timer esta a 100MHz deben usarse la Speed que termina en _PLL100_100us
	handlerBlinkyTimer.ptrTIMx                              = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP ;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed               = BTIMER_SPEED_PLL100_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period              = 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	//Si el timer esta a 100MHz deben usarse la Speed que termina en _PLL100_100us
	handler1HzTimer.ptrTIMx                              = TIM4;
	handler1HzTimer.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP ;
	handler1HzTimer.TIMx_Config.TIMx_speed               = BTIMER_SPEED_PLL100_100us;
	handler1HzTimer.TIMx_Config.TIMx_period              = 50;
	handler1HzTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handler1HzTimer);


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
	//cuando el micro este a 100MHz deben usarse los baudrate terminados en _100MHz
	UsartComm.ptrUSARTx                                     = USART6;
	UsartComm.USART_Config.USART_baudrate                   = USART_BAUDRATE_115200_100MHz;
	UsartComm.USART_Config.USART_datasize                   = USART_DATASIZE_8BIT;
	UsartComm.USART_Config.USART_mode                       = USART_MODE_RXTX;
	UsartComm.USART_Config.USART_parity                     = USART_PARITY_NONE;
	UsartComm.USART_Config.USART_stopbits                   = USART_STOPBIT_1;
	UsartComm.USART_Config.USART_enableIntTX                = USART_TX_INTERRUPT_DISABLE;
	UsartComm.USART_Config.USART_enableIntRX                = USART_RX_INTERRUPT_ENABLE;
	USART_Config(&UsartComm);

	/*Activando el MCO1 para probar la frecuencia del relol en HSI, LSE y el PLL en el PIN PA8*/
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
	//Cambiando el muestreo del acelerometro
	i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL, 0x08);
	i2c_writeSingleRegister(&handlerAccelerometer, BW_RATE, 0xF);

	//CONFIGURACION ADC


} // Termina el int_Hardware

//COMANDOS
void parseCommands(char *ptrBufferReception){

	sscanf(ptrBufferReception, "%s %u %u %u %s", cmd, &firstParameter, &secondParameter, &thirdParameter, userMsg);

	//primer comando para mostrar el menu
	if(strcmp(cmd, "help") == 0){
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
		sprintf(bufferMsj, "Presiona cualquier tecla para detener el muestreo. \n");
		writeMsg(&UsartComm, bufferMsj);
		usartDataReceived = '\0';
	}
	else if(strcmp(cmd, "relojMCO1") == 0){
		if(firstParameter == 0){
			handlerMCO1.Reloj = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsj, "El MCO1 se configuró en el HSI \n");
			writeMsg(&UsartComm, bufferMsj);
		} else if(firstParameter == 1){
			handlerMCO1.Reloj = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsj, "El MCO1 se configuró en el LSE \n");
			writeMsg(&UsartComm, bufferMsj);
		} else if(firstParameter == 2){
			handlerMCO1.Reloj = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsj, "El MCO1 se configuró en el PLL \n");
			writeMsg(&UsartComm, bufferMsj);
		}

	}
	else if(strcmp(cmd, "preescalerMCO1") == 0){
		if(firstParameter < 6 && firstParameter > 0){
			handlerMCO1.Preescaler = firstParameter;
			configMCO1(&handlerMCO1);
			sprintf(bufferMsj, "El preescaler del MCO1 se configuró a %u \n",firstParameter);
			writeMsg(&UsartComm, bufferMsj);
		} else{
			writeMsg(&UsartComm, "El preescaler seleccionado no es valido,debe ser un valor entre 1 y 5 \n");
		}
	}
	//RTC
	else if(strcmp(cmd, "actualizarhora") == 0){
		if(firstParameter > 23 || secondParameter > 59 || thirdParameter > 59 ){
			writeMsg(&UsartComm, "Valores erroneos, por favor escriba la hora verdadera \n \n");
		} else{
			handlerRTC.RTC_Config.Horas = firstParameter;
			handlerRTC.RTC_Config.Minutos= secondParameter;
			handlerRTC.RTC_Config.Segundos = thirdParameter;
			configRTC(&handlerRTC);
			sprintf(bufferMsj, "Se actualizó la hora \nhora:%u  minuto:%u  segundo:%u \n" , firstParameter, secondParameter, thirdParameter);
			writeMsg(&UsartComm, bufferMsj);
			sprintf(bufferMsj, "la hora es %u:%u:%u \n \n" , firstParameter, secondParameter, thirdParameter);
			writeMsg(&UsartComm, bufferMsj);
		}
	}
	else if(strcmp(cmd, "horaactual") == 0){
		dato = cargarRTC();
		horas = dato[3];
		minutos = dato[4];
		segundos = dato[5];
		sprintf(bufferMsj, "La hora actual es %u:%u:%u \n \n", horas, minutos, segundos );
		writeMsg(&UsartComm, bufferMsj);
	}
	else if(strcmp(cmd, "actualizarfecha") == 0){
		if(firstParameter > 30 || secondParameter > 12 || thirdParameter > 99 ){
			writeMsg(&UsartComm, "Valores erroneos, por favor escriba la fecha verdadera \n \n");
		} else{
			handlerRTC.RTC_Config.NumeroDia = firstParameter;
			handlerRTC.RTC_Config.Mes= secondParameter;
			handlerRTC.RTC_Config.Año = thirdParameter;
			configRTC(&handlerRTC);
			sprintf(bufferMsj, "Se actualizó la fecha \ndia:%u  mes:%u  año:%u \n" , firstParameter, secondParameter, thirdParameter+2000);
			writeMsg(&UsartComm, bufferMsj);
			sprintf(bufferMsj, "la fecha es %u/%u/%u \n \n" , firstParameter, secondParameter, thirdParameter+2000);
			writeMsg(&UsartComm, bufferMsj);
		}
	}
	else if(strcmp(cmd, "fechaactual") == 0){
		dato = cargarRTC();
		dia = dato[0];
		mes = dato[1];
		año = dato[2];
		sprintf(bufferMsj, "La fecha actual es %u/%u/%u \n \n", dia, mes, año+2000);
		writeMsg(&UsartComm, bufferMsj);
	}
	else if(strcmp(cmd, "fechaactual") == 0){

	}

	//ACELEROMETRO
	else if(strcmp(cmd, "resetacel") == 0){
		Press_r();
		Press_x();
		Press_y();
		Press_z();
	}
	else if(strcmp(cmd, "initFFT") == 0){
		statusInitFFT = arm_rfft_fast_init_f32(&config_Rfft_fast_f32, fftSize);

		if(statusInitFFT == ARM_MATH_SUCCESS){
			sprintf(bufferMsj, "Fourier initialization ... SUCCESS! \n");
			writeMsg(&UsartComm, bufferMsj);
		}
	}

	// FFT
	else if (strcmp(cmd, "FFT") == 0){
		sprintf(bufferMsj, "Tomando los datos con el acelerometro, espere 5 segundos \n");
		writeMsg(&UsartComm, bufferMsj);
		Press_d();

		int i = 0;
		int j = 0;
		sprintf(bufferMsj, "Iniciando FFT \n");
		writeMsg(&UsartComm, bufferMsj);
		if(statusInitFFT == ARM_MATH_SUCCESS){
			arm_rfft_fast_f32(&config_Rfft_fast_f32, ArregloZ, transformedSignal, ifftFlag);
			arm_abs_f32(transformedSignal, acelSignal, fftSize);

			for( i = 0; i < fftSize; i++){
				if(i % 2){
					FFTFinal[i] = acelSignal[i];
				}
			}
		}
		maximo = FFTFinal[0];
		for(j = 0; j < fftSize; j++ ){
			if(maximo < FFTFinal[j]){
				maximo = FFTFinal[j];
				indiceFFT = j;
			}
		}
		sprintf(bufferMsj, "el subindice de la frecuencia fundamental es %u \n", indiceFFT);
		writeMsg(&UsartComm, bufferMsj);
		FrecuenciaF = (indiceFFT*200/(fftSize));
		sprintf(bufferMsj, "La frecuencia es %#.4f Hz \n", FrecuenciaF);
		writeMsg(&UsartComm, bufferMsj);
	}


	else{
		writeMsg(&UsartComm, "COMANDO INVÁLIDO \n");
	}

}

void Press_r(void){
	sprintf(bufferMsj, "PWR_MGMT_1 reset\n");
	writeMsg(&UsartComm, bufferMsj);
	i2c_writeSingleRegister(&handlerAccelerometer, POWER_CTL , 0x2D);
}//FIN PRESS R

void Press_x(void){
	uint8_t AccelX_low =  i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
	uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
	int16_t AccelX = AccelX_high << 8 | AccelX_low;
	sprintf(bufferMsj, "AccelX = %.2f m/s² \n", (AccelX/210.f)*9.78);
	writeMsg(&UsartComm, bufferMsj);
}//FIN PRESS X

void Press_y(void){
	uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
	uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer,ACCEL_YOUT_H);
	int16_t AccelY = AccelY_high << 8 | AccelY_low;
	sprintf(bufferMsj, "AccelY = %.2f m/s² \n", (AccelY/210.f)*9.78);
	writeMsg(&UsartComm, bufferMsj);
}//FIN PRESS Y

void Press_z(void){
	uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
	uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
	sprintf(bufferMsj, "AccelZ = %.2f m/s² \n", (AccelZ/210.f)*9.78);
	writeMsg(&UsartComm, bufferMsj);
}//FIN PRESS Z



void Press_d(void){
	banderaMuestreo = 1;
	numeroMuestreo = 0;
	while (numeroMuestreo < numerodatos) {

		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
		int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;

		ArregloZ[numeroMuestreo] = (AccelZ / 210.f) * 9.78;
	}
	banderaMuestreo = 0;
	numeroMuestreo = 0;
	for (int i = 0; i < numerodatos; i++) {
		sprintf(bufferMsj, " Z = %.2f m/s²  | %u \n", ArregloZ[i], i);
		writeMsg(&UsartComm, bufferMsj);
	}
}//FIN PRESS D


// aca se ejecuta el Blinky
void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerLEDBlinky);
}
//Timmer a 200hz para tomar datos del acelerometro
void BasicTimer4_Callback(void){
	if(banderaMuestreo == 1){
		numeroMuestreo++;
	}
}

void callback_extInt3 (void){
 // aca se debe levantar una bandera
}

//recibir datos con el usart
void usart6Rx_Callback(void){
	usartDataReceived = getRxData();
}





