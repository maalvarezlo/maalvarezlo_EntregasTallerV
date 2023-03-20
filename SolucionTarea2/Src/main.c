/*
 * IntroGPIOxMain.c
 *
 *  Created on: 14/03/2023
 *      Author: Mateo Alvarez Lopera
 *      correo: maalvarezlo@unal.edu.co
 */

#include <stdint.h>
#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"


// Definimos todos los handler para los PINES que deseamos configurar.
GPIO_Handler_t handlerUserLedPin1 = {0};
GPIO_Handler_t handlerUserLedPin5 = {0};
GPIO_Handler_t handlerUserLed0 = {0};
GPIO_Handler_t handlerUserLed1 = {0};
GPIO_Handler_t handlerUserLed2 = {0};
GPIO_Handler_t handlerUserLed3 = {0};
GPIO_Handler_t handlerUserLed4 = {0};
GPIO_Handler_t handlerUserLed5 = {0};
GPIO_Handler_t handlerUserLed6 = {0};
GPIO_Handler_t handlerUserBoton = {0};

// Definimos todas las variables a utilizar.
uint8_t pin_1 = 0;
uint8_t pin_5 = 0;
uint8_t segundero = 0;
uint8_t USER_BUTTON = 0;
uint8_t bit0 = 0;
uint8_t bit1 = 0;
uint8_t bit2 = 0;
uint8_t bit3 = 0;
uint8_t bit4 = 0;
uint8_t bit5 = 0;
uint8_t bit6 = 0;

// Definicion de una funcion llamada EncenderLed (en el punto 3 se explica su funcionamiento)
void EncenderLed(uint8_t segundero);



// Funcion principal del programa. Es aca donde se ejecuta todo
int main(void){


	//PUNTO 1
	/*El problema en el archivo GPIOxDriver.c es que en la funcion GPIO_ReadPin() solo Desplaza el registro
	 * IDR a la derecha el numero de veces del pin, ignorando todos los numeros de la izquierda del pin, esto da
	 * problemas a la hora de leer 2 o mas pines con el mismo GPIO.
	 *
	 *Para solucionarlo solo hay que limpiar los numeros de la izquiera del pin, esto se logra con una mascara y
	 *una operacon AND, teniendo en cuenta que con el Shift que ya se realizó, el valor del Pin se encuentra en la
	 *la posicion 0 (esto se realiza en el archivo GPIOxDriver.c)
	 *
	 * A continuacion se hará la prueba de la correcciÓn con 2 pines (pin_1 y pin_5) para comprobar que
	 * ambos se leen correctamente.
	 */

	// Deseamos trabajar con el puerto GPIOB en ambos pines.
	// PIN 1
	handlerUserLedPin1.pGPIOx = GPIOB;
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinNumber			= PIN_1;
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinMode			    = GPIO_MODE_OUT;
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_PULLUP;
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinSpeed			    = GPIO_OSPEED_FAST;
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;	// Ninguna funcion
	//PIN 5
	handlerUserLedPin5.pGPIOx = GPIOB;
	handlerUserLedPin5.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerUserLedPin5.GPIO_PinConfig.GPIO_PinMode			    = GPIO_MODE_OUT;
	handlerUserLedPin5.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin5.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_PULLUP;
	handlerUserLedPin5.GPIO_PinConfig.GPIO_PinSpeed			    = GPIO_OSPEED_FAST;
	handlerUserLedPin5.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;	// Ninguna funcion

	//Cargamos la configuracion del pin especifico
	GPIO_Config(&handlerUserLedPin1);
	GPIO_Config(&handlerUserLedPin5);

	//Haciendo que ambos pines se enciendan
	GPIO_WritePin(&handlerUserLedPin1, SET);
	GPIO_WritePin(&handlerUserLedPin5, SET);

	//Ahora llamamos la funcion ReadPin para ver el estado del Pin (ambos deben tener el valor 1)
	pin_1 = GPIO_ReadPin(&handlerUserLedPin1);
	pin_5 = GPIO_ReadPin(&handlerUserLedPin5);


	//PUNTO 2
	/*Para crear la funcion GPIOxTooglePin se puede hacer uso de las funciones ReadPin y GPIO_WritePin, ya que
	 * es muy simple configurar un if y un else, para que cuando se lea el valor del pin se cambie por el contrario.
	 * La funcion está ubicada en el archivo GPIOxDriver.c y allí se describe su funcionamiento. Tambien se debe
	 * definir la funcion en el archivo GPIOxDriver.h
	 */

	// Probando el funcionamiento de la funcion GPIOxTooglePin
	// Usaremos uno de los Pines del punto anterior, el cual se encuentra con valor 1 y deberia cambiar a 0
	GPIOxTooglePin(&handlerUserLedPin1);
	pin_1 = GPIO_ReadPin(&handlerUserLedPin1); // actualizamos la lectura para verlo en el debug
	// Ahora probar que tambien funcione de 0 a 1
	GPIOxTooglePin(&handlerUserLedPin1);
	pin_1 = GPIO_ReadPin(&handlerUserLedPin1); // El Pin termina con valor 1


	// PUNTO 3
	/* Para el punto 3 se desea crear un reloj de numeros binarios que se reinicie en 60 segundos y que al mantener
	 * presionado el boton B1, el reloj cuente hacia atras y se reincie en 1. Para lograr esto, se realizo la
	 * configuracion de cada Puerto GPIO con el pin deseado, y se uso del ciclo while para incrementar un
	 * contador cada segundo (usando un ciclo for que no hace nada, teniendo en cuenta la frecuencia del micro).
	 * Dentro del mismo ciclo while tambien se configura el boton y como afecta al contador, ademas de usar la funcion
	 * EncenderLed (comentada abajo) para encender o apagar los led de cada bit en el segundero
	 */

	// Deseamos trabajar con el PA7 (puerto GPIOA y el pin 7) para el bit0
	handlerUserLed0.pGPIOx = GPIOA;
	handlerUserLed0.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerUserLed0.GPIO_PinConfig.GPIO_PinMode			    = GPIO_MODE_OUT;
	handlerUserLed0.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLed0.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerUserLed0.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUserLed0.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;	// Ninguna funcion

	// Deseamos trabajar con el PC8 (puerto GPIOC y el pin 8) para el bit1
	handlerUserLed1.pGPIOx = GPIOC;
	handlerUserLed1.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerUserLed1.GPIO_PinConfig.GPIO_PinMode			    = GPIO_MODE_OUT;
	handlerUserLed1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLed1.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerUserLed1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUserLed1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;	// Ninguna funcion

	// Deseamos trabajar con el PC7 (puerto GPIOC y el pin 7) para el bit2
	handlerUserLed2.pGPIOx = GPIOC;
	handlerUserLed2.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerUserLed2.GPIO_PinConfig.GPIO_PinMode			    = GPIO_MODE_OUT;
	handlerUserLed2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLed2.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerUserLed2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUserLed2.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;	// Ninguna funcion

	// Deseamos trabajar con el PA6 (puerto GPIOA y el pin 6) para el bit3
	handlerUserLed3.pGPIOx = GPIOA;
	handlerUserLed3.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerUserLed3.GPIO_PinConfig.GPIO_PinMode			    = GPIO_MODE_OUT;
	handlerUserLed3.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLed3.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerUserLed3.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUserLed3.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;	// Ninguna funcion

	// Deseamos trabajar con el PB8 (puerto GPIOB y el pin 8) para el bit4
	handlerUserLed4.pGPIOx = GPIOB;
	handlerUserLed4.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerUserLed4.GPIO_PinConfig.GPIO_PinMode			    = GPIO_MODE_OUT;
	handlerUserLed4.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLed4.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerUserLed4.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUserLed4.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;	// Ninguna funcion

	// Deseamos trabajar con el PC6 (puerto GPIOC y el pin 6) para el bit5
	handlerUserLed5.pGPIOx = GPIOC;
	handlerUserLed5.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerUserLed5.GPIO_PinConfig.GPIO_PinMode			    = GPIO_MODE_OUT;
	handlerUserLed5.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLed5.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerUserLed5.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUserLed5.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;	// Ninguna funcion

	// Deseamos trabajar con el PC9 (puerto GPIOC y el pin 9) para el bit6
	handlerUserLed6.pGPIOx = GPIOC;
	handlerUserLed6.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerUserLed6.GPIO_PinConfig.GPIO_PinMode			    = GPIO_MODE_OUT;
	handlerUserLed6.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLed6.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerUserLed6.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUserLed6.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;	// Ninguna funcion

	// Se desea configurar el Pin el boton, el cual se encuentra en PC13 (Puerto GPIOC pin 13).
	handlerUserBoton.pGPIOx = GPIOC;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUserBoton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	//Cargamos la configuracion de los pines
	GPIO_Config(&handlerUserLed0);
	GPIO_Config(&handlerUserLed1);
	GPIO_Config(&handlerUserLed2);
	GPIO_Config(&handlerUserLed3);
	GPIO_Config(&handlerUserLed4);
	GPIO_Config(&handlerUserLed5);
	GPIO_Config(&handlerUserLed6);
	GPIO_Config(&handlerUserBoton);

	//Haciendo que todos los pines se enciendan
	GPIO_WritePin(&handlerUserLed0, SET);
	GPIO_WritePin(&handlerUserLed1, SET);
	GPIO_WritePin(&handlerUserLed2, SET);
	GPIO_WritePin(&handlerUserLed3, SET);
	GPIO_WritePin(&handlerUserLed4, SET);
	GPIO_WritePin(&handlerUserLed5, SET);
	GPIO_WritePin(&handlerUserLed6, SET);

	// Se usa el ciclo While infinito para que se continue ejecutando el codigo
	while(1){

		USER_BUTTON = GPIO_ReadPin(&handlerUserBoton); // Leemos el estado del boton (0 presionado, 1 sin presionar).

		EncenderLed(segundero);   // Se Usa la funcion EncenderLed, la cual se encarga de encender o apagar cada pin

		// Hacer que el contador incremente o decremente segun el estado del botón
		if(USER_BUTTON == 1){
			segundero += 1;
		}else{
			segundero -= 1;
		}

		// Hacer que el contador se reinicie en 1 o en 60 segun sea el estado del boton
		if(segundero  > 60){
			segundero = 1;
		}
		if(segundero  < 1){
			segundero = 60;
		}

		// Hacer que el microprocesador ejecute una instruccion que no hace nada por una cantidad muy grande de veces
		// y que se demore aproximadamente 1 segundo.
		for (int i=0;(i < 1250000);i++){
			NOP();
		}
	}

} //fin de la funcion main

// Esta funcion se encarga de encender cada led, primero se toma el valor del contador (segundero) y se le aplica
// una mascara para cada unade las posiciones del bit, por ejemplo para el bit5 la mascara seria 0b0100000 y asi
// por cada bit, luego se desplaza el valor obtenido para dejar el bit deseado en la posicion 0 y saber si el resultado
// es un 1 o un 0, para luego hacer uso de condicionales y encender o apagar el led deseado.
void EncenderLed(uint8_t segundero){

	bit0 = segundero & 0b1;
	bit1 = segundero & 0b1 << 1;          // Se aplica la mascara al valor del segundero
	bit1 >>= 1;                           // se mueve el bit deseado a la posicion 0
	bit2 = segundero & 0b1 << 2;
	bit2 >>= 2;
	bit3 = segundero & 0b1 << 3;
	bit3 >>= 3;
	bit4 = segundero & 0b1 << 4;
	bit4 >>= 4;
	bit5 = segundero & 0b1 << 5;
	bit5 >>= 5;
	bit6 = segundero & 0b1 << 6;
	bit6 >>= 6;

	// depende el valor del bit se enciende o se apaga cada led haciendo uso de la funcion GPIO_WritePin
	if(bit0 == 1){
		GPIO_WritePin(&handlerUserLed0, SET);
	}else{
		GPIO_WritePin(&handlerUserLed0, RESET);
	}

	if(bit1 == 1){
		GPIO_WritePin(&handlerUserLed1, SET);
	}else{
		GPIO_WritePin(&handlerUserLed1, RESET);
	}

	if(bit2 == 1){
		GPIO_WritePin(&handlerUserLed2, SET);
	}else{
		GPIO_WritePin(&handlerUserLed2, RESET);
	}

	if(bit3 == 1){
		GPIO_WritePin(&handlerUserLed3, SET);
	}else{
		GPIO_WritePin(&handlerUserLed3, RESET);
	}

	if(bit4 == 1){
		GPIO_WritePin(&handlerUserLed4, SET);
	}else{
		GPIO_WritePin(&handlerUserLed4, RESET);
	}

	if(bit5 == 1){
		GPIO_WritePin(&handlerUserLed5, SET);
	}else{
		GPIO_WritePin(&handlerUserLed5, RESET);
	}

	if(bit6 == 1){
		GPIO_WritePin(&handlerUserLed6, SET);
	}else{
		GPIO_WritePin(&handlerUserLed6, RESET);
	}

}

