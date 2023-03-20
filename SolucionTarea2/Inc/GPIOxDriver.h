/*
 * GPIOxDriver.h
 *
 *  Created on: 10/03/2023
 *      Author: mateo
 *
 *      este programa se encargara de las definiciones relacionadas unicamente con el debido manejo y control
 *      del periferico GPIOx (funciones del controlador)
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_

// Incluyendo este archivo estamos inclyendo tambien el corriespondente GPIOx
#include "stm32f411xx_hal.h"

typedef struct
{
	uint8_t GPIO_PinNumber;              // PIN con el que se desea trabajar
	uint8_t GPIO_PinMode;                // Modo de la configuracion: entrada, salida, analogo, f. alternativa
	uint8_t GPIO_PinSpeed;               // Velocidad de respuesta PIN (solo para digital)
	uint8_t GPIO_PinPuPdControl;         // Seleccionamos si deseamos una salida Pull.up-down o "libre"
	uint8_t GPIO_PinOPType;              // Trabaja de la mano con el anterior, selecciona salida PUPD o Opendrain
	uint8_t GPIO_PinAltFunMode;          // Seleccionamos cual es l funcion alternativa que se está configurando

}GPIO_PinConfig_t;

/*
 * Esta es una estructura que contiene dos elementos:
 * - La direccion del puerto que se esta utilizando (la referencia al puerto)
 * - La configuracion especifica del PIN que se esta utilizando
 */

typedef struct
{
	GPIOx_RegDef_t         *pGPIOx;             // Direccion del puerto al que el PIN corresponde
	GPIO_PinConfig_t       GPIO_PinConfig;    // Configuracion del PIN

}GPIO_Handler_t;

//definicion de las cabeceras de las funciones del GPIODriver
void GPIO_Config (GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler);
void GPIOxTooglePin(GPIO_Handler_t *pPinHandler);

#endif /* GPIOXDRIVER_H_ */
