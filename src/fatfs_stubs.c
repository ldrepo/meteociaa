/* Copyright 2017, Agustin Bassi
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inlcusiones]============================================*/

#include "fatfs_stubs.h"       // <= Biblioteca FAT FS

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

static char FileName [50];

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

static void SaveFileName (char * fileName);

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/

static void SaveFileName (char * fileName){
char * pointerToFile;
uint8_t index = 0;

	pointerToFile = fileName;
	while (*pointerToFile != '\0'){
		FileName [index] = *pointerToFile;
		index++;
		pointerToFile++;
	}
	FileName [index] = '\0';
}

/*==================[definiciones de funciones externas]=====================*/

uint32_t    f_mount_ ( FATFS *fileSystem, 	char * alternativeText, uint32_t mountParameters){
#ifdef SD_DISABLE
	uartConfig (UART_SD_EMULATED, UART_SD_BAUDRATE);
    uartWriteString(UART_SD_EMULATED, "\n\r[DEB]Montando el file system.\n\r");
    return FR_OK;
#else
    return f_mount( fileSystem, "", 0 );
#endif
}

uint32_t    f_open_  ( FIL   *file, 		char * fileName, 		uint32_t openParameters){
#ifdef SD_DISABLE
uint8_t counter;

	SaveFileName(fileName);
	uartWriteString(UART_SD_EMULATED, "[DEB]Abriendo archivo: ");
	uartWriteString(UART_SD_EMULATED, FileName);
	uartWriteString(UART_SD_EMULATED, "\n\r");
	if (openParameters == (FA_WRITE | FA_CREATE_ALWAYS)){
		for (counter = 0; counter < 10; counter++){
			uartWriteString(UART_SD_EMULATED, "\n\r");
		}
	} else if (openParameters == (FA_WRITE | FA_OPEN_APPEND)){
		uartWriteString(UART_SD_EMULATED, "\n\r");
	}

	return FR_OK;
#else
	return f_open( file, fileName, openParameters);
#endif

}

void        f_write_ ( FIL   *file, 		char * messageToWrite, 	uint32_t bytesToWrite, 		uint32_t * bytesWritten){
#ifdef SD_DISABLE
	uint8_t counter;
	char * messagePointer = messageToWrite;

	    *bytesWritten = bytesToWrite;
	    for (counter = 0; counter < bytesToWrite && *messagePointer != '\0'; counter++, messagePointer++){
	        uartWriteByte(UART_SD_EMULATED, *messagePointer);
	    }
#else
	    f_write( file, messageToWrite, bytesToWrite, bytesWritten );
#endif
}


void        f_close_ ( FIL   *file){
#ifdef SD_DISABLE
	uartWriteString(UART_SD_EMULATED, "[DEB]Cerrando archivo: ");
	uartWriteString(UART_SD_EMULATED, FileName);
	uartWriteString(UART_SD_EMULATED, "\n\r");
#else
	f_close (file);
#endif
}


/*==================[fin del archivo]========================================*/
