/*
 * dht11.h
 *
 *  Created on: Nov 13, 2017
 *      Author: Sebastian Pablo Bedin <sebabedin@gmail.com>
 */

#ifndef _DHT11_H_
#define _DHT11_H_

#include "sapi.h"

#define DHT11_GPIO	(GPIO3)

void 	dht11_Init		(void);
bool_t 	dht11_Read		(float *phum, float *ptemp);

#endif /* _DHT11_H_ */
