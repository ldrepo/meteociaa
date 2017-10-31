/*
 * uartstrings.c
 *
 *  Created on: 30/10/2017
 *      Author: root
 */

#include "sapi.h"

/**
 * @brief	Toma una cadena de texto ingrsada desde la UART
 * @param	uart  : Indica desde qu� UART se quiere leer
 * @param	*receivedString  : Puntero con la direcci�n a donde se debe poner el string le�do de la UART
 * @param	lenghString  : Cantidad m�xima de caracteres que se pueden leer
 * @return	Cantidad de bytes le�dos
 * @note
 */
uint8_t uartReadString( uartMap_t uart, uint8_t* receivedString, uint8_t lenghString ){
	uint8_t datoRx;
	uint8_t largo = 0;
	uint8_t *ptrStr = receivedString;

	do{
		if(uartReadByte(uart, &datoRx)) {			//Si llega un caracter
			uartWriteByte( uart, datoRx );			//IMprime el ECO
			if(datoRx == '\n' || datoRx == '\r') {	//Si es fin de linea
				break;								//  termina recepci�n
			}
			else{
				*ptrStr++ = datoRx;					//Almacena dato recibido
			}
		}
	}while(largo < lenghString);					// hasta que solo queda un lugar
	*ptrStr = '\0';									//Termina con NULL al final de la cadena
	return largo;									//Retorna el largo de la cadena leida
}

