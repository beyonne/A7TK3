#include <config.h>
#include <net_manager.h>
#include <string.h>
#include <alloc_manager.h>


static PT_TCPWROpr g_ptTCPWROprHead;
static PT_TCPWROpr g_ptDefaultTcpOpr;
static PT_NetDeviceOpr g_ptNetDeviceOprHead;


T_ClientOpr Devclient[MenNum+1];
T_ClientOpr Devudp[MenNum];


/**********************************************************************
 * �������ƣ� RegisterOutputOpr
 * ���������� ע��"����ģ��"
 * ��������� ptOutputOpr - ����ģ��Ľṹ��ָ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2015/02/08	     V1.0	  ������	      ����
 ***********************************************************************/
int RegisterNetOpr(PT_TCPWROpr ptTcpOpr)
{
	PT_TCPWROpr ptTmp;

	if (!g_ptTCPWROprHead)
	{
		g_ptTCPWROprHead   = ptTcpOpr;
		ptTcpOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptTCPWROprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptTcpOpr;
		ptTcpOpr->ptNext = NULL;
	}

	return 0;
}

PT_TCPWROpr GetNetOpr(char *pcName)
{
	PT_TCPWROpr ptTmp = g_ptTCPWROprHead;
	
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

PT_TCPWROpr GetDefaultNetOpr(void)
{
	return g_ptTCPWROprHead;
}


int RegisterNetDeviceOpr(PT_NetDeviceOpr ptTcpOpr)
{
	PT_NetDeviceOpr ptTmp;

	if (!g_ptNetDeviceOprHead)
	{
		g_ptNetDeviceOprHead   = ptTcpOpr;
		ptTcpOpr->ptNext = NULL;
		//DBG_PRINTF("g_ptNetDeviceOprHead:%d\n",g_ptNetDeviceOprHead);
	}
	else
	{
		ptTmp = g_ptNetDeviceOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptTcpOpr;
		ptTcpOpr->ptNext = NULL;
		//DBG_PRINTF("nameptTmp:%d\n",ptTcpOpr->ptNext);
	}
	return 0;
}

PT_NetDeviceOpr GetNetDeviceOpr(char *pcName)
{
	PT_NetDeviceOpr ptTmp = g_ptNetDeviceOprHead;
	
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

//(int fd, int error, void *buf, int len, void *data)
//��ȡ���ݻص�����
SNL_READ_CALLBACK(UDPReadCallback) 
{
	unsigned char *buff;
	PT_MemOpr pt_Tmp;
//	int i;
	
	buff =(unsigned char *)buf;
	if(error == SNL_ERR_CLOSED)
	{
		DBG_PRINTF("UDP client closed connection!\n");
		PutMem(fd);
	} 
	else if(error == SNL_ERR_READ)
	{
		DBG_PRINTF("UDP client abnormal!\n");
		PutMem(fd);
	}
	if(error == SNL_ERR_OK)
	{
		pt_Tmp = GetMem(fd);
		if(pt_Tmp!=NULL)
		{
			if(!pt_Tmp)
			{
				DBG_PRINTF("UDP buff err!\n");
				return ;
			}

			if((len+pt_Tmp->HeadAddr)>(MEMSIZE-MEMYLSIZE))
			{
				pt_Tmp->HeadAddr = 0;
				pt_Tmp->EndAddr = 0;
			}
			memcpy(&pt_Tmp->DataAddr[pt_Tmp->HeadAddr],buff,len);
			pt_Tmp->HeadAddr+=len;
			if(pt_Tmp->HeadAddr>=MEMSIZE-MEMYLSIZE)
				pt_Tmp->HeadAddr = 0;
			//pt_Tmp->HeadAddr = 0;
			buff[len]='\0';
			pt_Tmp->DataLen = len;
			DBG_PRINTF("udp RX:%s\nlen:%d\n",buff,len);
		}
		else 
			DBG_PRINTF("GetNetMen error!\n");
	}
}

//SNL_READ_CALLBACK(ReadCallback)
void ReadCallback(int fd, int error, void *buf, int len, void *data)
{
	unsigned char *buff;
	PT_MemOpr pt_Tmp;
	int i;
	buff = (unsigned char *)buf;
	if(error == SNL_ERR_CLOSED)
	{
		DBG_PRINTF("------------TCP client closed connection!fd:%d--------------\n",fd);
		//ShowNetMem();
		PutMem(fd);

		for(i=0;i<MenNum;i++)
		{
			if(Devclient[i].fd==fd)
			{
				Devclient[i].fd = 0;
				Devclient[i].Port = 0;
				Devclient[i].answer = 0;
				Devclient[i].cmd = 0;
				break;
			}
		}
		//ShowNetMem();
	}
	else if(error == SNL_ERR_READ)
	{
		DBG_PRINTF("-----------------TCP client abnormal!-----------------\n");
		PutMem(fd);

		for(i=0;i<MenNum;i++)
		{
			if(Devclient[i].fd==fd)
			{
				Devclient[i].fd = 0;
				Devclient[i].Port = 0;
				Devclient[i].answer = 0;
				Devclient[i].cmd = 0;
				break;
			}
		}
	}
	else if(error == SNL_ERR_OK)
	{
		//ShowNetMem();
		pt_Tmp = GetMem(fd);
		if(pt_Tmp!=NULL)
		{
			if((pt_Tmp->HeadAddr+len)<MEMSIZE)
			{
				memcpy(&pt_Tmp->DataAddr[pt_Tmp->HeadAddr],buff,len);
				pt_Tmp->HeadAddr+=len;
				pt_Tmp->DataLen = len;
			}
			else
			{
				pt_Tmp->HeadAddr = 0;
				memcpy(&pt_Tmp->DataAddr[pt_Tmp->HeadAddr],buff,len);
				pt_Tmp->HeadAddr+=len;
				pt_Tmp->DataLen = len;
			}
			printf("pt_Tmp->DataLen:%d\n",pt_Tmp->DataLen);
		}
		else
			DBG_PRINTF("GetNetMen error!\n");
	}
}

//SNL_LISTEN_CALLBACK(ListenCallback)
void ListenCallback(unsigned short port, int error, int fd, void *data)
{
	PT_TCPWROpr ptTmp;
	
	DBG_PRINTF("TCP server connected:fd:%d\n",fd);
	
	if(error==SNL_ERR_OK)
	{
		ptTmp = GetNetOpr("netdevice");
		ptTmp->Read(fd,ReadCallback,data);
	}
	else
	{
		DBG_PRINTF("TCP server connected err :%d\n",error);
	}
}



/**********************************************************************
 * �������ƣ� InitTcpClient
 * ���������� ��ʼ���ͻ��������豸
 * ��������� name - ����
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2015/02/08	     V1.0	  ������	      ����
 ***********************************************************************/
int InitTcpClient(char *devname)
{
	PT_TCPWROpr ptTmp;
	PT_NetDeviceOpr ptNetDeviceOpr;

	ptTmp = GetNetOpr("netdevice");
	
	ptNetDeviceOpr = GetNetDeviceOpr(devname);
	
	if ((ptTmp)&&(ptNetDeviceOpr))
	{
		if ((ptNetDeviceOpr->fd = ptTmp->Connect(ptNetDeviceOpr->DIP,ptNetDeviceOpr->DPort)) < 0) 
		{

			return -1;
		} 

		ptTmp->Read(ptNetDeviceOpr->fd,ReadCallback, NULL);

		
		DBG_PRINTF("connect server fd %d\n",ptNetDeviceOpr->fd);
		return 0;
	}
	DBG_PRINTF("No find %s\n",devname);

	return -1;
}

//��ʼ��TCP�����������豸
int InitTcpServer(char *devname)
{
	PT_TCPWROpr ptTmp;
	PT_NetDeviceOpr ptNetDeviceOpr;
	ptTmp = GetNetOpr("netdevice");
	
	ptNetDeviceOpr = GetNetDeviceOpr(devname);
	DBG_PRINTF("IP:%s\n",ptNetDeviceOpr->IP);
	if(ptTmp&&ptNetDeviceOpr)
	{
		if ((ptNetDeviceOpr->fd = ptTmp->Listen(ptNetDeviceOpr->IP,ptNetDeviceOpr->Port,ListenCallback,NULL)) < 0) 
		{
			return -1;
		}
		return 0;
	}
	return -1;
}

//��ʼ��UDP�����������豸
int InitUDPServer(char *devname)
{
	PT_TCPWROpr ptTmp;
	PT_NetDeviceOpr ptNetDeviceOpr;

	ptTmp = GetNetOpr("netdevice");
	
	ptNetDeviceOpr = GetNetDeviceOpr(devname);
	
	if (ptTmp&&ptNetDeviceOpr)
	{
		if ((ptNetDeviceOpr->fd = ptTmp->UdpServerInit(ptNetDeviceOpr->Port,ptNetDeviceOpr->sin))<0) 
		{
			return -1;
		} 

		if(ptTmp->UDPRead(ptNetDeviceOpr->fd,UDPReadCallback,ptNetDeviceOpr->sin)<0)
		return -1;

		return 0;
	}
	return -1;
}

//��ʼ��UDP�ͻ��������豸
int InitUDPClient(char *devname)
{
	PT_TCPWROpr ptTmp;
	PT_NetDeviceOpr ptNetDeviceOpr;

	ptTmp = GetNetOpr("netdevice");
	
	ptNetDeviceOpr = GetNetDeviceOpr(devname);
	
	if (ptTmp&&ptNetDeviceOpr)
	{
		if ((ptNetDeviceOpr->fd = ptTmp->UdpClientInit(ptNetDeviceOpr->DIP,ptNetDeviceOpr->DPort,&ptNetDeviceOpr->sin))<0)
		{
			return -1;
		} 
		//DBG_PRINTF("udp send fd:%d\n",ptNetDeviceOpr->fd);

		return 0;
	}

	return -1;
}

int UdpSend(char *devname,char *data,int len)
{
	PT_TCPWROpr ptTmp;
	PT_NetDeviceOpr ptNetDeviceOpr;
	
	ptTmp = GetNetOpr("netdevice");

	ptNetDeviceOpr = GetNetDeviceOpr(devname);

	if(ptNetDeviceOpr&&ptTmp)
	{
		//DBG_PRINTF("ptNetDeviceOpr->fd:%d\n",ptNetDeviceOpr->fd);
		if(ptTmp->UdpClientSend(ptNetDeviceOpr->fd,data,len,ptNetDeviceOpr->sin)==0)
		return 0;
	}
	//DBG_PRINTF("2\n");
	return -1;
}

int UdpSendWay(char *DIP,int port,char *data,int len)
{
	PT_TCPWROpr ptTmp;
	
	ptTmp = GetNetOpr("netdevice");

	if(ptTmp)
	{
		if(ptTmp->SnlUdpSend(DIP,port,data,len)>0)
		return 0;
	}

	return -1;
}


//��������
int SendData(int fd,unsigned char *data,int len)
{
	PT_TCPWROpr ptTmp;
	
	ptTmp = GetNetOpr("netdevice");
	
	if(ptTmp)
	{
		if(ptTmp->Write(fd,data,len)==0)
		{
			//usleep(1000);
			return 0;
		}
	}
	return -1;
}

//���������豸
int AddNetDevice(char *name,char *IP,int Port,char *DIP,int DPort)
{
	PT_NetDeviceOpr ptNew;
	int i;

	ptNew = malloc(sizeof(T_NetDeviceOpr)+NET_NAME_MAX_LENTH+IP_CHAR_MAX_LENTH+DIP_CHAR_MAX_LENTH);
	if(!ptNew)
	return -1;
	
	ptNew->name = (char *)(ptNew + 1);
	ptNew->IP = (char *)(ptNew + 1) + NET_NAME_MAX_LENTH;
	ptNew->DIP = (char *)(ptNew + 1) + NET_NAME_MAX_LENTH+IP_CHAR_MAX_LENTH;

	memcpy(ptNew->name,name,NET_NAME_MAX_LENTH);
	memcpy(ptNew->IP,IP,IP_CHAR_MAX_LENTH);
	memcpy(ptNew->DIP,DIP,DIP_CHAR_MAX_LENTH);
	
	ptNew->DPort = DPort;
	ptNew->Port = Port;

//	DBG_PRINTF("ptNew:%d\n",ptNew);
//	DBG_PRINTF("ptNew-name:%d\n",ptNew->name);
//	DBG_PRINTF("ptNew-ip:%d\n",ptNew->IP);
//	DBG_PRINTF("ptNew-dip:%d\n",ptNew->DIP);
	RegisterNetDeviceOpr(ptNew);
	return 0 ;
}

//ע������
int NetRegister(void)
{
	int iError = 0;
	iError = TcpInit();			//ע������
	//iError = InitTcpClient("tcpclient0");
	return iError;
}


