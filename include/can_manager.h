#ifndef __CAN_MANAGER_H
#define __CAN_MANAGER_H

#include <config.h>

#define FILTER_NUMBER	3
#define CAN_NAME_MAX_LENTH 		10

#ifndef AF_CAN
#define AF_CAN 29
#endif
#ifndef PF_CAN
#define PF_CAN AF_CAN
#endif


#define CAN_READ_CALLBACK(CB) void (CB)(int fd, int error, void *buf, int len,int can_id, void *data)





#define CAN_ERR_OK       0   ///< no error
#define CAN_ERR_OPEN    -1   ///< could'nt open socket
#define CAN_ERR_LISTEN  -2   ///< error while listening to socket
#define CAN_ERR_CONNECT -3   ///< connecting to socket failed
#define CAN_ERR_BIND    -4   ///< couldn't bind to socket
#define CAN_ERR_ACCEPT  -5   ///< error while accepting connection
#define CAN_ERR_READ    -6   ///< error while reading from socket
#define CAN_ERR_WRITE   -7   ///< failed to write to socket
#define CAN_ERR_CLOSED  -8   ///< peer closed connection
#define CAN_ERR_BUFFER  -9   ///< out of memory (buffer allocation error)
#define CAN_ERR_ADDRESS -10  ///< could not get address from hostname


#ifndef NULL
#define NULL ((void *)0) ///< NULL is defined in a somehow save manner
#endif

//CAN缓存管理
typedef enum {
	NMS_CAN_FREE = 0,
	NMS_CAN_USED_FOR_CUR,	
}E_CanMemState;

typedef struct CanMemOpr{	
	int ID;
	int DataLen;
	int can_id;
	E_CanMemState eMemState;
	unsigned char DataAddr[8];
	struct CanMemOpr *ptNext;
}T_CanMemOpr, *PT_CanMemOpr;


typedef struct ReceiveParam_Can {
	int  fd;                      // filedescriptor to read from
	void *data;                   // pointer to user specific data
	int master;
	CAN_READ_CALLBACK(*cb);       // function to call after reading
} ReceiveParamCan;


typedef struct CanDeviceOpr {	
	int fd;
	char *name;				//名称;can0 can1;
	char filter_flag;			//是否开启过滤ID,1:开启过滤,0关闭过滤;
	int  can_id[FILTER_NUMBER];			//需要的ID;
	int  can_mask[FILTER_NUMBER];			//屏蔽的ID;
	int  filter_number;
	char af_can;			//29,11;
	char pf_can;			//29,11;
	int bitrate;			//波特率;
	int master;				
	struct CanDeviceOpr *ptNext;
}T_CanDeviceOpr, *PT_CanDeviceOpr;

typedef struct CanOpr {	
	char *name;
	int (*CANClose)(int fd);
	int (*CANInit)(int *fd,char *name,char filter_flag,int *can_id,int *can_mask,int filter_number,char af_can,char pf_can);
	int (*CANRead)(int fd, int master,CAN_READ_CALLBACK(*cb), void *data);
	int (*CANSendData)(int fd,int can_id,char *tx_buf,int data_len);
	struct CanOpr *ptNext;
}T_CanOpr, *PT_CanOpr;

int RegisterCanOpr(PT_CanOpr ptCanOpr);

int CanRegister(void);
PT_CanDeviceOpr GetCanOpr(char *pcName);


#endif
