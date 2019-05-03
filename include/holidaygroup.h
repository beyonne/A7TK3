#ifndef _HOLIDAY_H
#define _HOLIDAY_H	 


/*假日组结构*/
typedef struct Holiday_Group_Opr
{
	int  Holiday_Number;					//假日组序列号;
	int  Time_Auth;					//时间权限;
	short Date_Time[60];					//日期;   60个
}T_Holiday_Group_Opr,*PT_Holiday_Group_Opr; 



#endif

