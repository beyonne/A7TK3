#ifndef _USART_MANAGER_H
#define _USART_MANAGER_H	 

#define COM_NAME_MAX_LENTH		20
#define DEVICE_FILE_MAX_LENTH	20


#define COM_READ_CALLBACK(CB) void (CB)(int fd, int error, void *buf, int len, void *data)

#define COM_ERR_OK       0   ///< no error
#define COM_ERR_OPEN    -1   ///< could'nt open socket
#define COM_ERR_LISTEN  -2   ///< error while listening to socket
#define COM_ERR_CONNECT -3   ///< connecting to socket failed
#define COM_ERR_BIND    -4   ///< couldn't bind to socket
#define COM_ERR_ACCEPT  -5   ///< error while accepting connection
#define COM_ERR_READ    -6   ///< error while reading from socket
#define COM_ERR_WRITE   -7   ///< failed to write to socket
#define COM_ERR_CLOSED  -8   ///< peer closed connection
#define COM_ERR_BUFFER  -9   ///< out of memory (buffer allocation error)
#define COM_ERR_ADDRESS -10  ///< could not get address from hostname


#ifndef NULL
#define NULL ((void *)0) ///< NULL is defined in a somehow save manner
#endif



typedef struct ReceiveParam_Com {
	int  fd;                      // filedescriptor to read from
	void *data;                   // pointer to user specific data
	COM_READ_CALLBACK(*cb);       // function to call after reading
} ReceiveParamCom;

typedef enum {
	N = 0,
	O,
	E,
	S,
}E_FLOW_State;


typedef enum {
	S_BIT1 = 1,
	S_BIT2,
}E_S_BIT_State;

typedef enum {
	NONE = 0,
	RTS_CTS,
	XON_XOFF,
}E_NRX_State;

typedef enum {
	D_BIT5 = 5,
	D_BIT6,
	D_BIT7,
	D_BIT8,
}E_D_BIT_State;


typedef struct UsartOpr {	
	char *name;
	int (*USARTOpen)(int *fd,char *port);
	int (*USARTClose)(int fd);
	int (*USART0Init)(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity);
	int (*ComRead)(int fd, COM_READ_CALLBACK(*cb), void *data);
	int (*USARTSendData)(int fd, char *tx_buf,int data_len);
	struct UsartOpr *ptNext;
}T_UsartOpr, *PT_UsartOpr;

typedef struct UsartDeviceOpr {	
	char *name;		//设备名字
	char *dev;		//设备文件路径
	int  fd;		//文件描述符
	int speed;		//波特率
	int flowCtrl;	//数据流控
	int databits;	//数据位
	int stopbis;	//停止位
	int parity;		//奇偶校验 N,E,O,S 
	struct UsartDeviceOpr *ptNext;
}T_UsartDeviceOpr, *PT_UsartDeviceOpr;



int Usart_Register(void);
int UsartDevInit(void);
int AddUsartDevice(char *name,char *dev,int speed,\
int flowCtrl,int databits,int stopbit,int parity);

#endif
