/*
 * api.h
 *
 *  Created on: Oct 25, 2017
 *      Author: seb
 */

#ifndef API_H_
#define API_H_

#include "apiSD.h"
#include "sapi.h"
#include "sapi_spi.h"

uint8_t apiReadTemperatureHumdity		(uint16_t * dataTemp, uint16_t * dataHum);
uint8_t apiReadWind						(uint16_t * dataWind);
uint8_t apiReadSensor					(uint16_t * dataTemp, uint16_t * dataHum, uint16_t * dataWind);
uint8_t apiValToStr						(uint16_t valor, uint8_t * destiny);
uint8_t apiRTCToStr						(uint8_t * destiny);
int 	apiSaturaEntero					(int dato, int limInf, int limSup);
uint8_t apiProcessInformation			(uint16_t dataTemp, uint16_t dataHum, uint16_t dataWind, uint8_t * destiny);
uint8_t apiWriteSD						(char * filename, uint8_t * stringData);
uint8_t apiInit_rtc						(void);
uint8_t apiConfig_SensorTemp_Enable		(bool_t flagEnable);
uint8_t apiConfig_SensorHum_Enable		(bool_t flagEnable);
uint8_t apiConfig_SensorWind_Enable		(bool_t flagEnable);
uint8_t apiConfig						(void);
#endif /* API_H_ */