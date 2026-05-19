/*
 * service.h
 *
 *  Created on: Aug 16, 2023
 *      Author: User
 */

#ifndef INC_SERVICE_H_
#define INC_SERVICE_H_

#include <stdint.h>
#include "time.h"

struct weather_t {
	time_t time;
	int16_t temp;
	uint8_t ico_id;
};

int str_find(char* value, char* array, uint8_t num);



#endif /* INC_SERVICE_H_ */
