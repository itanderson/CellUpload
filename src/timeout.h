/*
 * timeout.h
 *
 *  Created on: Jan 15, 2016
 *      Author: Isaac
 */

#ifndef TIMEOUT_H_
#define TIMEOUT_H_

#include <stdint.h>

extern volatile uint8_t timed_out;

void Timeout_Init(void);
void Timeout_Start(uint16_t t);

#endif /* TIMEOUT_H_ */
