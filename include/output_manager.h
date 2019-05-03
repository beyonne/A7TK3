#ifndef _OUTPUT_MANAGER_H
#define _OUTPUT_MANAGER_H	 

#include <sys/time.h>
#include <pthread.h>

/* �����¼���� */
typedef enum {
	INPUT_TYPE_BUZZ = 0,
	INPUT_TYPE_RL,

}E_OutputState;


typedef struct OutEvent {
	//struct timeval tTime;   	/* ������������¼�ʱ��ʱ�� */
	E_OutputState iType;  				/* ���: stdin, touchsceen */
	int IOno;
	int OutintData;				/* ������������ */
	
}T_OutputEvent, *PT_OutputEvent;


typedef struct OutputOpr {
	char *name;          /* ����ģ������� */
	pthread_t tTreadID;  /* ���߳�ID */
	int (*DeviceInit)(void);  /* �豸��ʼ������ */
	int (*DeviceExit)(void);  /* �豸�˳����� */
	int (*OutputEvent)(PT_OutputEvent ptOutputEvent);  /* ����������� */
	struct OutputOpr *ptNext;
}T_OutputOpr, *PT_OutputOpr;


int RegisterOutputOpr(PT_OutputOpr ptOutputOpr);
int BuzzInit(void);
int RlInit(void);
int OuputInit(void);
int AllOutputDevicesInit(void);


#endif
