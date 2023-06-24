/*
 * LCDst7789Driver.h
 *
 *  Created on: 20/06/2023
 *      Author: mateo
 */

#ifndef LCDST7789DRIVER_H_
#define LCDST7789DRIVER_H_
#include "stm32f4xx.h"
#include "stdint.h"

//Colores
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define GRAY        0X8430
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define BROWN       0XBC40
#define BRRED       0XFC07
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458

#define LIGHTGREEN  0X841F
#define LGRAY       0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12

//tamaño
#define LCD_WIDTH   320 //LCD width
#define LCD_HEIGHT  240 //LCD height


//Prototipo de las funciones
void LCD_Reset(void);
void LCD_Write_Command(uint8_t cmd);
void LCD_WriteData_Word(uint8_t *buff, uint32_t buff_size);
void LCD_WriteData_Byte(uint8_t data);
void LCD_Init(void);
void LCD_SetWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t  y_end);
void LCD_Fill_Color(uint16_t color);
void LCD_configPin(void);
void LCD_Fill_Square(uint16_t WIDTH, uint16_t HEIGHT, uint16_t color);









#endif /* LCDST7789DRIVER_H_ */
