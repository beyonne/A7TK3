#include <can_manager.h>
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
#include <alloc_manager.h>

static PT_CanOpr g_ptCanOprHead;					//��������ͷ
static PT_CanDeviceOpr g_ptCanDeviceOprHead;		//�豸����ͷ

static PT_CanMemOpr g_ptMemHead;					//���ݻ�������ͷ

/*
һ��CANopenЭ���豸��ز��� :
1.NMT������ز���

			COB_ID			D0								
			700+NODE_ID		0x00							�ɴ˵ó��ڵ��NODE_ID��������
										
1.1 Ԥ����	COB_ID	D0	D1							
			0x000	0x80	0x00						�������нڵ����Preoperational Mode
					0x80	NODE_ID						����NODE_ID�ڵ����Preoperation Mode
										
1.2 ����	COB_ID	D0		D1							
			0x000	0x1		0x00						�������нڵ�
					0x1		NODE_ID						����NODE_ID�ڵ�
										
1.3 ֹͣ	COB_ID	D0		D1							
			0x000	0x2		0x00						ֹͣ���нڵ�
					0x2		NODE_ID						ֹͣNODE_ID�ڵ�
										
1.4 ����	COB_ID	D0	D1							
			0x000	0x82	0x00						����ͨѶ
					0x81	NODE_ID						���ýڵ�
			
---------------------------------------------------------------------------------------------------------------------

2.ʹ�ܵ��ʹ��/�ͷ�
3.����Ŀ���ٶ�

---------------------------------------------------------------------------------------------------------------------
CANopenЭ��SDO��ز���:

SDO����	����(��ȡ) COB_ID		D0	 	D1	D2	    D3		D4	D5	D6	D7	
				600+NODE_ID		0x40	����		������	0x00	0x00	0x00	0x00	д�����ݳ���Ϊ1���ֽ�
				
---------------------------------------------------------------------------------------------------------------------											
											
		д��	COB_ID			D0		D1	D2		D3		D4	D5	D6	D7	
				600+NODE_ID		0x2F	����		������	Data0	0x00	0x00	0x00	д�����ݳ���Ϊ1���ֽ�
								0x2B	����		������	Data0	Data1	0x00	0x00	д�����ݳ���Ϊ2���ֽ�
								0x27	����		������	Data0	Data1	Data2	0x00	д�����ݳ���Ϊ3���ֽ�
								0x23	����		������	Data0	Data1	Data2	Data3	д�����ݳ���Ϊ4���ֽ�
									
---------------------------------------------------------------------------------------------------------------------
				COB_ID			D0		D1	D2		D3		D4	D5	D6	D7	
SDO����	����	580+NODE_ID		0x4F	����		������	Data0	0x00	0x00	0x00	�������ݳ���Ϊ1���ֽ�ʱ
								0x4B	����		������	Data0	Data1	0x00	0x00	�������ݳ���Ϊ2���ֽ�ʱ
								0x47	����		������	Data0	Data1	Data2	0x00	�������ݳ���Ϊ3���ֽ�ʱ
								0x43	����		������	Data0	Data1	Data2	Data3	�������ݳ���Ϊ4���ֽ�ʱ
											
				COB_ID			D0		D1	D2		D3		D4	D5	D6	D7	
				580+NODE_ID		0x80	����		������	SDO abort code error				��ȡ��д��ʧ��
											
				COB_ID			D0		D1	D2		D3		D4	D5	D6	D7	
				580+NODE_ID		0x60	����		������	0x00	0x00	0x00	0x00	�ɹ���Ӧд�����������
---------------------------------------------------------------------------------------------------------------------	
SDO abort code error						
��ֹ����		���빦������					
05 03 00 00 	����λû�н���ı�					
05 04 00 00 	SDOЭ�鳬ʱ					
05 04 00 01 	�Ƿ���δ֪��Client/Server������					
05 04 00 02 	��Ч�Ŀ��С����Block Transferģʽ��					
05 04 00 03 	��Ч����ţ���Block Transferģʽ��					
05 03 00 04 	CRC���󣨽�Block Transferģʽ��					
05 03 00 05 	�ڴ����					
06 01 00 00 	����֧�ַ���					
06 01 00 01 	��ͼ��ֻд����					
06 01 00 02 	��ͼдֻ������					
06 02 00 00 	�����ֵ��ж��󲻴���					
06 04 00 41 	�����ܹ�ӳ�䵽PDO 					
06 04 00 42 	ӳ��Ķ������Ŀ�ͳ��ȳ���PDO����					
06 04 00 43 	һ���Բ���������					
06 04 00 47 	һ�����豸�ڲ�������					
06 06 00 00 	Ӳ�������¶������ʧ��					
06 06 00 10 	�������Ͳ�ƥ�䣬����������Ȳ�ƥ��					
06 06 00 12 	�������Ͳ�ƥ�䣬�����������̫��					
06 06 00 13 	�������Ͳ�ƥ�䣬�����������̫��					
06 09 00 11 	������������					
06 09 00 30 	����������ֵ��Χ��д����ʱ��					
06 09 00 31 	д�������ֵ̫��					
06 09 00 32 	д�����ֵ̫С					
06 09 00 36 	���ֵС����Сֵ					
08 00 00 00 	һ���Դ���					
08 00 00 20 	���ݲ��ܴ��ͻ򱣴浽Ӧ��					
08 00 00 21 	���ڱ��ؿ��Ƶ������ݲ��ܴ��ͻ򱣴浽Ӧ��					
08 00 00 22 	���ڵ�ǰ�豸״̬�������ݲ��ܴ��ͻ򱣴浽Ӧ��					
08 00 00 23 	�����ֵ䶯̬�������������ֵ䲻����					
---------------------------------------------------------------------------------------------------------------------
*/

//����fd����CAN���ݽ��ܻ��棬iNumΪ������Ŀ��
int Alloc_can_Mem(int fd,int iNum)
{
	int i;

	PT_CanMemOpr ptNew;
	
	/* �Ȱ��豸�����framebuffer��������
	 */
	ptNew = malloc(sizeof(T_CanMemOpr));
	if (ptNew == NULL)
	{
		return -1;
	}

	/* ָ��framebuffer */
	ptNew->ID = fd;
	ptNew->DataLen = 0;
	ptNew->eMemState = NMS_CAN_FREE;

	/* �������� */
	ptNew->ptNext = g_ptMemHead;
	g_ptMemHead = ptNew;
	
	/*
	 * �������ڻ����Mem
	 */
	for (i = 0; i < iNum; i++)
	{
		/* ����T_NetMem�ṹ�屾���"��framebufferͬ����С�Ļ���" */
		ptNew = malloc(sizeof(T_CanMemOpr));
		if (ptNew == NULL)
		{
			return -1;
		}
		/* ��T_NetMem�ṹ�����¼��������"��framebufferͬ����С�Ļ���" */

		ptNew->ID = fd;
		ptNew->DataLen = 0;
		ptNew->eMemState = NMS_CAN_FREE;
		
		/* �������� */
		ptNew->ptNext = g_ptMemHead;
		g_ptMemHead = ptNew;
	}
	
	return 0;
}

//��ȡ���л���
PT_CanMemOpr Get_can_Mem(int iID)
{
	PT_CanMemOpr ptTmp = g_ptMemHead;

	if(iID<1)
		return NULL;
	
	/* 1. ����: ȡ�����Լ�ռ�õġ�ID��ͬ��netmem */
	while (ptTmp)
	{
		if ((ptTmp->eMemState == NMS_CAN_FREE) && (ptTmp->ID == iID))
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
    
	return NULL;
}

//��ȡ�����е�����
PT_CanMemOpr get_can_mem_data(int iID)
{
	PT_CanMemOpr ptTmp = g_ptMemHead;

	if(iID<1)
		return NULL;
	
	/* 1. ����: ȡ�����Լ�ռ�õġ�ID��ͬ��netmem */
	while (ptTmp)       
	{
		if ((ptTmp->eMemState == NMS_CAN_USED_FOR_CUR) && (ptTmp->ID == iID))
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
    
	return NULL;
}


//�ͷſռ�
int Put_can_Mem(int iID)
{
	PT_CanMemOpr ptTmp = g_ptMemHead;
	
	/* 1. ����: ȡ�����Լ�ռ�õġ�ID��ͬ��netmem */
	while (ptTmp)
	{
		if ((ptTmp->eMemState == NMS_CAN_USED_FOR_CUR) && (ptTmp->ID == iID))
		{
			//ptTmp->eMemState = NMS_FREE;
			//ptTmp->ID = 0;
			return 0;
		}
		ptTmp = ptTmp->ptNext;
	}
    
	return -1;
}


void Show_can_Mem(void)
{
	PT_CanMemOpr ptTmp = g_ptMemHead;
	int i=0;
	
	/* 1. ����: ȡ�����Լ�ռ�õġ�ID��ͬ��netmem */
	printf("--------------------------\n");
	while (ptTmp)
	{
		printf("Sn:%d,ID:%d,Sta:%d\n",i+1,ptTmp->ID,ptTmp->eMemState);
		i++;
		ptTmp = ptTmp->ptNext;
	}
		printf("--------------------------\n");
    
}




int RegisterCanOpr(PT_CanOpr ptCanOpr)
{
	PT_CanOpr ptTmp;

	if (!g_ptCanOprHead)
	{
		g_ptCanOprHead   = ptCanOpr;
		ptCanOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptCanOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptCanOpr;
		ptCanOpr->ptNext = NULL;
	}

	return 0;
}

PT_CanOpr GetDefaultCanOpr(void)
{
	return g_ptCanOprHead;
}


int RegisterCanDeviceOpr(PT_CanDeviceOpr ptCanDeviceOpr)
{
	PT_CanDeviceOpr ptTmp;

	if (!g_ptCanDeviceOprHead)
	{
		g_ptCanDeviceOprHead   = ptCanDeviceOpr;
		ptCanDeviceOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptCanDeviceOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptCanDeviceOpr;
		ptCanDeviceOpr->ptNext = NULL;
	}

	return 0;
}

PT_CanDeviceOpr GetCanOpr(char *pcName)
{
	PT_CanDeviceOpr ptTmp = g_ptCanDeviceOprHead;
	
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

//#define CAN_READ_CALLBACK(CB) void (CB)(int fd, int error, void *buf, int len,int can_id, void *data)
//		rp->cb(rp->fd, err, (void *)frdup.data,frdup.can_dlc,frdup.can_id, rp->data);
CAN_READ_CALLBACK(CanReadCallback) 
{
	unsigned char *buff;
	PT_CanMemOpr pt_Tmp;
	int i;
	PT_CanOpr ptDev;
	buff =(unsigned char *)buf;
	ptDev = g_ptCanOprHead;
	if(error == CAN_ERR_READ)
	{
		printf("--------------CAN  read abnormal!--------------------\n");
		Put_can_Mem(fd);
	}
	if(error == CAN_ERR_OK)
	{
		//ShowNetMem();
		if(len>0)
		{
			pt_Tmp = Get_can_Mem(fd);
			if(pt_Tmp!=NULL)
			{
				memcpy(pt_Tmp->DataAddr,buff,len);
				pt_Tmp->DataLen = len;
				pt_Tmp->can_id = (can_id & CAN_EFF_MASK);
				pt_Tmp->eMemState = NMS_USED_FOR_CUR;
				//ptDev->CANSendData(fd,can_id,buff,len);
				//printf("pt_Tmp->DataLen:%d\n",pt_Tmp->DataLen);
			}
			else
			{
				//printf("GetMen can error!\n");
				//Show_can_Mem();
			}
		}
	}
}

int can_tx_data(int fd,int can_id,unsigned char *tx_buf,int data_len)
{
	PT_CanOpr ptDev;
	ptDev = g_ptCanOprHead;
	if(ptDev->CANSendData(fd,can_id,tx_buf,data_len)!=0)
		return -1;
	return 0;
}


//����CAN�豸
/*
name :�豸����,can0 ,can1
filter_flag :�Ƿ������� 0,1;
can_id :���ܵ�ID,�����Ƕ��0x200;
can_mask :���ε�ID,�����Ƕ��0x201;
filter_number :���λ��߽��ܵ�ID�ĸ���1,2,3....;
can_bit :��׼֡������չ֡ 11,29;
bitrate :������,500000,1000000;
master :Ԥ�������ӹ�������;
*/
int AddCanDevice(char *name,char filter_flag,int *can_id,int *can_mask,int filter_number,char can_bit,int bitrate,int master)
{
	PT_CanDeviceOpr ptNew;
	int i;

	ptNew = malloc(sizeof(T_CanDeviceOpr)+CAN_NAME_MAX_LENTH);
	
	if(!ptNew)
		return -1;
	
	ptNew->name = (char *)(ptNew + 1);       //����ΪʲôҪ+1?????????????????????_zhz
	//ptNew->IP = (char *)(ptNew + 1) + NET_NAME_MAX_LENTH;
	//ptNew->DIP = (char *)(ptNew + 1) +NET_NAME_MAX_LENTH+IP_CHAR_MAX_LENTH;

	memcpy(ptNew->name,name,CAN_NAME_MAX_LENTH);
	
	ptNew->filter_flag = filter_flag;			//�Ƿ�������ID;
	ptNew->filter_number = filter_number;
	
	for(i=0;i<filter_number;i++)
	{
		ptNew->can_id[i] = can_id[i];			//��Ҫ��ID;
		ptNew->can_mask[i] = can_mask[i];			//���ε�ID;
	}
	
	ptNew->af_can = can_bit;			//29,11;
	ptNew->pf_can = can_bit;			//29,11;
	ptNew->bitrate = bitrate;			//������;
	ptNew->master = master;
//	printf("ptNew:%d\n",ptNew);
//	printf("ptNew-name:%d\n",ptNew->name);
//	printf("ptNew-ip:%d\n",ptNew->IP);
//	printf("ptNew-dip:%d\n",ptNew->DIP);

	RegisterCanDeviceOpr(ptNew);

	return 0 ;
}


int CANDevInit(void)
{
	int iError = -1;
	
	PT_CanOpr ptTmp = g_ptCanOprHead;
	PT_CanDeviceOpr ptDevTmp=g_ptCanDeviceOprHead;

	if(ptTmp)
	{
		while(ptDevTmp)
		{	
			//printf("can\n");

			if(ptTmp->CANInit(&ptDevTmp->fd,ptDevTmp->name,ptDevTmp->filter_flag,ptDevTmp->can_id,\
			ptDevTmp->can_mask,ptDevTmp->filter_number,ptDevTmp->af_can,ptDevTmp->pf_can)==0)
			{
				printf("init:%s ...ok\n",ptDevTmp->name);
				Alloc_can_Mem(ptDevTmp->fd,128);			//
				ptTmp->CANRead(ptDevTmp->fd,ptDevTmp->master,CanReadCallback,NULL);
			}
			else
			{
				printf("init:%s fail!\n",ptDevTmp->name);
			}
						
			ptDevTmp = ptDevTmp->ptNext;
		}
	}
	return iError;
}

static void print_frame(struct can_frame *fr)
{	
	int i;	
	printf("%08x\n", fr->can_id & CAN_EFF_MASK);	
	//printf("%08x\n", fr->can_id);	
	printf("dlc = %d\n", fr->can_dlc);	
	printf("data = ");	
	for (i = 0; i < fr->can_dlc; i++)		
	printf("%02x ", fr->data[i]);	
	printf("\n");
}



static void handle_err_frame(const struct can_frame *fr)
{	
	if (fr->can_id & CAN_ERR_TX_TIMEOUT) 
	{		
	errout("CAN_ERR_TX_TIMEOUT");	
	}	
	if (fr->can_id & CAN_ERR_LOSTARB) 
	{		errout("CAN_ERR_LOSTARB");
	errcode(fr->data[0]);	}	
	if (fr->can_id & CAN_ERR_CRTL) 
	{		errout("CAN_ERR_CRTL");
	errcode(fr->data[1]);	}	
	if (fr->can_id & CAN_ERR_PROT) 
	{		errout("CAN_ERR_PROT");		
	errcode(fr->data[2]);		
	errcode(fr->data[3]);	}	
	if (fr->can_id & CAN_ERR_TRX) 
	{		errout("CAN_ERR_TRX");		
	errcode(fr->data[4]);	}	
	if (fr->can_id & CAN_ERR_ACK) 
	{		errout("CAN_ERR_ACK");	
	}	if (fr->can_id & CAN_ERR_BUSOFF) 
	{		
		errout("CAN_ERR_BUSOFF");	
	}	if (fr->can_id & CAN_ERR_BUSERROR) 
	{		
		errout("CAN_ERR_BUSERROR");	
	}	
	if (fr->can_id & CAN_ERR_RESTARTED) 
	{		
		errout("CAN_ERR_RESTARTED");	
	}

}

int Can_conctrl_Register(void)
{
	CanRegister();
}


