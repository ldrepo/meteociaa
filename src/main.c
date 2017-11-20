/************* APLICACION **************/
#include "main.h"

#include "api.h"
#include "sapi.h"
#include "uartstrings.h"
#include "sapi_esp8266HttpServer.h"
#include <stdlib.h>

/*==================[macros and definitions]=================================*/

CONSOLE_PRINT_ENABLE

/* Configuración de mi aplicación */
#define _SYS_CFG_DATALOG_FILENAME 		"datalog.txt"
#define _SYS_CFG_SAMPLINGTIME			(2000)
#define _SYS_CFG_SENSOR_ENABLE_TEMP		(1)
#define _SYS_CFG_SENSOR_ENABLE_HUM		(1)
#define _SYS_CFG_SENSOR_ENABLE_WIND		(1)

#define L_STRING_IN						(50)

// Se deben definir los datos del nombre de la red y la contrasenia.
//#define WIFI_NAME        "WiFi-Arnet-ucx1"
//#define WIFI_PASS        "NWVPHW7KWC"
#define WIFI_NAME        "AndroidAP"
#define WIFI_PASS        "12345678"

// El maximo tiempo que se espera una respuesta del modulo ESP8266
#define WIFI_MAX_DELAY   60000

/*==================[internal functions declaration]=========================*/
uint8_t appConfiguracion			(void);
uint8_t appConfiguracionMenu		(void);
uint8_t appEstacionMeterorologica	(void);
uint8_t appCofiguraRTC				(void);
uint8_t appCofiguraPeriodoMuestreo	(void);

/*==================[internal data definition]===============================*/
bool_t sysCfgSensorEnableTemp 	= _SYS_CFG_SENSOR_ENABLE_TEMP;
bool_t sysCfgSensorEnableHum 	= _SYS_CFG_SENSOR_ENABLE_HUM;
bool_t sysCfgSensorEnableWind 	= _SYS_CFG_SENSOR_ENABLE_WIND;
tick_t tiempoMuestreo			= _SYS_CFG_SAMPLINGTIME;
uint16_t sensorTempValue, sensorHumValue, sensorWindValue;
#include "web.h"		//Incluye arrays que contienen la web

/*==================[external data definition]===============================*/

/*==================[Funcion Principal]======================================*/
/************* APLICACION **************/
int main( void ){
	delay_t delayEstacion;
	uint8_t strRTCValue[20];
	uint8_t strTempValue[10];
	uint8_t strHumValue[10];
	uint8_t strWindValue[10];

	boardConfig();
	apiConfig();
	tickConfig(10, diskTickHook);

	appConfiguracion();
	delayConfig(&delayEstacion,tiempoMuestreo);

	/************* HTTP SERVER **************/
	bool_t error;
	delay_t wifiDelay;

	// Envia un mensaje de bienvenida.
	stdioPrintf(UART_USB, "\n\rLa configuracion del servidor HTTP Esp8266 con EDU CIAA puede tardar hasta 1 minuto.");

	error = FALSE;
	// Configura un delay para salir de la configuracion en caso de error.
	delayConfig(&wifiDelay, WIFI_MAX_DELAY);

	// Mientras no termine la configuracion o mientras no pase el tiempo maximo, ejecuta la configuracion.
	// A la configuracion se le pasa nombre y contrasenia de RED
	while (!esp8266ConfigHttpServer(WIFI_NAME, WIFI_PASS) && !error){
		if (delayRead(&wifiDelay)){
			error = TRUE;
		}
	}

	// Avisa al usuario como salio la configuracion
	if (!error){
		stdioPrintf(UART_USB, "\n\rServidor HTTP configurado. IP: %s", esp8266GetIpAddress());
		// Enciende LEDG indicando que el modulo esta configurado.
		//gpioWrite(LEDG, TRUE);
	} else {
		stdioPrintf(UART_USB, "\n\rError al configurar servidor HTTP.");
		// Enciende LEDR indicando que el modulo esta en error.
		//gpioWrite(LEDR, TRUE);
	}
	/****************************************/

	while(TRUE) {
		/************* HTTP SERVER **************/
		// Si llego una peticion al servidor http
		if (esp8266ReadHttpServer()){
			apiRTCToStr(strRTCValue);
			apiValToStr(sensorTempValue , strTempValue);
			apiValToStr(sensorHumValue  , strHumValue );
			apiValToStr(sensorWindValue , strWindValue);

			// Los datos a enviar a la web deben estar en formato HTML. Notar que
			// BEGIN_USER_LINE y END_USER_LINE solo formatean la cadena a enviar con tags HTML.
			//stdioSprintf(HttpWebPageBody, "%s VALOR CONTADOR: %d - VALOR ADC: %d %s", BEGIN_USER_LINE, counter, 3+counter*2, END_USER_LINE);
			stdioSprintf(HttpWebPageBody,
				"<caption>Hora de toma de datos: %s actualizados autom&aacute;ticamente cada 15 segundos</caption><tbody>"
				"<tr><td><strong>Magnitud<br /></strong></td><td><strong>Valor</strong></td><td><strong>Unidad</strong></td></tr>"
				"<tr><td>Temperatura</td><td>%s</td><td>&deg;C</td></tr>"
				"<tr><td>Humedad</td><td>%s</td><td>%%RH</td></tr>"
				"<tr><td>Velocidad del viento</td><td>%s</td><td>Km/h</td></tr>"
				,strRTCValue, strTempValue, strHumValue, strWindValue);

			error = FALSE;
			// Configura un delay para salir de la configuracion en caso de error.
			delayConfig(&wifiDelay, WIFI_MAX_DELAY);

			// Mientras no termine el envio o mientras no pase el tiempo maximo, ejecuta el envio.
			while (!esp8266WriteHttpServer(HttpWebPageHeader, HttpWebPageBody, HttpWebPageEnd) && !error){
				if (delayRead(&wifiDelay)){
					error = TRUE;
				}
			}

			// Avisa al usuario como fue el envio
			if (!error){
				stdioPrintf(UART_USB, "\n\rPeticion respondida al cliente HTTP %d. OK", esp8266GetConnectionId());
				//gpioToggle(LEDG);
			} else {
				stdioPrintf(UART_USB, "\n\rPeticion respondida al cliente HTTP %d. ERROR", esp8266GetConnectionId());
				//gpioToggle(LEDR);
			}
		}
		/****************************************/
		//Si se cumple el período de muestreo
		if( delayRead( &delayEstacion )){
			appEstacionMeterorologica();
		}
	}

	return 0;
}

/*==================[internal functions definition]==========================*/
uint8_t appConfiguracion(void) {
	uint32_t todosLosValoresCargados = FALSE;
	uint8_t dataUart;

	apiInit_rtc();

	appConfiguracionMenu();

	while(todosLosValoresCargados == FALSE){
		if(uartReadByte(UART_USB, &dataUart)){
			uartWriteByte(UART_USB, dataUart );		//Imprime el ECO
			switch(dataUart){
			case '0':
				todosLosValoresCargados = TRUE;
				break;

			case '1':
				sysCfgSensorEnableTemp 	= !sysCfgSensorEnableTemp;
				break;

			case '2':
				sysCfgSensorEnableHum 	= !sysCfgSensorEnableHum;
				break;

			case '3':
				sysCfgSensorEnableWind 	= !sysCfgSensorEnableWind;
				break;

			case '4':
				appCofiguraPeriodoMuestreo();
				break;

			case '5':
				appCofiguraRTC();
				break;
			default:
				consolePrintlnString("\n\rOpción Inválida");
				delay(1000);
			}
			appConfiguracionMenu();
		}
	}
	consolePrintlnString("\n\rConfiguracion terminada");

	return 0;
}

uint8_t appConfiguracionMenu(void){
	delay(100);
	uartWriteByte  ( UART_USB, 27 );
	uartWriteString( UART_USB, "[2J" );
	uartWriteByte  ( UART_USB, 27 );
	uartWriteString( UART_USB, "[H" );
	delay(10);

	consolePrintlnString("\n\r  *** Bienvenido!!! Estacion METEO-CIAA INET ***");
	consolePrintString("1 - Temperatura, log activo: ");
	if(sysCfgSensorEnableTemp){
		consolePrintlnString("SI");
	} else {
		consolePrintlnString("NO");
	}
	consolePrintString("2 - Humedad, log activo: ");
	if(sysCfgSensorEnableHum){
		consolePrintlnString("SI");
	} else {
		consolePrintlnString("NO");
	}
	consolePrintString("3 - Viento, log activo: ");
	if(sysCfgSensorEnableWind){
		consolePrintlnString("SI");
	} else {
		consolePrintlnString("NO");
	}
	stdioPrintf(UART_USB, "4 - Período de muestreo: %d", tiempoMuestreo/1000);
	consolePrintlnString(" Seg");
	consolePrintlnString("5 - Fecha y Hora");
	consolePrintlnString("0 - Salir");
	consolePrintlnString("Seleccione una opcion: ");
	return 0;
}

uint8_t appEstacionMeterorologica(void) {
//	uint16_t sensorTempValue, sensorHumValue, sensorWindValue;
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

	consolePrintString("\n\rIngrese período de muestreo [de 2 a 3600 segundos]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );		//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);			//Convierte a entero
	valIngresado = apiSaturaEntero(valIngresado, 2, 3600);	//Satura el valor leido
	valIngresado *= 1000;									//Pasa a milisegundos
	tiempoMuestreo = valIngresado;							//Setea el valor
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
	rtc.year = apiSaturaEntero(valIngresado, 1, 4095);				//Satura el valor leido

	consolePrintString("\n\rMes[1 a 12]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.month = apiSaturaEntero(valIngresado, 1, 12);				//Satura el valor leido

	consolePrintString("\n\rDia del mes[1 a 31]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.mday = apiSaturaEntero(valIngresado, 1, 31);				//Satura el valor leido

	consolePrintString("\n\rDia de la semana[1 a 7]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.wday = apiSaturaEntero(valIngresado, 1, 7);					//Satura el valor leido

	consolePrintString("\n\rHora[0 a 23]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.hour = apiSaturaEntero(valIngresado, 1, 23);				//Satura el valor leido

	consolePrintString("\n\rMinutos[0 a 59]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.min = apiSaturaEntero(valIngresado, 0, 59);					//Satura el valor leido

	consolePrintString("\n\rSegundos[0 a 59]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.sec = apiSaturaEntero(valIngresado, 0, 59);					//Satura el valor leido

	val = rtcWrite( &rtc );											//Establece la Hora
	return val;
}

/*==================[external functions definition]==========================*/
