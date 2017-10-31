/************* APLICACION **************/
#include "main.h"

#include "api.h"
#include "sapi.h"
#include "uartstrings.h"
#include <stdlib.h>

CONSOLE_PRINT_ENABLE

/* Configuración de mi aplicación */
#define _SYS_CFG_DATALOG_FILENAME 		"datalog.txt"
#define _SYS_CFG_SAMPLINGTIME			(1000)
#define _SYS_CFG_SENSOR_ENABLE_TEMP		(1)
#define _SYS_CFG_SENSOR_ENABLE_HUM		(1)
#define _SYS_CFG_SENSOR_ENABLE_WIND		(1)

#define L_STRING_IN						(50)

bool_t sysCfgSensorEnableTemp 	= _SYS_CFG_SENSOR_ENABLE_TEMP;
bool_t sysCfgSensorEnableHum 	= _SYS_CFG_SENSOR_ENABLE_HUM;
bool_t sysCfgSensorEnableWind 	= _SYS_CFG_SENSOR_ENABLE_WIND;
tick_t tiempoMuestreo			= _SYS_CFG_SAMPLINGTIME;

uint8_t appConfiguracion			(void);
uint8_t appEstacionMeterorologica	(void);
uint8_t appCofiguraRTC				(void);
uint8_t appCofiguraPeriodoMuestreo	(void);

/************* APLICACION **************/
int main( void ){

	boardConfig();
	uartConfig(UART_USB, 115200);
	adcConfig(ADC_ENABLE);
	spiConfig(SPI0);
	tickConfig(10, diskTickHook);

	appConfiguracion();

	while(TRUE) {
		delay(tiempoMuestreo);
		appEstacionMeterorologica();
	}

	return 0;
}

uint8_t appConfiguracion(void) {
	uint32_t todosLosValoresCargados = FALSE;
	uint8_t dataUart;

	apiInit_rtc();

	consolePrintlnString("-------------------------------------");
	consolePrintlnString("  Bienvenido!!! Estacion Meteo INET  ");
	consolePrintlnString("-------------------------------------");
	consolePrintlnString("1 - Temperatura");
	consolePrintlnString("2 - Humedad");
	consolePrintlnString("3 - Viento");
	consolePrintlnString("4 - Período de muestreo");
	consolePrintlnString("5 - Fecha y Hora");
	consolePrintlnString("0 - Salir");
	consolePrintString  ("Seleccione una opcion: ");

	while(todosLosValoresCargados == FALSE){
		if(uartReadByte(UART_USB, &dataUart)){
			uartWriteByte(UART_USB, dataUart );		//Imprime el ECO
			switch(dataUart){
			case '0':
				todosLosValoresCargados = TRUE;
				break;

			case '1':
				sysCfgSensorEnableTemp 	= !sysCfgSensorEnableTemp;
				consolePrintString("\n\rTemperatura :");
				if(sysCfgSensorEnableTemp){
					consolePrintlnString("SI");
				} else {
					consolePrintlnString("NO");
				}
				break;

			case '2':
				sysCfgSensorEnableHum 	= !sysCfgSensorEnableHum;
				consolePrintString("\n\rHumedad :");
				if(sysCfgSensorEnableHum){
					consolePrintlnString("SI");
				} else {
					consolePrintlnString("NO");
				}
				break;

			case '3':
				sysCfgSensorEnableWind 	= !sysCfgSensorEnableWind;
				consolePrintString("\n\rViento :");
				if(sysCfgSensorEnableWind){
					consolePrintlnString("SI");
				} else {
					consolePrintlnString("NO");
				}
				break;

			case '4':
				appCofiguraPeriodoMuestreo();
				break;

			case '5':
				appCofiguraRTC();
				break;
			default:
				consolePrintlnString("\n\rOpción Inválida");
			}
		}
	}
	consolePrintlnString("\n\rConfiguracion terminada");
	consolePrintlnString("Fin");

	return 0;
}

uint8_t appEstacionMeterorologica(void) {
	uint16_t sensorTempValue, sensorHumValue, sensorWindValue = 0;
	uint8_t bufferDataLog[128];

	apiConfig_SensorTemp_Enable(sysCfgSensorEnableTemp);
	apiConfig_SensorHum_Enable(sysCfgSensorEnableHum);
	apiConfig_SensorWind_Enable(sysCfgSensorEnableWind);

	apiReadSensor(&sensorTempValue, &sensorHumValue, &sensorWindValue);

	apiProcessInformation(sensorTempValue,
			sensorHumValue,
			sensorWindValue,
			bufferDataLog);

	apiWriteSD(_SYS_CFG_DATALOG_FILENAME, bufferDataLog);

	return 0;
}

/**
 * @brief	Configura el Período de Muestreo
 * @param	nada
 * @return	_API_STATE_OK si se configura correctamente
 * @note
 */
uint8_t appCofiguraPeriodoMuestreo(void){
	uint8_t inString[L_STRING_IN];
	tick_t valIngresado = _SYS_CFG_SAMPLINGTIME;

	consolePrintString("\n\rIngrese período de muestreo [de 1 a 3600 segundos]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );	//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);		//Convierte a entero

	if(valIngresado < 1){ valIngresado = 1; }			//Satura el valor leido
	else if(valIngresado > 3600){ valIngresado = 3600; }

	valIngresado *= 1000;								//Pasa a milisegundos
	tiempoMuestreo = valIngresado;						//Setea el valor
	return _API_STATE_OK;
}


/**
 * @brief	Configura la fecha y la hora
 * @param	nada
 * @return	1 si se configura correctamente
 * @note
 */
uint8_t appCofiguraRTC(void){
	uint8_t inString[L_STRING_IN];
	rtc_t rtc;				/* Estructura RTC */
	bool_t val = 0;
	int valIngresado;

	consolePrintString("\n\rIngrese fecha y hora: ");

	consolePrintString("\n\rAño[1 a 4095]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	if(valIngresado < 1){valIngresado = 1;}							//Satura el valor leido
	else if(valIngresado > 4095){valIngresado = 4095;}
	rtc.year = valIngresado;

	consolePrintString("\n\rMes[1 a 12]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	if(valIngresado < 1){valIngresado = 1;}							//Satura el valor leido
	else if(valIngresado > 12){valIngresado = 12;}
	rtc.month = valIngresado;

	consolePrintString("\n\rDia del mes[1 a 31]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	if(valIngresado < 1){valIngresado = 1;}							//Satura el valor leido
	else if(valIngresado > 31){valIngresado = 31;}
	rtc.mday = valIngresado;

	consolePrintString("\n\rDia de la semana[1 a 7]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	if(valIngresado < 1){valIngresado = 1;}							//Satura el valor leido
	else if(valIngresado > 7){valIngresado = 7;}
	rtc.wday = valIngresado;

	consolePrintString("\n\rHora[0 a 23]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	if(valIngresado < 1){valIngresado = 1;}							//Satura el valor leido
	else if(valIngresado > 23){valIngresado = 23;}
	rtc.hour = valIngresado;

	consolePrintString("\n\rMinutos[0 a 59]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	if(valIngresado < 1){valIngresado = 1;}							//Satura el valor leido
	else if(valIngresado > 59){valIngresado = 59;}
	rtc.min = valIngresado;

	consolePrintString("\n\rSegundos[0 a 59]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	if(valIngresado < 1){valIngresado = 1;}							//Satura el valor leido
	else if(valIngresado > 59){valIngresado = 59;}
	rtc.sec = valIngresado;

	val = rtcWrite( &rtc );											//Establece la Hora
	return val;
}

