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

#ifndef _FATFS_STUBS_H_
#define _FATFS_STUBS_H_

/*==================[inclusions]=============================================*/

#include "sapi.h"
#include "ff.h"       // <= Biblioteca FAT FS

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*
 * Comentar o descomentar algunas de estas definiciones si se cuenta
 * o no se cuenta con la memoria SD.
 */

//#define SD_ENABLE
#define SD_DISABLE

#define UART_SD_EMULATED	UART_USB
#define UART_SD_BAUDRATE	115200

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*
 * Todas las funciones declaradas en este archivo son stubs de la biblioteca
 * fatfs. Se utilizan estas funciones para emular la escritura de datos en
 * una memoria SD a traves de la UART. En caso de contar con la memoria SD,
 * lo unico que se debe hacer es comentar #define SD_DISABLE y descomentar
 * #define SD_ENABLE.
 * Unicamente realizando estas acciones ya se va a poder escribir en la
 * memoria SD en vez de por la UART.
 */

uint32_t    f_mount_ ( FATFS *fileSystem, 	char * alternativeText, uint32_t mountParameters);
uint32_t    f_open_  ( FIL   *file, 		char * fileName, 		uint32_t openParameters);
void        f_write_ ( FIL   *file, 		char * messageToWrite, 	uint32_t bytesToWrite, 		uint32_t * bytesWritten);
void        f_close_ ( FIL   *file);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _FATFS_STUBS_H_ */
