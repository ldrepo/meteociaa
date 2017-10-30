/*
 * api.c
 *
 *  Created on: Oct 25, 2017
 *      Author: seb
 */

#include "api.h"

#include "apiSD.h"
#include "sapi.h"

static bool_t enableTemp 	= 1;
static bool_t enableHum 	= 1;
static bool_t enableWind 	= 1;

static void FormatInformationArray(uint16_t valor, uint8_t * destiny, uint8_t pos);

uint8_t apiConfig_SensorTemp_Enable(bool_t flagEnable) {
	enableTemp = flagEnable;
	return _API_STATE_OK;
}

uint8_t apiConfig_SensorHum_Enable(bool_t flagEnable) {
	enableHum = flagEnable;
	return _API_STATE_OK;
}

uint8_t apiConfig_SensorWind_Enable(bool_t flagEnable) {
	enableWind = flagEnable;
	return _API_STATE_OK;
}

uint8_t apiReadTemperatureHumdity(uint16_t * dataTemp, uint16_t * dataHum) {
	uint16_t adcValue1, adcValue2;

	adcValue1 = adcRead(CH1); // temp
	(*dataTemp) = adcValue1;

	adcValue2 = adcRead(CH2); // hum
	(*dataHum) = adcValue1;

	return _API_STATE_OK;
}

uint8_t apiReadWind(uint16_t * dataWind) {
	uint16_t adcValue3 = 0;

	adcValue3 = adcRead(CH3); // viento
	(*dataWind) = adcValue3;

	return _API_STATE_OK;
}

uint8_t apiReadSensor(uint16_t * dataTemp, uint16_t * dataHum, uint16_t * dataWind) {

	apiReadTemperatureHumdity(dataTemp, dataHum);

	apiReadWind(dataWind);

	return _API_STATE_OK;
}

static void FormatInformationArray(uint16_t valor, uint8_t * destiny, uint8_t pos){
	destiny[pos] = valor/1000 		+ '0';
	pos++;
	destiny[pos] = (valor%1000)/100 + '0';
	pos++;
	destiny[pos] = (valor%100)/10 	+ '0';
	pos++;
	destiny[pos] = (valor%10) 		+ '0';
}

uint8_t apiProcessInformation(uint16_t dataTemp, uint16_t dataHum, uint16_t dataWind, uint8_t * destiny) {

	uint8_t pos;
	/*
	 * Recibir 3 datos enteros X, Y, Z
	 * posiciones	= 0123 4 5678 9 0123 4 5  6  7
	 * formato 		= XXXX ; YYYY ; ZZZZ ; \r \n \0
	 * formato 		= XXXX ;  ;  ; \r \n \0
	 */

	pos = 0;

	if(enableTemp) {
		FormatInformationArray(dataTemp, destiny, pos);
		pos += 4;
	}
	destiny[pos] = ';';
	pos++;

	if(enableHum) {
		FormatInformationArray(dataHum, destiny, pos);
		pos += 4;
	}
	destiny[pos] = ';';
	pos++;

	if(enableWind) {
		FormatInformationArray(dataWind, destiny, pos);
		pos += 4;
	}
	destiny[pos] = ';';
	pos++;

	destiny[pos] = '\r';
	pos++;

	destiny[pos] = '\n';
	pos++;

	destiny[pos] = '\0';
	pos++;

	return _API_STATE_OK;
}

uint8_t apiWriteSD(uint8_t * filename, uint8_t * stringData) {

	if(apiSD_Init() == _API_STATE_ERROR) {
		// error
	} else {
		if(apiSD_Write((char*)filename, (char*)stringData) == _API_STATE_ERROR) {
			// error
			gpioWrite( LEDR, ON );
		} else {
			// ok
			gpioWrite( LEDG, ON );
		}
	}

	return _API_STATE_OK;
}
