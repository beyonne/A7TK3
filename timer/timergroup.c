#include <config.h>
#include <string.h>
#include <unistd.h>
#include <timergroup.h>




int Down_Time_Group(unsigned char *Temp)
{
	int i,Len;
	PT_Time_Group_Opr *TimePara; 					//时间组定义;
	unsigned char DataTemp[135];
	unsigned char *Data;
	Len = Temp[4];
	Data = &Temp[6];
	
	TimePara = malloc(sizeof(T_Time_Group_Opr)*7);

	if(Len == 87&&Data[0]=='W')
	{
		TimePara[Data[1]]->Time_Group_Number = Data[1];
		for(i=0;i<4;i++)
		{
			TimePara[Data[1]]->Time_Section[i].Start_Date   = Data[2+i*21];
			TimePara[Data[1]]->Time_Section[i].Start_Date   = TimePara[Data[1]]->Time_Section[i].Start_Date << 8;
			TimePara[Data[1]]->Time_Section[i].Start_Date  |= Data[3+i*21];
			
			TimePara[Data[1]]->Time_Section[i].End_Date     = Data[4+i*21];
			TimePara[Data[1]]->Time_Section[i].End_Date     = TimePara[Data[1]]->Time_Section[i].End_Date << 8;
			TimePara[Data[1]]->Time_Section[i].End_Date    |= Data[5+i*21];
			
			TimePara[Data[1]]->Time_Section[i].Week         = Data[6+i*21];

			TimePara[Data[1]]->Time_Section[i].Mon         = Data[7+i*21]<<21;
			TimePara[Data[1]]->Time_Section[i].Mon	       |= Data[8+i*21]<<16;
			TimePara[Data[1]]->Time_Section[i].Mon	       |= Data[9+i*21]<<8;
			TimePara[Data[1]]->Time_Section[i].Mon	       |= Data[10+i*21];
			
			TimePara[Data[1]]->Time_Section[i].Start_Time[0]  = Data[11+i*21];
			TimePara[Data[1]]->Time_Section[i].Start_Time[0]  = TimePara[Data[1]]->Time_Section[i].Start_Time[0] << 8;
			TimePara[Data[1]]->Time_Section[i].Start_Time[0] |= Data[12+i*21];
			
			TimePara[Data[1]]->Time_Section[i].End_Time[0]    = Data[13+i*21];
			TimePara[Data[1]]->Time_Section[i].End_Time[0]    = TimePara[Data[1]]->Time_Section[i].End_Time[0] << 8;
			TimePara[Data[1]]->Time_Section[i].End_Time[0]   |= Data[14+i*21];

			TimePara[Data[1]]->Time_Section[i].Start_Time[1]  = Data[15+i*21];
			TimePara[Data[1]]->Time_Section[i].Start_Time[1]  = TimePara[Data[1]]->Time_Section[i].Start_Time[1] << 8;
			TimePara[Data[1]]->Time_Section[i].Start_Time[1] |= Data[16+i*21];
			
			TimePara[Data[1]]->Time_Section[i].End_Time[1]    = Data[17+i*21];
			TimePara[Data[1]]->Time_Section[i].End_Time[1]    = TimePara[Data[1]]->Time_Section[i].End_Time[1] << 8;
			TimePara[Data[1]]->Time_Section[i].End_Time[1]   |= Data[18+i*21];

			TimePara[Data[1]]->Time_Section[i].Start_Time[2]  = Data[19+i*21];
			TimePara[Data[1]]->Time_Section[i].Start_Time[2]  = TimePara[Data[1]]->Time_Section[i].Start_Time[2] << 8;
			TimePara[Data[1]]->Time_Section[i].Start_Time[2] |= Data[20+i*21];
			
			TimePara[Data[1]]->Time_Section[i].End_Time[2]    = Data[21+i*21];
			TimePara[Data[1]]->Time_Section[i].End_Time[2]    = TimePara[Data[1]]->Time_Section[i].End_Time[2] << 8;
			TimePara[Data[1]]->Time_Section[i].End_Time[2]   |= Data[22+i*21];
		}

		Write_TimePara(TimePara);
//		DataTemp[0] = 'W';
//		DataTemp[1] = 'O';
//		Can_Tcp_TX_To_PC(RESPONSE_DOWN_TIMES,DataTemp,2);
		free(TimePara);
		return 0;
	}
	else if(Len == 3)
	{
		if(Data[0]=='R')
		{
			TimePara[Data[1]]->Time_Group_Number = Data[1];
			DataTemp[0] = 'R' ;
			DataTemp[1] = TimePara[Data[1]]->Time_Group_Number ;
		}
		else
		{
			free(TimePara);
			return -1;
		}
		Read_TimePara(TimePara);
		
		for(i=0;i<4;i++)
		{
			DataTemp[2+i*21]  = (TimePara[Data[1]]->Time_Section[i].Start_Date&0xff00)>>8 ;
			DataTemp[3+i*21]  = TimePara[Data[1]]->Time_Section[i].Start_Date&0xff;

			DataTemp[4+i*21]  = (TimePara[Data[1]]->Time_Section[i].End_Date&0xff00)>>8 ;
			DataTemp[5+i*21]  = TimePara[Data[1]]->Time_Section[i].End_Date&0xff;

			DataTemp[6+i*21]  = TimePara[Data[1]]->Time_Section[i].Week;

			DataTemp[7+i*21]  = (TimePara[Data[1]]->Time_Section[i].Mon&0xff000000)>>24;
			DataTemp[8+i*21]  = (TimePara[Data[1]]->Time_Section[i].Mon&0xff0000)>>16;
			DataTemp[9+i*21]  = (TimePara[Data[1]]->Time_Section[i].Mon&0xff00)>>8;
			DataTemp[10+i*21]  = (TimePara[Data[1]]->Time_Section[i].Mon&0xff);
			
			DataTemp[11+i*21]  = (TimePara[Data[1]]->Time_Section[i].Start_Time[0]&0xff00)>>8 ;
			DataTemp[12+i*21]  = TimePara[Data[1]]->Time_Section[i].Start_Time[0]&0xff;

			DataTemp[13+i*21]  = (TimePara[Data[1]]->Time_Section[i].End_Time[0]&0xff00)>>8 ;
			DataTemp[14+i*21]  = TimePara[Data[1]]->Time_Section[i].End_Time[0]&0xff;

			DataTemp[15+i*21] = (TimePara[Data[1]]->Time_Section[i].Start_Time[1]&0xff00)>>8 ;
			DataTemp[16+i*21] = TimePara[Data[1]]->Time_Section[i].Start_Time[1]&0xff;

			DataTemp[17+i*21] = (TimePara[Data[1]]->Time_Section[i].End_Time[1]&0xff00)>>8 ;
			DataTemp[18+i*21] = TimePara[Data[1]]->Time_Section[i].End_Time[1]&0xff;

			DataTemp[19+i*21] = (TimePara[Data[1]]->Time_Section[i].Start_Time[2]&0xff00)>>8 ;
			DataTemp[20+i*21] = TimePara[Data[1]]->Time_Section[i].Start_Time[2]&0xff;

			DataTemp[21+i*21] = (TimePara[Data[1]]->Time_Section[i].End_Time[2]&0xff00)>>8 ;
			DataTemp[22+i*21] = TimePara[Data[1]]->Time_Section[i].End_Time[2]&0xff;
			
			
		}
		//Can_Tcp_TX_To_PC(RESPONSE_DOWN_TIMES,DataTemp,sizeof(Time_Group_Struct)+1)	;//00011001 1000 0001  // 12 12 1 
	}
	else
	{
		DataTemp[0] = 'E';
		//Can_Tcp_TX_To_PC(RESPONSE_DOWN_TIMES,DataTemp,1) ;
	}
	free(TimePara);
	return 0;
	
}



int Write_TimePara(PT_Time_Group_Opr *pt_Time)
{
	FILE *fd;

	if((fd = fopen("time","wb+")) == NULL)
	{
		DBG_PRINTF("Write open file time fail!\n");
		return -1;
	}

	fwrite(pt_Time,sizeof(T_Time_Group_Opr),7,fd);

	fclose(fd);
	return 0;
}

int Read_TimePara(PT_Time_Group_Opr *pt_Time)
{
	FILE *fd;
	
	if((fd = fopen("time","rb+")) == NULL)
	{
		DBG_PRINTF("Read open time file fail!\n");
		return -1;
	}
	
	fread(pt_Time,sizeof(T_Time_Group_Opr),7,fd);

	fclose(fd);
	return 0;
}



