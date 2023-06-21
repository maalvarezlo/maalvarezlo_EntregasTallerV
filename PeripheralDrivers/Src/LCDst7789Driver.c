
#include "LCDst7789Driver.h"
#include "SysTickDriver.h"
#include "SpiDriver.h"
#include "stm32f4xx.h"
#include "GPIOxDriver.h"

GPIO_Handler_t handlerDIN         = {0};
GPIO_Handler_t handlerMISO         = {0};
GPIO_Handler_t handlerCLK         = {0};
GPIO_Handler_t handlerCS          = {0};
GPIO_Handler_t handlerDC          = {0};
GPIO_Handler_t handlerRST         = {0};
GPIO_Handler_t handlerBL          = {0};
SPI_Handler_t handlerSPI          = {0};

void LCD_configPin(void){


	handlerDIN.pGPIOx = GPIOA;
	handlerDIN.GPIO_PinConfig.GPIO_PinNumber           = PIN_7;
	handlerDIN.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerDIN.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerDIN.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_HIGH;
	handlerDIN.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	handlerDIN.GPIO_PinConfig.GPIO_PinAltFunMode       = AF5;
	GPIO_Config(&handlerDIN);

	handlerCLK.pGPIOx = GPIOB;
	handlerCLK.GPIO_PinConfig.GPIO_PinNumber           = PIN_3;
	handlerCLK.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_ALTFN;
	handlerCLK.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerCLK.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_HIGH;
	handlerCLK.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	handlerCLK.GPIO_PinConfig.GPIO_PinAltFunMode       = AF5;
	GPIO_Config(&handlerCLK);

	handlerCS.pGPIOx = GPIOA;
	handlerCS.GPIO_PinConfig.GPIO_PinNumber           = PIN_10;
	handlerCS.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerCS.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerCS.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_HIGH;
	handlerCS.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerCS);

	handlerDC.pGPIOx = GPIOA;
	handlerDC.GPIO_PinConfig.GPIO_PinNumber           = PIN_1;
	handlerDC.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerDC.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerDC.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_HIGH;
	handlerDC.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDC);

	handlerRST.pGPIOx = GPIOA;
	handlerRST.GPIO_PinConfig.GPIO_PinNumber           = PIN_0;
	handlerRST.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	handlerRST.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	handlerRST.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_HIGH;
	handlerRST.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerRST);

//	handlerBL.pGPIOx = GPIOA;
//	handlerBL.GPIO_PinConfig.GPIO_PinNumber           = PIN_5;
//	handlerBL.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
//	handlerBL.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
//	handlerBL.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_HIGH;
//	handlerBL.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
//	GPIO_Config(&handlerBL);

	//SPI
	handlerSPI.ptrSPIx                         = SPI1;
	handlerSPI.SPI_Config.SPI_mode             = SPI_MODE_3;
	handlerSPI.SPI_Config.SPI_baudrate         = SPI_BAUDRATE_FPCLK_4;
	handlerSPI.SPI_Config.SPI_datasize         = SPI_DATASIZE_8_BIT;
	handlerSPI.SPI_Config.SPI_fullDupplexEnable         = SPI_FULL_DUPPLEX;
	spi_config(handlerSPI);

}


void LCD_Reset(void){
	delay_ms(200);
	GPIO_WritePin(&handlerRST, RESET);
	delay_ms(200);
	GPIO_WritePin(&handlerRST, SET);
	delay_ms(200);
}

/*******************************************************************************
function:
		Write data and commands
*******************************************************************************/
void LCD_Write_Command(uint8_t cmd){
	GPIO_WritePin(&handlerCS, RESET);
	GPIO_WritePin(&handlerDC, RESET);
	spi_transmit(handlerSPI, &cmd, 1);
}
//void initcom(void){
//	GPIO_WritePin(&handlerCS, RESET);
//}
//
//void endcom(void){
//	GPIO_WritePin(&handlerCS, SET);
//}

void LCD_WriteData_Word(uint8_t *buff, uint32_t buff_size){
	GPIO_WritePin(&handlerCS, RESET);
	GPIO_WritePin(&handlerDC, SET);
	spi_transmit(handlerSPI, buff, buff_size);
	GPIO_WritePin(&handlerCS, SET);
}	

void LCD_WriteData_Byte(uint8_t data){
	GPIO_WritePin(&handlerCS, RESET);
	GPIO_WritePin(&handlerDC, SET);
	spi_transmit(handlerSPI, &data, 1);
	GPIO_WritePin(&handlerCS, SET);
}  



//funcion de inicializacion
void LCD_Init(void){
	LCD_Reset();

	LCD_Write_Command(0x36);
	LCD_WriteData_Byte(0xA0); 

	LCD_Write_Command(0x3A); 
	LCD_WriteData_Byte(0x05);

	LCD_Write_Command(0x21); 

	LCD_Write_Command(0x2A);
	LCD_WriteData_Byte(0x00);
	LCD_WriteData_Byte(0x01);
	LCD_WriteData_Byte(0x00);
	LCD_WriteData_Byte(0x3F);

	LCD_Write_Command(0x2B);
	LCD_WriteData_Byte(0x00);
	LCD_WriteData_Byte(0x00);
	LCD_WriteData_Byte(0x00);
	LCD_WriteData_Byte(0xEF);

	LCD_Write_Command(0xB2);
	LCD_WriteData_Byte(0x0C);
	LCD_WriteData_Byte(0x0C);
	LCD_WriteData_Byte(0x00);
	LCD_WriteData_Byte(0x33);
	LCD_WriteData_Byte(0x33);

	LCD_Write_Command(0xB7);
	LCD_WriteData_Byte(0x35); 

	LCD_Write_Command(0xBB);
	LCD_WriteData_Byte(0x1F);

	LCD_Write_Command(0xC0);
	LCD_WriteData_Byte(0x2C);

	LCD_Write_Command(0xC2);
	LCD_WriteData_Byte(0x01);

	LCD_Write_Command(0xC3);
	LCD_WriteData_Byte(0x12);

	LCD_Write_Command(0xC4);
	LCD_WriteData_Byte(0x20);

	LCD_Write_Command(0xC6);
	LCD_WriteData_Byte(0x0F);

	LCD_Write_Command(0xD0);
	LCD_WriteData_Byte(0xA4);
	LCD_WriteData_Byte(0xA1);

	LCD_Write_Command(0xE0);
	LCD_WriteData_Byte(0xD0);
	LCD_WriteData_Byte(0x08);
	LCD_WriteData_Byte(0x11);
	LCD_WriteData_Byte(0x08);
	LCD_WriteData_Byte(0x0C);
	LCD_WriteData_Byte(0x15);
	LCD_WriteData_Byte(0x39);
	LCD_WriteData_Byte(0x33);
	LCD_WriteData_Byte(0x50);
	LCD_WriteData_Byte(0x36);
	LCD_WriteData_Byte(0x13);
	LCD_WriteData_Byte(0x14);
	LCD_WriteData_Byte(0x29);
	LCD_WriteData_Byte(0x2D);

	LCD_Write_Command(0xE1);
	LCD_WriteData_Byte(0xD0);
	LCD_WriteData_Byte(0x08);
	LCD_WriteData_Byte(0x10);
	LCD_WriteData_Byte(0x08);
	LCD_WriteData_Byte(0x06);
	LCD_WriteData_Byte(0x06);
	LCD_WriteData_Byte(0x39);
	LCD_WriteData_Byte(0x44);
	LCD_WriteData_Byte(0x51);
	LCD_WriteData_Byte(0x0B);
	LCD_WriteData_Byte(0x16);
	LCD_WriteData_Byte(0x14);
	LCD_WriteData_Byte(0x2F);
	LCD_WriteData_Byte(0x31);

	LCD_Write_Command(0x21);

	LCD_Write_Command(0x11);

	LCD_Write_Command(0x29);

}

void LCD_SetWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t  y_end){ 


	LCD_Write_Command(0x2A);
//	LCD_WriteData_Byte(0x00);
	/* Column Address set */
	uint8_t datax[] = {x_start >> 8, x_start & 0xFF, (x_end-1) >> 8, (x_end-1) & 0xFF};
	LCD_WriteData_Word(datax, 4);

	LCD_Write_Command(0x2B);
//	LCD_WriteData_Byte(0x00);
	/* Row Address set */
	uint8_t datay[] = {y_start >> 8, y_start & 0xFF, (y_end-1) >> 8, (y_end-1) & 0xFF};
	LCD_WriteData_Word(datay, 5);

	LCD_Write_Command(0x2C);
}

/**
 * @brief Fill the DisplayWindow with single color
 * @param color -> color to Fill with
 * @return none
 */
void LCD_Fill_Color(uint16_t color){
	uint16_t i;
	LCD_SetWindow(0, 0, LCD_WIDTH, LCD_HEIGHT);

	uint16_t j;
	for (i = 0; i < LCD_WIDTH; i++){
		for (j = 0; j < LCD_HEIGHT; j++) {
			uint8_t data[] = {color >> 8, color & 0xFF};
			LCD_WriteData_Word(data, sizeof(data));
		}
	}
}


