#ifndef _TIMERGROUP_H
#define _TIMERGROUP_H	 


/*ʱ��νṹ*/
typedef struct Time_Section_Opr
{
	unsigned short Start_Date;				//��ʼ������;
	unsigned short End_Date;				//��ֹ������;	
	unsigned char  Week;					//���ڿ���;
	int Mon;					//���ڿ���
	unsigned short Start_Time[3];			//��ʼʱ��;3����
	unsigned short End_Time[3];			//��ֹʱ��;3����
}Time_Section_Opr; 

/*ʱ����ṹ*/
typedef struct Time_Group_Opr
{
	int Time_Group_Number;				//ʱ�������к�;
	Time_Section_Opr Time_Section[4];//ʱ��νṹ; 4���Σ�
}T_Time_Group_Opr,*PT_Time_Group_Opr; 


#endif

