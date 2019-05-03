#include <config.h>
#include <net_manager.h>
#include <string.h>
#include <communication.h>
#include <para.h>
#include <cardmanager.h>
#include <store_manager.h>
#include <sys/reboot.h>
#include <gpio_manager.h>
#include <can_manager.h>
#include <canopen.h>
#include <pthread.h>     // pthread_*()

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>
#include <usart_manager.h>
#include <motion.h>
#include <timer.h>
#include <alloc_manager.h>
#include <canopen.h>

static unsigned char motion_R_rx_timeout_stop=0;		//接收超时标志;
static unsigned char motion_L_rx_timeout_stop=0;		//接收超时标志;

unsigned char ptz_angle[4];		//PTZ旋转角度
//unsigned char arm_angle[32];		//机械手臂关节角度
unsigned char wc_data[4]={0};

unsigned short arm_limit_state;			//限位状态
unsigned char ptz_limit_angle;
unsigned char ptz_lamp_state;
unsigned char a7_lamp_state;
unsigned char camera,lamp_IR,lamp_led,lamp_drv;
unsigned char rx_led_flag=0;

int temperature ,Vbat;
int adccnt;
__GRIPPER_MOTOR     JOINTS[13];

T_senser_Opr senser_data;
T_GasSenserFrame GasSensorData;

static void *Thread_runapp(void *param);




/***********************
 *  
 *  CCITT16 CRC table and function
 *
 *	Liang Ma
 *
 *****************************/

const short CRC16Table[256]={
0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};


static unsigned char  Pro_SOH  = 0xA6;
static unsigned char BK_Pro_SOH 	= 0xA6;
static PT_pcTxTime PcTxTime;

 short Flipper_Position;
 short Flipper_speed;
 short Track_L_speed;
 short Track_R_speed;
 
 unsigned char Flipper_Error; 	//0-ok,  1-no connect
 unsigned char Track_L_Error; 	//0-ok,  1-no connect
 unsigned char Track_R_Error; 	//0-ok,  1-no connect
 unsigned char arm_Error; 		//0-ok,  1-no connect

 unsigned char joint_error[6];
  

static unsigned char From_OCU_ID;

static unsigned char BfCfFlag;			//布防撤防标志位；

int wdt_fd = -1; 	//看门狗

static unsigned char HEX_BCD(unsigned char bcd_data)
{
 	unsigned char hex_data;
    hex_data=(bcd_data/10)*16+bcd_data%10;
	return hex_data;
}


/*
打印日志到文件  ! add
*/
void DBG_PRINTF_txt(const char *str,int val)
{

	char *sql = "%s %d %X \n";
	char sqltemp[256];
	FILE * fp;

	if(strlen(str)<255)
	{
		sprintf(sqltemp,sql,str,val,val);
	}
	else
	{
		sprintf(sqltemp,sql,"error",8,8);
	}
					
	fp = fopen("/opt/log.txt","a+");
	
    //DBG_PRINTF(fp,"%s",sqltemp);

	fclose(fp);
	
}




/********************************************************************************** 
 函 数 名：CRC_Cal
 功    能：CRC校验,异或校验
 说    明：
 入口参数：CRC_buf:待校验的数据包;buf_len:数据长度DeviceSn
 返 回 值：校验结果
**********************************************************************************/
unsigned char CRC_Cal(unsigned char *CRC_Buf,int buf_len)
{
	int	i;
	unsigned char dat = 0x00;
	
	for (i = 0; i < buf_len; i++)
		dat	^= *CRC_Buf++;
	
	return dat;

}

unsigned char CRC_Cal_9(unsigned char *CRC_Buf,int buf_len)
{
	int	i;
	unsigned char dat = 0x00;
	
	for (i = 0; i < buf_len; i++)
	{
		if(i!=9)
		dat	^= *CRC_Buf++;
		else
		CRC_Buf++;
	}
	
	return dat;

}

int check_device_type(int type)
{
	if(type==0x87)
	{
		return 0;
	}
	else if(type==0x88)
	{
		return 0;
	}
	else
		return -1;
	
}

int PcTxTimeMalloc(void)
{
	PcTxTime = malloc(sizeof(T_pcTxTime));
}

void PcTxTimeFree(void)
{
	free(PcTxTime);
}

PT_pcTxTime GetPcTxTime(void)
{
	return PcTxTime;
}



/*
int wdtinit(void)
{
	int timeout;
	wdt_fd = open(WDT, O_WRONLY);
	
	if (wdt_fd == -1) 
	{
		DBG_PRINTF("fail to open "WDT "!\n");
	}
	
	DBG_PRINTF(WDT " is opened!\n");
	
	timeout = 3;
	
	ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);
	ioctl(wdt_fd, WDIOC_GETTIMEOUT, &timeout);
	
	DBG_PRINTF("The timeout was is %d seconds\n", timeout);


	return 0;
}

void wdting(void)
{
	write(wdt_fd, "\0", 1);
}
*/


static void *Thread_runapp(void *param)
{
	unsigned char *temp=param;

	system(temp);
}



int Table_CRC16(const unsigned char* pMSG, unsigned char length)
{
	unsigned char CRCindex,j;
	unsigned short CRC16temp;
	CRC16temp =0;
	
	for(j=0;j<length;j++)
	{
		CRCindex = (CRC16temp>>8)^*pMSG;
		CRC16temp <<=8;
		CRC16temp ^= CRC16Table[CRCindex];
		pMSG++;
	}
	return(CRC16temp);

}
 
 
 //AMC sinngle loop crc
 void crccheck(unsigned char data, unsigned short *accumulator, unsigned short *crctable)
{
	*accumulator = ( *accumulator << 8 ) ^ crctable[( *accumulator >> 8) ^ data];
}

unsigned short crcCalate(unsigned char *string,unsigned int start,unsigned int end)
{
	unsigned int i = 0;
	unsigned short accumulator = 0;
	
	for (i = start; i<end; i++)
	{ 
		crccheck(string[i],&accumulator,CRC16Table);
	}
	return accumulator;	
}


//读取Head底板的数据；
int ReadHeadBuf_bak(int fd,unsigned char *DataAddrs)
{
	int i;
	PT_MemOpr ptTmp;
	unsigned short crcval;
	int len=0;
	
	ptTmp = GetMemData(fd);

	if(ptTmp==NULL)
	return -1;

	if(ptTmp->EndAddr==ptTmp->HeadAddr)
		return 0;

	//DBG_PRINTF("ptTmp->EndAddr:%d,HeadAddr:%d\n",ptTmp->EndAddr,ptTmp->HeadAddr);
	//DBG_PRINTF("ptTmp->DataLen:%d.\n",ptTmp->DataLen);

	while(1)
	{
		if(ptTmp->DataAddr[ptTmp->EndAddr]==0x48)				//脜脨露脧脰隆脥路
		{
			if(ptTmp->DataAddr[ptTmp->EndAddr+1]==0x44)			//脜脨露脧脰隆脥路
			{
				if(ptTmp->DataLen>=FROM_HEAD_LENGTH)
				{	
					len = FROM_HEAD_LENGTH;
					
					crcval = ptTmp->DataAddr[ptTmp->EndAddr+(len-2)]<<8;
					crcval |= ptTmp->DataAddr[ptTmp->EndAddr+(len-1)];
					
					if(crcval == Table_CRC16(&ptTmp->DataAddr[ptTmp->EndAddr],len-2))
					{
						memcpy(DataAddrs,&ptTmp->DataAddr[ptTmp->EndAddr],len);
						
						ptTmp->EndAddr += (len);
						ptTmp->DataLen -= (len);

						if(ptTmp->EndAddr >= (MEMSIZE))
						{
							ptTmp->EndAddr = 0;
						}
						
						memset(&ptTmp->DataAddr[ptTmp->EndAddr-(len)],0,len);
						if(len==FROM_HEAD_LENGTH)
						{
							//DBG_PRINTF("head!\n");
							return len;
						}
						else return -1;
						
					}
				}
				else
				{
					return -1;
				}
			}
		}
		
		ptTmp->EndAddr++;
		ptTmp->DataLen--;
		
		if(ptTmp->EndAddr >= (MEMSIZE))
			ptTmp->EndAddr = 0;

		if(ptTmp->EndAddr==ptTmp->HeadAddr)
		return -1;
	}
	return -1;
	
}

int Read_Byte(int fd)
{
	int r_data=-1;
	PT_MemOpr ptTmp;

	ptTmp = GetMemData(fd);
	if(ptTmp==NULL)
	{
		return r_data;
	}
	
	if(ptTmp->EndAddr==ptTmp->HeadAddr)
		return r_data;
		
	r_data = ptTmp->DataAddr[ptTmp->EndAddr];
	
	ptTmp->EndAddr++;
	if(ptTmp->EndAddr>=MEMSIZE)
		ptTmp->EndAddr=0;
		
	return r_data;
}


//从HEAD板读取数据
int ReadHeadBuf(int fd,PT_TCP_Rx_Opr Rx_STR)
{
	int i;
	
	unsigned short crcval;
	int len=0;
	int usart_no = 1;
	unsigned char  rx_len;
 	unsigned short cal_crc1,data_crc1,cal_crc2,data_crc2;
 	short usart_rx;
	
	
	//u8 *DataTemp;
	
	//DataTemp = (u8*)malloc(sizeof(u8)*FRAME_SIZE);
  	usart_rx = Read_Byte(fd);

	if(usart_rx<0)
	{
		//free(DataTemp);

		return 0;
	}	
	
	i = Rx_STR[usart_no-1].Rx_Bot;
	Rx_STR[usart_no-1].Rx_Data[i]=usart_rx;
	
	switch(i)
	{
		case 0:
		{
		   	//Rx_STR[usart_no-1].Rx_Len=0;
		   	//Rx_STR[usart_no-1].Rx_OK=0;
		   
			if(usart_rx==HEAD_BOARD_HAEDDATA)	
			{
				Rx_STR[usart_no-1].Rx_Bot++; 
				//BK_Pro_SOH=usart_rx;
			}	  
			break;
	   }
		case 1:
		{	
			if(usart_rx==HEAD_BOARD_HAEDDATA1)	
			{
				Rx_STR[usart_no-1].Rx_Bot++; 
				//BK_Pro_SOH=usart_rx;
			}	
			break;
		}
		case 2:
		case 3:
		case 4:
		case 5:
	   	{
		    Rx_STR[usart_no-1].Rx_Bot++; 
			break;
	   	}
	   	
		default:
		{
		    if(Rx_STR[usart_no-1].Rx_Bot>24)  	// 修改 1
			{
			 	Rx_STR[usart_no-1].Rx_Bot=0;
			 	DBG_PRINTF("Head error1\n"); 
			 	break;
			}
		}
	
		if(i<22)          		// 修改 2
	    {
	     	Rx_STR[usart_no-1].Rx_Bot++;
	    }
	    else   															//数据接收结束
	    {
	         rx_len = i+1;
	         
		     cal_crc1 = Table_CRC16(&Rx_STR[usart_no-1].Rx_Data[0],rx_len-2);
		     //DBG_PRINTF("---- ok\n"); 
		     data_crc1 = Rx_STR[usart_no-1].Rx_Data[(rx_len-2)]<<8;
			 data_crc1 |= Rx_STR[usart_no-1].Rx_Data[(rx_len-1)];
		     
	         if(cal_crc1 != data_crc1)
		     {
				Rx_STR[usart_no-1].Rx_OK=0;
				Rx_STR[usart_no-1].Rx_Bot=0;
				DBG_PRINTF("Head error\n"); 
				//SendtoAMC(Rx_STR[usart_no-1].Rx_Data[2]&0xFC,8,NULL,0);
				break; 
			 }
		     else
		     {
				Rx_STR[usart_no-1].Rx_OK=1;
				Rx_STR[usart_no-1].Rx_Len=rx_len;
				Rx_STR[usart_no-1].Rx_Bot=0;
				//DBG_PRINTF("Head ok\n"); 
				//free(SRAMIN,DataTemp);
				return 0;	   			     
			 }
	    }
	
	    break;
	}

	return 1;	   
	
}



//读取翻转臂和轮子的数据；
int ReadMotionBuf(int fd,PT_TCP_Rx_Opr Rx_STR)
{
	int i,j;
	
	unsigned short crcval;
	int len=0;
	int usart_no = 1;
	unsigned char  rx_len;
 	unsigned short cal_crc1,data_crc1,cal_crc2,data_crc2;
 	short usart_rx;
	
	
	//u8 *DataTemp;
	
	//DataTemp = (u8*)malloc(sizeof(u8)*FRAME_SIZE);
  	usart_rx = Read_Byte(fd);

	if(usart_rx<0)
	{
		//free(DataTemp);

		return 0;
	}	
	//DBG_PRINTF("%X %d\n",usart_rx,Rx_STR[usart_no-1].Rx_Bot);
	i = Rx_STR[usart_no-1].Rx_Bot;
	Rx_STR[usart_no-1].Rx_Data[i]=usart_rx;
	
	switch(i)
	{
		case 0:
		{
		   	//Rx_STR[usart_no-1].Rx_Len=0;
		   	//Rx_STR[usart_no-1].Rx_OK=0;
		   
			if(usart_rx==AMC_Pro_SOH)	
			{
				Rx_STR[usart_no-1].Rx_Bot++; 
				//BK_Pro_SOH=usart_rx;
			}	  
			break;
	   }
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
	   	{
		    Rx_STR[usart_no-1].Rx_Bot++; 
			break;
	   	}
	   	
		default:
		{
		    if(((Rx_STR[usart_no-1].Rx_Data[5]*2)+8+(!!Rx_STR[usart_no-1].Rx_Data[5]*2))>32)  	// 修改 1
			{
			 	Rx_STR[usart_no-1].Rx_Bot=0;
			 	break;
			}
		}
		
		if((Rx_STR[usart_no-1].Rx_Data[2]&0x01)==0x01)	//读取数据时，数据部分为空无需校验；
		{
			if(i<7)          		// 修改 2
		    {
		     	Rx_STR[usart_no-1].Rx_Bot++;
		    }
		    else   															//数据接收结束
		    {
		         rx_len = i+1;
		         
			     cal_crc1 = Table_CRC16(&Rx_STR[usart_no-1].Rx_Data[0],6);
			     
			     data_crc1 = (Rx_STR[usart_no-1].Rx_Data[6]<<8)+Rx_STR[usart_no-1].Rx_Data[7];
			     
		         if(cal_crc1 != data_crc1)
			     {
					Rx_STR[usart_no-1].Rx_OK=0;
					Rx_STR[usart_no-1].Rx_Bot=0;
					DBG_PRINTF("motion error3\n");
					//SendtoAMC(Rx_STR[usart_no-1].Rx_Data[2]&0xFC,8,NULL,0);
					break; 
				 }
			     else
			     {
					Rx_STR[usart_no-1].Rx_OK=1;
					Rx_STR[usart_no-1].Rx_Len=rx_len;
					Rx_STR[usart_no-1].Rx_Bot=0;
					DBG_PRINTF("motion ok\n"); 
					//free(SRAMIN,DataTemp);
					return 0;	   			     
				 }
		    }
		}
		else			//其他命令
		{
			if(i<((Rx_STR[usart_no-1].Rx_Data[5]*2)+7+(!!Rx_STR[usart_no-1].Rx_Data[5]*2)))          		// 修改 2
		    {
		     	Rx_STR[usart_no-1].Rx_Bot++;
		    }
		    else   															//数据接收结束
		    {
		         rx_len = i+1;
		         
			     cal_crc1 = Table_CRC16(&Rx_STR[usart_no-1].Rx_Data[0],6);
			     
			     data_crc1 = (Rx_STR[usart_no-1].Rx_Data[6]<<8)+Rx_STR[usart_no-1].Rx_Data[7];
			     
		         if(cal_crc1 != data_crc1)	//第一次校验不通过
			     {
					  Rx_STR[usart_no-1].Rx_OK=0;
				      Rx_STR[usart_no-1].Rx_Bot=0;

				      for(j=0;j<rx_len;j++)
				      {
				      		DBG_PRINTF("%X ",Rx_STR[usart_no-1].Rx_Data[j]);
				      }
				      
				      DBG_PRINTF("\nmotion error\n");
				      //SendtoAMC(Rx_STR[usart_no-1].Rx_Data[2]&0xFC,8,NULL,0);
				      break;
				      
				 }
			     else			//帧头校验通过
			     {
			     	if(Rx_STR[usart_no-1].Rx_Data[5]>0)
			     	{
				     	cal_crc2 = Table_CRC16(&Rx_STR[usart_no-1].Rx_Data[8],Rx_STR[usart_no-1].Rx_Data[5]*2);
				     
				     	data_crc2 = (Rx_STR[usart_no-1].Rx_Data[8+(Rx_STR[usart_no-1].Rx_Data[5]*2)]<<8)+(Rx_STR[usart_no-1].Rx_Data[9+(Rx_STR[usart_no-1].Rx_Data[5]*2)]);
				     	
				     	if(cal_crc2==data_crc2)//数据校通过
				     	{
							Rx_STR[usart_no-1].Rx_OK=1;
							Rx_STR[usart_no-1].Rx_Len=rx_len;
							Rx_STR[usart_no-1].Rx_Bot=0;
							//DBG_PRINTF("motion ok\n");
							//free(SRAMIN,DataTemp);
							return 0;	   
					    }
					    else		//数据校验bu通过
					    {
							Rx_STR[usart_no-1].Rx_OK=0;
					      	Rx_STR[usart_no-1].Rx_Bot=0;
					      	//SendtoAMC(Rx_STR[usart_no-1].Rx_Data[2]&0xFC,8,NULL,0);
					      	DBG_PRINTF("motion error1\n");
					      	break;
					    }
				    }
				    else
				    {
						Rx_STR[usart_no-1].Rx_OK=1;
						Rx_STR[usart_no-1].Rx_Len=rx_len;
						Rx_STR[usart_no-1].Rx_Bot=0;
						//DBG_PRINTF("motion ok\n");
						//free(SRAMIN,DataTemp);
						return 0;	   
				    }
			    }
		    }
	    }
	    break;
	}
	return 1;	   
}

unsigned char LRC_Check(unsigned char *data,unsigned char length)
{
	unsigned char i;
	unsigned int k;
	unsigned char result;
	unsigned char lrcdata[length];
	for(i=1;i<length+1;i++)
	{
		if(data[i]>0x40)
			lrcdata[i-1]=data[i]-0x41+10;
		else
			lrcdata[i-1]=data[i]-0x30;
	}
	k=0;
	for(i=0;i<length/2;i++)
	{
		k+=(lrcdata[2*i]*16+lrcdata[2*i+1]);
	}
	k=k%256;
	k=256-k;
	result=k%256;
	return result;
}

//从ARM臂读取数据
//气体传感器报文格式解析
int ReadArmBuf(int fd,PT_TCP_Rx_Opr Rx_STR)
{
	int i;
	unsigned short crcval;
	int len=0;
	int usart_no = 1;
	unsigned char  rx_len;
 	unsigned char cal_crc1,data_crc1;
 	short usart_rx;
	//u8 *DataTemp;
	//DataTemp = (u8*)malloc(sizeof(u8)*FRAME_SIZE);
  	usart_rx = Read_Byte(fd);
	if(usart_rx<0)
	{
		//free(DataTemp);
		return 0;
	}
	i = Rx_STR[usart_no-1].Rx_Bot;
	Rx_STR[usart_no-1].Rx_Data[i]=usart_rx;
//	DBG_PRINTF("%02X \n",Rx_STR[usart_no-1].Rx_Data[i]);
	switch(i)
	{
		case 0://0x3A
		   	//Rx_STR[usart_no-1].Rx_Len=0;
		   	//Rx_STR[usart_no-1].Rx_OK=0;
			if(usart_rx==ARMRXHAEDDATA)
			{
				Rx_STR[usart_no-1].Rx_Bot++;
				//BK_Pro_SOH=usart_rx;
			}
			else
			{
				Rx_STR[usart_no-1].Rx_Bot = 0;
			}
			break;
		case 1://地址
			GasSensorData.Arr = (Rx_STR[usart_no-1].Rx_Data[i] - 0x30)<<4;
		    Rx_STR[usart_no-1].Rx_Bot++;
			break;
		case 2://地址
			GasSensorData.Arr += (Rx_STR[usart_no-1].Rx_Data[i] - 0x30);
		    Rx_STR[usart_no-1].Rx_Bot++;
			break;
		case 3://指令
			GasSensorData.Cmd = (Rx_STR[usart_no-1].Rx_Data[i] - 0x30)<<4;
		    Rx_STR[usart_no-1].Rx_Bot++;
			break;
		case 4://指令
			GasSensorData.Cmd += (Rx_STR[usart_no-1].Rx_Data[i] - 0x30);
		    Rx_STR[usart_no-1].Rx_Bot++;
			break;
		case 5://长度
			GasSensorData.Len = (Rx_STR[usart_no-1].Rx_Data[i] - 0x30)<<4;
		    Rx_STR[usart_no-1].Rx_Bot++;
			break;
		case 6://长度
			GasSensorData.Len += (Rx_STR[usart_no-1].Rx_Data[i] - 0x30);
		    Rx_STR[usart_no-1].Rx_Bot++; 
			break;
		default://数据
//			DBG_PRINTF("GasSensorData.Len=%d",GasSensorData.Len);
		    if((Rx_STR[usart_no-1].Rx_Data[6]-0x30)>12) //修改１
		    {
			 	Rx_STR[usart_no-1].Rx_Bot=0;
			 	GasSensorData.Len = 0;
//			 	DBG_PRINTF("Arm error2\n");
			 	break;
			}
//			if(i<((Rx_STR[usart_no-1].Rx_Data[6]-0x30)+12)) //修改２
		    if(usart_rx != 0x0D)
		    {
		    	Rx_STR[usart_no-1].Rx_Bot++;
		    }
			else
			{//接收到回车
//				DBG_PRINTF("catch the Enter\r\n");
				if(i<=((GasSensorData.Len)*2+6+2)) //修改3
				{//数据字节太少
					Rx_STR[usart_no-1].Rx_Bot=0;
					GasSensorData.Len = 0;
//					DBG_PRINTF("Frame error\n");
					break;
				}
				else
				{
					rx_len = i;
					//DBG_PRINTF("a %X,%d,%d\n",cal_crc1,Rx_STR[usart_no-1].Rx_Data[6],i);
					cal_crc1 = LRC_Check(&Rx_STR[usart_no-1].Rx_Data[0],rx_len-2);
					//DBG_PRINTF("---- ok\n");
					StrToHex(&data_crc1,&Rx_STR[usart_no-1].Rx_Data[11],1);
					//DBG_PRINTF("Arm %X,%X,%d\n",cal_crc1,data_crc1,rx_len);
					if(cal_crc1 != data_crc1)
					{
						Rx_STR[usart_no-1].Rx_OK=0;
						Rx_STR[usart_no-1].Rx_Bot=0;
						GasSensorData.Len = 0;
//						DBG_PRINTF("Arm error1\n");
						//SendtoAMC(Rx_STR[usart_no-1].Rx_Data[2]&0xFC,8,NULL,0);
						break;
					}
					else
					{
//						DBG_PRINTF("Gas Sensor Frame ok\n");
						Rx_STR[usart_no-1].Rx_OK=1;
						Rx_STR[usart_no-1].Rx_Len=rx_len;
						Rx_STR[usart_no-1].Rx_Bot=0;
						GasSensorData.Len = 0;
						//DBG_PRINTF("Arm ok\n");
						//free(SRAMIN,DataTemp);
						return 0;
					}
				}
			}
	    break;
	}
	return 1;
}


//从ARM臂读取数据
int ReadArmBuf_bak(int fd,unsigned char *DataAddrs)
{
	int i;
	PT_MemOpr ptTmp;
	unsigned short crcval;
	int len=0;
	
	ptTmp = GetMemData(fd);

	if(ptTmp==NULL)
	return -1;

	if(ptTmp->EndAddr==ptTmp->HeadAddr)
		return 0;

	//DBG_PRINTF("ptTmp->EndAddr:%d,HeadAddr:%d\n",ptTmp->EndAddr,ptTmp->HeadAddr);
	//DBG_PRINTF("ptTmp->DataLen:%d.\n",ptTmp->DataLen);

	while(1)
	{
		if(ptTmp->DataAddr[ptTmp->EndAddr]==ARMRXHAEDDATA)				//脜脨露脧脰隆脥路
		{
			if(ptTmp->DataAddr[ptTmp->EndAddr+1]==ARMRXHAEDDATA1)			//脜脨露脧脰隆脥路
			{
				if(ptTmp->DataLen>=ptTmp->DataAddr[ptTmp->EndAddr+2])
				{	
					len = ptTmp->DataAddr[ptTmp->EndAddr+2];
					
					crcval = ptTmp->DataAddr[ptTmp->EndAddr+(len-2)]<<8;
					crcval |= ptTmp->DataAddr[ptTmp->EndAddr+(len-1)];
					
					if(crcval == Table_CRC16(&ptTmp->DataAddr[ptTmp->EndAddr],len-2))
					{
						memcpy(DataAddrs,&ptTmp->DataAddr[ptTmp->EndAddr],len);
						
						ptTmp->EndAddr += (len);
						ptTmp->DataLen -= (len);

						if(ptTmp->EndAddr >= (MEMSIZE))
						{
							ptTmp->EndAddr = 0;
						}
						//DBG_PRINTF("arm len:%d!\n",len);
						memset(&ptTmp->DataAddr[ptTmp->EndAddr-(len)],0,len);

						return len;
					}
				}
				else
				{
					return -1;
				}
			}
		}
		
		ptTmp->EndAddr++;
		ptTmp->DataLen--;
		
		if(ptTmp->EndAddr >= (MEMSIZE))
			ptTmp->EndAddr = 0;

		if(ptTmp->EndAddr==ptTmp->HeadAddr)
		return -1;
	}
	return -1;
}



//从光纤，WIFI，RF处读取OCU的数据
int ReadOcuBuf_bak(int fd,unsigned char *DataAddrs)
{
	int i;
	PT_MemOpr ptTmp;
	unsigned short crcval;
	int len=0;
	
	ptTmp = GetMemData(fd);

	if(ptTmp==NULL)
	return -1;

	if(ptTmp->EndAddr==ptTmp->HeadAddr)
		return 0;

	//DBG_PRINTF("E:%d,H:%d\n",ptTmp->EndAddr,ptTmp->HeadAddr);
	//DBG_PRINTF("Len:%d.\n",ptTmp->DataLen);

	while(1)
	{
		if(ptTmp->DataAddr[ptTmp->EndAddr]==OCURXHAEDDATA)				//脜脨露脧脰隆脥路
		{
			if(ptTmp->DataAddr[ptTmp->EndAddr+1]==OCURXHAEDDATA1)			//脜脨露脧脰隆脥路
			{
				if(ptTmp->DataLen>=ptTmp->DataAddr[ptTmp->EndAddr+3])
				{	
					len = ptTmp->DataAddr[ptTmp->EndAddr+3];
					
					crcval = ptTmp->DataAddr[ptTmp->EndAddr+(len-2)]<<8;
					crcval |= ptTmp->DataAddr[ptTmp->EndAddr+(len-1)];
					
					if(crcval == Table_CRC16(&ptTmp->DataAddr[ptTmp->EndAddr],len-2))
					{
						memcpy(DataAddrs,&ptTmp->DataAddr[ptTmp->EndAddr],len);
						
						ptTmp->EndAddr += (len);
						ptTmp->DataLen -= (len);

						if(ptTmp->EndAddr >= (MEMSIZE))
						{
							ptTmp->EndAddr = 0;
						}
						//DBG_PRINTF("ocu ok!\n");
						memset(&ptTmp->DataAddr[ptTmp->EndAddr-(len)],0,len);

						return len;
					}
				}
				else
				{
					return -1;
				}
			}
		}
		
		ptTmp->EndAddr++;
		ptTmp->DataLen--;
		
		if(ptTmp->EndAddr >= (MEMSIZE))
			ptTmp->EndAddr = 0;

		if(ptTmp->EndAddr==ptTmp->HeadAddr)
		return -1;
	}
	return -1;
	
}


//从ARM臂读取数据
int ReadOcuBufdata(int fd,PT_TCP_Rx_Opr Rx_STR)
{
	int i;
	
	unsigned short crcval;
	int len=0;
	int usart_no = 1;
	unsigned char  rx_len;
 	unsigned short cal_crc1,data_crc1,cal_crc2,data_crc2;
 	short usart_rx;
	
	
	//u8 *DataTemp;
	
	//DataTemp = (u8*)malloc(sizeof(u8)*FRAME_SIZE);
  	usart_rx = Read_Byte(fd);

	if(usart_rx<0)
	{
		//free(DataTemp);

		return 0;
	}	
	
	i = Rx_STR[usart_no-1].Rx_Bot;
	Rx_STR[usart_no-1].Rx_Data[i]=usart_rx;
	
	switch(i)
	{
		case 0:
		{
		   	//Rx_STR[usart_no-1].Rx_Len=0;
		   	//Rx_STR[usart_no-1].Rx_OK=0;
		   
			if(usart_rx==OCURXHAEDDATA)
			{
				Rx_STR[usart_no-1].Rx_Bot++; 
				//BK_Pro_SOH=usart_rx;
			}	  
			break;
	   }
		case 1:
		{	
			if(usart_rx==OCURXHAEDDATA1)	
			{
				Rx_STR[usart_no-1].Rx_Bot++; 
				//BK_Pro_SOH=usart_rx;
			}	
			break;
		}
		case 2:
		case 3:
		case 4:
		case 5:
	   	{
		    Rx_STR[usart_no-1].Rx_Bot++; 
			break;
	   	}
	   	
		default:
		{
		    if(Rx_STR[usart_no-1].Rx_Data[3]>256)  	// 修改 1
			{
			 	Rx_STR[usart_no-1].Rx_Bot=0;
			 	DBG_PRINTF("ocu error2\n"); 
			 	break;
			}
		}
	
		if(i<(Rx_STR[usart_no-1].Rx_Data[3]-1))          		// 修改 2
	    {
	     	Rx_STR[usart_no-1].Rx_Bot++;
	    }
	    else   															//数据接收结束
	    {
	         rx_len = i+1;
	         
		     cal_crc1 = Table_CRC16(&Rx_STR[usart_no-1].Rx_Data[0],rx_len-2);
		     //DBG_PRINTF("---- ok\n"); 
		     data_crc1 = Rx_STR[usart_no-1].Rx_Data[(rx_len-2)]<<8;
			 data_crc1 |= Rx_STR[usart_no-1].Rx_Data[(rx_len-1)];
		     
	         if(cal_crc1 != data_crc1)
		     {
				Rx_STR[usart_no-1].Rx_OK=0;
				Rx_STR[usart_no-1].Rx_Bot=0;
				DBG_PRINTF("ocu error1\n"); 
				//SendtoAMC(Rx_STR[usart_no-1].Rx_Data[2]&0xFC,8,NULL,0);
				break; 
			 }
		     else
		     {
				Rx_STR[usart_no-1].Rx_OK=1;
				Rx_STR[usart_no-1].Rx_Len=rx_len;
				Rx_STR[usart_no-1].Rx_Bot=0;
				//intf("Arm ok\n"); 
				//free(SRAMIN,DataTemp);
				return 0;	   			     
			 }
	    }
	
	    break;
	}

	return 1;	   
	
}


int Tack_To_PTZ(int fd,unsigned char* buffer)
{
	int temp_int;
	static unsigned char To_Head_ID;
	static unsigned char ptzdata[4];
	//mfrc 63102 hn
	unsigned char camera,lamp_IR,lamp_led,lamp_drv;
	
	DBG_PRINTF("-------------------ptz:%X---------------\n",buffer[3]);
	//DBG_PRINTF("ptz_RUN\n");
	if(buffer[3]==0x3c)			//数据命令判断
		ptzdata[0] = 1;
	else if(buffer[3]==0x40)
		ptzdata[0] = 2;
	else if(buffer[3]==0x01)
	{
		DBG_PRINTF("-------------------crc");
		can0_SDO_write_frame(PTZ_ID,0x0008,0,ptzdata,4);			//PTZ控制；角度读取，旋转控制；
	}
	else 
		ptzdata[0] = 0;

	//if(ptzdata[0]!=0)
	//can0_SDO_write_frame(PTZ_SSG_ID,SSG_CMD,0,ptzdata,1);			//ptz升降杆控制；

	ptzdata[0] = buffer[6];
	ptzdata[1] = buffer[7];
	ptzdata[2] = buffer[8];
	ptzdata[3] = buffer[9];

	can0_SDO_write_frame(PTZ_ID,PTZ_READ_CMD,0,ptzdata,4);			//PTZ控制；角度读取，旋转控制；
	return 0;
}


//水泡发射板
int Tack_To_water_cannon(unsigned char* buffer)
{
	unsigned char headdata[8];
	int can_id;
	PT_CanDeviceOpr ptDevTmp;

	ptDevTmp = GetCanOpr("can0");

	can_id = 0x33 + 0x600;

	headdata[0] = 0x68;
	headdata[1] = 0x02;
	headdata[2] = buffer[2]; //Laser
	headdata[3] = buffer[3]; //Firing Control
	headdata[4] = buffer[4]; //Firing ch1 Duration
	headdata[5] = buffer[5];
	headdata[6] = buffer[6];
	headdata[7] = buffer[7];

    //DBG_PRINTF("FiringControl:%02X %02X %02X %02X %02X %02X\r\n",buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
	if(can_tx_data(ptDevTmp->fd,can_id,headdata,8)!=0)
		return -1;
	usleep(1000);
	return 0;
	//can0_SDO_write_frame(WATER_CANNON_ID,PTZ_READ_CMD,0,headdata,4);
}

int ReadOcuBuf(int fd,PT_TCP_Rx_Opr OcuRx)		//head板数据处理
{
	PT_UsartDeviceOpr ptHead;
	int i;
	
	for(i=0;i<1024;i++)
	{
		if(!ReadOcuBufdata(fd,OcuRx))
		{
			break;
		}
	}

	if(!OcuRx->Rx_OK)
	{
		return -1;
	}
	else
	{
		OcuRx->Rx_OK = 0;
		//DBG_PRINTF("ocu:%d,%d,%d\n",OcuRx->Rx_Bot,OcuRx->Rx_OK,OcuRx->Rx_Len);
		return 0;
	}
}


int Tack_To_Head(int fd,unsigned char* buffer)
{
	int temp_int;
	static unsigned char To_Head_ID;
	unsigned char G_MSG_To_Head[32];

	// Prepare the header	
	G_MSG_To_Head[0] = HEAD_BOARD_HAEDDATA;
	G_MSG_To_Head[1] = HEAD_BOARD_HAEDDATA1;
	G_MSG_To_Head[2] = To_Head_ID;
	To_Head_ID++;
	
	// copy from OCU frame;
	memcpy(&G_MSG_To_Head[3],buffer,9);// 3 header and 2 CCITT
	// CCITT 
	temp_int = Table_CRC16(G_MSG_To_Head,TO_HEAD_LENGTH-2);
	G_MSG_To_Head[12] = (unsigned char) (temp_int>>8);     // High 
	G_MSG_To_Head[13] = (unsigned char) (temp_int & 0xff); // Low

	UsartSendDatafd(fd,G_MSG_To_Head,TO_HEAD_LENGTH);
	return 1;
}

//Command Data:  1 = no data, 2 = have data
unsigned char reqMeasure(unsigned char Net_ID, _Cmd_Data d)
{
	return (Net_ID<<2) + d;
 ///   return d;
}


//wordlen是双字节
//发送给AMC数据封装
int A7_SendtoAMC(int fd,unsigned char ID,unsigned char CB,unsigned char state,int data,unsigned char worddatalen)
{
	unsigned char sendtemp[50];
	unsigned short crcdata,crcdata1;
	int len=8;
	int i;

	sendtemp[0] = 0xA5;
	sendtemp[1] = ID;
	sendtemp[2] = CB;
	sendtemp[3] = state;
	sendtemp[4] = 0x00;
	sendtemp[5] = worddatalen;

	crcdata = crcCalate(sendtemp,0,6);

	sendtemp[6] = crcdata>>8;
	sendtemp[7] = crcdata;

	if(worddatalen>0)
	{
		//memcpy(&sendtemp[8],data,worddatalen*2);
		sendtemp[8] = data;
		sendtemp[9] = data>>8;
		sendtemp[10] = data>>16;
		sendtemp[11] = data>>24;
		
		crcdata1 = crcCalate(&sendtemp[8],0,worddatalen*2);
		sendtemp[8+worddatalen*2] = crcdata1>>8;
		sendtemp[9+worddatalen*2] = crcdata1;
		len = 8+(worddatalen*2)+(!!worddatalen*2);
	}
	//for(i=0;i<len;i++)
	//printf("%X ",sendtemp[i]);
	//printf("\n");
	UsartSendDatafd(fd,sendtemp,len);
	//uartsend(UART_AMC,sendtemp,len);
	return 0;
}

//发送数据给AMC
int AMC_Pack(int fd,JointId jointID)
{
	A7_SendtoAMC(fd,JOINTS[jointID].NET_ID,reqMeasure(JOINTS[jointID].NET_ID, 2),0x45,JOINTS[jointID].CMD_VELOCITY*JOINTS[jointID].CMD_SCALE,2);	//速度值发送
	//usleep(4000);
	//A7_SendtoAMC(fd,JOINTS[jointID].NET_ID,reqMeasure(jointID, 1),0x1A,JOINTS[jointID].CMD_VELOCITY*JOINTS[jointID].CMD_SCALE,2);	//角度值读取数据部分不需要;
	return 0;
}
//发送数据给AMC
int AMC_Pack_angle(int fd,JointId jointID)
{
	//A7_SendtoAMC(fd,JOINTS[jointID].NET_ID,reqMeasure(jointID, 2),0x45,JOINTS[jointID].CMD_VELOCITY*JOINTS[jointID].CMD_SCALE,2);	//速度值发送
	//usleep(4000);
	A7_SendtoAMC(fd,JOINTS[jointID].NET_ID,reqMeasure(JOINTS[jointID].NET_ID, 1),0x1A,JOINTS[jointID].CMD_VELOCITY*JOINTS[jointID].CMD_SCALE,2); //角度值读取数据部分不需要;
	return 0;
}

int AMC_SetCamera_Power(int fd,unsigned char *buff )
{
    unsigned int CRC;
	unsigned char cmd[12]={0xA4,'c',0x01,0x00,0x00,0x02,0x01,0x47,0x00,0x00,0x00,0x00};
	cmd[1]= 'c'; // 0x63
	cmd[2] =0x00; //set command
	CRC=crcCalate(cmd,0, 6);
	cmd[6] = (unsigned char) (CRC>>8);
	cmd[7] = (unsigned char)(CRC&0x00ff);
    cmd[8] = buff[0];
    cmd[9] = buff[1];
    CRC=crcCalate(cmd,8, 10);
    cmd[10] = (unsigned char) (CRC>>8);
	cmd[11] = (unsigned char)(CRC&0x00ff);
	UsartSendDatafd(fd,cmd,12);
	//print(UART_AMC,cmd,12);
 //   delayMs(1);
    return 1;
}

static  int led_sent_count  = 0;
static unsigned char io_sat_pre = 0xff;
unsigned short Digital_WR(int fd,unsigned int OutputIOs)
{
    unsigned char buff[2]={1,1};
    unsigned char  io_sat = (OutputIOs>>20) & 0x03;
    if( io_sat_pre != io_sat )
    {
        io_sat_pre = io_sat;
        led_sent_count = 0;
    }
    else
    {
        if(led_sent_count > 10)
            return ;
        led_sent_count++;
    }
    switch(io_sat)
    {
        case 0:buff[0] = 1;buff[1] = 1; break;// IR open ， LED  open
        case 1:buff[0] =0;buff[1] =1;break;//IR  OFF  led open
        case 2:buff[0] =1;buff[1] =0;break ;//IR open  led off
        default:buff[0] =0;buff[1] =0;break ;//all close        
    }
    //delayMs(2);//必须大于2ms 驱动器才能工作
    AMC_SetCamera_Power(fd,buff);
    //we don't care about replay for instance
    return 0;
}
//Enable Joint
void AMC_Enable(int fd,JointId jointID)
{
	unsigned int CRC;
	unsigned char cmd[12]=
		{0xA5,0x3F,0x02,0x01,0x00,0x01,0x01,0x47,
			0x00,0x00,0x00,0x00};
        unsigned char ID = JOINTS[jointID].NET_ID;
        _Cmd_Data CD = HaveData;
	cmd[1]=ID;
	cmd[2] = reqMeasure(ID,CD);
	CRC=crcCalate(cmd,0, 6);
	cmd[6] = (unsigned char) (CRC>>8);
	cmd[7] = (unsigned char)(CRC&0x00ff);
	UsartSendDatafd(fd,cmd,12);
    //usleep(1000);
	//print(UART_OCU,cmd,12);//debug
}

//Disable Joint
void AMC_Disable(int fd,JointId jointID)
{
	unsigned int CRC;
	unsigned char cmd[12]={0xA5,0x3F,0x02,0x01,0x00,0x01,0x01,0x47,0x01,0x00,0x33,0x31};
	unsigned char ID = JOINTS[jointID].NET_ID;
	_Cmd_Data CD = HaveData;
	cmd[1]=ID;
	cmd[2] = reqMeasure(ID,CD);
	CRC=crcCalate(cmd,0, 6);
	cmd[6] = (unsigned char) (CRC>>8);
	cmd[7] = (unsigned char)(CRC&0x00ff);
	//print(UART_AMC,cmd,12);
	UsartSendDatafd(fd,cmd,12);
    //usleep(1000);
	//print(UART_OCU,cmd,12);//debug
}

void AMC_WriteAccess(int fd,JointId jointID)
{
	unsigned int CRC;
	unsigned char cmd[12]={0xA5,0x3F,0x02,0x07,0x00,0x01,0xB3,0xE7,0x0F,0x00,0x10,0x3E};
	unsigned char ID = JOINTS[jointID].NET_ID;
        _Cmd_Data CD = HaveData;
	cmd[1] = ID;
	cmd[2] = reqMeasure(ID,CD) ;
	CRC=crcCalate(cmd,0, 6);
	cmd[6] = (unsigned char) (CRC>>8);
	cmd[7] = (unsigned char)(CRC&0x00ff);
	UsartSendDatafd(fd,cmd,12);
    //usleep(1000);
    //print(UART_OCU,cmd,12);//debug
}

//Disable Joint
void AMC_Init(int fd,JointId jointID)
{
    AMC_WriteAccess(fd,jointID);
    usleep(10000);
    AMC_Disable(fd,jointID);
    usleep(10000);
}


void AMC_Inintial_PARAMETER(void)
{
	PT_UsartDeviceOpr ptArm;
	int fd;

	ptArm = GetUsartOpr(COM_ARM);
	fd = ptArm->fd;
	
    JOINTS[J_6].NET_ID      = 6;		//旋转台
    JOINTS[Shoulder].NET_ID     = 1;	//肩部
    JOINTS[Elbow].NET_ID        = 2;	//肘部
    JOINTS[Pitch].NET_ID        = 5;	//手抓俯仰
    JOINTS[Roll].NET_ID         = 4;	//手抓开合
    JOINTS[J_11].NET_ID      = 3;		//手抓360°旋转
     
    JOINTS[J_6].MSR_type      = Potentiometer;
    JOINTS[Shoulder].MSR_type   = Potentiometer;
    JOINTS[Elbow].MSR_type      = Potentiometer;
    JOINTS[Pitch].MSR_type      = Potentiometer;
    JOINTS[Roll].MSR_type       = IncEncoder;
    JOINTS[J_11].MSR_type    = IncEncoder;
    
    JOINTS[J_6].CMD_STATUS  = Disable;
    JOINTS[Shoulder].CMD_STATUS = Disable;
    JOINTS[Elbow].CMD_STATUS    = Disable;
    JOINTS[Pitch].CMD_STATUS    = Disable;
    JOINTS[Roll].CMD_STATUS     = Disable;
    JOINTS[J_11].CMD_STATUS  = Disable;
 
    JOINTS[J_6].CMD_SCALE  = 1;
    JOINTS[Shoulder].CMD_SCALE  = 1;
    JOINTS[Elbow].CMD_SCALE     = 1;
    JOINTS[Pitch].CMD_SCALE     = 1;
    JOINTS[Roll].CMD_SCALE      = 1;
    JOINTS[J_11].CMD_SCALE   = 1;
    
    JOINTS[J_6].CMD_VELOCITY_PRE =0;
    JOINTS[Shoulder].CMD_VELOCITY_PRE =0;
    JOINTS[Elbow].CMD_VELOCITY_PRE =0;
    JOINTS[Pitch].CMD_VELOCITY_PRE =0;
    JOINTS[Roll].CMD_VELOCITY_PRE =0;
    JOINTS[J_11].CMD_VELOCITY_PRE   = 0;
    
    JOINTS[J_6].cmd_count =0;
    JOINTS[Shoulder].cmd_count =0;
    JOINTS[Elbow].cmd_count =0;
    JOINTS[Pitch].cmd_count =0;
    JOINTS[Roll].cmd_count =0;
    JOINTS[J_11].cmd_count   = 0;
    
    JOINTS[J_6].read_time_out =0;
    JOINTS[Shoulder].read_time_out =0;
    JOINTS[Elbow].read_time_out =0;
    JOINTS[Pitch].read_time_out =0;
    JOINTS[Roll].read_time_out =0;
    JOINTS[J_11].read_time_out   = 0;
    
	usleep(1000);//wait driver boot up ready

	AMC_Init(fd,J_6);
	usleep(500000);
	AMC_Init(fd,Shoulder);
	usleep(500000);
	AMC_Init(fd,Elbow);
	usleep(500000);
	AMC_Init(fd,Pitch);
	usleep(500000);
	AMC_Init(fd,Roll);
	usleep(500000);
	AMC_Init(fd,J_11);
	usleep(500000);
}

//OCU_to_ARM 机械臂
int OCU_to_arm_Info_Process(int fd,unsigned char *alldata)
{
	unsigned char *data=&alldata[4];
	unsigned int Digital_map;
	unsigned short BrakeCHNs,AMC_DI;
	short VelocityCHNs[13];
	int i;
	static int j;

	PT_TimeStopList_Opr	time_stop;
	time_stop = get_def_time();

	/*
    JOINTS[J_6].NET_ID      = 6;		//旋转台
    JOINTS[Shoulder].NET_ID     = 1;	//肩部
    JOINTS[Elbow].NET_ID        = 2;	//肘部
    JOINTS[Pitch].NET_ID        = 5;	//手抓俯仰
    JOINTS[Roll].NET_ID         = 4;	//手抓开合
    JOINTS[J_11].NET_ID      = 3;		//手抓360°旋转
    */
	switch(alldata[3])			//命令判断
	{
		case Velocity: 				/*处理轮循应答*/
		{
			for(i=0; i<4; i++)
				cvtUInt32.c[i] = data[i];
		    Digital_map = cvtUInt32.i;

		    cvtUInt16.c[0] = data[4+1];
		    cvtUInt16.c[1] = data[4];
		    BrakeCHNs = cvtUInt16.s;

		    for (i = 0; i<13; i++)
		    {
		        cvtInt16.c[0] = data[i*2+6];
		        cvtInt16.c[1] = data[i*2+6+1];
		        VelocityCHNs[i]= cvtInt16.s;
		    }
		    
			JOINTS[J_6].CMD_VELOCITY = VelocityCHNs[J_6];				//转台
			JOINTS[Shoulder].CMD_VELOCITY = VelocityCHNs[Shoulder];		//肩部
			JOINTS[Elbow].CMD_VELOCITY = VelocityCHNs[Elbow];			//肘部
			JOINTS[Pitch].CMD_VELOCITY = VelocityCHNs[Pitch];			//手抓俯仰
			JOINTS[Roll].CMD_VELOCITY = VelocityCHNs[Roll];				//手抓开合
			JOINTS[J_11].CMD_VELOCITY = VelocityCHNs[J_11];				//手抓旋转
			
			if(j==0)
			{
				AMC_Pack_angle(fd,J_11);
				time_stop->joint[JOINTS[J_11].NET_ID-1].TimeEn = 1;
				time_stop->joint[JOINTS[J_11].NET_ID-1].Times = JOINT_TIME;
			}
			else if(j==1)
			{
				AMC_Pack_angle(fd,J_6);
				time_stop->joint[JOINTS[J_6].NET_ID-1].TimeEn = 1;
				time_stop->joint[JOINTS[J_6].NET_ID-1].Times = JOINT_TIME;
				//DBG_PRINTF("ID1:%d\n");
			}
			else if(j==2)
			{
				AMC_Pack_angle(fd,Shoulder);
				time_stop->joint[JOINTS[Shoulder].NET_ID-1].TimeEn = 1;
				time_stop->joint[JOINTS[Shoulder].NET_ID-1].Times = JOINT_TIME;
			}
			else if(j==3)
			{
				AMC_Pack_angle(fd,Elbow);
				time_stop->joint[JOINTS[Elbow].NET_ID-1].TimeEn = 1;
				time_stop->joint[JOINTS[Elbow].NET_ID-1].Times = JOINT_TIME;
			}
			else if(j==4)
			{
				AMC_Pack_angle(fd,Pitch);
				time_stop->joint[JOINTS[Pitch].NET_ID-1].TimeEn = 1;
				time_stop->joint[JOINTS[Pitch].NET_ID-1].Times = JOINT_TIME;
			}
			else if(j==5)
			{
				AMC_Pack_angle(fd,Roll);
				time_stop->joint[JOINTS[Roll].NET_ID-1].TimeEn = 1;
				time_stop->joint[JOINTS[Roll].NET_ID-1].Times = JOINT_TIME;
			}
			usleep(4000);
			j++;

			if(j>=6)
				j = 0;
			
            AMC_Pack(fd,Shoulder);
            usleep(3000);
			
            AMC_Pack(fd,Elbow);
            usleep(3000);
			
            AMC_Pack(fd,Pitch);
            usleep(3000);
			
            AMC_Pack(fd,Roll);
            usleep(3000);

			AMC_Pack(fd,J_6); //2016年11月23日14:27:02 J_6  is Torrent
		    usleep(3000);
			
            AMC_Pack(fd,J_11);//2016年11月23日15:56:53 Gripper
            usleep(3000);

            AMC_DI = Digital_WR(fd,Digital_map); //write and read   
            //packageOCU(AMC_DI,AMC_Position);
            //writeOCU();
            //LED2_TOGGLE();  //yellow led indicate cmd running      
            //delayMs(1);
			break;
		}
		default:
		{
			break;
		}
	}
	return 0;
}

void StrToHex(char *pbDest, char *pbSrc, int nLen)
{
	char h1,h2;
	char s1,s2;
	int i;

	for (i=0; i<nLen; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9)
			s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9)
			s2 -= 7;

		pbDest[i] = s1*16 + s2;
	}
}

void HexToStr(char *pbDest, char *pbSrc, int size)
{
	char ddl,ddh;
	int i;

	for (i=0; i<size; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i*2] = ddh;
		pbDest[i*2+1] = ddl;
	}
	pbDest[size*2] = '\0';
}

int MODBUS_ASCII_TX(int fd,char id,char cmd,int addr,unsigned int data,int len)
{
	unsigned char tx_buf[32];
	unsigned char lrc_val;
	tx_buf[0] = 0x3A;
	tx_buf[1] = 0x30;
	tx_buf[2] = 0x30+id;
	tx_buf[3] = 0x30;
	tx_buf[4] = 0x30+cmd;
	tx_buf[5] = 0x30;
	tx_buf[6] = 0x30;
	tx_buf[7] = 0x30;
	tx_buf[8] = 0x30+addr;
	tx_buf[9] = 0x30;
	tx_buf[10] = 0x30;
	tx_buf[11] = 0x30;
	tx_buf[12] = 0x30+len;

	lrc_val = LRC_Check(tx_buf,13);
	HexToStr(&tx_buf[13],&lrc_val,1);
	tx_buf[15] = 0x0D;//回车
	tx_buf[16] = 0x0A;//换行
/*	if(PtzRx->Rx_OK)
	{
		PtzRx->Rx_OK = 0;
		senser_data.type = PtzRx->Rx_Data[2]-0x30;
		StrToHex(temp,&PtzRx->Rx_Data[7],2);
		senser_data.val = (temp[0]<<8);
		senser_data.val |= temp[1];
		//DBG_PRINTF("t:%X,%X",temp[0],temp[1]);
		//DBG_PRINTF("TimeStop:%X,%X,%X,%X\r\n",PtzRx->Rx_Data[7],PtzRx->Rx_Data[8],PtzRx->Rx_Data[9],PtzRx->Rx_Data[10]);
	}
*/
	//memcpy();

	set_value(RS485_DE_SENSER,1);
	usleep(1000);
	UsartSendDatafd(fd,tx_buf,17);//
	usleep(20000);
	set_value(RS485_DE_SENSER,0);
	usleep(20000);
//	DBG_PRINTF("Gas sensor data check!\r\n");
	return 0;
}
//int Rx_OCU_Prs(PT_TCP_Rx_Opr OcuRx)
//{
//	PT_UsartDeviceOpr ptRf,ptFiber,ptWifi,ptHead,ptArm,ptPtz,ptMotion;
//	unsigned char *pdata,*pcoredata,*pheaddata,*pptzdata,*parmdata,i;
//	PT_TimeStop_Opr Rx_Ocutmp;
//	static char senser_id=0;
//	static char ptz_tx_flag=0;
//
//	int Head_module_frame_address;
//	int Head_module_frame_length;			//head底板
//	int core_module_frame_address;
//	int core_module_frame_length;			//核心板(轮子翻转臂)
//	int ptz_module_frame_address;
//	int ptz_module_frame_length;			//ptz
//	int arm_module_frame_address;
//	int arm_module_frame_length;			//arm臂
//
//	ptRf = GetUsartOpr(COM_RF);
//	ptFiber = GetUsartOpr(COM_FIBER);
//	ptWifi = GetUsartOpr(COM_WIFI);
//
//	ptHead = GetUsartOpr(COM_HEAD);
//	ptArm = GetUsartOpr(COM_ARM);
//	ptPtz = GetUsartOpr(COM_PTZ);
//
//	//ptMotion = GetUsartOpr(COM_MOTION);
//	PT_pcTxTime SendOCU;
//	PT_TimeStopList_Opr time_stop;
//
//	//Rx_Ocutmp = GetTimeOpr("RxOcu");
//	time_stop = get_def_time();
//
//	//DBG_PRINTF("1\n");
//	if(ReadOcuBuf(ptRf->fd,OcuRx)==0)
//	{
//		//continue;
//	}
//	else if(ReadOcuBuf(ptFiber->fd,OcuRx)==0)
//	{
//		//continue;
//	}
//	else if(ReadOcuBuf(ptWifi->fd,OcuRx)==0)
//	{
//		//continue;
//	}
//	else if(Devclient[0].fd>0)
//	{
//		//DBG_PRINTF("ocu0:%d,%d,%d,fd:%d\n",OcuRx->Rx_Bot,OcuRx->Rx_OK,OcuRx->Rx_Len,ptRf->fd);
//		//DBG_PRINTF("ocu0:%d,%d,%d\n",OcuRx->Rx_Bot,OcuRx->Rx_OK,OcuRx->Rx_Len);
//		if(ReadOcuBuf(Devclient[0].fd,OcuRx)==0)
//		{
//		}
//		else
//		{
//			return 0;
//		}
//	}
//	else if(Devclient[1].fd>0)
//	{
//		//DBG_PRINTF("ocu1:%d,%d,%d\n",OcuRx->Rx_Bot,OcuRx->Rx_OK,OcuRx->Rx_Len);
//		if(ReadOcuBuf(Devclient[1].fd,OcuRx)==0)
//		{
//		}
//		else
//		{
//			return 0;
//		}
//	}
//	else
//	{
////		MODBUS_ASCII_TX(ptPtz->fd,senser_id,READ_CMD,0x00,"",1);	//气体传感器
//		//没有数据
//		return 0;
//	}
//	//DBG_PRINTF("1\n");
//	SendOCU = GetPcTxTime();
//	time_stop->ocu_rx_timeout.Times = OCU_RX_OUTTIME;
//	time_stop->ocu_rx_timeout.TimeEn = 1;
//	time_stop->motion_arm_timeout.TimeEn = 1;
//	time_stop->motion_arm_timeout.Times = MOTION_L_RX_TIMEOUT;
//	time_stop->claw_timeout.TimeEn = 1;
//	time_stop->claw_timeout.Times = MOTION_L_RX_TIMEOUT;
//	SendOCU->PC_TX_Out_En = 1;
//	SendOCU->PC_TX_Time_Out = SEND_TO_OCU_CNT;
//	time_stop->motion_send.TimeEn  = 1;
//	//time_stop->motion_send.Times = SEND_TO_MOTION;
//	//Rx_Ocutmp->TimeEn = 1;
//	//Rx_Ocutmp->Times = RX_OCU_TIMEOUT;
//
//	pdata = OcuRx->Rx_Data;
//	//DBG_PRINTF("ID:%d\n",pdata[2]);
//	From_OCU_ID = pdata[2];
//	/*
//	0.3A
//	1.AA
//	2.包序号
//	3.包长度
//	*/
//continue_prs:
//
//	core_module_frame_length  = pdata[4];
//	core_module_frame_address = pdata[5];
//
//	Head_module_frame_length  = core_module_frame_length -10;
//	Head_module_frame_address = core_module_frame_address + 7;
//
//	ptz_module_frame_length   = pdata[6];
//	ptz_module_frame_address  = pdata[7];
//
//	arm_module_frame_length   = pdata[8];
//	arm_module_frame_address  = pdata[9];
//
//	pcoredata = &pdata[core_module_frame_address];
//	pheaddata = &pdata[Head_module_frame_address];
//	pptzdata = &pdata[ptz_module_frame_address];
//	parmdata = &pdata[arm_module_frame_address];
//
//	if(ptHead!=NULL)			//head板数据
//	{
//		Tack_To_water_cannon(pheaddata);
//		Tack_To_Head(ptHead->fd,pheaddata);							//数据合成转发给Head板
//	}
//	if(ptz_module_frame_length)
//	{
//		Tack_To_PTZ(ptPtz->fd,pptzdata);							//数据转发给ptz
//		UsartSendDatafd(ptPtz->fd,pptzdata,ptz_module_frame_length);//数据转发给ptz
//	}
//	//UsartSendDatafd(ptArm->fd,parmdata,arm_module_frame_length);//数据转发给arm臂
//
//	OCU_to_arm_Info_Process(ptArm->fd,parmdata);							//数据转发给arm臂
//	//DBG_PRINTF("ID3:%d\n",pdata[2]);
//	//DBG_PRINTF("pcoredata[0]:%X\n",pcoredata[0]);
//	get_js_input(pcoredata);									//数据经过计算控制车体
////	MODBUS_ASCII_TX(ptPtz->fd,senser_id++,READ_CMD,0x00,"",1);	//气体传感器
//	if(ptz_tx_flag)
//	{
//		MODBUS_ASCII_TX(ptPtz->fd,senser_id++,READ_CMD,0x00,"",1);	//气体传感器
//		if(senser_id>=8)
//		{
//			senser_id = 1;
//		}
//	}
//	else
//	{
//		if(ptz_module_frame_length)
//		{
//			set_value(RS485_DE_SENSER,1);
//			usleep(500);
//			UsartSendDatafd(ptPtz->fd,&pptzdata[4],7);//数据转发给PTZ
//			usleep(12000);
//			set_value(RS485_DE_SENSER,0);
//			DBG_PRINTF("Gas data check\r\n");
//	/*
//			if(pptzdata[3]==0x3c)			//数据命令判断
//				IOCwrite(GPIO_DR1,0);
//			else if(pptzdata[3]==0x40)
//				IOCwrite(GPIO_DR2,0);
//			else
//			{
//				IOCwrite(GPIO_DR1,1);
//				IOCwrite(GPIO_DR2,1);
//			}*/
//		}
//	}
//
//	ptz_tx_flag = !ptz_tx_flag;
//
//	return 0;
//}
int Rx_OCU_Prs(PT_TCP_Rx_Opr OcuRx)
{
	PT_UsartDeviceOpr ptRf,ptFiber,ptWifi,ptHead,ptArm,ptPtz,ptMotion;
	unsigned char *pdata,*pcoredata,*pheaddata,*pptzdata,*parmdata,i;
	PT_TimeStop_Opr Rx_Ocutmp;
//	static char senser_id=0;
	
	int Head_module_frame_address;
	int Head_module_frame_length;			//head底板
	int core_module_frame_address;			
	int core_module_frame_length;			//核心板(轮子翻转臂)
	int ptz_module_frame_address;	
	int ptz_module_frame_length;			//ptz
	int arm_module_frame_address;
	int arm_module_frame_length;			//arm臂

	ptRf = GetUsartOpr(COM_RF);
	ptFiber = GetUsartOpr(COM_FIBER);
	ptWifi = GetUsartOpr(COM_WIFI);

	ptHead = GetUsartOpr(COM_HEAD);
	ptArm = GetUsartOpr(COM_ARM);
	ptPtz = GetUsartOpr(COM_PTZ);
	
	//ptMotion = GetUsartOpr(COM_MOTION);
	PT_pcTxTime SendOCU;
	PT_TimeStopList_Opr time_stop;
	
	//Rx_Ocutmp = GetTimeOpr("RxOcu");
	time_stop = get_def_time();
	
	//DBG_PRINTF("1\n");
	if(ReadOcuBuf(ptRf->fd,OcuRx)==0)
	{
		//continue;
	}
	else if(ReadOcuBuf(ptFiber->fd,OcuRx)==0)
	{
		//continue;
	}
	else if(ReadOcuBuf(ptWifi->fd,OcuRx)==0)
	{
		//continue;
	}
	else if(Devclient[0].fd>0)
	{//是否连接网络
		//DBG_PRINTF("ocu0:%d,%d,%d,fd:%d\n",OcuRx->Rx_Bot,OcuRx->Rx_OK,OcuRx->Rx_Len,ptRf->fd);
		//DBG_PRINTF("ocu0:%d,%d,%d\n",OcuRx->Rx_Bot,OcuRx->Rx_OK,OcuRx->Rx_Len);
		if(ReadOcuBuf(Devclient[0].fd,OcuRx)==0)
		{
		}
		else
		{
			//没有数据
//			printf("Devclient[0]ocu_rx_timeout:%d\r\n",time_stop->ocu_rx_timeout.Times);
			return 0;
		}
	}
	else if(Devclient[1].fd>0)
	{
		//DBG_PRINTF("ocu1:%d,%d,%d\n",OcuRx->Rx_Bot,OcuRx->Rx_OK,OcuRx->Rx_Len);
		if(ReadOcuBuf(Devclient[1].fd,OcuRx)==0)
		{
		}
		else
		{
			return 0;
		}
	}
	else
	{
//		MODBUS_ASCII_TX(ptPtz->fd,senser_id,READ_CMD,0x00,"",1);	//气体传感器
		//没有数据
//		printf("ocu_rx_timeout:%d\r\n",time_stop->ocu_rx_timeout.Times);
		return 0;
	}
	//DBG_PRINTF("1\n");
	SendOCU = GetPcTxTime();
//	printf("OCU_RX_OUTTIME:%d\r\n",time_stop->ocu_rx_timeout.Times);
	time_stop->ocu_rx_timeout.Times = OCU_RX_OUTTIME;
	time_stop->ocu_rx_timeout.TimeEn = 1;
	time_stop->motion_arm_timeout.TimeEn = 1;
	time_stop->motion_arm_timeout.Times = MOTION_L_RX_TIMEOUT;
	time_stop->claw_timeout.TimeEn = 1;
	time_stop->claw_timeout.Times = MOTION_L_RX_TIMEOUT;
	SendOCU->PC_TX_Out_En = 1;
	SendOCU->PC_TX_Time_Out = SEND_TO_OCU_CNT;
	time_stop->motion_send.TimeEn  = 1;
	//time_stop->motion_send.Times = SEND_TO_MOTION;
	//Rx_Ocutmp->TimeEn = 1;
	//Rx_Ocutmp->Times = RX_OCU_TIMEOUT;
	
	pdata = OcuRx->Rx_Data;
	//DBG_PRINTF("ID:%d\n",pdata[2]);
	From_OCU_ID = pdata[2];
	/*
	0.3A
	1.AA
	2.包序号
	3.包长度
	*/
continue_prs:
	core_module_frame_length  = pdata[4];
	core_module_frame_address = pdata[5];
	
	Head_module_frame_length  = core_module_frame_length -10;
	Head_module_frame_address = core_module_frame_address + 7;
	
	ptz_module_frame_length   = pdata[6];
	ptz_module_frame_address  = pdata[7];	
	
	arm_module_frame_length   = pdata[8];
	arm_module_frame_address  = pdata[9];	

	pcoredata = &pdata[core_module_frame_address];
	pheaddata = &pdata[Head_module_frame_address];
	pptzdata = &pdata[ptz_module_frame_address];
	parmdata = &pdata[arm_module_frame_address];

	if(ptHead!=NULL)		//head板数据
	{
		Tack_To_water_cannon(pheaddata);
		Tack_To_Head(ptHead->fd,pheaddata);							//数据合成转发给Head板
	}
	if(ptz_module_frame_length)
	{
		Tack_To_PTZ(ptPtz->fd,pptzdata);							//数据转发给ptz
		UsartSendDatafd(ptPtz->fd,pptzdata,ptz_module_frame_length);//数据转发给ptz
	}
	//UsartSendDatafd(ptArm->fd,parmdata,arm_module_frame_length);//数据转发给arm臂

	OCU_to_arm_Info_Process(ptArm->fd,parmdata);							//数据转发给arm臂
	//DBG_PRINTF("ID3:%d\n",pdata[2]);
	//DBG_PRINTF("pcoredata[0]:%X\n",pcoredata[0]);
	get_js_input(pcoredata);									//数据经过计算控制车体
//	MODBUS_ASCII_TX(ptPtz->fd,senser_id++,READ_CMD,0x00,"",1);	//气体传感器
//	MODBUS_ASCII_TX(ptPtz->fd,senser_id,READ_CMD,0x00,"",1);	//气体传感器
//	MODBUS_ASCII_TX(ptPtz->fd,senser_id++,READ_CMD,0x00,"",1);	//气体传感器
//	if(senser_id>=8)
//	{
//		senser_id = 1;
//	}

	return 0;
}

int Rx_Ptz_Prs(PT_TCP_Rx_Opr PtzRx)		//PTZ接收处理
{
	PT_UsartDeviceOpr ptPtz;
	int i;
	unsigned char temp[4];

	ptPtz = GetUsartOpr(COM_PTZ);

	for(i=0;i<1024;i++)
	{
		if(!ReadArmBuf(ptPtz->fd,PtzRx))
		{
			break;
		}
	}

	if(PtzRx->Rx_OK)
	{
		PtzRx->Rx_OK = 0;
//		senser_data.type = PtzRx->Rx_Data[2]-0x30;
		senser_data.type = (PtzRx->Rx_Data[1]-0x30)*16+(PtzRx->Rx_Data[2]-0x30);
		StrToHex(temp,&PtzRx->Rx_Data[7],2);
		senser_data.val = ((unsigned short)temp[0]<<8);
		senser_data.val |= temp[1];

		DBG_PRINTF("Gas sensor:%02X--%d\r\n",senser_data.type,senser_data.val);
		//DBG_PRINTF("TimeStop:%X,%X,%X,%X\r\n",PtzRx->Rx_Data[7],PtzRx->Rx_Data[8],PtzRx->Rx_Data[9],PtzRx->Rx_Data[10]);
	}
	return 0;
}


unsigned char Rx_AMC_uart_pack(int fd,PT_TCP_Rx_Opr Rx_STR)
{
 	unsigned char  rx_len,i;
 	unsigned short cal_crc1,data_crc1,cal_crc2,data_crc2;
 	short usart_rx;

 	int usart_no = 1;
	//u8 *DataTemp;
	
	//DataTemp = (u8*)malloc(SRAMIN,sizeof(u8)*FRAME_SIZE);
  	usart_rx = Read_Byte(fd);
	
	if(usart_rx<0)
	{
		//free(SRAMIN,DataTemp);

		return 0;
	}	

	//DBG_PRINTF("ARM_ERROR\n");
	
	i = Rx_STR[usart_no-1].Rx_Bot;
	Rx_STR[usart_no-1].Rx_Data[i]=usart_rx;
	
	switch(i)
	{
		case 0:
		{
		   	Rx_STR[usart_no-1].Rx_Len=0;
		   	Rx_STR[usart_no-1].Rx_OK=0;
		   
			if(usart_rx==AMC_Pro_SOH)
			{
				Rx_STR[usart_no-1].Rx_Bot++; 
				//BK_Pro_SOH=usart_rx;
			}	  
			break;
	   }
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
	   	{
		    Rx_STR[usart_no-1].Rx_Bot++; 
			break;
	   	}
	   	
		default:
		{
		    if(((Rx_STR[usart_no-1].Rx_Data[5]*2)+8+(!!Rx_STR[usart_no-1].Rx_Data[5]*2))>1024)  	// 修改 1
			{
			 	Rx_STR[usart_no-1].Rx_Bot=0;
			 	break;
			}
		}
		
		if(i<((Rx_STR[usart_no-1].Rx_Data[5]*2)+7+(!!Rx_STR[usart_no-1].Rx_Data[5]*2)))          		// 修改 2
	    {
	     	Rx_STR[usart_no-1].Rx_Bot++;
	    }
	    else   															//数据接收结束
	    {
	         rx_len = i+1;
	         
		     cal_crc1 = crcCalate(&Rx_STR[usart_no-1].Rx_Data[0],0,6);
		     
		     data_crc1 = (Rx_STR[usart_no-1].Rx_Data[6]<<8)+Rx_STR[usart_no-1].Rx_Data[7];
		     
	         if(cal_crc1 != data_crc1)                      //判断帧头部分是否正确
		     {
				  Rx_STR[usart_no-1].Rx_OK=0;
			      Rx_STR[usart_no-1].Rx_Bot=0;
			      break;
			      //Str[usart_no-1].Rx_E -= (i-5);
			 }
		     else if(Rx_STR[usart_no-1].Rx_Data[5]) //判断数据部分是否有数据,如有数据进去进行校验;
		     {
		     	cal_crc2 = crcCalate(&Rx_STR[usart_no-1].Rx_Data[8],0,Rx_STR[usart_no-1].Rx_Data[5]*2);
		     
		     	data_crc2 = (Rx_STR[usart_no-1].Rx_Data[8+(Rx_STR[usart_no-1].Rx_Data[5]*2)]<<8)+(Rx_STR[usart_no-1].Rx_Data[9+(Rx_STR[usart_no-1].Rx_Data[5]*2)]);
		     	
		     	if(cal_crc2==data_crc2)
		     	{
					Rx_STR[usart_no-1].Rx_OK=1;
					Rx_STR[usart_no-1].Rx_Len=rx_len;
					Rx_STR[usart_no-1].Rx_Bot=0;
					//myfree(SRAMIN,DataTemp);
					return 0;	   
			    }
			    else
			    {
					Rx_STR[usart_no-1].Rx_OK=0;
			      	Rx_STR[usart_no-1].Rx_Bot=0;
			      	break;
			    }
		     }
             else           //如果数据部分没数据.则数据通过;
             {
                 Rx_STR[usart_no-1].Rx_OK=1;
                 Rx_STR[usart_no-1].Rx_Len=rx_len;
                 Rx_STR[usart_no-1].Rx_Bot=0;
                 //myfree(SRAMIN,DataTemp);
                 return 0;
             }
	    }
	    break;
	}


	return 1;	    
} 


int AMC_Position[13];

int AMC_Info_Process(unsigned char *data)
{
	unsigned char JOINTS_ID;
	PT_UsartDeviceOpr ptArm;
	PT_TimeStopList_Opr	time_stop;

	
	//如果返回角度信息保存下来发送给OCU；

	//如果返回未写允许，发送写允许；

	//返回速度值；

	//返回其他；
	JOINTS_ID = data[2]>>2;


	switch(data[3])			//命令判断
	{
		case 6: 			//没有写允许;
		{
			ptArm = GetUsartOpr(COM_ARM);
			AMC_Init(ptArm->fd,JOINTS_ID);				//发送写允许;
			usleep(2000);
			DBG_PRINTF("AMC INIT----------------------------------:%d\n",JOINTS_ID);
			break;
		}
		case 1:				//完成发送指令;
		{
			//DBG_PRINTF("AMC RX:%d\n",JOINTS_ID);
			if(data[5]==2)	//代表角度数据;
			{
				time_stop = get_def_time();

				time_stop->joint[JOINTS_ID-1].TimeEn = 0;
				time_stop->joint[JOINTS_ID-1].Times = JOINT_TIME;
				joint_error[JOINTS_ID-1] = 0;
				
				if(JOINTS_ID==6)
				{
					JOINTS_ID = 0;
				}
				
				AMC_Position[JOINTS_ID] = data[8];
				AMC_Position[JOINTS_ID] |= (data[9]<<8);
				AMC_Position[JOINTS_ID] |= (data[10]<<16);
				AMC_Position[JOINTS_ID] |= (data[11]<<24);
				//if(JOINTS_ID==0)
				//DBG_PRINTF("AMC_Position:%d,%d\n",AMC_Position[JOINTS_ID],JOINTS_ID);
			}
			break;
		}
		default:		//数据格式有误
		{			
			DBG_PRINTF("AMC_----------------:%d\n",JOINTS_ID);
			break;
		}
	}
}

unsigned char to_OCU[128];

void packageOCU(int readDigital, int *PositionCHNs)
{
    int i, j;

    cvtUInt16.s = readDigital;
    
    for (i = 0; i<13; i++)
    {
        cvtInt32.i = PositionCHNs[i];
        to_OCU[i*4+2] = cvtInt32.c[0];
        to_OCU[i*4+3] = cvtInt32.c[1];
        to_OCU[i*4+4] = 0;
        to_OCU[i*4+5] = 0;			
    }

    to_OCU[0] = 0;
    to_OCU[1] = 0;
    
    to_OCU[0] = (unsigned char)(joint_error[5]<<2)|(joint_error[4]<<0);
    to_OCU[1] = (unsigned char)(joint_error[3]<<6)|(joint_error[2]<<4)|(joint_error[1]<<2)|(joint_error[0]<<0);


}

int back_to_OCU(unsigned char *buf)
{
    unsigned int  i, CRC;
   
    unsigned char Frame_Size = 54;

    packageOCU(0,AMC_Position);

    buf[0] = 0x51;  // start A
    buf[1] = 0xEF;  // start B
    buf[2] = Frame_Size + 6;    //=54+6 data length, Code 0x 1 000 0000
    buf[3] = 128;   //Addr & type
    
    for (i=0; i< Frame_Size; i ++) 
		buf[i+4] = to_OCU[i];    //Messages
		
    CRC = crcCalate(buf,0, Frame_Size+4);
    buf[Frame_Size+4] = (unsigned char)(CRC>>8);  //CRC A
    buf[Frame_Size+4+1] = (unsigned char)(CRC&0x00ff); //CRC B
    
   	return Frame_Size+6;
}


int Rx_ARM_Prs(PT_TCP_Rx_Opr USART_Rx_STR)			//arm臂数据处理
{
	PT_UsartDeviceOpr ptArm;
	ptArm = GetUsartOpr(COM_ARM);
	int length,i;
		
	/*读串口接收缓存数据*/
	for(i=0;i<2048;i++)
	{
		if(!Rx_AMC_uart_pack(ptArm->fd,USART_Rx_STR))				//如果有数据循环读串口buf区数据;
			break;
	}
	
	if(USART_Rx_STR->Rx_OK)  					//一组协议的数据读取完毕;
	{
		USART_Rx_STR->Rx_OK = 0;
		//DBG_PRINTF("------------------arm------------------\n");
		//for(i=0;i<USART_Rx_STR->Rx_Len;i++)
		//printf("%X ",USART_Rx_STR->Rx_Data[i]);
		//printf("\n");
		AMC_Info_Process(USART_Rx_STR->Rx_Data);
	}
}

int Rx_Head_Prs(PT_TCP_Rx_Opr HeadRx)		//head板数据处理
{
	PT_UsartDeviceOpr ptHead;
	int i;
	ptHead = GetUsartOpr(COM_HEAD);

	for(i=0;i<1024;i++)
	{
		if(!ReadHeadBuf(ptHead->fd,HeadRx))
		{
			break;
		}
	}
	return 0;
}

int Rx_Motion_Prs(PT_TCP_Rx_Opr MotionRx)	//轮子和翻转臂数据处理
{
	PT_UsartDeviceOpr ptMotion;
	unsigned char usercmd,ID;
	unsigned char *ptdata;
	unsigned int speed=0;
	unsigned short angle=0;
	int i;
	PT_TimeStopList_Opr	time_stop;
	time_stop = get_def_time();
	
	ptMotion = GetUsartOpr(COM_MOTION);

	for(i=0;i<1024;i++)
	{
		if(!ReadMotionBuf(ptMotion->fd,MotionRx))
		{
			break;
		}
	}
	
	if(MotionRx->Rx_OK)
	{
		MotionRx->Rx_OK = 0;

		ptdata = MotionRx->Rx_Data;
		
		usercmd = (ptdata[2]&0x0c)>>2;			//用户自定义命令;
		ID = (ptdata[2]&0x30)>>4;				//从设备ID;

		if(ID==TRACK_L)
		{
			time_stop->motion_L_rx_timeout.Times = MOTION_L_RX_TIMEOUT;		//接收应答
			time_stop->motion_L_rx_timeout.TimeEn = 0;
			motion_L_rx_timeout_stop = 0;
		}
		else if(ID==TRACK_R)
		{
			time_stop->motion_R_rx_timeout.Times = MOTION_R_RX_TIMEOUT;		//接收应答
			time_stop->motion_R_rx_timeout.TimeEn = 0;
			motion_L_rx_timeout_stop = 0;
		}
		else if(ID==FLIPPER)
		{
			time_stop->motion_arm_timeout.Times = MOTION_R_RX_TIMEOUT;		//接收应答
			time_stop->motion_arm_timeout.TimeEn = 0;
			//motion_L_rx_timeout_stop = 0;
		}
		
		//DBG_PRINTF("id:%d usercmd:%X\n",ID,ptdata[2]);
  		if(ptdata[3]==0x06)
  		{
  			DBG_PRINTF("----------------------Motion_Init-------------------\n");
			Motion_Init();
			usleep(5000);
			motor_set_speed(TRACK_R,0,0);
			usleep(5000);
			motor_set_speed(TRACK_L,0,0);
			usleep(5000);
			motor_set_speed(FLIPPER,0,0);
			usleep(5000);

			if(ID==TRACK_R)
			{
				Track_R_Error = 2; 	
			}
			else if(ID==TRACK_L)
			{
				Track_L_Error = 2; 	
			}
			else if(ID==FLIPPER)
			{
				Flipper_Error = 2; 
			}
			return 0;
  		}
  		else
  		{
			if(ID==TRACK_R)
			{
				Track_R_Error = 0; 	
			}
			else if(ID==TRACK_L)
			{
				Track_L_Error = 0; 	
			}
			else if(ID==FLIPPER)
			{
				Flipper_Error = 0; 
			}
  		}

  		switch(usercmd)			//命令判断
		{
			case USERSET_READ_NONE:
			{
				//DBG_PRINTF("id:%d cmd:%d\n",ID,usercmd);
				break;
			}
			case USERSET_READ_SPEED: 				
			{
				speed = ptdata[8];
				speed |= ptdata[9]<<8;
				speed |= ptdata[10]<<16;
				speed |= ptdata[11]<<24;
				
				DBG_PRINTF("id:%d speed:%d\n",ID,speed);
				
				if(ID==TRACK_R)
				{
					Track_R_speed = (((speed*20000)/2^17)*60)/8000; 	
				}
				else if(ID==TRACK_L)
				{	
					Track_L_speed = (((speed*20000)/2^17)*60)/8000; 	
				}
				else if(ID==FLIPPER)
				{
					Flipper_speed = (((speed*20000)/2^17)*60)/8000; 		
				}
				break;
			}
			case USERSET_READ_ANGLE: 				
			{
				angle = ptdata[8];
				angle |= ptdata[9]<<8;
				DBG_PRINTF("angle:%d\n",angle);
				
				if(ID==FLIPPER)
				{
					Flipper_Position = angle;
				}
				break;
			}
			default:
			{
				DBG_PRINTF("ERROR:%d\n",angle);
				break;
			}
  		}
	}
}
/*
0.55
1.69
2.ID,
3.包长度
4.08  
5.1F
*/

int Tacker_To_Ocu(PT_TCP_Rx_Opr HeadRx,PT_TCP_Rx_Opr ARMRx)
{
	unsigned char Global_MSG_To_OCU[128];
	unsigned char To_OCU_Header_Length,To_OCU_Core_Length;
	unsigned char  To_OCU_ID,core_module_to_OCU_id;
	unsigned char Lenth;
	short crc16;
	PT_pcTxTime SendOCU;
	PT_UsartDeviceOpr ptRf,ptFiber,ptWifi;
	
//	printf("enter Tacker_To_Ocu\r\n");
	PT_Sys_Para_Opr pt_Sys_Para = GetDefultSysParaOpr();					//获得配置参数;

	ptRf = GetUsartOpr(COM_RF);
	ptFiber = GetUsartOpr(COM_FIBER);
	ptWifi = GetUsartOpr(COM_WIFI);
	
	SendOCU = GetPcTxTime();

	if(SendOCU->PC_TX_Out_En)
	{
		if(!SendOCU->PC_TX_Time_Out)
		{
			//ARMRx->Rx_Len = 0;
			SendOCU->PC_TX_Out_En = 0;
			SendOCU->PC_TX_Time_Out = SEND_TO_OCU_CNT;
		}
		else
			return 0;
	}
	else
		return 0;

	HeadRx->Rx_OK = 0;
	//ARMRx->Rx_OK = 0;
	
	Global_MSG_To_OCU[0] = 0x55;
	Global_MSG_To_OCU[1] = 0x69;	
	Global_MSG_To_OCU[2] = From_OCU_ID ;	
	Global_MSG_To_OCU[3] = 0;  	//  It is whole frame size, will fill in the end 此帧数据总长度
	Global_MSG_To_OCU[4] = To_OCU_Header_Length = 8; // Address of the core and header module
	Global_MSG_To_OCU[5] = To_OCU_Core_Length = To_OCU_Core_Sub_Length;
	Global_MSG_To_OCU[6] = 0;	// Address of Arm frame
	Global_MSG_To_OCU[7] = 0;	// Length of Arm frame	
	Lenth = 8; 
	Global_MSG_To_OCU[Lenth++] = core_module_to_OCU_id++; 

	//轮子和翻转臂数据；
	Global_MSG_To_OCU[Lenth++] = Flipper_Position>>8; 
	Global_MSG_To_OCU[Lenth++] = Flipper_Position; 

	Global_MSG_To_OCU[Lenth++] = Flipper_speed>>8; 
	Global_MSG_To_OCU[Lenth++] = Flipper_speed; 

	Global_MSG_To_OCU[Lenth++] = Track_L_speed>>8; 
	Global_MSG_To_OCU[Lenth++] = Track_L_speed; 

	Global_MSG_To_OCU[Lenth++] = Track_R_speed>>8; 
	Global_MSG_To_OCU[Lenth++] = Track_R_speed; 

	Global_MSG_To_OCU[Lenth++] = (Flipper_Error | (Track_L_Error<<2) | (Track_R_Error<<4)|(arm_Error<<6));
	Global_MSG_To_OCU[Lenth++] = pt_Sys_Para->codeVersion;

	//DBG_PRINTF("state:%X\n",Global_MSG_To_OCU[Lenth-1]);

	//head板数据；
	memcpy(&Global_MSG_To_OCU[Lenth],&HeadRx->Rx_Data[3],17);
	Global_MSG_To_OCU[Lenth+3] = wc_data[0];					//水泡数据发送给OCU
	Global_MSG_To_OCU[Lenth+4] = wc_data[1];
	Global_MSG_To_OCU[Lenth+5] = wc_data[2];

	Global_MSG_To_OCU[Lenth+6] = senser_data.type;					//气体传感器
	Global_MSG_To_OCU[Lenth+7] = (unsigned char)(senser_data.val>>8);
	Global_MSG_To_OCU[Lenth+8] = (unsigned char)senser_data.val;

	Lenth+=17;

	Global_MSG_To_OCU[Lenth++] = 0;
	Global_MSG_To_OCU[Lenth++] = 3;
	
	Global_MSG_To_OCU[Lenth++] = From_OCU_ID;		//接收ID；
	//DBG_PRINTF("Lenth:%d\r\n",Lenth);	
	
	//arm臂数据；
	Global_MSG_To_OCU[6] = Lenth;		// Address of Arm frame
	Global_MSG_To_OCU[7] = back_to_OCU(&Global_MSG_To_OCU[Lenth]);

	//memcpy(&Global_MSG_To_OCU[Lenth],ARMRx->Rx_Data,ARMRx->Rx_Len);
	Lenth += Global_MSG_To_OCU[7];

	//准备校验
	Global_MSG_To_OCU[3] = Lenth + 2; // include CCITT to data length

	crc16 = Table_CRC16(Global_MSG_To_OCU,Lenth);

	Global_MSG_To_OCU[Lenth++] = crc16>>8;
	Global_MSG_To_OCU[Lenth++] = crc16;

	UsartSendDatafd(ptRf->fd,Global_MSG_To_OCU,Lenth);//数据转发给
	UsartSendDatafd(ptFiber->fd,Global_MSG_To_OCU,Lenth);//数据转发给
	UsartSendDatafd(ptWifi->fd,Global_MSG_To_OCU,Lenth);//数据转发给
//	printf("SendData\r\n");

	if(Devclient[0].fd>0)
	{
//		SendData(Devclient[0].fd,Global_MSG_To_OCU,Lenth);//zuze
	}
	else if(Devclient[1].fd>0)
	{
//		SendData(Devclient[1].fd,Global_MSG_To_OCU,Lenth);
	}
//	printf("exit Tacker_To_Ocu\r\n");
	return 0;
}

void canopen_data_pro(PT_TCP_Rx_Opr HeadRx,PT_TCP_Rx_Opr ARMRx,PT_TCP_Rx_Opr PTZRx)
{
	PT_CanDeviceOpr pt_dev;
	int NODE_ID,len,can_id,Sn,Sn_id;
	unsigned char cmd,can_data[8];
	unsigned short index;

	pt_dev = GetCanOpr("can0");
	len = read_can_data(pt_dev->fd,can_data,&can_id);
	if(len<0)
		return ;

	cmd = can_data[0];
	Sn = (can_data[2]<<8)+can_data[1];
	Sn_id = can_data[3];
	NODE_ID = can_id-0x580;
	index = ((can_data[2] << 8) | can_data[1]);
	
//	DBG_PRINTF("--------rx:%X,%X,%X\n",cmd,can_id,index);
	switch(cmd) 		//命令判断
	{
		case 0x68:
		{
			if(0x33 == NODE_ID)
			{
				wc_data[0] = can_data[2];
				wc_data[1] = can_data[3];
				wc_data[2] = can_data[4];
				DBG_PRINTF("Canon:%02X %02X %02X\r\n",wc_data[0],wc_data[1],wc_data[2]);	
				break;
			}
		}
		case RX_ONE_BYTE:
		{
			break;
		}
		case RX_TWO_BYTE:
		{
			break;
		}
		case RX_THREE_BYTE:
		{
			break;
		}
		case 0x42:
		case RX_FOUR_BYTE:
		{
			switch(index) 		//命令判断
			{
				case PTZ_READ_CMD:			//PTZ角度读取
				{
					ptz_limit_angle = can_data[3];
					ptz_angle[0] = can_data[4];
					ptz_angle[1] = can_data[5];
					ptz_angle[2] = can_data[6];
					ptz_angle[3] = can_data[7];
					//DBG_PRINTF("PTZ:%X %X %X %X \n",ptz_angle[0],ptz_angle[1],ptz_angle[2],ptz_angle[3]);
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case FAIL_OPERATION:
		{
			DBG_PRINTF("%X SDO abort code error , fail \n",can_id);
			DBG_PRINTF("%2X %2X %2X %2X \n",can_data[0],can_data[1],can_data[2],can_data[3]);
			DBG_PRINTF("%2X %2X %2X %2X \n",can_data[4],can_data[5],can_data[6],can_data[7]);
			//CAN0_NMT_frame_manager(NODE_ID,NMT_RESET_NODE);			//重启节点；
			//usleep(100000);
			//Motion_set(0,NODE_ID);		//初始化
			break;
		}
		case SUCCESS_OPERATION:
		{
			break;
		}
		default:
		{
		/*
			myerr("SDO abort code error\n");
			DBG_PRINTF("%X \n",can_id);
			DBG_PRINTF("%2X %2X %2X %2X ",can_data[0],can_data[1],can_data[2],can_data[3]);
			DBG_PRINTF("%2X %2X %2X %2X \n",can_data[4],can_data[5],can_data[6],can_data[7]);
			//CAN0_NMT_frame_manager(NODE_ID,NMT_RESET_NODE);			//重启节点；
			//usleep(500000);
			//Motion_set(0,NODE_ID);		//初始化
			*/
			break;
		}
	}
}
void motion_send(void)
{
	PT_TimeStopList_Opr	time_stop;
	int i;
	static int readcnt;

	time_stop = get_def_time();
	
	if(time_stop->motion_send.Times==0)
	{
		time_stop->motion_send.Times = SEND_TO_MOTION;
		//get_motion_speed();
	}
	else if(time_stop->motion_send.Times==6)
	{
		readcnt ++;
		if(readcnt>=10)
		{
			readcnt=0;
//			DBG_PRINTF("angle read\n");
			get_motion_angle();
		}
	}
	else if(time_stop->motion_send.Times==9)
	{
	}
	/*OCU数据超时检测*/
//	printf("TimeEn=%d,Times=%d\r\n",time_stop->ocu_rx_timeout.TimeEn,time_stop->ocu_rx_timeout.Times);
	if(time_stop->ocu_rx_timeout.TimeEn)
	{
		if(!time_stop->ocu_rx_timeout.Times)		//检测接收OCU超时
		{
			//停止车体动作;
			printf("Platform_Stop\r\n");
			Platform_Stop();
			time_stop->ocu_rx_timeout.TimeEn = 0;
			//disable_bridge(int id)
		}
	}
	/*左侧轮子速度设定数据超时检测*/
	if(time_stop->motion_L_rx_timeout.TimeEn)		
	{
		if(!time_stop->motion_L_rx_timeout.Times)		//检测接收超时
		{
			//停止车体动作;
			//Platform_Stop();
			time_stop->motion_L_rx_timeout.TimeEn = 0;
			motion_L_rx_timeout_stop = 1;
			Track_L_Error = 1;
		}
	}
	/*右侧轮子速度设定数据超时检测*/
	if(time_stop->motion_R_rx_timeout.TimeEn)		
	{
		if(!time_stop->motion_R_rx_timeout.Times)		//检测接收超时
		{
			//停止车体动作;
			//Platform_Stop();
			time_stop->motion_R_rx_timeout.TimeEn = 0;
			motion_R_rx_timeout_stop = 1;
			Track_R_Error = 1;
		}
	}

	for(i=0;i<6;i++)
	{
		if(time_stop->joint[i].TimeEn)
			if(!time_stop->joint[i].Times)
			{
				time_stop->joint[i].TimeEn = 0;
				joint_error[i] = 1;
			}
	}
}

void get_motion_speed(void)			//现在总线数据非常拥挤.所以这里获取转速暂时不做要求;
{
	motion_get_speed(TRACK_L);
	usleep(5000);
	motion_get_speed(TRACK_R);
	//usleep(5000);
	//motion_get_speed(FLIPPER);
	//usleep(5000);
}

void get_motion_angle(void)
{
	motion_get_angle(FLIPPER);
	//motion_get_speed(TRACK_L);
	//motion_get_speed(TRACK_R);
}
