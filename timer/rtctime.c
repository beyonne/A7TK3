#include <time.h>
#include <linux/rtc.h>
#include <stdio.h>
#include <sys/time.h>
#include <communication.h>
#include <rtctime.h>

//Time_Struct Rtc;

static PT_Time_Opr g_ptTimeHead;

//char *dt="2015-8-15 20:30:30";



int RegisterTimerOpr(PT_Time_Opr ptTimeOpr)
{
	PT_Time_Opr ptTmp;

	if (!g_ptTimeHead)
	{
		g_ptTimeHead   = ptTimeOpr;
		ptTimeOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptTimeHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptTimeOpr;
		ptTimeOpr->ptNext = NULL;
	}

	return 0;
}


PT_Time_Opr GetDefultTimerOpr(void)
{
	return g_ptTimeHead;
}



int RtcTimeInit(void)
{
	PT_Time_Opr pt_Timer;
	pt_Timer = malloc(sizeof(T_Time_Opr));

	RegisterTimerOpr(pt_Timer);	

	
	
}

int SetSystemTime(char *dt)
{/*
	struct rtc_time tm;
	struct tm _tm;
	struct timeval tv;
	time_t timep;
	
	sscanf(dt, "%d-%d-%d %d:%d:%d", &tm.tm_year,
	&tm.tm_mon, &tm.tm_mday,&tm.tm_hour,
	&tm.tm_min, &tm.tm_sec);
	
	_tm.tm_sec = tm.tm_sec;
	_tm.tm_min = tm.tm_min;
	_tm.tm_hour = tm.tm_hour;
	_tm.tm_mday = tm.tm_mday;
	_tm.tm_mon = tm.tm_mon - 1;
	_tm.tm_year = tm.tm_year - 1900;
	timep = mktime(&_tm);
	
	tv.tv_sec = timep;
	tv.tv_usec = 0;
	if(settimeofday (&tv, (struct timezone *) 0) < 0)
	{
		DBG_PRINTF("Set system datatime error!\n");
		return -1;
	}*/
	return 0;
}

int gettime(char *timer)
{/*
	char *wday[]={"-7-","-1-","-2-","-3-","-4-","-5-","-6-"};
	time_t timep;
	struct tm *tm_now;
	//char timer[20];
	char *temer1 = "%d-%d-%d %d:%d:%d: <%d>";
	//SetSystemTime(pt);
	
	time(&timep);
	tm_now=localtime(&timep);
	tm_now->tm_year-=100;
	tm_now->tm_mon+=1;
	
	if(!(tm_now->tm_wday))
	{
		tm_now->tm_wday = 7;
	}
	
	timer[0] = tm_now->tm_year;
	timer[1] = tm_now->tm_mon;
	timer[2] = tm_now->tm_mday;
	timer[3] = tm_now->tm_wday;
	timer[4] = tm_now->tm_hour;
	timer[5] = tm_now->tm_min;
	timer[6] = tm_now->tm_sec;
	
	*/
	
	//return timer;
	//DBG_PRINTF("now datetime: %d-%d-%d %d:%d:%d\n", tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
}
/********************************************************************************** 
 函 数 名：BCD_HEX
 功    能：BCD码转十六进制
 说    明：
 入口参数： BCD码
 返 回 值：无
**********************************************************************************/
static unsigned char BCD_HEX(unsigned char bcd_data)
{
 	unsigned char hex_data;
    hex_data=(bcd_data/16)*10+bcd_data%16;
	return hex_data;
}

static unsigned char HEX_BCD(unsigned char bcd_data)
{
 	unsigned char hex_data;
    hex_data=(bcd_data/10)*16+bcd_data%10;
	return hex_data;
}


int Efficacy_time(char *data)
{
	unsigned char DataTemp[20];
	unsigned char temp[100];
	unsigned char i;
	
	char *time = "20%d-%d-%d %d:%d:%d";

	for(i=0;i<7;i++)
	data[i]=BCD_HEX(data[i]);
	
	sprintf(temp,time,data[0],data[1],data[2],data[4],data[5],data[6]);
	//DBG_PRINTF("temp:%s\n",temp);
	SetSystemTime(temp);

	
	return 0;
}

void FM_Read_Time(unsigned char *pBuf)
{
	unsigned char temp[7] ;
	
	gettime(temp);
	
	pBuf[0]=temp[6];//秒
	pBuf[1]=temp[5];//分
	pBuf[2]=temp[4];//时
	pBuf[4]=temp[2];//日
	pBuf[5]=temp[1];//月
	pBuf[3]=temp[3];//周
	pBuf[6]=temp[0];//年
}

/*
int main()
{
	char time[25];
	gettime(time);
	time[25] = '\0';
	DBG_PRINTF("%s\n",time);
}
*/


