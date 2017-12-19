/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

//#include "main.h"   // <= own header (optional)
#include "sapi.h"        // <= sAPI header
#include "cooperativeOs_isr.h"       // <= dispatcher and task management header
#include "cooperativeOs_scheduler.h" // <= scheduler and system initialization header

#include "api.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void iniciaTareas(void)  ;

/*==================[declaraciones de funciones externas]====================*/

bool_t TareaLeerSensores			( void* ptr );
bool_t TareaEnviarSensoresSd		( void* ptr );
bool_t TareaEnviarSensoresUart		( void* ptr );
bool_t TareaEscribirSensoresLcd		( void* ptr );
bool_t TareaMostrarTemperaturaEnLed	( void* ptr );
bool_t TareaEscribirSensoresBluetooth( void* ptr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){
   // ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma y tareas
   iniciaTareas();

   // FUNCION que inicializa el planificador de tareas
   schedulerInit();

   // Se agrega la tarea tarea1 al planificador
   schedulerAddTask( TareaLeerSensores, // funcion de tarea a a�adir
                     0,       // offset de ejecucion en ticks
                     2000     // periodicidad de ejecucion en ticks
                   );

   // Se agrega la tarea tarea2 al planificador
   schedulerAddTask( TareaEnviarSensoresSd, 10, 5000 );

   // Se agrega la tarea tarea3 al planificador
   schedulerAddTask( TareaEnviarSensoresUart, 20, 2000 );

   // Se agrega la tarea tarea4 al planificador
   schedulerAddTask( TareaEscribirSensoresLcd, 30, 2000 );

   // Se agrega la tarea tarea5 al planificador
   schedulerAddTask( TareaMostrarTemperaturaEnLed, 50, 2500 );

   // Se agrega la tarea tarea5 al planificador
   schedulerAddTask( TareaEscribirSensoresBluetooth, 60, 2500 );

   // FUNCION que inicializa la interrupcion que ejecuta el planificador de
   // tareas con tick cada 1ms.
   schedulerStart( 1 );

   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE )
   {
	  apiEscribirSensoresWifi();

      // Se despachan (ejecutan) las tareas marcadas para su ejecucion.
      // Luego se pone el sistema en bajo consumo hasta que ocurra la
      // proxima interrupcion, en este caso la de Tick.
      // Al ocurrir la interrupcion de Tick se ejecutara el planificador
      // que revisa cuales son las tareas a marcar para su ejecucion.
      schedulerDispatchTasks();
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/
/* FUNCION que inicializa las tareas. */
void iniciaTareas(void){
	apiConfigHardware();
	apiConfiguracion();
}

/*==================[definiciones de funciones externas]=====================*/
bool_t TareaLeerSensores( void* ptr ){
   apiReadSensor();
   return 0;
}

bool_t TareaEnviarSensoresSd( void* ptr ){
   //apiEnviarDatosASd();
   return 0;
}

bool_t TareaEnviarSensoresUart( void* ptr ){
   apiEnviarDatosAUart();
   return 0;
}

bool_t TareaEscribirSensoresLcd( void* ptr ){
   apiEscribirSensoresLcd();
   return 0;
}

bool_t TareaMostrarTemperaturaEnLed ( void* ptr ){
	apiMostrarTemperaturaEnLed();
	return 0;
}

bool_t TareaEscribirSensoresBluetooth ( void* ptr ){

	apiEscribirSensoresBluetooth();
	return 0;
}

/*==================[fin del archivo]========================================*/
