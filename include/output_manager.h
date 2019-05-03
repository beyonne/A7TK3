#ifndef _OUTPUT_MANAGER_H
#define _OUTPUT_MANAGER_H	 

#include <sys/time.h>
#include <pthread.h>

/* 输入事件类别 */
typedef enum {
	INPUT_TYPE_BUZZ = 0,
	INPUT_TYPE_RL,

}E_OutputState;


typedef struct OutEvent {
	//struct timeval tTime;   	/* 发生这个输入事件时的时间 */
	E_OutputState iType;  				/* 类别: stdin, touchsceen */
	int IOno;
	int OutintData;				/* 输入整形数据 */
	
}T_OutputEvent, *PT_OutputEvent;


typedef struct OutputOpr {
	char *name;          /* 输入模块的名字 */
	pthread_t tTreadID;  /* 子线程ID */
	int (*DeviceInit)(void);  /* 设备初始化函数 */
	int (*DeviceExit)(void);  /* 设备退出函数 */
	int (*OutputEvent)(PT_OutputEvent ptOutputEvent);  /* 获得输入数据 */
	struct OutputOpr *ptNext;
}T_OutputOpr, *PT_OutputOpr;


int RegisterOutputOpr(PT_OutputOpr ptOutputOpr);
int BuzzInit(void);
int RlInit(void);
int OuputInit(void);
int AllOutputDevicesInit(void);


#endif
