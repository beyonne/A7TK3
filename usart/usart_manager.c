#include <usart_manager.h>
#include <config.h>
#include <string.h>
#include <alloc_manager.h>

static PT_UsartOpr g_ptUsartOprHead;					//��������ͷ
static PT_UsartDeviceOpr g_ptUsartDeviceOprHead;		//�豸����ͷ



int RegisterUsartOpr(PT_UsartOpr ptUsartOpr)
{
	PT_UsartOpr ptTmp;

	if (!g_ptUsartOprHead)
	{
		g_ptUsartOprHead   = ptUsartOpr;
		ptUsartOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptUsartOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptUsartOpr;
		ptUsartOpr->ptNext = NULL;
	}

	return 0;
}

PT_UsartOpr GetDefaultUsartOpr(void)
{
	return g_ptUsartOprHead;
}


int RegisterUsartDeviceOpr(PT_UsartDeviceOpr ptUsartDeviceOpr)
{
	PT_UsartDeviceOpr ptTmp;

	if (!g_ptUsartDeviceOprHead)
	{
		g_ptUsartDeviceOprHead   = ptUsartDeviceOpr;
		ptUsartDeviceOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptUsartDeviceOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptUsartDeviceOpr;
		ptUsartDeviceOpr->ptNext = NULL;
	}

	return 0;
}

PT_UsartDeviceOpr GetUsartOpr(char *pcName)
{
	PT_UsartDeviceOpr ptTmp = g_ptUsartDeviceOprHead;
	
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


//#define COM_READ_CALLBACK(CB) void (CB)(int fd, int error, void *buf, int len, void *data)

/*
while (1) 
{
	usleep(5000);
	received = read(rp->fd, buf, 4096);
	if(rp->fd==10)
	printf("---------------------------------len:%d fd:%d\n",received,rp->fd);
	//if (received == 0) 
	{
	//	err = COM_ERR_CLOSED;
	//	goto close_com;
	} 
	if (received < 0) 
	{
		if (errno == EINTR) continue; 
		err = COM_ERR_READ;
		goto close_com;
	} 
	
	rp->cb(rp->fd, err, (void *)buf, received, rp->data);   //�����ڵײ�˿�read()������ݷ������ݻ�������
	
}
*/


COM_READ_CALLBACK(ComReadCallback) 
{
	unsigned char *buff;
	PT_MemOpr pt_Tmp;
	int i;
	int max_length;
	
	buff =(unsigned char *)buf;
	
	if (error == COM_ERR_CLOSED) 
	{
		DBG_PRINTF("------------com closed connection!fd:%d--------------\n",fd);
		//ShowNetMem();
		PutMem(fd);
		//ShowNetMem();
	} 
	else if(error == COM_ERR_READ)
	{
		DBG_PRINTF("--------------com abnormal!--------------------\n");
		PutMem(fd);
	}
	if(error == COM_ERR_OK)     //����״̬����
	{
		//ShowNetMem();
		if(len>0)       //��ʾ������
		{
			pt_Tmp = GetMem(fd);  //�򿪽��ջ�����ָ��
			
			//DBG_PRINTF(" ----\n");
			
			if(pt_Tmp!=NULL)    //������
			{	
				//printf("---1---\n");   
				if((pt_Tmp->HeadAddr+len)<=MEMSIZE)   //û�з���������������������
				{
					memcpy(&pt_Tmp->DataAddr[pt_Tmp->HeadAddr],buff,len);   //������ֱ��׷�ӽ�������
					
					pt_Tmp->HeadAddr+=len;
					pt_Tmp->DataLen += len;
					if(pt_Tmp->HeadAddr>=MEMSIZE)
						pt_Tmp->HeadAddr = 0;
				}
				else                               //����������������
				{
					//printf("---3---\n");
					max_length = (MEMSIZE-pt_Tmp->HeadAddr);    //�Ƚ�һ������������������������MEMSIZE�����
					
					memcpy(&pt_Tmp->DataAddr[pt_Tmp->HeadAddr],buff,max_length);  
					
					pt_Tmp->HeadAddr = 0;      //ʣ�µ��ǲ������ݣ��ӻ�������ʼ����д��
					memcpy(&pt_Tmp->DataAddr[pt_Tmp->HeadAddr],&buff[max_length],(len-max_length));
					pt_Tmp->HeadAddr+=(len-max_length); //��������ͷָ��
					pt_Tmp->DataLen += len;    //�������ݳ���

					if(pt_Tmp->HeadAddr>=MEMSIZE)    //�������ͷָ�����
						pt_Tmp->HeadAddr = 0;    //ǿ�ƽ�ָ��ָ�򻺴�����ʼ��
				}
				//DBG_PRINTF("pt_Tmp->DataLen:%d\n",pt_Tmp->DataLen);
			}
			else
			DBG_PRINTF("GetNetMen error!\n");
		}
	}
	else
		DBG_PRINTF("comx error!\n");
	
}



int UsartDevInit(void)
{
	int iError = -1;
	
	PT_UsartOpr ptTmp = g_ptUsartOprHead;
	PT_UsartDeviceOpr ptDevTmp=g_ptUsartDeviceOprHead;

	if(ptTmp)
	{
		while(ptDevTmp)
		{	
			
			if(ptTmp->USARTOpen(&ptDevTmp->fd,ptDevTmp->dev) == 0)
			{
				
				if(ptTmp->USART0Init(ptDevTmp->fd,ptDevTmp->speed,ptDevTmp->flowCtrl,ptDevTmp->databits,ptDevTmp->stopbis,ptDevTmp->parity)==0)
				{
					DBG_PRINTF("init:%s fd:%d!\n",ptDevTmp->name,ptDevTmp->fd);
					//if(strcmp(COM_HEAD,ptDevTmp->name)!=0)
					ptTmp->ComRead(ptDevTmp->fd,ComReadCallback,NULL);  
					//�˴�ͨ��ComReadCallback����COM_READ_CALLBACK(*cb)����ʼִ�д������ݶ�ȡ�߳�
					//ComRead(int fd, COM_READ_CALLBACK(*cb), void *data)
					//pthread_create(&thr_id, NULL, &Thread_read, (void *)rp)
					//rp->cb(rp->fd, err, (void *)buf, received, rp->data);   //�����ڵײ�˿�read()������ݷ������ݻ�������(��)_zhz
					//COM_READ_CALLBACK(ComReadCallback) 
					//pt_Tmp = GetMem(fd);  //�򿪽��ջ�����ָ��,�˴��������ݺ�ѭ��������������������COM_READ_CALLBACK(ComReadCallback) 
				}
				else
				{
					DBG_PRINTF("init:%s fail!\n",ptDevTmp->name);
				}
			}
			else
			{
				DBG_PRINTF("open:%s fail!\n",ptDevTmp->name);
			}
			ptDevTmp = ptDevTmp->ptNext;
		}
	}
	return iError;
}

int UsartSendData(char *devname,char *data,int len)
{
	int iError = -1;
	
	PT_UsartOpr ptTmp = g_ptUsartOprHead;
	PT_UsartDeviceOpr ptDevTmp;

	ptDevTmp = GetUsartOpr(devname);

	while (ptTmp)
	{
		while(ptDevTmp)
		{
			ptTmp->USARTSendData(ptDevTmp->fd,data,len);
		}
	}
}

int UsartSendDatafd(int fd,unsigned char *data ,int lenth)  //UART�ӿڿ���PTZ
{
	int iError = -1;
	PT_UsartOpr ptTmp = g_ptUsartOprHead;
	ptTmp->USARTSendData(fd,data,lenth);		
	return 0;
}

//���������豸
//name:���ںţ�dev:�豸·���Լ������ļ����ƣ�speed:�����ʣ�flowCtrl:�������أ�
//databits:����λ,stopbit:ֹͣλ,parity:��żУ�飻
int AddUsartDevice(char *name,char *dev,int speed,int flowCtrl,int databits,int stopbit,int parity)
{
	PT_UsartDeviceOpr ptNew;
	
	ptNew = malloc(sizeof(T_UsartDeviceOpr)+COM_NAME_MAX_LENTH+DEVICE_FILE_MAX_LENTH);

	ptNew->name = (unsigned char *)(ptNew + 1);
	ptNew->dev= (unsigned char *)(ptNew + 1) + COM_NAME_MAX_LENTH;

	memcpy(ptNew->name,name,COM_NAME_MAX_LENTH);
	memcpy(ptNew->dev,dev,DEVICE_FILE_MAX_LENTH);

	ptNew->speed = speed;
	ptNew->flowCtrl = flowCtrl;
	ptNew->databits = databits;
	ptNew->stopbis = stopbit;
	ptNew->parity = parity;

	RegisterUsartDeviceOpr(ptNew);
	return 0;
}

int Usart_Register(void)
{
	int Error = -1;
	Error = UsartRegister();

	return Error;
}


