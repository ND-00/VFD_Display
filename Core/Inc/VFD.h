/*
 * VFD.h
 *
 *  Created on: Jun 19, 2023
 *      Author: Nick
 */
#ifndef INC_VFD_H_
#define INC_VFD_H_

#include "stdio.h"
#include "stm32f4xx.h"

#define BIT_CLK		0x01
#define BIT_BK		0x02
#define BIT_LAT		0x04
#define BIT_SIN		0x08

#define LINE_0		0x01
#define LINE_1		0x00

void init_array ();
void vfd_print (char* arr, uint8_t line, uint8_t pos);
uint8_t vfd_print_symbol (uint8_t sym, uint8_t line, uint8_t offset, uint8_t amount);
void vfd_clear ();
void vfd_clear_line (uint8_t line);
void vfd_display_error (uint8_t line);

#endif /* INC_VFD_H_ */
