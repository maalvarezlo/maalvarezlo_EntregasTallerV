/*
 * Taller8_template.c
 *
 *  Created on: 28/03/2023
 *      Author: alvelezp - jutoroa
 */

// Taller 6 - Paso por valor y paso por referencia

#include <stdint.h>

// Creacion de una funcion que duplique el valor de un numero

// 1.0 Paso por valor Básico

void duplicarNumero(uint8_t numero);

// 1.1 Paso por referencia

void duplicarNumeroRef(uint8_t *numero);

// 1.2 Paso por valor reasignando variables.

uint8_t duplicarNumeroReturn(uint8_t numero);

// 1.3 Arreglos por Referencia

void abonarDeudas(uint16_t misDeudas[], uint8_t cantidadDeudas);

// ***** // SOLUCION EJERCICIO // ***** //

void stringCaseConverter(uint8_t *string);

int main(void){

	uint8_t variable = 10;

	duplicarNumeroRef(&variable);

	uint16_t deudas[] = {1000, 5000 , 8000, 7000};

	abonarDeudas(deudas, 4);



	// 1.5 EJERCICIO:

	// Crear una función llamada stringCaseConverter que no retorne ningún
	// valor, y reciba una string.

	// Esta función deberá cambiar las minúsculas por mayúsculas y viceversa
	// del string original.

	while(1){

	}

}

void duplicarNumero(uint8_t numero){
	numero = numero*2;

}

uint8_t duplicarNumeroReturn(uint8_t numero){
	return numero*2;
}

void duplicarNumeroRef(uint8_t *numero){
	*numero = (*numero)*2;
}


void abonarDeudas(uint16_t *misDeudas, uint8_t cantidadDeudas){

	for(uint8_t i = 0; i<cantidadDeudas; i++){

		misDeudas[i] = misDeudas[i]/2;
		//*(misDeudas+i) = *(misDeudas+i)/2; para puntero arriba en la funcion *misDeudas=misDeudas[]
	}
}

void stringCaseConverter(uint8_t *string){
	uint8_t i = 0;
	while(*(string + i) != 0){


	}
}

