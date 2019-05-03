#ifndef __RTCTIME_H
#define __RTCTIME_H




/*ʱ��ṹ*/
typedef struct Time_Opr
{
	unsigned char s ;
	unsigned char Min ;
	unsigned char Hour ;
	unsigned char Week;
	unsigned char Days ;
	unsigned char Months ;
	unsigned char Year ;
	struct Time_Opr *ptNext;
}T_Time_Opr,*PT_Time_Opr;   //ʱ��ṹ


int SetSystemTime(char *dt);

int gettime(char *timer);



int RegisterTimerOpr(PT_Time_Opr ptTimeOpr);

int RtcTimeInit(void);

PT_Time_Opr GetDefultTimerOpr(void);
/*;
int main()
{
	char time[25];
	gettime(time);
	time[25] = '\0';
	DBG_PRINTF("%s\n",time);
}
*/
#endif



