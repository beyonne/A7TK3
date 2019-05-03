#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H	 

#include <sys/time.h>
#include <pthread.h>

/* 输入事件类别 */
typedef enum {
	INPUT_TYPE_WEIGEN0 = 0,
	INPUT_TYPE_WEIGEN1,
	INPUT_TYPE_WEIGEN2,
	INPUT_TYPE_WEIGEN3,
	
}E_InputState;


typedef struct InputEvent {
	//struct timeval tTime;   	/* 发生这个输入事件时的时间 */
	E_InputState iType;  		/* 几号读卡器 */
	int GetIntData;				/* 输入整形数据 */
	int *GetData;				/* 输入大量数据 */
	int WeiGenType;				/* 韦根类型 26或者34 */
}T_InputEvent, *PT_InputEvent;


typedef struct InputOpr {
	char *name;          /* 输入模块的名字 */
	pthread_t tTreadID;  /* 子线程ID */
	int (*DeviceInit)(void);  /* 设备初始化函数 */
	int (*DeviceExit)(void);  /* 设备退出函数 */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);  /* 获得输入数据 */
	struct InputOpr *ptNext;
}T_InputOpr, *PT_InputOpr;


int KeyInit(void);
int ButtonInit(void);
int MenciInit(void);
int WeiGenDevInit(void);

int RegisterInputOpr(PT_InputOpr ptInputOpr);

int InputInit(void);
int AllInputDevicesInit(void);
PT_InputOpr GetDefaultInputOpr(void);

#endif
