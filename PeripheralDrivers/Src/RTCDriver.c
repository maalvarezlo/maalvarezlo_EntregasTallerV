/*
 * RTCDriver.c
 *
 *  Created on: 8/06/2023
 *      Author: mateo
 */


#include "RTCDriver.h"

uint16_t datos[6] = {0};

void configRTC(RTC_Handler_t *ptrRTCConfig){

	// primero se debe encender el LSE para que el RTC funcione con el cristal

	// activamos el reloj del bus
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	//El bit DBP debe ser establecido para habilitar el acceso de escritura a los registros del RTC. (quitar bloqueo).
	PWR->CR |= PWR_CR_DBP;

	RCC->BDCR |= RCC_BDCR_RTCEN;     //activando el RTC
	RCC->BDCR |= RCC_BDCR_LSEON;     //encendemos el LSE
	RCC->BDCR |= RCC_BDCR_RTCSEL_0;	 // seleccionamos el LSE como el reloj del RTC

	//Esperando que se estabilice
	while(!(RCC->BDCR & RCC_BDCR_LSERDY)){
		__NOP() ;
	}

	/*Después de un reinicio del dominio de respaldo, todos los registros del RTC están protegidos contra escritura.
	la escritura en los registros del RTC se habilita escribiendo una clave en el registro de protección de escritura, RTC_WPR.*/
	//Escribir '0xCA' en el registro RTC_WPR.
	RTC->WPR = 0xCA;
	//Escribir '0x53' en el registro RTC_WPR.
	RTC->WPR = 0x53;

	//Establecer el bit INIT en 1 en el registro RTC_ISR para ingresar al modo de inicialización.
	//En este modo, el contador del calendario se detiene y su valor puede ser actualizado.
	RTC->ISR |= RTC_ISR_INIT;
	// Esperamos que se seingresealmodo de inicializacion
	while (!(RTC->ISR & RTC_ISR_INITF)){
		__NOP() ;
	}

	//Usamos los preescaler sincronicos y asincronicos para pasar el reloj de 32.768KHz a 1HZ (para que el tiempo sea de 1 segundo).
	// ponemos el factor del preescaler asincronico en 0bx1111111 (127) y como el ck_apre frequency = RTCCLK frequency/(PREDIV_A+1)
	RTC->PRER |= RTC_PRER_PREDIV_A; //32.768Khz/128 = 256Hz
	// ahora ponemos 255 en elpreescaler sincronico para tener como resultado 1Hz ck_spre frequency = ck_apre frequency/(PREDIV_S+1)
	RTC->PRER |= 255 << 0; // 256hz/256 = 1hz

	// Para que se haga con la mayor precision posible es necesario evitar el uso de los registros sombra y usar directamente
	// los contadores principales. para esto  colocamos el bit BYPSHAD en 1
	RTC->CR |= RTC_CR_BYPSHAD;

	// configurando la hora inicial desdecon los datos del handler

	//TR para la hora
	RTC->TR = 0;
	//DR para la fecha
	RTC->DR = 0;

	// Configuramos la hora en el DT, el valor es en elBCD

	//Horas
	RTC->TR |= ((ptrRTCConfig->RTC_Config.Horas) / 10) << RTC_TR_HT_Pos; // decenas
	RTC->TR |= ((ptrRTCConfig->RTC_Config.Horas) % 10) << RTC_TR_HU_Pos; // unidades

	//Minutos
	RTC->TR |= ((ptrRTCConfig->RTC_Config.Minutos) / 10) << RTC_TR_MNT_Pos; // decenas
	RTC->TR |= ((ptrRTCConfig->RTC_Config.Minutos) % 10) << RTC_TR_MNU_Pos; // unidades

	//Segundos
	RTC->TR |= ((ptrRTCConfig->RTC_Config.Segundos) / 10) << RTC_TR_ST_Pos; // decenas
	RTC->TR |= ((ptrRTCConfig->RTC_Config.Segundos) % 10) << RTC_TR_SU_Pos; // unidades

	//Configuramos la fecha en el DR, el valor es en BCD
	//año
	RTC->DR |= ((ptrRTCConfig->RTC_Config.Año) / 10) << RTC_DR_YT_Pos;  //decenas
	RTC->DR |= ((ptrRTCConfig->RTC_Config.Año) % 10) << RTC_DR_YU_Pos;  //unidades

	//Mes
	RTC->DR |= ((ptrRTCConfig->RTC_Config.Mes) / 10) << RTC_DR_MT_Pos;  //decenas
	RTC->DR |= ((ptrRTCConfig->RTC_Config.Mes) % 10) << RTC_DR_MU_Pos;  //unidades

	//Dia del mes (numero)
	RTC->DR |= ((ptrRTCConfig->RTC_Config.NumeroDia) / 10) << RTC_DR_DT_Pos;  // decenas
	RTC->DR |= ((ptrRTCConfig->RTC_Config.NumeroDia) % 10) << RTC_DR_DU_Pos;  // unidades

	//Saliendo del modo de inicializacion
	RCC->BDCR |= RCC_BDCR_RTCEN; //Activando el RTC
	RTC->ISR &= ~RTC_ISR_INIT;   //modo libre
	PWR->CR  &= ~PWR_CR_DBP;

	//ponemos en 0 el bit para seleccionar los registros sombra
	RTC->CR &= ~RTC_CR_BYPSHAD;
}

void *cargarRTC(void){

	// Definimos unas variables donde se pondra el numero en decenas
	 uint8_t Dia = 0;
	 uint8_t Mes = 0;
	 uint8_t Año = 0;
	 uint8_t Horas     = 0;
	 uint8_t Minutos = 0;
	 uint8_t Segundos = 0;

	 //leemos todo el registro TR para tener la hora completa en BCD
	 uint32_t HoraCompleta = 0;
	 HoraCompleta = RTC->TR;
	 //convertimos los bits respectivos
	 Horas   = convertBcdRTC(((HoraCompleta & 0x3F0000) >> 16));
	 Minutos = convertBcdRTC(((HoraCompleta & 0x007F00) >> 8));
	 Segundos = convertBcdRTC((HoraCompleta  & 0x7F));

	 //leemos todo el registro DR para tener la Fecha completa en BCD
	 uint32_t FechaCompleta = 0;
	 FechaCompleta = RTC->DR;
	 //Convertimos los bits respectivos
	 Año = convertBcdRTC(((FechaCompleta & 0xFF0000) >> 16));
	 Mes = convertBcdRTC(((FechaCompleta & 0x1F00)   >> 8));
	 Dia = convertBcdRTC((FechaCompleta  & 0x3F));


	 datos[0] = Dia;
	 datos[1] = Mes;
	 datos[2] = Año;
	 datos[3] = Horas;
	 datos[4] = Minutos;
	 datos[5] = Segundos;
	return datos;
}

uint8_t convertBcdRTC(uint16_t BCD){

    uint8_t numero = ((BCD /16 * 10) + (BCD % 16));
    return numero;

}

