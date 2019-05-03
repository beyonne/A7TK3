
#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <debug_manager.h>

#define errout(_s)	fprintf(stderr, "error class: %s\n", (_s))
#define errcode(_d) fprintf(stderr, "error code: %02x\n", (_d))
#define myerr(str)	fprintf(stderr, "%s, %s, %d: %s\n", __FILE__, __func__, __LINE__, str)

#define BUTTON_DEVICE_NAME 	"/dev/key"
#define GPIO_DEVICE_NAME 	"/dev/inoutgpio"
#define BEEP_DEVICE_NAME 	"/dev/beep"


#define WEIGEN0_DEVICE_NAME "/dev/weigen0"
#define WEIGEN1_DEVICE_NAME "/dev/weigen1"
#define WEIGEN2_DEVICE_NAME "/dev/weigen2"
#define WEIGEN3_DEVICE_NAME "/dev/weigen3"

#define USART0_DEVICE_NAME 	"/dev/ttySAC0"
#define USART1_DEVICE_NAME 	"/dev/ttySAC1"
#define RS485_DEVICE_NAME 	"/dev/rsuart"
#define WDT 				"/dev/watchdog"
/*
#define COM_NAME1			"/dev/ttymxc0"					//Ԥ��
#define COM_NAME2			"/dev/ttymxc1"					//RF_OCU
#define COM_NAME3			"/dev/ttymxc2"					//����_OCU
#define COM_NAME4			"/dev/ttymxc3"					//���Ӻͷ�ת��
#define COM_NAME5			"/dev/ttymxc4"					//PTZ
#define COM_NAME6			"/dev/ttymxc5"					//ARM��
#define COM_NAME7			"/dev/ttymxc6"					//Head�װ�
#define COM_NAME8			"/dev/ttymxc7"					//WIFI_OCU
*/
#define COM_RES				"/dev/ttymxc0"					//Ԥ��
#define COM_RF				"/dev/ttymxc2"					//RF_OCU
#define COM_FIBER			"/dev/ttymxc1"					//����_OCU
#define COM_MOTION			"/dev/ttymxc6"					//���Ӻͷ�ת��
#define COM_PTZ				"/dev/ttymxc7"					//PTZ
#define COM_ARM				"/dev/ttymxc5"					//ARM��
#define COM_HEAD			"/dev/ttymxc3"					//Head�װ�
#define COM_WIFI			"/dev/ttymxc4"					//WIFI_OCU



#define CONNECT_TO_SERVER				"tcp_client"
#define PHONE_SERVER					"tcp_server"
#define UDP_SERVER						"udp_rev"



#define AAI_TRACTION_LOCAL_SCALAR (6)
#define FLIPPER_LOCAL_SCALAR (40)



#define DEV_INFO 	"dev_info"
#define GROUP_INFO 	"group_info"
#define SCENE_INFO 	"scene_info"
#define SENSORLD_INFO 	"sensorld_info"

//#define USER_INFO_INX 	"user_info_index"

#define MenNum 	10

#define BUFSIZE		512

#define HAEDDATA 		0x3A
#define HAEDDATA1 		0xAA

#define OCURXHAEDDATA 		0x3A
#define OCURXHAEDDATA1 		0xAA

#define RX_OCU_TIMEOUT		20


#define TxBufSize	1024
#define RxBufSize	1024

#define TCP_SERVER_LISTEN_MAX	5

#define GetIO(x,y)		(x*32)+y


#define DEFAULT_DIR "/"

#define COLOR_BACKGROUND   0xE7DBB5  /* ???? */
#define COLOR_FOREGROUND   0x514438  /* ???? */


#define SEND_TO_OCU_CNT		5			///OCU??????OCU??????? 5*5ms?????;???????????????????;
#define SEND_TO_MOTION		30			//???????????????;
#define OCU_RX_OUTTIME		40			//ocu???????,????????;????;

#define MOTION_R_RX_TIMEOUT		10
#define MOTION_L_RX_TIMEOUT		10




//#define DBG_PRINTF(...)  
//#define DBG_PRINTF DebugPrint
#define DBG_PRINTF DebugPrint


/* ?????? */
#define ICON_PATH  "/etc/digitpic/icons"

#endif /* _CONFIG_H */
