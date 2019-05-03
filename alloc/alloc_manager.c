#include <alloc_manager.h>
#include <string.h>
#include <config.h>

static PT_MemOpr g_ptMemHead;


int AllocMem(int iNum)
{
	int i;

	PT_MemOpr ptNew;
	

	/* �Ȱ��豸�����framebuffer��������
	 */
	ptNew = malloc(sizeof(T_MemOpr)+MEMSIZE);
	if (ptNew == NULL)
	{
		return -1;
	}

	/* ָ��framebuffer */
	ptNew->ID = 0;
	ptNew->HeadAddr = 0;
	ptNew->EndAddr  = 0;
	ptNew->eMemState = NMS_FREE;
	ptNew->DataAddr = (unsigned char *)(ptNew + 1);
	/* �������� */
	ptNew->ptNext = g_ptMemHead;   //ԭ���ı�ͷ���뵽"�µĽڵ�"�ĺ���
	g_ptMemHead = ptNew;       //��ͷ����"�µĽڵ�"������ "�µĽڵ�"��Ϊ"�µı�ͷ"ʹ��
	
	/*
	 * �������ڻ����Mem
	 */
	for (i = 0; i < iNum; i++)
	{
		/* ����T_NetMem�ṹ�屾���"��framebufferͬ����С�Ļ���" */
		ptNew = malloc(sizeof(T_MemOpr) + MEMSIZE);
		if (ptNew == NULL)
		{
			return -1;
		}
		/* ��T_NetMem�ṹ�����¼��������"��framebufferͬ����С�Ļ���" */
		ptNew->DataAddr = (unsigned char *)(ptNew + 1);

		ptNew->ID = 0;
		ptNew->DataLen = 0;
		ptNew->HeadAddr = 0;
		ptNew->EndAddr = 0;
		ptNew->eMemState = NMS_FREE;
		
		/* �������� */
		ptNew->ptNext = g_ptMemHead;
		g_ptMemHead = ptNew;
	}
	
	return 0;
}

PT_MemOpr GetMem(int iID)        //�˴���ʼ����ʱִ�У��������ݿ飬��ע���ݿ�ʹ��״̬ΪNMS_USED_FOR_CUR
{
	PT_MemOpr ptTmp = g_ptMemHead;

	if(iID<1)
		return NULL;
	
	/* 1. ����: ȡ�����Լ�ռ�õġ�ID��ͬ��netmem */
	while (ptTmp)
	{
		if ((ptTmp->eMemState == NMS_USED_FOR_CUR) && (ptTmp->ID == iID))   
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	DBG_PRINTF("No buff:%d\n",iID);
	/* 2. ���ǰ�治�ɹ�, ȡ��һ�����еĲ�������û�����ݵ�NetMem */
	ShowMem();
	ptTmp = g_ptMemHead;
	while (ptTmp)
	{
		if (ptTmp->eMemState == NMS_FREE)
		{
			ptTmp->ID = iID;
			ptTmp->eMemState = NMS_USED_FOR_CUR;
			memset(ptTmp->DataAddr,0,MEMSIZE);
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}	
    DBG_PRINTF("No mem %d!\n",iID);
	return NULL;
}


PT_MemOpr GetMemData(int iID)
{
	PT_MemOpr ptTmp = g_ptMemHead;

	if(iID<1)
		return NULL;
	
	/* 1. ����: ȡ�����Լ�ռ�õġ�ID��ͬ��netmem */
	while (ptTmp)
	{
		if ((ptTmp->eMemState == NMS_USED_FOR_CUR) && (ptTmp->ID == iID))
		{
			return ptTmp;     //�ɹ�ȡ��
		}
		ptTmp = ptTmp->ptNext;  //�������������鿴��һָ�룬�����ȶ�
	}
    
	return NULL;
}

int PutMem(int iID)
{
	PT_MemOpr ptTmp = g_ptMemHead;
	
	/* 1. ����: ȡ�����Լ�ռ�õġ�ID��ͬ��netmem */
	while (ptTmp)
	{
		if ((ptTmp->eMemState == NMS_USED_FOR_CUR) && (ptTmp->ID == iID))
		{
			ptTmp->eMemState = NMS_FREE;
			ptTmp->HeadAddr = 0;
			ptTmp->EndAddr = 0;
			ptTmp->ID = 0;
			return 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return -1;
}

void ShowMem(void)
{
	PT_MemOpr ptTmp = g_ptMemHead;
	int i=0;
	
	/* 1. ����: ȡ�����Լ�ռ�õġ�ID��ͬ��netmem */
	DBG_PRINTF("--------------------------\n");
	while (ptTmp)
	{
		DBG_PRINTF("Sn:%d,ID:%d,Sta:%d\n",i+1,ptTmp->ID,ptTmp->eMemState);
		i++;
		ptTmp = ptTmp->ptNext;
	}
		DBG_PRINTF("--------------------------\n");
	for(i=0;i<MenNum;i++)
	{
		
	}
		DBG_PRINTF("--------------------------\n");
    
}


int GetDataNum(int fd)
{
	PT_MemOpr pt_Mem;

	pt_Mem = GetMemData(fd);
	//DBG_PRINTF("pt_NetMem:%d\n",pt_NetMem);
	if(pt_Mem==NULL)
	{
		//DBG_PRINTF("fd1:%d\n",fd);
		return -1;
	}
	//DBG_PRINTF("333333\n");
	if(pt_Mem->HeadAddr == pt_Mem->EndAddr)
	{
		return 0;
	}
	
	return 1;
}


