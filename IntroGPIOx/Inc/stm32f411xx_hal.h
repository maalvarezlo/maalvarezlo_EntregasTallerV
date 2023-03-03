/*
 * stm32f411xx_hal.h
 *
 *  Created on: Mar 3, 2023
 *      Author: maalvarezlo
 *
 *  Este archivo contiene la informacion mas basica del micro:
 *  - Valores del reloj principal
 *  - Distribucion basica de la memoria (descrito en la figura 14 de la hoja de datos del micro)
 *  - Posiciones de memoria de los perifericos disponibles en el micro descrito enla tabla 1 (Memory Map)
 *  - Incluye los demas registros de los perifericos
 *  - Definiciones de las constantes basicas
 *
 *  Nota: La definicion del NVIC será realizada al momento de describir el uso de las interrupciones
 *
 *
 *  *************************************************************************************************/



#ifndef INC_STM32F411XX_HAL_H_
#define INC_STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED     16000000      //Value for the main clock signal (HSI -> High Speed Internal)
#define HSE_CLOCK_SPEED     4000000       //Value for the main clock signal (HSI -> High Speed External)

#define NOP()      asm("NOP")
#define __weak   __attribute__((weak))

/* Base addresses of Flash and SRAM memories
 * Datasheet, Memory Map, Figure 14
 * (Remember, 1KByte = byte)
 */

#define FLASH_BASE_ADDR       0x08000000U   //Esta es la memoria del programa, 512KB
#define SRAM_BASE_ADDR        0x20000000U   // Esta es la memoria RAM, 128KB


/* NOTA: Observar que existen unos registros especificos del Cortex M4 en la reion 0xE0000000U
 * Los controladores de las interrupciones se encuentran alli, por ejemplo.
 * esto se vera a su debido tiempo
*/

/* NOTA:
 * Ahora agregamos la direccion de memoria base para cada uno de los perifericos que posee el micro
 * En el "datasheet" del micro, Figura 14 (Memory map) encontramos el mapa de los buses:
 * - APB1 (advance peripheal Bus)
 * - APB2
 * - AHB1 (Advance High-performance Bus)
 * - AHB2
*/

/**
 * AHBx and APBx bus Peripheal base addresses
 */
#define APB1_BASE_ADDR     0x40000000U
#define APB2_BASE_ADDR     0x40010000U
#define AHB1_BASE_ADDR     0x40020000U
#define AHB2_BASE_ADDR     0x50000000U

/**
 * Y ahora debemos hacer lo mismo pero cada una de las posiciones de memoria de cada de los perifericos
 * descritos en la tabla 1 del manual de referencia del micro.
 * Observe que en dicha tabla está a su vez dividida en cuatro segmentos, cada uno correspondiente
 * a APB1, APB2, AHB2, AHB2.
 *
 * Comenzar de arriba hacia abajo como se muestra en la tabla. Inicia USB_OTG_FS (AHB2)
 *
 * NOTA: Solo lo vamos a hacer con los elementos que necesitamos en este ejercicio: RCC y GPIOx
*/

/* Posiciones de memoria para perifericos del AHB2 */
#define USB_OTG_FS_BASE_ADDR    (AHB2_BASE_ADDR + 0x0000U)





#endif /* STM32F411XX_HAL_H_ */
