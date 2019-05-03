#ifndef _TIMERGROUP_H
#define _TIMERGROUP_H	 


/*时间段结构*/
typedef struct Time_Section_Opr
{
	unsigned short Start_Date;				//起始年月日;
	unsigned short End_Date;				//截止年月日;	
	unsigned char  Week;					//星期控制;
	int Mon;					//日期控制
	unsigned short Start_Time[3];			//起始时分;3个段
	unsigned short End_Time[3];			//截止时分;3个段
}Time_Section_Opr; 

/*时间组结构*/
typedef struct Time_Group_Opr
{
	int Time_Group_Number;				//时间组序列号;
	Time_Section_Opr Time_Section[4];//时间段结构; 4个段；
}T_Time_Group_Opr,*PT_Time_Group_Opr; 


#endif

