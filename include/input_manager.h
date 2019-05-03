#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H	 

#include <sys/time.h>
#include <pthread.h>

/* �����¼���� */
typedef enum {
	INPUT_TYPE_WEIGEN0 = 0,
	INPUT_TYPE_WEIGEN1,
	INPUT_TYPE_WEIGEN2,
	INPUT_TYPE_WEIGEN3,
	
}E_InputState;


typedef struct InputEvent {
	//struct timeval tTime;   	/* ������������¼�ʱ��ʱ�� */
	E_InputState iType;  		/* ���Ŷ����� */
	int GetIntData;				/* ������������ */
	int *GetData;				/* ����������� */
	int WeiGenType;				/* Τ������ 26����34 */
}T_InputEvent, *PT_InputEvent;


typedef struct InputOpr {
	char *name;          /* ����ģ������� */
	pthread_t tTreadID;  /* ���߳�ID */
	int (*DeviceInit)(void);  /* �豸��ʼ������ */
	int (*DeviceExit)(void);  /* �豸�˳����� */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);  /* ����������� */
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
