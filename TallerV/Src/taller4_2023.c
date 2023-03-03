/*
 * taller4_2023.c
 *
 *  Created on: 2/03/2023
 *      Author: mateo
 */


#include <stdint.h>
//8 bits chat
char var1 = 0 ;
//32 bits int
int  var2 = 0;
//short ## bits
short var3 = 0;
//long ## bits
long var4 = 0;

//con libreria solo pongo int para entero y uint sin signo
//el numero de bits y _t
uint8_t var5 = 0;
int8_t var6 = 0;


int main(void){

	uint16_t testshift = 0b0000010101101110;

	while(1){
		//<<1 duplica >>1 divide2
		testshift = testshift >>1;
	}
}
