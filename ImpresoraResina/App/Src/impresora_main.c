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
#include "AdcDriver.h"
#include "LCDst7789Driver.h"



/* Definicion de los elementos del sistema */
/*Handler PLL para la velocidad del micro*/
PLL_Handler_t handlerPLL16MHz            = {0};

// Handlers de los pines
GPIO_Handler_t handlerLEDBlinky          = {0};
GPIO_Handler_t handlerMotorOn              = {0};
GPIO_Handler_t handlerSTEEP              = {0};
GPIO_Handler_t handlerDIR                = {0};
GPIO_Handler_t handlerUV                = {0};
GPIO_Handler_t handlerFC                = {0};

// Handlers y banderas de los timers
BasicTimer_Handler_t handlerBlinkyTimer  = {0};
BasicTimer_Handler_t handlerMotor  = {0};

//EXTIS
EXTI_Config_t Exti_FinalCarrera            = {0};


// Comunicacion USART
RTC_Handler_t handlerRTC                 = {0};
uint16_t *dato                           = 0;
uint8_t horas                            = 0;
uint8_t minutos                          = 0;
uint8_t segundos                         = 0;
uint8_t dia                              = 0;
uint8_t mes                              = 0;
uint8_t año                              = 0;
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

//Variables
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
uint32_t contador  = 0;
uint8_t finalcarrera  = 0;




//Definiendo las Funciones
void init_hardware (void);
void parseCommands(char *ptrBufferReception);
void inicializacion(void);
void SecuenciaMotor (uint16_t);
void LCD_Fill_Image(void);


int main(void){
	//Activamos el coprocesador Matematico
	SCB->CPACR |= (0xF << 20);
	/*Inicialización de todos los elementos del sistema*/
	init_hardware();
	//	inicializacion();
	LCD_configPin();
	LCD_Init();
	LCD_Fill_Color(WHITE);
	GPIO_WritePin(&handlerUV, SET);
	delay_ms(4000);
	GPIO_WritePin(&handlerUV, RESET);
	LCD_Fill_Color(BLACK);
	delay_ms(2000);

	/* definiendo la altura de capa en pasos*/
	/* Teniendo en cuenta que cada paso del motor mueve la cama una altura de 0.4mm, es necesario que la altura de capa deseada
	 * sea multiplo de 0.04mm*/
	AlturaCapamm = 0.12;
	/* Para calcular los pasos de está altura de capa (teniendo en cuenta el motor y la varilla roscada*/
	AlturaCapaPasos = (AlturaCapamm*200)/8;

	// creamos un while para que el motor baje hasta que choque con el final de carrera, este será nuestra 0
	while(finalcarrera == 0){

		GPIO_WritePin(&handlerMotorOn, SET);
		GPIO_WritePin(&handlerDIR, SET);
		if(banderaMotor == 1){
			GPIOxTooglePin(&handlerSTEEP);
			banderaMotor = 0;
		}
	}
	//Apagams los pines para que empiece la secuencia de cada capa y esperamos 3s para iniciar
	GPIO_WritePin(&handlerMotorOn, RESET);
	GPIO_WritePin(&handlerDIR, RESET);
	delay_ms(3000);

	while(1){
		// MOTOR y LCD
		DIR = GPIO_ReadPin(&handlerDIR);
		if (banderaMotor == 1) {
			SecuenciaMotor(AlturaCapaPasos);
			banderaMotor = 0;
		}
	} // Fin while
} //Fin funcion main


/*aca se inicializan todos los elemntos*/
void init_hardware (void){

/*Configuramos la velocidad del micro*/
	handlerPLL16MHz.FrecuenciaClock  = PLL_CLOCK_16MHz;
	configPLL(&handlerPLL16MHz);
	config_SysTick_ms(SYSTICK_LOAD_VALUE_16MHz_1ms);

	/****************************************************PINES*************************************************************************/
/*Configuracion del LED del blinky en el puesto PH1 */
	handlerLEDBlinky.pGPIOx                                  = GPIOH;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinNumber           = PIN_1;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerLEDBlinky.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLEDBlinky);
	GPIO_WritePin(&handlerLEDBlinky, SET);

	/*Configuracion del PIN que controla el final de carrera en el puesto PC5 */
	handlerFC.pGPIOx                                  = GPIOC;
	handlerFC.GPIO_PinConfig.GPIO_PinNumber           = PIN_5;
	handlerFC.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_IN;
	handlerFC.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerFC.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerFC.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerFC);

	/*Configuracion del PIN que controla los pasos del motor en el puesto PC13 */
	handlerSTEEP.pGPIOx                                  = GPIOC;
	handlerSTEEP.GPIO_PinConfig.GPIO_PinNumber           = PIN_13;
	handlerSTEEP.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerSTEEP.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerSTEEP.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerSTEEP.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerSTEEP);

	/*Configuracion del PIN que controla el encendido del motor en el puesto PC1 */
	handlerMotorOn.pGPIOx                                  = GPIOC;
	handlerMotorOn.GPIO_PinConfig.GPIO_PinNumber           = PIN_1;
	handlerMotorOn.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerMotorOn.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerMotorOn.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerMotorOn.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerMotorOn);

	/*Configuracion del PIN que controla la direccion del motor en el puesto PC2 */
	handlerDIR.pGPIOx                                  = GPIOC;
	handlerDIR.GPIO_PinConfig.GPIO_PinNumber           = PIN_2;
	handlerDIR.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerDIR.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerDIR.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerDIR.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDIR);
	GPIO_WritePin(&handlerDIR, RESET);

		/*Configuracion del PIN que controla la iluminacion UV en el puesto PB7 */
	handlerUV.pGPIOx                                  = GPIOB;
	handlerUV.GPIO_PinConfig.GPIO_PinNumber           = PIN_7;
	handlerUV.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerUV.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerUV.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_FAST;
	handlerUV.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUV);

/*****************************************************TIMERS**********************************************************/
	/* Configuracion del TIM2 para que haga un blinky cada 250ms*/
	//Si el timer esta a 100MHz deben usarse la Speed que termina en _PLL100_100us
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

	/************************************************** EXTI******************************************************/
	// Configurando la interrucion del encoder al girar
	Exti_FinalCarrera.edgeType                                = EXTERNAL_INTERRUPT_RISING_EDGE;
	Exti_FinalCarrera.pGPIOHandler                            = &handlerFC;
	extInt_Config(&Exti_FinalCarrera);

	/**************************************************USART******************************************************/
	//PINTX del usart para Usart6(PA11 AF8) Para Usart2(PA2 AF7)
	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber           = PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF7;
	GPIO_Config(&handlerPinTX);
	//PINRX del usart para Usart6(PA12 AF8) Para Usart2(PA3 AF7)
	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber           = PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode       = AF7;
	GPIO_Config(&handlerPinRX);
	//USART a 16MHz
	UsartComm.ptrUSARTx                                     = USART2;
	UsartComm.USART_Config.USART_baudrate                   = USART_BAUDRATE_230400;
	UsartComm.USART_Config.USART_datasize                   = USART_DATASIZE_8BIT;
	UsartComm.USART_Config.USART_mode                       = USART_MODE_RXTX;
	UsartComm.USART_Config.USART_parity                     = USART_PARITY_NONE;
	UsartComm.USART_Config.USART_stopbits                   = USART_STOPBIT_1;
	UsartComm.USART_Config.USART_enableIntTX                = USART_TX_INTERRUPT_DISABLE;
	UsartComm.USART_Config.USART_enableIntRX                = USART_RX_INTERRUPT_ENABLE;
	USART_Config(&UsartComm);

} // Termina el int_Hardware

//COMANDOS
void parseCommands(char *ptrBufferReception){

	sscanf(ptrBufferReception, "%s %u %u %u %s", cmd, &firstParameter, &secondParameter, &thirdParameter, userMsg);

	//primer comando para mostrar el menu
	if(strcmp(cmd, "help") == 0){
		sprintf(bufferMsj, "Hola, soy el MCU de Mateo y tengo una frecuencia de %d Hz \n ", getConfigPLL());
		writeMsg(&UsartComm, bufferMsj);
		writeMsg(&UsartComm, "- Escribe el comando help para mostrar este menu cuando desees.\n");
		writeMsg(&UsartComm, "- Escribe el comando \"relojMCO1 #\" para seleccionar el reloj del pin PA8. \n");
		writeMsg(&UsartComm, "HSI = 0 ; LSE = 1 ; PLL = 2 \n ");
		writeMsg(&UsartComm, "- Escribe el comando \"preescalerMCO1 #\" para cambiar el preescaler del micro \n");
		writeMsg(&UsartComm, "El preescaler debe ser del 1 al 5 \n");
		writeMsg(&UsartComm, "- Escribe el comando \"actualizarhora h# m# s#\" para actualizar la hora (el formato es 24H) \n");
		writeMsg(&UsartComm, "- Escribe el comando \"horaactual\" para ver la hora actual \n");
		writeMsg(&UsartComm, "- Escribe el comando \"actualizafecha d# m# a#\" para actualizar la fecha (el formato es dm/mm/aa) \n");
		writeMsg(&UsartComm, "- Escribe el comando \"fechaactual\" para ver la fecha actual \n");
		writeMsg(&UsartComm, "- Escribe el comando \"actualizarsamp #\" actualizar la frecuencia de sampleo del PWM del ADC (800-1500) \n");
		writeMsg(&UsartComm, "- Escribe el comando \"adc\" para obtener ambos los valores de las señales obtenidas por el ADC\n");
		writeMsg(&UsartComm, "- Escribe el comando \"resetacel\" para resetear el acelerometro \n");
		writeMsg(&UsartComm, "- Escribe el comando \"initFFT\" para inicializar la funcion FFT \n");
		writeMsg(&UsartComm, "- Escribe el comando \"FFT\" para tomar  todos los datos del acelerometro, y mostrar la frecuencia fundamental \n");
	}

	//RTC, comando para actualizar la hora
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
	//RTC, comando para ver la hora actual
	else if(strcmp(cmd, "horaactual") == 0){
		dato = cargarRTC();
		horas = dato[3];
		minutos = dato[4];
		segundos = dato[5];
		sprintf(bufferMsj, "La hora actual es %u:%u:%u \n \n", horas, minutos, segundos );
		writeMsg(&UsartComm, bufferMsj);
	}

	else{
		writeMsg(&UsartComm, "COMANDO INVÁLIDO \n");
	}

}

void inicializacion(void){
	sprintf(bufferMsj,
	"Hola, soy el MCU de Mateo y tengo una frecuencia de %d Hz \n ",
	getConfigPLL());
	writeMsg(&UsartComm, bufferMsj);
	writeMsg(&UsartComm,
			"- Escribe el comando help para mostrar este menu cuando desees.\n");
	writeMsg(&UsartComm,
			"- Escribe el comando \"relojMCO1 #\" para seleccionar el reloj del pin PA8. \n");
	writeMsg(&UsartComm, "HSI = 0 ; LSE = 1 ; PLL = 2 \n ");
	writeMsg(&UsartComm,
			"- Escribe el comando \"preescalerMCO1 #\" para cambiar el preescaler del micro \n");
	writeMsg(&UsartComm, "El preescaler debe ser del 1 al 5 \n");
	writeMsg(&UsartComm,
			"- Escribe el comando \"actualizarhora h# m# s#\" para actualizar la hora (el formato es 24H) \n");
	writeMsg(&UsartComm,
			"- Escribe el comando \"horaactual\" para ver la hora actual \n");
	writeMsg(&UsartComm,
			"- Escribe el comando \"actualizafecha d# m# a#\" para actualizar la fecha (el formato es dm/mm/aa) \n");
	writeMsg(&UsartComm,
			"- Escribe el comando \"fechaactual\" para ver la fecha actual \n");
	writeMsg(&UsartComm,
			"- Escribe el comando \"actualizarsamp #\" actualizar la frecuencia de sampleo del PWM del ADC (800-1500) \n");
	writeMsg(&UsartComm,
			"- Escribe el comando \"adc\" para obtener ambos los valores de las señales obtenidas por el ADC\n");
	writeMsg(&UsartComm,
			"- Escribe el comando \"resetacel\" para resetear el acelerometro \n");
	writeMsg(&UsartComm,
			"- Escribe el comando \"initFFT\" para inicializar la funcion FFT \n");
	writeMsg(&UsartComm,
			"- Escribe el comando \"FFT\" para tomar  todos los datos del acelerometro, y mostrar la frecuencia fundamental \n");
}


// El motor subirá 200 pasos (8mm) y bajará el numero de pasos determiando para que quede la diferencia de la altura de capa
void SecuenciaMotor(uint16_t AlturadeCapa){
	if(GPIO_ReadPin(&handlerDIR) == RESET){
		//Encender motor
		GPIO_WritePin(&handlerMotorOn, SET);
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
			//Apagar motor
			GPIO_WritePin(&handlerMotorOn, RESET);
			/*aca se debe encender la LCD*/
			LCD_Fill_Image();
			GPIO_WritePin(&handlerUV, SET);
			delay_ms(10000);
			GPIO_WritePin(&handlerUV, RESET);
			/*aca se debe apagar la LCD*/
			LCD_Fill_Color(BLACK);
			delay_ms(500);
		}
	}
	PasosTotales++;
	alturaActualmm = (alturaActualPasos*8)/200;
}

//DIbujar imagen que llega por usart
void LCD_Fill_Image(void){
	writeMsg(&UsartComm, "1");
	uint16_t i;
	LCD_SetWindow(0, 0, LCD_WIDTH, LCD_HEIGHT);

	uint16_t j;
	for (i = 0; i < LCD_WIDTH; i++){
		for (j = 0; j < LCD_HEIGHT; j++) {
			while (usartDataReceived == '\0') {
				__NOP();
			}
			if (usartDataReceived == '\001') {
				uint16_t color = BLACK;
				uint8_t data[] = { color >> 8, color & 0xFF };
				LCD_WriteData_Word(data, sizeof(data));
				usartDataReceived = '\0';
			} else if (usartDataReceived == '\002') {
				uint16_t color = WHITE;
				uint8_t data[] = { color >> 8, color & 0xFF };
				LCD_WriteData_Word(data, sizeof(data));
				usartDataReceived = '\0';
			}
		}
	}
}

// aca se ejecuta el Blinky
void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerLEDBlinky);
}

void BasicTimer3_Callback(void){
	banderaMotor = 1;
}


//recibir datos con el usart
void usart2Rx_Callback(void){
	usartDataReceived = getRxData();
	contador++;
}

//Interurpcion
void callback_extInt5 (void){
	finalcarrera = 1; // se levanta una bandera para que se ejecute el codigo de los contadores
}






