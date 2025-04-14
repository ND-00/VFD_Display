#ifndef	_ESP8266_H
#define	_ESP8266_H

#include "ESP8266Config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "stm32f4xx.h"
#include "service.h"

extern UART_HandleTypeDef 	_WIFI_USART;

typedef	enum
{
	WifiMode_Error                        =     0,
	WifiMode_Station                      =     1,
	WifiMode_SoftAp                       =     2,
	WifiMode_StationAndSoftAp             =     3,
}WifiMode_t;

typedef enum
{
  WifiEncryptionType_Open                 =     0,
  WifiEncryptionType_WPA_PSK              =     2,
  WifiEncryptionType_WPA2_PSK             =     3,
  WifiEncryptionType_WPA_WPA2_PSK         =     4,
}WifiEncryptionType_t;

typedef enum
{
  WifiConnectionStatus_Error              =     0,
  WifiConnectionStatus_GotIp              =     2,
  WifiConnectionStatus_Connected          =     3,
  WifiConnectionStatus_Disconnected       =     4,
  WifiConnectionStatus_ConnectionFail     =     5,
}WifiConnectionStatus_t;

typedef struct
{
  WifiConnectionStatus_t    status;
  uint8_t                   LinkId;
  char                      Type[4];
  char                      RemoteIp[17];
  uint16_t                  RemotePort;
  uint16_t                  LocalPort;
  uint8_t                   RunAsServer;
}WifiConnection_t;

typedef struct
{
	//----------------Usart	Paremeter
	uint8_t                 usartBuff;
	uint8_t                 RxBuffer[_WIFI_RX_SIZE];
	uint8_t                 TxBuffer[_WIFI_TX_SIZE];
	uint16_t                RxIndex;
	uint8_t                 RxIsData;
	//----------------General	Parameter
	WifiMode_t              Mode;
	char                    MyIP[16];
	char                    MyGateWay[16];
	//----------------Station	Parameter
	char					SSID_Connected[20];
	uint8_t                 StationDhcp;
	char                    StationIp[16];
	//----------------SoftAp Parameter
	uint8_t                 SoftApDhcp;
	char                    SoftApConnectedDevicesIp[6][16];
	char                    SoftApConnectedDevicesMac[6][18];
	//----------------TcpIp Parameter
	uint8_t                 TcpIpMultiConnection;
	uint16_t                TcpIpPingAnswer;
	WifiConnection_t        TcpIpConnections[5];
}Wifi_t;

uint8_t WiFi_GetTime(time_t *result);
uint8_t WiFi_GetWeather(struct weather_t weather[4]);

void 	Wifi_RxClear(void);
uint8_t Wifi_SendString(char *data);
uint8_t Wifi_WaitForString(uint32_t TimeOut_ms,uint8_t *result,uint8_t CountOfParameter,...);
void 	Wifi_RxCallBack(void);

// Basic functions of ESP8266
uint8_t Wifi_Init(void);
uint8_t Wifi_Restart(void);
uint8_t Wifi_DeepSleep(uint16_t DelayMs);
uint8_t Wifi_FactoryReset(void);
uint8_t Wifi_Update(void);
uint8_t Wifi_SetRfPower(uint8_t Power_0_to_82);

// Mode of the ESP8266: Station or SoftAP or both
uint8_t Wifi_SetMode(WifiMode_t	WifiMode_);
uint8_t Wifi_GetMode(void);
uint8_t Wifi_GetMyIp(char* IP);
uint8_t Wifi_Station_ConnectToAp(char *SSID,char *Pass,char *MAC);
uint8_t Wifi_Station_Disconnect(void);
uint8_t Wifi_Station_DhcpEnable(uint8_t Enable);
uint8_t Wifi_Station_DhcpIsEnable(void);
uint8_t Wifi_Station_SetIp(char *IP,char *GateWay,char *NetMask);

uint8_t Wifi_SoftAp_Create(char *SSID,char *password,uint8_t channel,
		WifiEncryptionType_t WifiEncryptionType,uint8_t MaxConnections_1_to_4, uint8_t HiddenSSID);
uint8_t Wifi_GetApConnection(void);
uint8_t Wifi_SoftAp_GetConnectedDevices(void);

uint8_t Wifi_TcpIp_GetConnectionStatus(void);
uint8_t Wifi_TcpIp_Ping(char *PingTo);
uint8_t Wifi_TcpIp_SetMultiConnection(uint8_t EnableMultiConnections);
uint8_t Wifi_TcpIp_GetMultiConnection(void);
uint8_t Wifi_TcpIp_StartTcpConnection(uint8_t LinkId,char *RemoteIp,uint16_t RemotePort, uint16_t TimeOut_S);
uint8_t Wifi_TcpIp_StartUdpConnection(uint8_t LinkId,char *RemoteIp,uint16_t RemotePort, uint16_t LocalPort);
uint8_t Wifi_TcpIp_Close(uint8_t LinkId);
uint8_t Wifi_TcpIp_SetEnableTcpServer(uint16_t PortNumber);
uint8_t Wifi_TcpIp_SetDisableTcpServer(uint16_t PortNumber);
uint8_t Wifi_TcpIp_SendDataUdp(uint8_t LinkId,uint16_t dataLen,uint8_t *data);
uint8_t Wifi_TcpIp_SendDataTcp(uint8_t LinkId,uint16_t dataLen,uint8_t *data);
#endif

