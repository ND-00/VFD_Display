#ifndef	_WIFICONFIG_H
#define	_WIFICONFIG_H

/*
 * You have to change the settings according to your settings for the project
 * the times of execution and the other things are necessary to work correctly
 * without a problem
 */


#define 	_WIFI_USART  					huart1
#define		_WIFI_RX_SIZE					4096
#define		_WIFI_TX_SIZE					1024
#define 	_MAX_SEND_BYTES				2048

// The times are defined in ms
#define		_WIFI_WAIT_TIME_LOW			1000
#define		_WIFI_WAIT_TIME_MED			5000
#define		_WIFI_WAIT_TIME_HIGH			15000
#define		_WIFI_WAIT_TIME_VERYHIGH		25000
#define		_WIFI_WAIT_TIME_FOREWER		-1


#endif
