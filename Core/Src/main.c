#include "main.h"
#include "ASCII_array.h"
#include "VFD.h"
#include "ESP8266.h"
#include <time.h>
#include "string.h"
#include "service.h"

SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

extern uint8_t output_screen_buff_bin[20][198];

char SSID[] = "astronel.ru";
char Pass[] = "Of97U1iqf";

char esp_ip[16];

time_t 		unix_time;
struct weather_t weather[4];
struct tm 	*current_time;
uint8_t 	flag_time_inc = 0x00;
uint8_t 	display_mode  = 0x01;
char *weeks[7] =   {"Воскресенье", "Понедельник", "    Вторник", "      Среда", "    Четверг", "    Пятница", "    Суббота" };
char *months[12] = { "Января  ", "Февраля ", "Марта   ", "Апреля  ", "Мая     ", "Июня    ",
					 "Июля    ", "Августа ", "Сентября", "Октября ", "Ноября  ", "Декабря " };

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);

uint8_t esp_init();
uint8_t esp_get_time();
uint8_t esp_get_weather();

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	static uint16_t counter = 0;

	if(htim->Instance == htim1.Instance)
	{
		GPIOA->ODR = (GPIOA->ODR & 0x00) | output_screen_buff_bin[0][counter];
		if(++counter > 3960)
			counter = 0;
	}
	if(htim->Instance == htim3.Instance)
	{
		flag_time_inc++;
		unix_time++;

		if(display_mode)
		{
			if((unix_time % 25) == 0)
				display_mode = 0x00;
		}
		else
		{
			if((unix_time % 5) == 0)
				display_mode = 0x01;
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	Wifi_RxCallBack();
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();

	MX_GPIO_Init();
	MX_SPI1_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	MX_USART1_UART_Init();
	MX_TIM3_Init();

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim1);

	init_array();
	vfd_print("VFD Часы v0.2", 0, 3);
	HAL_Delay(5000);

	while(!esp_init())
	  vfd_display_error(1);

	vfd_print("Соединение", 0, 5);
	vfd_print("Установлено!", 1, 4);

	uint8_t fail_counter = 0;
	vfd_clear();
	vfd_print("Получение времени", 0, 0);

	while(!WiFi_GetTime(&unix_time))
	{
		vfd_clear();
		vfd_print("Время не получено", 0, 0);
		if(fail_counter++ > 5)
		{
			while(!esp_init()) vfd_display_error(1);
			fail_counter = 0;
		}
		HAL_Delay(2000);
	}

	vfd_clear();
	vfd_print("Получение погоды", 0, 0);
	while(!WiFi_GetWeather(weather))
	{
		vfd_clear();
		vfd_print("Погода не получена", 0, 0);
		if(fail_counter++ > 5)
		{
			while(!esp_init()) vfd_display_error(1);
			fail_counter = 0;
		}
		HAL_Delay(2000);
	}

	HAL_TIM_Base_Start_IT(&htim3);
	vfd_clear();

	while (1)
	{
		if(flag_time_inc)
		{
			if((unix_time % 300) == 0)
			{
				vfd_clear();
				vfd_print("Обновление погоды", 0, 0);
				if(!WiFi_GetWeather(weather))
					vfd_print(" ERROR!!!", 1 , 0);
				else
					vfd_print(" OK ", 1, 0);
				HAL_Delay(400);
			}
			if(!HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin))
			{
				static uint8_t flag_dot = 0x00;
				static char dot_char;
				current_time = localtime(&unix_time);

				if(flag_dot)
					dot_char = ':';
				else
					dot_char = SYM_A_DOUBLE_DOT;

				char time_string[20];
				sprintf(time_string, "%2d%c%.2d%c%.2d %s",	 current_time->tm_hour,
															 dot_char,
															 current_time->tm_min,
															 dot_char,
															 current_time->tm_sec,
															 weeks[current_time->tm_wday]);
				vfd_print(time_string, 0, 0);
				sprintf(time_string, "%d %s%s%d год", 		 current_time->tm_mday,
															 months[current_time->tm_mon],
															 current_time->tm_mday > 9 ? " ":"  ",
															 current_time->tm_year + 1900);
				vfd_print(time_string, 1, 0);
				flag_dot = !flag_dot;
			}
			else
			{
				char weather_string[2][6];
				for(uint8_t i = 0; i < 4; i++)
				{
					uint8_t weather_ico_id;
					switch(weather[i].ico_id)
					{
						case 1:
							weather_ico_id = SYM_CLEAR_SKY;
							break;
						case 2:
							weather_ico_id = SYM_FEW_CLOUDS;
							break;
						case 3:
							weather_ico_id = SYM_SCATTERD_CLOUS;
							break;
						case 4:
							weather_ico_id = SYM_BROKEN_CLOUDS;
							break;
						case 9:
							weather_ico_id = SYM_SHOWER_RAIN;
							break;
						case 10:
							weather_ico_id = SYM_RAIN;
							break;
						case 11:
							weather_ico_id = SYM_THUNDERSTORM;
							break;
						case 13:
							weather_ico_id = SYM_SNOW;
							break;
						case 50:
							weather_ico_id = SYM_MIST;
							break;
						default:
					}

					sprintf(weather_string[0], "%c%c%c%c%c", 	localtime(&weather[i].time)->tm_hour + 139,
																SYM_SMALL_DOBLEDOT,
																SYM_SMALL_0,
																weather_ico_id,
																' ');
					sprintf(weather_string[1], "%+3d%c%c", 		weather[i].temp,
																SYM_CELSIUS,
																' ');
					vfd_print(weather_string[0], 0, i*5);
					vfd_print(weather_string[1], 1, i*5);
				}
			}
			flag_time_inc--;
		}
	}
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}

static void MX_SPI1_Init(void)
{
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) Error_Handler();
}

static void MX_TIM1_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 39;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 7;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) Error_Handler();

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) Error_Handler();

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) Error_Handler();
}

static void MX_TIM2_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 8;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 530;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) Error_Handler();

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) Error_Handler();
	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) Error_Handler();

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) Error_Handler();

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 85;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) Error_Handler();

	HAL_TIM_MspPostInit(&htim2);
}

static void MX_TIM3_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 8399;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 9999;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) Error_Handler();

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) Error_Handler();

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) Error_Handler();
}

static void MX_USART1_UART_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) Error_Handler();
}

static void MX_GPIO_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();

	  HAL_GPIO_WritePin(GPIOA, CLK_Pin|BK_Pin|LAT_Pin|SIN_Pin, GPIO_PIN_RESET);

	  GPIO_InitStruct.Pin = CLK_Pin|BK_Pin|LAT_Pin|SIN_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = BTN_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  HAL_GPIO_Init(BTN_GPIO_Port, &GPIO_InitStruct);

}

int _write(int file, char *ptr, int len)
{
	int DataIdx;
    for(DataIdx = 0; DataIdx < len; DataIdx++)
    	ITM_SendChar(*ptr++);
    return len;
}

uint8_t esp_init()
{
	uint8_t offset = 0;
 	vfd_print("Подключение к WiFi:", 0, 0);

 	if(!Wifi_Restart())
		return 0x00;
	HAL_Delay(500);
	vfd_print_symbol(SYM_PROGRESS_BAR, 1, offset, 4);
	offset += 4;

	if(!Wifi_Init())
		return 0x00;
	HAL_Delay(500);
	vfd_print_symbol(SYM_PROGRESS_BAR, 1, offset, 4);
	offset += 4;

	if(!Wifi_SetMode(WifiMode_StationAndSoftAp))
		return 0x00;
	HAL_Delay(500);
	vfd_print_symbol(SYM_PROGRESS_BAR, 1, offset, 4);
	offset += 4;

	if(!Wifi_Station_ConnectToAp(SSID, Pass, NULL))
		return 0x00;
	HAL_Delay(2000);
	vfd_print_symbol(SYM_PROGRESS_BAR, 1, offset, 4);

	HAL_Delay(500);
	vfd_clear();
	return 1;
}

void Error_Handler(void)
{
  __disable_irq();
  while (1);
}
