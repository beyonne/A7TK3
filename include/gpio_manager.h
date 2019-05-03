#ifndef _GPIO_MANAGER_H
#define _GPIO_MANAGER_H	 


//#define	DEV_PATH	"/sys/class/gpio/gpio68/value"	    	// 输入输出电平值设备
#define	EXPORT_PATH	"/sys/class/gpio/export"	    		// GPIO设备导出设备
//#define	DIRECT_PATH	"/sys/class/gpio/gpio68/direction"		// GPIO输入输出控制设备
#define	    OUT		"out"
#define	    IN		"in"


/*
#define	    GPIO_RL3			68									// GPIO3_4
#define	    GPIO_RL2			42
#define	    GPIO_PWR_TR			45
#define	    GPIO_FTR3 			18
#define	    GPIO_FTR2			115
#define	    GPIO_FTR1			48
#define	    GPIO_FLED			50
#define	    GPIO_CAM_SWT0		51
#define	    GPIO_SWTCH			53

#define	    GPIO_FFUS2			53
#define	    GPIO_FFUS3			122
#define	    GPIO_FFUS4			124
#define	    GPIO_RL1			44

#define	    GPIO_LGTOIR1		87
#define	    GPIO_LGT_LED1		49
#define	    GPIO_LGTOIR2		116
#define	    GPIO_LGT_LED2		117

#define	    GPIO_QUAD_CH4		118
#define	    GPIO_QUAD_CH3		119
#define	    GPIO_QUAD_CH2		120
#define	    GPIO_QUAD_CH1		121
#define	    GPIO_QUAD_MENU		123
*/


#define	    GPIO_PTT			83
#define	    GPIO_TEST			81
#define	    GPIO_DR1			77
#define	    GPIO_DR2			71

#define	    GPIO_LGTOIR1		49
#define	    GPIO_LGT_LED1		48
#define	    GPIO_LGTOIR2		50
#define	    GPIO_LGT_LED2		51

#define	    GPIO_QUAD_CH4		45
#define	    GPIO_QUAD_CH3		44
#define	    GPIO_QUAD_CH2		42
#define	    GPIO_QUAD_CH1		41
#define	    GPIO_QUAD_MENU		18

#define 	RS485_DE_SENSER			41

#define	HIGH_LEVEL	"1"
#define	LOW_LEVEL	"0"

typedef struct IOctlOpr {
	char *name;          /* ����ģ������� */
	
	int (*Init)(void);  /* �豸��ʼ������ */
	int (*Exit)(void);  /* �豸�˳����� */
	int (*IOwrite)(int io,int val);  /* �豸��ʼ������ */
	int (*IOread)(int io);  /* �豸�˳����� */
	int (*IOctl)(int cmd,int GPIO, int *arg);
	struct IOctlOpr *ptNext;
}T_IOctlOpr, *PT_IOctlOpr;

int RegisterIOctlOpr(PT_IOctlOpr ptIOctlOpr);


int IOinit(void);
int IODevicesInit(void);
int IOCwrite(int io ,int val);
int IOCread(int io);


#endif
