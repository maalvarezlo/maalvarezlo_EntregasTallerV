/*
 * RTCDriver.h
 *
 *  Created on: 8/06/2023
 *      Author: mateo
 */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include <stm32f4xx.h>

#define LUNES					1
#define MARTES 				    2
#define MIERCOLES				3
#define JUEVES				    4
#define VIERNES					5
#define SABADO				    6
#define DOMINGO				 	7

typedef struct{

uint16_t NumeroDia; //Configuración valor día
uint16_t Año; //Configuración del año
uint16_t Mes; //Configuración del mes
uint16_t Horas; //Configuración de las horas
uint16_t Minutos; //Configuración de los minutos
uint16_t Segundos; //Configuración de los segundos

}RTC_Config_t;

typedef struct{
	RTC_TypeDef 	*ptrRTC; //Dirección
	RTC_Config_t	RTC_Config; //Configuración

}RTC_Handler_t;

//Funciones
void configRTC(RTC_Handler_t *RTCConfig);
uint8_t convertBcdRTC(uint16_t BCD);
void *cargarRTC(void);


#endif /* RTCDRIVER_H_ */
