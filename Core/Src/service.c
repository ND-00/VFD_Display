/*
 * service.c
 *
 *  Created on: Aug 16, 2023
 *      Author: User
 */
#include "service.h"
#include "string.h"

int str_find(char* value, char* array, uint8_t num)
{
	char* index = array - 1;
	for(int i = 0; i < num; i++)
	{
		index = strstr(array + (index - array) + i, value);
		if(index == 0)
			return -1;
	}
	return index - array + strlen(value);
}

