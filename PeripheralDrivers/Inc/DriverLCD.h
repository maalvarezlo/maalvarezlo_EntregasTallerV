/*
 * @file		: DriverLCD.h
 *
 */


#ifndef DRIVERLCD_H_
#define DRIVERLCD_H_

#include <stm32f4xx.h>
#include "I2CDriver.h"
#include "SysTickDriver.h"

void InicioLCD(I2C_Handler_t *ptrHandlerI2C);
void CMD_toLCD(I2C_Handler_t *ptrHandlerI2C, char cmd);
void sendDataLCD(I2C_Handler_t *ptrHandlerI2C, char data);
void sendMSJCD(I2C_Handler_t *ptrHandlerI2C, char *str);
void LimpiarLCD(I2C_Handler_t *ptrHandlerI2C);
void moverCursorLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y);
void ResetScreenLCD(I2C_Handler_t *ptrHandlerI2C);
void EscribirLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);


#endif /* DRIVERLCD_H_ */

