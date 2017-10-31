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

rtc_t rtc;							/* Estructura RTC */

static void FormatInformationArray(uint16_t valor, uint8_t * destiny, uint8_t pos);

uint8_t apiInit_rtc(void) {
	rtc_t rtc;				/* Estructura RTC */
	bool_t val = 0;

	rtc.year = 2017;
	rtc.month = 10;
	rtc.mday = 30;
	rtc.wday = 1;
	rtc.hour = 13;
	rtc.min = 30;
	rtc.sec= 0;

	val = rtcConfig( &rtc );/* Inicializar RTC */

	return val;
}

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
	(*dataHum)  = adcValue2;

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
	destiny[pos++] = (valor/1000) 		+ '0';
	destiny[pos++] = (valor%1000)/100 	+ '0';
	destiny[pos++] = (valor%100)/10 	+ '0';
	destiny[pos]   = (valor%10) 		+ '0';
}

uint8_t apiProcessInformation(uint16_t dataTemp, uint16_t dataHum, uint16_t dataWind, uint8_t * destiny) {
	uint8_t pos;
	rtc_t rtc;				/* Estructura RTC */
	bool_t val;

    val = rtcRead( &rtc );
	/*
	 * Recibir 3 datos enteros X, Y, Z
	 * posiciones	= 0123 4 5678 9 0123 4 56789012345678901234 5  6
	 * formato 		= XXXX ; YYYY ; ZZZZ ; 2016/12/10_20:04:36;\r \n \0
	 * formato 		= XXXX ;  ;  ; \r \n \0
	 * x;y;z;time-stamp;
	 * 154;0;-25;2016/12/10_20:04:36;
	 */
	pos = 0;
	if(enableTemp) {
		FormatInformationArray(dataTemp, destiny, pos);
		pos += 4;
	}
	destiny[pos++] = ';';

	if(enableHum) {
		FormatInformationArray(dataHum, destiny, pos);
		pos += 4;
	}
	destiny[pos++] = ';';

	if(enableWind) {
		FormatInformationArray(dataWind, destiny, pos);
		pos += 4;
	}
	destiny[pos++] = ';';
	//Formato de fecha: 2016/12/10_20:04:36;
	destiny[pos++] = (rtc.year/1000) + '0';
	destiny[pos++] = ((rtc.year%1000)/100) + '0';
	destiny[pos++] = ((rtc.year%100)/10) + '0';
	destiny[pos++] = (rtc.year%10) + '0';
	destiny[pos++] = '/';
	destiny[pos++] = (rtc.month/10) + '0';
	destiny[pos++] = (rtc.month%10) + '0';
	destiny[pos++] = '/';
	destiny[pos++] = (rtc.mday/10) + '0';
	destiny[pos++] = (rtc.mday%10) + '0';
	destiny[pos++] = '_';
	destiny[pos++] = (rtc.hour/10) + '0';
	destiny[pos++] = (rtc.hour%10) + '0';
	destiny[pos++] = ':';
	destiny[pos++] = (rtc.min/10) + '0';
	destiny[pos++] = (rtc.min%10) + '0';
	destiny[pos++] = ':';
	destiny[pos++] = (rtc.sec/10) + '0';
	destiny[pos++] = (rtc.sec%10) + '0';
	destiny[pos++] = ';';
	destiny[pos++] = '\n';
	destiny[pos++] = '\r';
	destiny[pos]   = '\0';
	return pos;
}

uint8_t apiWriteSD(char * filename, uint8_t * stringData) {

	if(apiSD_Init() == _API_STATE_ERROR) {
		// error
	} else {
		if(apiSD_Write(filename, (char*)stringData) == _API_STATE_ERROR) {
			// error
			gpioWrite( LEDR, ON );
		} else {
			// ok
			gpioWrite( LEDG, ON );
		}
	}

	return _API_STATE_OK;
}
