#include <stdio.h>	
#include <sys/time.h>  
#include <stdlib.h>  
#include <unistd.h>
#include <communication.h>
#include <timer.h>  
#include <signal.h>
#include <time.h>  
#include <config.h>



static timer_t timer;  
static PT_TimeStop_Opr g_ptTimeOprHead;
static PT_TimeStopList_Opr time_stop;

PT_TimeStopList_Opr get_def_time(void)
{
	return time_stop;
}


static void *signal_handler(void *param) 
{  
	static int PCACKSendTime,msec;
	int j,i;
	PT_pcTxTime PCTx;
	PT_TimeStopList_Opr time_stop;

	time_stop = get_def_time();
	PCTx = GetPcTxTime();
	
	//Rx_Ocutmp = GetTimeOpr("RxOcu");

	while(1)
	{
		usleep(5000);
		
		if(PCTx->PC_TX_Out_En)
		{
			if(PCTx->PC_TX_Time_Out)
				PCTx->PC_TX_Time_Out--;
		}

		if(time_stop->motion_send.TimeEn)
		{
			if(time_stop->motion_send.Times)
				time_stop->motion_send.Times--;
		}

		if(time_stop->ocu_rx_timeout.TimeEn)
		{
			if(time_stop->ocu_rx_timeout.Times)
				time_stop->ocu_rx_timeout.Times--;
		}
		
		if(time_stop->motion_R_rx_timeout.TimeEn)
		{
			if(time_stop->motion_R_rx_timeout.Times)
				time_stop->motion_R_rx_timeout.Times--;
		}

		if(time_stop->motion_L_rx_timeout.TimeEn)
		{
			if(time_stop->motion_L_rx_timeout.Times)
				time_stop->motion_L_rx_timeout.Times--;
		}

		if(time_stop->motion_arm_timeout.TimeEn)
		{
			if(time_stop->motion_arm_timeout.Times)
				time_stop->motion_arm_timeout.Times--;
		}

		if(time_stop->claw_timeout.TimeEn)
		{
			if(time_stop->claw_timeout.Times)
				time_stop->claw_timeout.Times--;
		}

		for(i=0;i<6;i++)
		{
			if(time_stop->joint[i].TimeEn)
				if(time_stop->joint[i].Times)
					time_stop->joint[i].Times--;
		}
		
/*
		if(Rx_Ocutmp->TimeEn)
		{
			if(Rx_Ocutmp->Times)
				Rx_Ocutmp->Times--;
		}*/
	}
}  

int time_create(int val)
{/*
	struct sigevent evp;
	int ret;  
	
	memset (&evp, 0, sizeof (evp));  
	evp.sigev_value.sival_ptr = &timer;  
	evp.sigev_notify = SIGEV_THREAD;  
	evp.sigev_notify_function = signal_handler;  
	evp.sigev_value.sival_int = 3; 						//��Ϊhandle()�Ĳ���  
	
	ret = timer_create(CLOCK_REALTIME, &evp, &timer);  
	if(ret)  
	{
    	DBG_PRINTF("timer_create fail"); 
    	return -1;
    }
    DBG_PRINTF("signal_handler1\n");
    return 0;*/
    
    time_stop = malloc(sizeof(T_TimeStopList_Opr));	//��ʼ����ʱ����

	//��ʼ����ʱ��־;
	time_stop->ocu_rx_timeout.TimeEn = 1;
	
	time_stop->motion_send.TimeEn = 0;
    time_stop->motion_send.Times = SEND_TO_MOTION;

    time_stop->motion_R_rx_timeout.TimeEn = 0;

    time_stop->motion_L_rx_timeout.TimeEn = 0;

    time_stop->motion_arm_timeout.TimeEn = 0;

    time_stop->claw_timeout.TimeEn = 0;
    
    
    pthread_t thr_id;

	if (pthread_create(&thr_id, NULL, &signal_handler, NULL)) {
		return (-1);
	}
	
	if (pthread_detach(thr_id)) {
		return (-1);
	}
}

/*
int ilsec:��ʱ��
int ilnsec:��ʱ����
int iesec:��ʱ������
int iensec:��ʱ��������
*/
int set_timer1(int ilsec,int ilnsec,int iesec,int iensec) 
{
	int ret;  
	/*
	struct itimerspec ts;
	
	ts.it_interval.tv_sec = ilsec;  
	ts.it_interval.tv_nsec = ilnsec;  
	ts.it_value.tv_sec = iesec;  
	ts.it_value.tv_nsec = iensec;  

	//sleep(4);

	ret = timer_settime(timer, TIMER_ABSTIME, &ts, NULL);  
	if(ret)  
	{
    	DBG_PRINTF("timer_initset fail"); 
    	return -1;
    }
    DBG_PRINTF("signal_handler2\n");*/
	return 0 ;
}  
  

//char *pt="2006-4-20 20:30:30"

/*  
int main()	
{  
	signal(SIGALRM, signal_handler);  
	set_timer();  
	while(count < 10000);  
	exit(0);  
	return 1;  
}  
*/
