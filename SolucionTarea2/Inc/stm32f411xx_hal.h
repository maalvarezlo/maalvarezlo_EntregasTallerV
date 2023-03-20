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


/* posiciones de memoria para periféricos del AHB1
 * observar que NO esta completa
 */

#define RCC_BASE_ADDR             (AHB1_BASE_ADDR + 0x3800U)
#define GPIOH_BASE_ADDR           (AHB1_BASE_ADDR + 0x1c00U)
#define GPIOE_BASE_ADDR           (AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR           (AHB1_BASE_ADDR + 0x0c00U)
#define GPIOC_BASE_ADDR           (AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR           (AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR           (AHB1_BASE_ADDR + 0x0000U)

/*
 * Macros genericos
 */

#define ENABLE               1
#define DISABLE              0
#define SET                  ENABLE
#define CLEAR                DISABLE
#define RESET                DISABLE
#define GPIO_PIN_SET         SET
#define GPIO_PIN_RESET       RESET
#define FLAG_SET             SET
#define FLAG_RESET           RESET
#define I2C_WRITE            0
#define I2C_READ             1

/*
 * +++======= INICIO de la descripcion de los elementos que componen el periferico =======+++*/

/* Definicion de la estructura de datos que representa a acada uno de los registros que componen el periferico RCC
 *
 * Debido a los temas que se van a manejar en el curso, solo se deben definir los bit de los registros:
 * 6.3.1 (RCC_CR) gasta 6.3.12 (RCC_APB2ENR), 6.3.17 (RCC_BDCR) y 6.3.18 (RCC_CSR)
 *
 * NOTA: La posicon de memoria (offset) debe encajar perfectamente con la posicion de memoria indicada
 * en la hoja de datos del equipo. Observe que los elemtos "reservedx" tambien estan presentes allí.
 *  */

typedef struct
{
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t reserved0;
	volatile uint32_t reserved1;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t reserved2;
	volatile uint32_t reserved3;
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t reserved4;
	volatile uint32_t reserved5;
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t reserved6;
	volatile uint32_t reserved7;
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	volatile uint32_t reserved8;
	volatile uint32_t reserved9;
	volatile uint32_t APB1LPERN;
	volatile uint32_t APB2LPERN;
	volatile uint32_t reserved10;
	volatile uint32_t reserved11;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	volatile uint32_t reserved12;
	volatile uint32_t reserved13;
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
	volatile uint32_t reserved14;
	volatile uint32_t DCKCFGR;

} RCC_RegDef_t;

/*
 * como se vio en la clase de introduccion a las estructuras, hacemos un puntero RCC_RegDef_t que
 * apunta a la posicion exacta del periferico RCC, de forma que cada miembro de la estructura coincide
 * con cada uno de los SFR en la memoria MCU. Esta accion la estamos haciendo en un MACRO, de forma que el nuevo elemnto "RCC"
 * queda disponible para cada clase .c (archivo .c) que incluya este archivo
 */
#define RCC         ((RCC_RegDef_t *) RCC_BASE_ADDR)


/* 6.3.9 RCC_AHB1ENR */
#define RCC_AHB1ENR_GPIOA_EN           0
#define RCC_AHB1ENR_GPIOB_EN           1
#define RCC_AHB1ENR_GPIOC_EN           2
#define RCC_AHB1ENR_GPIOD_EN           3
#define RCC_AHB1ENR_GPIOE_EN           4
#define RCC_AHB1ENR_GPIOH_EN           7
#define RCC_AHB1ENR_CRCEN              12
#define RCC_AHB1ENR_DMA1_EN            21
#define RCC_AHB1ENR_DMA2_EN            22

/*
 * ======== FIN de la descripcion de los elementos que componen el periferico RCC =========== */

/*+++=== INICIO de la descripcion de los elementos que componen el periferico GPIOx =====+++ */
/*periferico GPIO.
 * Debido a que el periferico GPIOx es muy simple, no es muy necesario crear la descripcion bit a bit de cada uno
 * de los regustros que componen dicho periferico, pero si es necesarion comprender que hace cada registro, para poder
 * cargar correctamente la configuracion.
 */

typedef struct
{
	volatile uint32_t MODER;     // port mode register      ADDR_OFFSET: 0x00
	volatile uint32_t OTYPER;    // port output type register
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;

} GPIOx_RegDef_t;

/* al igual que el RCC creamos un puntero a la estructura que define GPIOx y debemos hacer que cada GPIOx (A, B, C)
 * quede ubicado exactamente sobre la posicion de memoria correcta. Debido a que son varios perifericos GPIOx, es
 * necesario hacer la definicion por cada uno.
 */

/*
 * NOTA: tener cuidado que cada elemto coincida con su respectiva base
 */
#define GPIOA             ((GPIOx_RegDef_t *) GPIOA_BASE_ADDR)
#define GPIOB             ((GPIOx_RegDef_t *) GPIOB_BASE_ADDR)
#define GPIOC             ((GPIOx_RegDef_t *) GPIOC_BASE_ADDR)
#define GPIOD             ((GPIOx_RegDef_t *) GPIOD_BASE_ADDR)
#define GPIOE             ((GPIOx_RegDef_t *) GPIOE_BASE_ADDR)
#define GPIOH             ((GPIOx_RegDef_t *) GPIOH_BASE_ADDR)

/*
 * valores estadar para las configuraciones */

/*
 * 8.4.1 GPIOx_moder (dos bits por cada pin*/

#define GPIO_MODE_IN         0
#define GPIO_MODE_OUT        1
#define GPIO_MODE_ALTFN      2
#define GPIO_MODE_ANALOG     3

/* 8.4.2 GPIOx_moder (dos bits por cada pin*/

#define GPIO_OTYPE_PUSHPULL         0
#define GPIO_OTYPE_OPENDRAIN        1

/* 8.4.3 GPIOx_moder (dos bits por cada pin*/

#define GPIO_OSPEED_LOW         0
#define GPIO_OSPEED_MEDIUM      1
#define GPIO_OSPEED_FAST        2
#define GPIO_OSPEED_HIGH        3

/* 8.4.3 GPIOx_moder (dos bits por cada pin*/

#define GPIO_PUPDR_NOTHING          0
#define GPIO_PUPDR_PULLUP           1
#define GPIO_PUPDR_PULLDOWN         2
#define GPIO_PUPDR_RESERVED         3


/*DEFINICION DE LO NOMBRES DE LOS PINES*/
#define PIN_0           0
#define PIN_1           1
#define PIN_2           2
#define PIN_3           3
#define PIN_4           4
#define PIN_5           5
#define PIN_6           6
#define PIN_7           7
#define PIN_8           8
#define PIN_9           9
#define PIN_10          10
#define PIN_11          11
#define PIN_12          12
#define PIN_13          13
#define PIN_14          14
#define PIN_15          15

/* definicion de las funciones alternativas */
#define AF0      0b0000
#define AF1      0b0001
#define AF2      0b0010
#define AF3      0b0011
#define AF4      0b0100
#define AF5      0b0101
#define AF6      0b0110
#define AF7      0b0111
#define AF8      0b1000
#define AF9      0b1001
#define AF10     0b1010
#define AF11     0b1011
#define AF12     0b1100
#define AF13     0b1101
#define AF14     0b1110
#define AF15     0b1111





#endif /* STM32F411XX_HAL_H_ */
