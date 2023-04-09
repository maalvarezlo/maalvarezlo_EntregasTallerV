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
// Handlers de los pines
GPIO_Handler_t handlerLEDBlinky = {0};
GPIO_Handler_t handlerTranUnidades = {0};
GPIO_Handler_t handlerTranDecenas = {0};
GPIO_Handler_t handlerLEDa = {0};
GPIO_Handler_t handlerLEDb = {0};
GPIO_Handler_t handlerLEDc = {0};
GPIO_Handler_t handlerLEDd = {0};
GPIO_Handler_t handlerLEDe = {0};
GPIO_Handler_t handlerLEDf = {0};
GPIO_Handler_t handlerLEDg = {0};
GPIO_Handler_t handlerEncoderCLK = {0};
GPIO_Handler_t handlerEncoderDT = {0};
GPIO_Handler_t handlerEncoderSW = {0};

// Handlers de los timers
BasicTimer_Handler_t handlerBlinkyTimer = {0};
BasicTimer_Handler_t handlerTimerDisplay = {0};

// Extis
EXTI_Config_t Exti_encoderCLK = {0};
EXTI_Config_t Exti_encoderDT = {0};
EXTI_Config_t Exti_encoderSW = {0};

// Variables
uint8_t counterExti1 = 0;
int8_t contador = 0;
uint8_t contador2 = 0;
uint8_t banderaSw = 0;
uint8_t banderaClock = 0;
uint8_t unidades = 0;
uint8_t decenas = 0;
uint8_t i = 0;

// Se definen 2 arreglos para facilitar la secuencia para encender el led de la culebrita segun el contador
uint8_t culebritaDecenas[] = {1, 2, 3, 4, 7, 8};
uint8_t culebritaUnidades[] = {0, 5, 6, 9, 10, 11};


/* Funciones */
void init_hardware (void);
void Encender_Display(uint8_t numero);
void Encender_Culebra(uint8_t numero2);

int main(void){

/*Inicialización de todos los elementos del sistema*/
	init_hardware();

	while(1){

		/* El programa empezará en el modo contador, la banderaSW cambiara de 0 a 1 y viceversa cada vez que
		se presiona el boton del encoder (esto se configura en la funcion callback_extInt5) */
		if(banderaSw == 0){
			if(GPIO_ReadPin(&handlerEncoderDT) == 1 && banderaClock){
				if(contador2 < 99){
					contador2++;
				}
				banderaClock = 0;
			}
			else if(GPIO_ReadPin(&handlerEncoderDT) == 0 && banderaClock ){
				if(contador2 > 0){
					contador2--;
				}
				banderaClock = 0;
			}
			unidades = contador2 % 10;
			decenas = contador2/10;
		}
		// Aca se cambiara al modo culebrita cuando se presiona el boton del encoder
		else{
			if(GPIO_ReadPin(&handlerEncoderDT) == 1 && banderaClock){
				contador++;
				if(contador > 11){
					contador = 0;
				}
				banderaClock = 0;
			}
			else if(GPIO_ReadPin(&handlerEncoderDT) == 0 && banderaClock ){
				contador--;
				if (contador < 0){
					contador = 11;
				}
				banderaClock = 0;
			}
		}



	} // Fin while
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
	GPIO_Config(&handlerLEDBlinky);
	GPIO_WritePin(&handlerLEDBlinky, SET);

/*Configuracion pin para el transistor de las unidades PC2 */
	handlerTranUnidades.pGPIOx = GPIOC;
	handlerTranUnidades.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerTranUnidades.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerTranUnidades.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerTranUnidades.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerTranUnidades.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerTranUnidades);
	GPIO_WritePin(&handlerTranUnidades, SET);

/*Configuracion pin para el transistor de las decenas PC3 */
	handlerTranDecenas.pGPIOx = GPIOC;
	handlerTranDecenas.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerTranDecenas.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerTranDecenas.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerTranDecenas.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerTranDecenas.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerTranDecenas);

/* Configurando todos los pines del 7 segmentos	*/
	// LEDa en el PB7
	handlerLEDa.pGPIOx = GPIOB;
	handlerLEDa.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
	handlerLEDa.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLEDa.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDa.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDa.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	// LEDb en el PC13
	handlerLEDb.pGPIOx = GPIOC;
	handlerLEDb.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerLEDb.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLEDb.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDb.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDb.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	// LEDc en el PC7
	handlerLEDc.pGPIOx = GPIOC;
	handlerLEDc.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
	handlerLEDc.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLEDc.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDc.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDc.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	// LEDd en el PA11
	handlerLEDd.pGPIOx = GPIOA;
	handlerLEDd.GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	handlerLEDd.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLEDd.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDd.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDd.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	// LEDe en el PB12
	handlerLEDe.pGPIOx = GPIOB;
	handlerLEDe.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	handlerLEDe.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLEDe.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDe.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDe.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	// LEDf en el PA5
	handlerLEDf.pGPIOx = GPIOA;
	handlerLEDf.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerLEDf.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLEDf.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDf.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDf.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	// LEDg en el PA6
	handlerLEDg.pGPIOx = GPIOA;
	handlerLEDg.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
	handlerLEDg.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLEDg.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLEDg.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerLEDg.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

/* Cargar la configuracion de todos los led del display*/
	GPIO_Config(&handlerLEDa);
	GPIO_Config(&handlerLEDb);
	GPIO_Config(&handlerLEDc);
	GPIO_Config(&handlerLEDd);
	GPIO_Config(&handlerLEDe);
	GPIO_Config(&handlerLEDf);
	GPIO_Config(&handlerLEDg);
	// encendiendo todos los led para revisar que esten funcionando todos (se encienden con SET ya que el
	// Display es catodo comun)
	GPIO_WritePin(&handlerLEDa, SET);
	GPIO_WritePin(&handlerLEDb, SET);
	GPIO_WritePin(&handlerLEDc, SET);
	GPIO_WritePin(&handlerLEDd, SET);
	GPIO_WritePin(&handlerLEDe, SET);
	GPIO_WritePin(&handlerLEDf, SET);
	GPIO_WritePin(&handlerLEDg, SET);


/* Configuracion del TIM2 para que haga un blinky cada 250ms*/
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP ;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);


/* Configuracion del TIM3 para encender y apagar los transistores del display*/
	handlerTimerDisplay.ptrTIMx = TIM3;
	handlerTimerDisplay.TIMx_Config.TIMx_mode = BTIMER_MODE_UP ;
	handlerTimerDisplay.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerTimerDisplay.TIMx_Config.TIMx_period = 10;  // Lanza una interrupción cada 300ms
	handlerTimerDisplay.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerTimerDisplay);

	// configurando el pin del clock
	handlerEncoderCLK.pGPIOx = GPIOB;
	handlerEncoderCLK.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerEncoderCLK.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerEncoderCLK.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerEncoderCLK.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerEncoderCLK.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerEncoderCLK.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
	GPIO_Config(&handlerEncoderCLK);

	// configurando el pin del Data
	handlerEncoderDT.pGPIOx = GPIOB;
	handlerEncoderDT.GPIO_PinConfig.GPIO_PinNumber			= PIN_4;
	handlerEncoderDT.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerEncoderDT.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerEncoderDT.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerEncoderDT.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerEncoderDT.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
	GPIO_Config(&handlerEncoderDT);

	//configurando el pin para el SW (cuando se presiona el boton del encoder)
	handlerEncoderSW.pGPIOx = GPIOB;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerEncoderSW.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
	GPIO_Config(&handlerEncoderSW);


	// Configurando la interrucion del encoder al girar
	Exti_encoderCLK.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	Exti_encoderCLK.pGPIOHandler = &handlerEncoderCLK;
	extInt_Config(&Exti_encoderCLK);


	// Configurando la interrucion del encoder al presionar el boton
	Exti_encoderSW.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	Exti_encoderSW.pGPIOHandler = &handlerEncoderSW;
	extInt_Config(&Exti_encoderSW);

} // Termina el int_Hardware


void Encender_Display(uint8_t numero){
	// apagando todos los led
	GPIO_WritePin(&handlerLEDa, RESET);
	GPIO_WritePin(&handlerLEDb, RESET);
	GPIO_WritePin(&handlerLEDc, RESET);
	GPIO_WritePin(&handlerLEDd, RESET);
	GPIO_WritePin(&handlerLEDe, RESET);
	GPIO_WritePin(&handlerLEDf, RESET);
	GPIO_WritePin(&handlerLEDg, RESET);

	if(numero == 1){ //debe encender los led b y c
		GPIO_WritePin(&handlerLEDb, SET);
		GPIO_WritePin(&handlerLEDc, SET);
	}
	else if(numero == 2){ // deben encender los led a,b,g,e y d
		GPIO_WritePin(&handlerLEDa, SET);
		GPIO_WritePin(&handlerLEDb, SET);
		GPIO_WritePin(&handlerLEDg, SET);
		GPIO_WritePin(&handlerLEDe, SET);
		GPIO_WritePin(&handlerLEDd, SET);
	}
	else if(numero == 3){ // deben encender los led a,b,g,c y d
		GPIO_WritePin(&handlerLEDa, SET);
		GPIO_WritePin(&handlerLEDb, SET);
		GPIO_WritePin(&handlerLEDg, SET);
		GPIO_WritePin(&handlerLEDc, SET);
		GPIO_WritePin(&handlerLEDd, SET);
	}
	else if(numero == 4){ // deben encender los led f,g,b y c
		GPIO_WritePin(&handlerLEDf, SET);
		GPIO_WritePin(&handlerLEDg, SET);
		GPIO_WritePin(&handlerLEDb, SET);
		GPIO_WritePin(&handlerLEDc, SET);
	}
	else if(numero == 5){ // deben encender los led a,f,g,c y d
		GPIO_WritePin(&handlerLEDa, SET);
		GPIO_WritePin(&handlerLEDf, SET);
		GPIO_WritePin(&handlerLEDg, SET);
		GPIO_WritePin(&handlerLEDc, SET);
		GPIO_WritePin(&handlerLEDd, SET);
	}
	else if(numero == 6){ // deben encender los led a,f,g,c, d y e
		GPIO_WritePin(&handlerLEDa, SET);
		GPIO_WritePin(&handlerLEDf, SET);
		GPIO_WritePin(&handlerLEDg, SET);
		GPIO_WritePin(&handlerLEDc, SET);
		GPIO_WritePin(&handlerLEDd, SET);
		GPIO_WritePin(&handlerLEDe, SET);
	}
	else if(numero == 7){ // deben encender los led a,b y c
		GPIO_WritePin(&handlerLEDa, SET);
		GPIO_WritePin(&handlerLEDb, SET);
		GPIO_WritePin(&handlerLEDc, SET);
	}
	else if(numero == 8){ // deben encender los led a,b,g,f, e, d y c
		GPIO_WritePin(&handlerLEDa, SET);
		GPIO_WritePin(&handlerLEDb, SET);
		GPIO_WritePin(&handlerLEDc, SET);
		GPIO_WritePin(&handlerLEDd, SET);
		GPIO_WritePin(&handlerLEDe, SET);
		GPIO_WritePin(&handlerLEDf, SET);
		GPIO_WritePin(&handlerLEDg, SET);
	}
	else if(numero == 9){ // deben encender los led a,b,g,f, d y c
		GPIO_WritePin(&handlerLEDa, SET);
		GPIO_WritePin(&handlerLEDb, SET);
		GPIO_WritePin(&handlerLEDc, SET);
		GPIO_WritePin(&handlerLEDf, SET);
		GPIO_WritePin(&handlerLEDg, SET);
		GPIO_WritePin(&handlerLEDd, SET);
	}
	else if(numero == 0){ // deben encender los led a,b,f, e, d y c
		GPIO_WritePin(&handlerLEDa, SET);
		GPIO_WritePin(&handlerLEDb, SET);
		GPIO_WritePin(&handlerLEDc, SET);
		GPIO_WritePin(&handlerLEDd, SET);
		GPIO_WritePin(&handlerLEDe, SET);
		GPIO_WritePin(&handlerLEDf, SET);
	}
}

void Encender_Culebra(uint8_t numero2){
	// apagando todos los led
	GPIO_WritePin(&handlerLEDa, RESET);
	GPIO_WritePin(&handlerLEDb, RESET);
	GPIO_WritePin(&handlerLEDc, RESET);
	GPIO_WritePin(&handlerLEDd, RESET);
	GPIO_WritePin(&handlerLEDe, RESET);
	GPIO_WritePin(&handlerLEDf, RESET);
	GPIO_WritePin(&handlerLEDg, RESET);

	if(numero2 == 0){ //debe encender a de las unidades
		GPIO_WritePin(&handlerLEDa, SET);
	}
	else if(numero2 == 1){ // debe encender a de las decenas
		GPIO_WritePin(&handlerLEDa, SET);
	}
	else if(numero2 == 2){ // debe encender f de las decenas
		GPIO_WritePin(&handlerLEDf, SET);
	}
	else if(numero2 == 3){ // debe encender e de las decenas
		GPIO_WritePin(&handlerLEDe, SET);
	}
	else if(numero2 == 4){ // debe encender d de las decenas
		GPIO_WritePin(&handlerLEDd, SET);
	}
	else if(numero2 == 5){ // debe encender e de las unidades
		GPIO_WritePin(&handlerLEDe, SET);
	}
	else if(numero2 == 6){ // debe encender f de las unidades
		GPIO_WritePin(&handlerLEDf, SET);
	}
	else if(numero2 == 7){ // debe encender b de las decenas
		GPIO_WritePin(&handlerLEDb, SET);
	}
	else if(numero2 == 8){ // debe encender c de las decenas
		GPIO_WritePin(&handlerLEDc, SET);
	}
	else if(numero2 == 9){ // debe encender d de las unidades
		GPIO_WritePin(&handlerLEDd, SET);
	}
	else if(numero2 == 10){ // debe encender c de las unidades
		GPIO_WritePin(&handlerLEDc, SET);
	}
	else if(numero2 == 11){ // debe encender b de las unidades
		GPIO_WritePin(&handlerLEDb, SET);
	}

}

void BasicTimer2_Callback(void){

	GPIOxTooglePin(&handlerLEDBlinky);

}

void BasicTimer3_Callback(void){

	if(banderaSw == 0){
		if(GPIO_ReadPin(&handlerTranDecenas) == 1){

			Encender_Display(decenas);
			GPIOxTooglePin(&handlerTranUnidades);
			GPIOxTooglePin(&handlerTranDecenas);
		}
		else if(GPIO_ReadPin(&handlerTranUnidades) == 1){

			Encender_Display(unidades);
			GPIOxTooglePin(&handlerTranDecenas);
			GPIOxTooglePin(&handlerTranUnidades);
		}
	}
	else{

		for(i = 0; i < 6; i++){ //se usa el for para que se recorra todo el arreglo buscando el contador
			if(culebritaDecenas[i] == contador){

				Encender_Culebra(contador);
				GPIO_WritePin(&handlerTranDecenas, RESET);
				GPIO_WritePin(&handlerTranUnidades, SET);

			}
			else if(culebritaUnidades[i] == contador){

				Encender_Culebra(contador);
				GPIO_WritePin(&handlerTranUnidades, RESET);
				GPIO_WritePin(&handlerTranDecenas, SET);

			}
		}
	}
}


void callback_extInt3 (void){
	banderaClock = 1; // se levanta una bandera para que el codigo en el while se ejecute

}


void callback_extInt5 (void){
	// se levanta o baja la bandera cada vez que se presiona el boton del encoder
	if(banderaSw == 0){
		banderaSw = 1;
	}
	else{
		banderaSw = 0;
	}
	counterExti1++;
}




