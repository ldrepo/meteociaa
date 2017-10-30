/************* APLICACION **************/
#include "main.h"

#include "api.h"
#include "sapi.h"

CONSOLE_PRINT_ENABLE

/* configuración de mi aplicación */
#define _SYS_CFG_DATALOG_FILENAME 		"datalog.txt"
#define _SYS_CFG_SAMPLINGTIME			(1000)
#define _SYS_CFG_SENSOR_ENABLE_TEMP		(1)
#define _SYS_CFG_SENSOR_ENABLE_HUM		(1)
#define _SYS_CFG_SENSOR_ENABLE_WIND		(1)

bool_t sysCfgSensorEnableTemp 	= _SYS_CFG_SENSOR_ENABLE_TEMP;
bool_t sysCfgSensorEnableHum 	= _SYS_CFG_SENSOR_ENABLE_HUM;
bool_t sysCfgSensorEnableWind 	= _SYS_CFG_SENSOR_ENABLE_WIND;

uint8_t appConfiguracion			(void);
uint8_t appEstacionMeterorologica	(void);

/************* APLICACION **************/
int main( void ){

	boardConfig();
	uartConfig(UART_USB, 115200);
	adcConfig(ADC_ENABLE);
	spiConfig(SPI0);
	tickConfig(10, diskTickHook);

	appConfiguracion();

	while(TRUE) {
		delay(_SYS_CFG_SAMPLINGTIME);
		appEstacionMeterorologica();
	}

	return 0;
}

uint8_t appConfiguracion(void) {
	uint32_t todosLosValoresCargados = FALSE;
	uint8_t dataUart;

	consolePrintlnString("-------------------------------------\n");
	consolePrintlnString("  Bienvenido!!! Estacion Meteo INET  \n");
	consolePrintlnString("-------------------------------------\n");
	consolePrintlnString("Seleccione el sensor\n");
	consolePrintlnString("1 - Temperatura\n");
	consolePrintlnString("2 - Humedad\n");
	consolePrintlnString("3 - Viento\n");
	consolePrintlnString("0 - Salir\n");

	while(todosLosValoresCargados == FALSE){
		if(uartReadByte(UART_USB, &dataUart)){
			if(dataUart == '0' || dataUart == '1' || dataUart == '2' || dataUart == '3'){
				if(dataUart == '0'){
					todosLosValoresCargados = TRUE;
				} else if(dataUart == '1'){
					sysCfgSensorEnableTemp 	= !sysCfgSensorEnableTemp;
					consolePrintlnString("Temperatura :");
					if(sysCfgSensorEnableTemp){
						consolePrintlnString("SI\n");
					} else {
						consolePrintlnString("NO\n");
					}
				} else if(dataUart == '2'){
					sysCfgSensorEnableHum 	= !sysCfgSensorEnableHum;
					consolePrintlnString("Humedad :");
					if(sysCfgSensorEnableHum){
						consolePrintlnString("SI\n");
					} else {
						consolePrintlnString("NO\n");
					}
				} else if(dataUart == '3'){
					sysCfgSensorEnableWind 	= !sysCfgSensorEnableWind;
					consolePrintlnString("Viento :");
					if(sysCfgSensorEnableWind){
						consolePrintlnString("SI\n");
					} else {
						consolePrintlnString("NO\n");
					}
				}
			} else {
				// repetir el menu
				// dar un aviso de tecla no válida
			}
		}
	}
	consolePrintlnString("Configuracion terminada\n");
	consolePrintlnString("Fin\n");

	return 0;
}

uint8_t appEstacionMeterorologica(void) {
	uint16_t sensorTempValue, sensorHumValue, sensorWindValue = 0;
	char bufferDataLog[128];

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
