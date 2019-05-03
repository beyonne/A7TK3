#ifndef __TIMER_H
#define __TIMER_H

int time_create(int val);

int set_timer1(int ilsec,int ilnsec,int iesec,int iensec) ;
 
#define TIME100MS	100000000
#define TIME1MS		1000000

typedef struct TimeStop_Opr
{		
	char *name;  
	int Times;
	int TimeEn;
}T_TimeStop_Opr,*PT_TimeStop_Opr;  

typedef struct TimeStopList_Opr
{		
	T_TimeStop_Opr motion_send;			//翻转臂角度等数据间隔发送读取
	T_TimeStop_Opr ocu_rx_timeout;		//OCU接收超时判断;
	T_TimeStop_Opr motion_L_rx_timeout;	//接受来自轮子的应答.如果没有应答则断电;防止失控;
	T_TimeStop_Opr motion_R_rx_timeout;	//接受来自轮子的应答.如果没有应答则断电;防止失控;
	T_TimeStop_Opr motion_arm_timeout;
	T_TimeStop_Opr claw_timeout;		//手臂应答计时；

	T_TimeStop_Opr joint[6];

	
	
}T_TimeStopList_Opr,*PT_TimeStopList_Opr;  

extern int RegisterTimeStopOpr(PT_TimeStop_Opr ptTimeOpr);
extern int RegisterTimeOpr(PT_TimeStop_Opr ptTimeOpr);
extern PT_TimeStop_Opr GetTimeOpr(char *pcName);


#endif
