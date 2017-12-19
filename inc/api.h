/*============================================================================
 * Licencia: 
 * Autor: 
 * Fecha: 
 *===========================================================================*/

#ifndef _API2_H_
#define _API2_H_

/*==================[inclusiones]============================================*/

#include "sapi.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/
uint8_t apiConfigHardware			(void);
uint8_t apiConfiguracion			(void);
uint8_t apiReadSensor				(void);
uint8_t apiEnviarDatosASd			(void);
uint8_t apiEnviarDatosAUart			(void);
uint8_t apiValoresToStr				(char * strRTCValue,char * strTempValue,char * strHumValue,char * strWindValue);
uint8_t apiEscribirSensoresLcd		(void);
uint8_t apiMostrarTemperaturaEnLed	(void);
uint8_t apiEscribirSensoresWifi		(void);
uint8_t apiEscribirSensoresBluetooth(void);

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _API2_H_ */
