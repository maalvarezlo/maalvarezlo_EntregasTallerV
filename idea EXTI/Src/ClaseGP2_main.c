/*
 * ClaseGP2_main.c
 *
 *  Created on: 24/03/2023
 *      Author: mateo
 */


#include "stdint.h"
#include "stm32f4xx.h"

/*definicion de elementos del prograba*/
uint32_t counterG2 = 0;
uint32_t auxVariableG2 = 0;

uint32_t *ptr_CounterG2;

uint8_t byteVariableG2;
uint8_t *ptr_ByteVariableG2;



int main(void){

	/* trabajando con las variables y los punteros */
	counterG2 = 3456789;
	auxVariableG2 = counterG2;

	ptr_CounterG2 = &counterG2; // pasando la posicion a la memoria del counter

	*ptr_CounterG2 = 9876543;  //

	ptr_CounterG2++;
	*ptr_CounterG2 = 9876543;

	byteVariableG2 = 234;
	ptr_ByteVariableG2 = &byteVariableG2;
	*ptr_ByteVariableG2 = 87;

	//ptr_ByteVariableG2 = &counterG2;
	ptr_CounterG2 = &counterG2;
	auxVariableG2 = (uint32_t)ptr_CounterG2;

	ptr_ByteVariableG2 = (uint8_t)auxVariableG2;
	*ptr_ByteVariableG2 = 2;


	while(1){

		return(0);
	}
}
