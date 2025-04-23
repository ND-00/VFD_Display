/*
 * VFD.c
 *
 *  Created on: Jun 19, 2023
 *      Author: Nick
 */
#include "VFD.h"

extern const uint8_t symbol[256][7];

uint8_t  output_screen_buff_bin [20][198];
volatile uint16_t cntr = 0;

void init_array ()
{
  for (uint8_t c = 0; c < 20 ; c++)
    for (int i = 6; i < 198; i++)
      output_screen_buff_bin[c][i] = 0x00;

  for (uint8_t c = 0; c < 20 ; c++)
    for (int i = 6; i < 198; i++)
      if((i & 1) == 0)
    	  output_screen_buff_bin[c][i] |= BIT_CLK;

  for (uint8_t i = 0; i < 20; i++)
  {
    output_screen_buff_bin[i][0] |= BIT_BK;
    output_screen_buff_bin[i][1] |= BIT_BK;
    output_screen_buff_bin[i][2] |= BIT_BK | BIT_LAT;
    output_screen_buff_bin[i][3] |= BIT_BK | BIT_LAT;
    output_screen_buff_bin[i][4] |= BIT_BK;
  }
  for (uint8_t i = 0; i < 20; i++)
  {
    output_screen_buff_bin[i][13 + (2 * i)] |= BIT_SIN;
    output_screen_buff_bin[i][14 + (2 * i)] |= BIT_SIN;
  }
}

void vfd_clear ()
{
	for (uint8_t i = 0; i < 20; i++)
		for (int j = 57; j < 198; j++)
			output_screen_buff_bin[i][j] &= ~BIT_SIN;
}

void vfd_clear_line (uint8_t line)
{
	line = !line;
	for(int8_t count = 19; count >= 0; count--)
	{
		for (uint8_t w = 0; w < 35; w++)
		{
			output_screen_buff_bin[count][(w * 2) + (57 + (70 * line))] &= ~BIT_SIN;
			output_screen_buff_bin[count][(w * 2) + (58 + (70 * line))] &= ~BIT_SIN;
		}
	}
}


void vfd_print (char* arr, uint8_t line, uint8_t offset) {
	uint8_t i = 0, w = 0, count = 0;
	line = !line;
	while ((arr[i] != '\0') && ((offset + count) < 20))
	{
		for (uint8_t j = 0; j < 7; j++)
		{
			for (int8_t s = 4; s >= 0; s--)
			{
			  if ((symbol[(uint8_t)arr[count]][j] >> s) & 0x01)
			  {
				output_screen_buff_bin[19 - (offset + count)][(w * 2) + (57 + (70 * line))] |= BIT_SIN;
				output_screen_buff_bin[19 - (offset + count)][(w * 2) + (58 + (70 * line))] |= BIT_SIN;
			  }
			  else
			  {
				output_screen_buff_bin[19 - (offset + count)][(w * 2) + (57 + (70 * line))] &= ~BIT_SIN;
 				output_screen_buff_bin[19 - (offset + count)][(w * 2) + (58 + (70 * line))] &= ~BIT_SIN;
			  }
			  w++;
			}
		}
		w = 0;
		i++;
		count++;
	}
}

uint8_t vfd_print_symbol (uint8_t sym, uint8_t line, uint8_t offset, uint8_t amount) {
	uint8_t w = 0;
	line = !line;

	for(uint8_t i = amount; i > 0; i--)
	{
		for (uint8_t j = 0; j < 7; j++)
		{
			for (int8_t s = 4; s >= 0; s--)
			{
			  if ((symbol[sym][j] >> s) & 0x01)
			  {
				output_screen_buff_bin[19 - offset][(w * 2) + (57 + (70 * line))] |= BIT_SIN;
				output_screen_buff_bin[19 - offset][(w * 2) + (58 + (70 * line))] |= BIT_SIN;
			  }
			  else
			  {
				output_screen_buff_bin[19 - offset][(w * 2) + (57 + (70 * line))] &= ~BIT_SIN;
				output_screen_buff_bin[19 - offset][(w * 2) + (58 + (70 * line))] &= ~BIT_SIN;
			  }
			  w++;
			}
		}
		offset++;
		w = 0;
	}
	return amount;
}

void vfd_display_error(uint8_t line)
{
	for(int i = 0; i < 3; i++){
		vfd_clear_line(1);
		HAL_Delay(400);
		vfd_print("ERROR!!!", line, 5);
		HAL_Delay(400);
	}
	vfd_clear_line(1);
}

void vfd_set_brightness (int br)
{
	br = (br - 1) * (198 - 15) / (100 - 1) + 15;

	for (int i = 0; i < 20; i++)
		for (int j = 5; j < 199; j++)
			output_screen_buff_bin[i][j] &= ~BIT_BK;

	for (int i = 0; i < 20; i++)
		for (int j = br; j < 199; j++)
			output_screen_buff_bin[i][j] |= BIT_BK;
}

