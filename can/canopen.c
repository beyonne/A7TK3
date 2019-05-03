#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <canopen.h>
#include <can_manager.h>





int read_can_data(int fd,unsigned char *can_data,int *can_id)
{
	PT_CanMemOpr pt_Tmp;
	int i;

	
	pt_Tmp = get_can_mem_data(fd);

	if(pt_Tmp==NULL)
	{
		//myerr("pt_Tmp==NULL\n");
		return -1;
	}
	if(pt_Tmp->DataLen>8)
	{
		myerr("datalen>8\n");
		return -1;
	}
/*
	printf("%08X\n", pt_Tmp->can_id );		
	
	printf("dlc = %d\n",pt_Tmp->DataLen);	
	printf("data = ");	
	
	for (i = 0; i < pt_Tmp->DataLen; i++)		
	printf("%02X ", pt_Tmp->DataAddr[i]);	
	
	printf("\n");
*/
	memcpy(can_data,pt_Tmp->DataAddr,pt_Tmp->DataLen);
	*can_id = pt_Tmp->can_id;
	
	pt_Tmp->eMemState = NMS_CAN_FREE;

	return pt_Tmp->DataLen;
	
}


//数据请求，数据读
int SDO_request_frame(int fd,int node_id,short index ,unsigned char subindex)
{
	int can_id ;
	unsigned char data[8];

	can_id = SDO_WRITE_ID + node_id;

	data[0] = TX_REQUEST_FRAME;
	
	data[1] = index;
	data[2] = index>>8;
	data[3] = subindex;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	
	if(can_tx_data(fd,can_id,data,8)!=0)
	return -1;

	return 0;
}

//NMT管理
int NMT_frame(int fd ,int node_id ,unsigned char NMT_cmd)
{
	int can_id = NMT_ID;
	unsigned char data[8];

	can_id = NMT_ID;

	data[0] = NMT_cmd;
	
	data[1] = node_id;
	data[2] = 0;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	
	if(can_tx_data(fd,can_id,data,8)!=0)
	return -1;

	return 0;
}

int CAN0_NMT_frame_manager(int node_id ,unsigned char NMT_cmd)
{
	PT_CanDeviceOpr ptDevTmp;
	
	ptDevTmp=GetCanOpr("can0");

	
	if(NMT_frame(ptDevTmp->fd,node_id,NMT_cmd)!=0)
	return -1;

	return 0;
}


//SDO数据写，数据帧,数据低位在前，高位在后
int SDO_write_frame(int fd,int node_id,short index ,unsigned char subindex,unsigned char *send_data,int len)
{
	int can_id ;
	unsigned char data[8];

	can_id = SDO_WRITE_ID + node_id;    // 0x600+node_id

	if(len>4)
	return -1;
	

	switch(len) 		//命令判断,填充data[0]，确定用户数据的字节数
	{
		case 1:
		{
			data[0] = TX_ONE_BYTE;
			break;
		}
		case 2:
		{
			data[0] = TX_TWO_BYTE;
			break;
		}
		case 3:
		{
			data[0] = TX_THREE_BYTE;
			break;
		}
		case 4:
		{
			data[0] = TX_FOUR_BYTE;
			break;
		}
		default:
		{
			data[0] = TX_REQUEST_FRAME;
			break;
		}
	}

	data[1] = index;    //填充data[1]
	data[2] = index>>8; //填充data[2]
	data[3] = subindex; //填充data[3]
	
	memcpy(&data[4],send_data,len);   //填充data[4]、data[5]、data[6]、data[7]
	
	if(can_tx_data(fd,can_id,data,8)!=0) //调用发送函数
	return -1;

	return 0;

}

//SD0数据写，数据帧,数据低位在前，高位在后
int can0_SDO_write_frame(int node_id,short index,unsigned char subindex,unsigned char * send_data,int len)
{
	PT_CanDeviceOpr ptDevTmp;
	
	ptDevTmp=GetCanOpr("can0");

	if(SDO_write_frame(ptDevTmp->fd,node_id,index,subindex,send_data,len)!=0)
	return -1;
	usleep(500);
	return 0;
	
}	

//SD0数据写，数据帧,数据低位在前，高位在后
int can0_SDO_request_frame(int node_id,short index,unsigned char subindex)
{
	PT_CanDeviceOpr ptDevTmp;
	
	ptDevTmp=GetCanOpr("can0");

	if(SDO_request_frame(ptDevTmp->fd,node_id,index,subindex)!=0)
	return -1;
	usleep(500);
	return 0;
	
}	




//PDO数据写，数据帧,数据低位在前，高位在后
int PDO2_write_frame(int fd,int node_id,short index ,unsigned char subindex,unsigned char *send_data,int len)
{
	int can_id ;
	unsigned char data[8];

	can_id = PDO2_WRITE_ID + node_id;

	if(len>4)
	return -1;
	

	switch(len) 		//命令判断
	{
		case 1:
		{
			data[0] = TX_ONE_BYTE;
			break;
		}
		case 2:
		{
			data[0] = TX_TWO_BYTE;
			break;
		}
		case 3:
		{
			data[0] = TX_THREE_BYTE;
			break;
		}
		case 4:
		{
			data[0] = TX_FOUR_BYTE;
			break;
		}
		default:
		{
			data[0] = TX_REQUEST_FRAME;
			break;
		}
	}

	data[1] = index;
	data[2] = index>>8;
	data[3] = subindex;
	
	memcpy(&data[4],send_data,len);
	
	if(can_tx_data(fd,can_id,data,8)!=0)
	return -1;

	return 0;

}

//PDO数据写，数据帧,数据低位在前，高位在后
int can0_PDO2_write_frame(int node_id,short index,unsigned char subindex,unsigned char * send_data,int len)
{
	PT_CanDeviceOpr ptDevTmp;
	
	ptDevTmp=GetCanOpr("can0");

	if(PDO2_write_frame(ptDevTmp->fd,node_id,index,subindex,send_data,len)!=0)
	return -1;

	return 0;
	
}	

