/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

/*==================[inlcusiones]============================================*/
#include "api.h"		// <= own header (optional)
#include "sapi.h"       // <= sAPI header

#include <stdlib.h>
#include "uartstrings.h"
#include "apiSD.h"

#include "lcd_custchars.h"
#include "web.h"
#include "sapi_esp8266.h"

/*==================[definiciones y macros]==================================*/
//CONSOLE_PRINT_ENABLE
/* Configuración de mi aplicación */
#define _SYS_CFG_DATALOG_FILENAME 		"datalog.txt"
#define _SYS_CFG_SAMPLINGTIME			(2000)
#define _SYS_CFG_SENSOR_ENABLE_TEMP		(1)
#define _SYS_CFG_SENSOR_ENABLE_HUM		(1)
#define _SYS_CFG_SENSOR_ENABLE_WIND		(1)

#define L_STRING_IN						(50)

// Se deben definir los datos del nombre de la red y la contrasenia.
#define WIFI_NAME        "AndroidAP"
#define WIFI_PASS        "12345678"
// El maximo tiempo que se espera una respuesta del modulo ESP8266
#define WIFI_MAX_DELAY 60000

/*==================[definiciones de datos internos]=========================*/

static delay_t      appDelayTMuestreo;
static uartMap_t	appUart = UART_USB;
static uint32_t	    appUartBaudRate = 115200;
static uartMap_t	btUart = UART_GPIO;
static uint32_t	    btUartBaudRate = 9600;

static char 		bufferDataLog[128];

static bool_t 		sysCfgSensorEnableTemp 	= _SYS_CFG_SENSOR_ENABLE_TEMP;
static bool_t 		sysCfgSensorEnableHum 	= _SYS_CFG_SENSOR_ENABLE_HUM;
static bool_t 		sysCfgSensorEnableWind 	= _SYS_CFG_SENSOR_ENABLE_WIND;
static tick_t 		tiempoMuestreo			= _SYS_CFG_SAMPLINGTIME;

static uint16_t 	sensorTempValue, sensorHumValue, sensorWindValue;

static bool_t enableTemp 	= 1;
static bool_t enableHum 	= 1;
static bool_t enableWind 	= 1;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
static uint8_t 	apiReadTemperatureHumdity		(uint16_t * dataTemp, uint16_t * dataHum);
static uint8_t 	apiReadWind						(uint16_t * dataWind);
static uint8_t 	apiValToStr						(uint16_t valor, char * destiny);
static uint8_t 	apiRTCToStr						(char * destiny);
static int 		apiSaturaEntero					(int dato, int limInf, int limSup);
static uint8_t 	apiProcessInformation			(uint16_t dataTemp, uint16_t dataHum, uint16_t dataWind, char * destiny);
static uint8_t 	apiWriteSD						(char * filename, uint8_t * stringData);
static uint8_t 	apiInit_rtc						(void);
static uint8_t 	apiConfig_SensorTemp_Enable		(bool_t flagEnable);
static uint8_t 	apiConfig_SensorHum_Enable		(bool_t flagEnable);
static uint8_t 	apiConfig_SensorWind_Enable		(bool_t flagEnable);
static uint8_t 	appCofiguraRTC					(void);
static uint8_t 	apiEnviarMensajeBienvenida		(void);
static uint8_t 	appCofiguraPeriodoMuestreo		(void);
static uint8_t	apiConfigurarSensores			(void);
static uint8_t 	apiHabilitarSensores			(void);
static void 	FormatInformationArray			(uint16_t valor, char * destiny, uint8_t pos);
static void 	apiConfigServer					(void);
static void 	apiMostrarSensoresEnLeds 		(void);
/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/
static uint8_t apiReadTemperatureHumdity(uint16_t * dataTemp, uint16_t * dataHum) {
	float hum, temp;
	uint8_t valret;

	if(TRUE == dht11Read(&hum, &temp)){
		(*dataTemp) = (uint16_t) (temp*10.0f+0.5f);	//Se pasa a deceimas redondeadas
		(*dataHum)  = (uint16_t) (hum *10.0f+0.5f);	//
		valret = _API_STATE_OK;
	}
	else{
		valret = _API_STATE_ERROR;
	}
	return valret;
}

static uint8_t apiReadWind(uint16_t * dataWind) {
	uint16_t adcValue3 = 0;

	adcValue3 = adcRead(CH3); // viento
	(*dataWind) = adcValue3;

	return _API_STATE_OK;
}

static void FormatInformationArray(uint16_t valor, char * destiny, uint8_t pos){
	destiny[pos++] = (valor/1000) 		+ '0';
	destiny[pos++] = (valor%1000)/100 	+ '0';
	destiny[pos++] = (valor%100)/10 	+ '0';
	destiny[pos++] = '.';
	destiny[pos]   = (valor%10) 		+ '0';
}

static uint8_t apiValToStr(uint16_t valor, char * destiny){
	FormatInformationArray(valor, destiny, 0);
	destiny[5]='\0';
	return _API_STATE_OK;
}

static uint8_t apiRTCToStr(char * destiny){
	rtc_t rtc;				/* Estructura RTC */

	rtcRead( &rtc );
	destiny[0] = (rtc.year/1000) + '0';
	destiny[1] = ((rtc.year%1000)/100) + '0';
	destiny[2] = ((rtc.year%100)/10) + '0';
	destiny[3] = (rtc.year%10) + '0';

	destiny[4] = (rtc.month/10) + '0';
	destiny[5] = (rtc.month%10) + '0';

	destiny[6] = (rtc.mday/10) + '0';
	destiny[7] = (rtc.mday%10) + '0';
	destiny[8] = '_';
	destiny[9] = (rtc.hour/10) + '0';
	destiny[10] = (rtc.hour%10) + '0';
	destiny[11] = ':';
	destiny[12] = (rtc.min/10) + '0';
	destiny[13] = (rtc.min%10) + '0';
	destiny[14] = ':';
	destiny[15] = (rtc.sec/10) + '0';
	destiny[16] = (rtc.sec%10) + '0';
	destiny[17]   = '\0';

	return _API_STATE_OK;
}

static int apiSaturaEntero(int dato, int limInf, int limSup){
	if(dato < limInf){
		dato = limInf;
	}
	else if(dato > limSup){
		dato = limInf;
	}
	return dato;
}

static uint8_t apiProcessInformation(uint16_t dataTemp, uint16_t dataHum, uint16_t dataWind, char * destiny) {
	uint8_t pos;
	rtc_t rtc;				/* Estructura RTC */
	//bool_t val;

    //val = rtcRead( &rtc );
	rtcRead( &rtc );
	/*
	 * Recibir 3 datos enteros X, Y, Z
	 * posiciones	= 01234 5 67890 1 23456 7 89012345678901234567 8  9
	 * formato 		= XXX.X ; YYY.Y ; ZZZ.Z ; 2016/12/10_20:04:36;\r \n \0
	 * formato 		= XXXX ;  ;  ; \r \n \0
	 * x;y;z;time-stamp;
	 * 154;0;-25;2016/12/10_20:04:36;
	 */
	pos = 0;
	if(enableTemp) {
		FormatInformationArray(dataTemp, destiny, pos);
		pos += 5;
	}
	destiny[pos++] = ';';

	if(enableHum) {
		FormatInformationArray(dataHum, destiny, pos);
		pos += 5;
	}
	destiny[pos++] = ';';

	if(enableWind) {
		FormatInformationArray(dataWind, destiny, pos);
		pos += 5;
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

static uint8_t apiWriteSD(char * filename, uint8_t * stringData) {

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

static uint8_t apiInit_rtc(void) {
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

static uint8_t apiConfig_SensorTemp_Enable(bool_t flagEnable) {
	enableTemp = flagEnable;
	return _API_STATE_OK;
}

static uint8_t apiConfig_SensorHum_Enable(bool_t flagEnable) {
	enableHum = flagEnable;
	return _API_STATE_OK;
}

static uint8_t apiConfig_SensorWind_Enable(bool_t flagEnable) {
	enableWind = flagEnable;
	return _API_STATE_OK;
}

static uint8_t apiHabilitarSensores(void){
	apiConfig_SensorTemp_Enable(sysCfgSensorEnableTemp);
	apiConfig_SensorHum_Enable(sysCfgSensorEnableHum);
	apiConfig_SensorWind_Enable(sysCfgSensorEnableWind);
	return 0;
}

static uint8_t apiConfigurarSensores(void){
	uint32_t todosLosValoresCargados = FALSE;
	uint8_t dataUart;

	apiMostrarSensoresEnLeds ();
	apiEnviarMensajeBienvenida();
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
				stdioPrintf(UART_USB, "\n\rOpción Inválida");
				delay(1000);
			}
			apiMostrarSensoresEnLeds ();
			delay(100);
			uartWriteByte  ( UART_USB, 27 );
			uartWriteString( UART_USB, "[2J" );
			uartWriteByte  ( UART_USB, 27 );
			uartWriteString( UART_USB, "[H" );
			delay(10);
			apiEnviarMensajeBienvenida();
		}
	}
	stdioPrintf(UART_USB, "\n\rConfiguracion terminada\n\r");
	return 0;
}

static uint8_t apiEnviarMensajeBienvenida(void){
	stdioPrintf(appUart, "\n\r  *** Bienvenido!!! Estacion METEO-CIAA INET ***\n\r");
	stdioPrintf(UART_USB, "1 - Temperatura, log activo: ");
	if(sysCfgSensorEnableTemp){
		stdioPrintf(UART_USB, "SI\n\r");
	} else {
		stdioPrintf(UART_USB, "NO\n\r");
	}
	stdioPrintf(UART_USB, "2 - Humedad, log activo: ");
	if(sysCfgSensorEnableHum){
		stdioPrintf(UART_USB, "SI\n\r");
	} else {
		stdioPrintf(UART_USB, "NO\n\r");
	}
	stdioPrintf(UART_USB, "3 - Viento, log activo: ");
	if(sysCfgSensorEnableWind){
		stdioPrintf(UART_USB, "SI\n\r");
	} else {
		stdioPrintf(UART_USB, "NO\n\r");
	}
	stdioPrintf(UART_USB, "4 - Período de muestreo: %d", tiempoMuestreo/1000);
	stdioPrintf(UART_USB, " Seg\n\r");
	stdioPrintf(UART_USB, "5 - Fecha y Hora\n\r");
	stdioPrintf(UART_USB, "0 - Salir\n\r");
	stdioPrintf(UART_USB, "Seleccione una opcion: \n\r");
	return 0;
}

/**
 * @brief	Configura el Período de Muestreo
 * @param	nada
 * @return	_API_STATE_OK si se configura correctamente
 * @note
 */
static uint8_t appCofiguraPeriodoMuestreo(void){
	uint8_t inString[L_STRING_IN];
	tick_t valIngresado = _SYS_CFG_SAMPLINGTIME;

	stdioPrintf(UART_USB, "\n\rIngrese período de muestreo [de 2 a 3600 segundos]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );		//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);			//Convierte a entero
	valIngresado = apiSaturaEntero(valIngresado, 2, 3600);	//Satura el valor leido
	valIngresado *= 1000;									//Pasa a milisegundos
	tiempoMuestreo = valIngresado;							//Setea el valor
	delayConfig( &appDelayTMuestreo, valIngresado);
	return _API_STATE_OK;
}


/**
 * @brief	Configura la fecha y la hora
 * @param	nada
 * @return	1 si se configura correctamente
 * @note
 */
static uint8_t appCofiguraRTC(void){
	uint8_t inString[L_STRING_IN];
	rtc_t rtc;				/* Estructura RTC */
	bool_t val = 0;
	int valIngresado;

	stdioPrintf(UART_USB, "\n\rIngrese fecha y hora: ");

	stdioPrintf(UART_USB, "\n\rAño[1 a 4095]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.year = apiSaturaEntero(valIngresado, 1, 4095);				//Satura el valor leido

	stdioPrintf(UART_USB, "\n\rMes[1 a 12]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.month = apiSaturaEntero(valIngresado, 1, 12);				//Satura el valor leido

	stdioPrintf(UART_USB, "\n\rDia del mes[1 a 31]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.mday = apiSaturaEntero(valIngresado, 1, 31);				//Satura el valor leido

	stdioPrintf(UART_USB, "\n\rDia de la semana[1 a 7]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.wday = apiSaturaEntero(valIngresado, 1, 7);					//Satura el valor leido

	stdioPrintf(UART_USB, "\n\rHora[0 a 23]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.hour = apiSaturaEntero(valIngresado, 1, 23);				//Satura el valor leido

	stdioPrintf(UART_USB, "\n\rMinutos[0 a 59]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.min = apiSaturaEntero(valIngresado, 0, 59);					//Satura el valor leido

	stdioPrintf(UART_USB, "\n\rSegundos[0 a 59]: ");
	uartReadString( UART_USB, inString, L_STRING_IN );				//Lee cadena de la UART
	valIngresado = atoi((const char *)inString);					//Convierte a entero
	rtc.sec = apiSaturaEntero(valIngresado, 0, 59);					//Satura el valor leido

	val = rtcWrite( &rtc );											//Establece la Hora
	return val;
}

static void apiConfigServer(void){
	bool_t error;
	delay_t wifiDelay;

   // Envia un mensaje de bienvenida.
   stdioPrintf(UART_USB, "\n\rBienvenido al servidor HTTP Esp8266 con EDU CIAA");
   stdioPrintf(UART_USB, "\n\rLa configuracion puede tardar hasta 1 minuto.");

   error = FALSE;
   // Configura un delay para salir de la configuracion en caso de error.
   delayConfig(&wifiDelay, WIFI_MAX_DELAY);

   // Mientras no termine la configuracion o mientras no pase el tiempo maximo, ejecuta la configuracion.
   // A la configuracion se le pasa nombre y contrasenia de RED
   // En caso de querer ver los mensajes que se envian/reciben desde el modulo
   // se debe tambien mandar la UART por donde van a salir esos datos. Ejemplo:
   // esp8266ConfigHttpServer(WIFI_NAME, WIFI_PASS, UART_USB, 115200);
   while (!esp8266ConfigHttpServer(WIFI_NAME, WIFI_PASS, 0, 0) && !error){
      if (delayRead(&wifiDelay)){
         error = TRUE;
      }
   }

   // Avisa al usuario como salio la configuracion
   if (!error){
      stdioPrintf(UART_USB, "\n\rServidor HTTP configurado. IP: %s", esp8266GetIpAddress());
      // Enciende LEDG indicando que el modulo esta configurado.
      gpioWrite(LEDG, TRUE);
   } else {
      stdioPrintf(UART_USB, "\n\rError al configurar servidor HTTP");
      // Enciende LEDR indicando que el modulo esta en error.
      gpioWrite(LEDR, TRUE);
   }
}

void apiMostrarSensoresEnLeds (void){
	gpioWrite(LED1, sysCfgSensorEnableTemp);
	gpioWrite(LED2, sysCfgSensorEnableHum);
	gpioWrite(LED3, sysCfgSensorEnableWind);
}



/*==================[definiciones de funciones externas]=====================*/
uint8_t apiConfigHardware(void){
	boardConfig();
	uartConfig(appUart, appUartBaudRate);
	uartConfig(btUart, btUartBaudRate);
	adcConfig(ADC_ENABLE);

	lcdInit( 16, 2, 5, 8 );
	delay(100);					//Provisorio hasta repara sapi_lcd
	lcdClear(); 				// Borrar la pantalla
	delay(100);					//Provisorio hasta repara sapi_lcd
	//lcdGoToXY( 1, 1 ); 			// Poner cursor en 1, 1
	lcdSendStringRaw("** METEO_CIAA **");
	delay(100);					//Provisorio hasta repara sapi_lcd
	lcdGoToXY( 1, 2 ); 			// Poner cursor en 2, 1
	delay(100);					//Provisorio hasta repara sapi_lcd
	lcdSendStringRaw("Iniciando . . ." );

	dht11Config();
	spiConfig(SPI0);
	//tickConfig(10, diskTickHook);
	apiInit_rtc();

	// Cargar el caracter a CGRAM
	// El primer parï¿½metro es el cï¿½digo del caracter (0 a 7).
	// El segundo es el puntero donde se guarda el bitmap (el array declarado anteriormente)
	lcdCreateChar( 0, lcdSymbol0 );
	lcdCreateChar( 1, lcdSymbol1 );
	lcdCreateChar( 2, lcdSymbol2 );
	lcdCreateChar( 3, lcdSymbol3 );
	lcdCreateChar( 4, lcdSymbol4 );

	lcdClear(); 				// Borrar la pantalla
	delay(100);					//Provisorio hasta repara sapi_lcd
	lcdGoToXY( 1, 1 ); 		// Poner cursor en 1, 1
	delay(100);					//Provisorio hasta repara sapi_lcd
	lcdSendStringRaw( "Temp" );
    lcdData(0);
	lcdSendStringRaw( " Hum" );
    lcdData(1);
    lcdSendStringRaw( "  Vie" );
    lcdData(2);

    apiConfigServer();

    delay(500);

	return 0;
}

uint8_t apiConfiguracion (void){
	//apiEnviarMensajeBienvenida();
	apiConfigurarSensores();
	apiHabilitarSensores();
	return 0;
}

uint8_t apiReadSensor(void) {
	apiReadTemperatureHumdity(&sensorTempValue, &sensorHumValue);
	apiReadWind(&sensorWindValue);
	return _API_STATE_OK;
}

uint8_t apiEnviarDatosASd(void) {
	apiProcessInformation(sensorTempValue,
			sensorHumValue,
			sensorWindValue,
			bufferDataLog);
	apiWriteSD(_SYS_CFG_DATALOG_FILENAME, (uint8_t *)bufferDataLog);
	return 0;
}

uint8_t apiEnviarDatosAUart(void) {
	apiProcessInformation(sensorTempValue,
			sensorHumValue,
			sensorWindValue,
			bufferDataLog);
	stdioPrintf(UART_USB, (char*)bufferDataLog);
	stdioPrintf(UART_USB, "\n\r");
	return 0;
}

uint8_t apiValoresToStr(char * strRTCValue,char * strTempValue,char * strHumValue,char * strWindValue){
	apiRTCToStr(strRTCValue);
	apiValToStr(sensorTempValue , strTempValue);
	apiValToStr(sensorHumValue  , strHumValue );
	apiValToStr(sensorWindValue , strWindValue);
	return _API_STATE_OK;
}

uint8_t apiEscribirSensoresLcd(void){
	char tempStr[10];
//	//lcdClear(); 				// Borrar la pantalla
//	lcdGoToXY( 1, 1 ); 		// Poner cursor en 1, 1
//	lcdSendStringRaw( "Temp  Hum   Vie " );
	lcdGoToXY( 1, 2 );  		// Poner cursor en 1, 2
	apiValToStr(sensorTempValue , tempStr);
	lcdSendStringRaw( tempStr );
	lcdGoToXY( 7, 2 );  		// Poner cursor en 7, 2
	apiValToStr(sensorHumValue , tempStr);
	lcdSendStringRaw( tempStr );
	lcdGoToXY( 13, 2 ); 		// Poner cursor en 13, 2
	apiValToStr(sensorWindValue , tempStr);
	lcdSendStringRaw( tempStr );
	return 0;
}

uint8_t apiMostrarTemperaturaEnLed(void){
	if (sensorTempValue < 200){
		gpioWrite(LEDR, OFF);
		gpioWrite(LEDG, OFF);
		gpioWrite(LEDB, ON);
	} else if (sensorTempValue < 270){
		gpioWrite(LEDR, OFF);
		gpioWrite(LEDG, ON);
		gpioWrite(LEDB, OFF);
	} else {
		gpioWrite(LEDR, ON);
		gpioWrite(LEDG, OFF);
		gpioWrite(LEDB, OFF);
	}
	return 0;
}

uint8_t apiEscribirSensoresWifi(void){
	bool_t error;
	delay_t wifiDelay;
	char strRTCValue[20];
	char strTempValue[10];
	char strHumValue[10];
	char strWindValue[10];

	// Si llego una peticion al servidor http
	if (esp8266ReadHttpServer()){
		apiRTCToStr(strRTCValue);
		apiValToStr(sensorTempValue , strTempValue);
		apiValToStr(sensorHumValue  , strHumValue );
		apiValToStr(sensorWindValue , strWindValue);
		//Formatea el arreglo HttpWebPageBody con datos HTML y los valores de los sensores
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
			stdioPrintf(UART_USB, "\n\rPeticion respondida al cliente HTTP %d.", esp8266GetConnectionId());
			gpioToggle(LEDG);
		} else {
			stdioPrintf(UART_USB, "\n\rPeticion respondida al cliente HTTP %d.", esp8266GetConnectionId());
			gpioToggle(LEDR);
		}
	}
	return 0;
}
uint8_t apiEscribirSensoresBluetooth(void){
//	apiProcessInformation(sensorTempValue,
//			sensorHumValue,
//			sensorWindValue,
//			bufferDataLog);
	uint8_t pos;
	char * destiny;
	/*
	 * Recibir 3 datos enteros X, Y, Z
	 * posiciones	= 01234 5 67890 1 23456 7 89012345678901234567 8  9
	 * formato 		= *TXXX.X*HYYY.Y*VZZZ.Z ; 2016/12/10_20:04:36;\r \n \
	 * formato 		= XXXX ;  ;  ; \r \n \0
	 * x;y;z;time-stamp;
	 * 154;0;-25;2016/12/10_20:04:36;
	 */
	pos = 0;
	destiny = bufferDataLog;
	destiny[pos++] = '*';
	destiny[pos++] = 'T';
	if(enableTemp) {
		FormatInformationArray(sensorTempValue, destiny, pos);
		pos += 5;
	}
	destiny[pos++] = '*';
	destiny[pos++] = 'H';
	if(enableHum) {
		FormatInformationArray(sensorHumValue, destiny, pos);
		pos += 5;
	}
	destiny[pos++] = '*';
	destiny[pos++] = 'V';
	if(enableWind) {
		FormatInformationArray(sensorWindValue, destiny, pos);
		pos += 5;
	}

	stdioPrintf(UART_GPIO, (char*)bufferDataLog);
	return 0;
}

/*==================[fin del archivo]========================================*/
