#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>
#include <store_manager.h>
#include <para.h>
#include <net_manager.h>

static PT_Sys_Para_Opr g_ptSysParaHead;


int RegisterSysParaOpr(PT_Sys_Para_Opr ptSyaParaOpr)
{
	PT_Sys_Para_Opr ptTmp;


	g_ptSysParaHead   = ptSyaParaOpr;


	return 0;
}


PT_Sys_Para_Opr GetDefultSysParaOpr(void)
{
	return g_ptSysParaHead;
}

int ParaInit(void)
{
	int i,j;
	
	PT_Sys_Para_Opr pt_Sys_Para;
	pt_Sys_Para = malloc(sizeof(T_Sys_Para_Opr));

	RegisterSysParaOpr(pt_Sys_Para);
	//DBG_PRINTF("para:%d\n",pt_Sys_Para);

	if(GetSysPara(pt_Sys_Para)<0)
	{
		DBG_PRINTF("Para read fail!\n");
	}
	
	if(pt_Sys_Para->SetFlag!=0x12345678)
	{
		//��ʼ������
		DBG_PRINTF("Para Initing !\n");
		pt_Sys_Para->DeviceSn[0] = 0xFF;		
		pt_Sys_Para->DeviceSn[1] = 0xFF;		
		pt_Sys_Para->DeviceSn[2] = 0xFF;		
		pt_Sys_Para->DeviceSn[3] = 0xFF;		
		
		pt_Sys_Para->DIP_Addr[0] = 120; //120.24.227.181
		pt_Sys_Para->DIP_Addr[1] = 24; 	//52.58.57.163
		pt_Sys_Para->DIP_Addr[2] = 227; 
		pt_Sys_Para->DIP_Addr[3] = 181; 

		pt_Sys_Para->IP_Addr[0] = 192;
		pt_Sys_Para->IP_Addr[1] = 168;
		pt_Sys_Para->IP_Addr[2] = 1;
		pt_Sys_Para->IP_Addr[3] = 136;
		
		
		memcpy(pt_Sys_Para->DeviceName,"admin",5);
		pt_Sys_Para->Device_NameLen = 5;
		memcpy(pt_Sys_Para->Password,"888888",6);
		pt_Sys_Para->Passwordlen = 6;

		pt_Sys_Para->ParaSetFlag = 0;
		pt_Sys_Para->SetFlag = 0x12345678;
		pt_Sys_Para->BfCfflag = 0;

		for(i=0;i<MenNum;i++)
		{
			if(Devclient[i].fd==0)
			{
				Devclient[i].fd = 0;
				Devclient[i].Port = 0;
				Devclient[i].answer = 0;
				Devclient[i].cmd = 0;
				break;
			}
		}
		
/*
		DorpTable(DEV_INFO);
		DorpTable(GROUP_INFO);
		DorpTable(SCENE_INFO);
		DorpTable(SENSORLD_INFO);
		
		CreateGroupTable(GROUP_INFO);
		CreateGroupTable(SCENE_INFO);
		CreateTable(DEV_INFO);
		CreateLdTable(SENSORLD_INFO);
*/
		SetSysPara(pt_Sys_Para);
		
	}

	pt_Sys_Para->DeviceVersion = 0x010C;
	pt_Sys_Para->codeVersion = 0x03;
	
	return 0;
}

void ParaFree(void)
{
	free(g_ptSysParaHead);
}

int GetSysPara(PT_Sys_Para_Opr pt_Sys_Para)
{
	PT_FileStoreOpr FileStore;
	
	FileStore = GetFileOpr("FileStore");

	return FileStore->ReadFile("/opt/syspara",pt_Sys_Para,sizeof(T_Sys_Para_Opr));
}

int SetSysPara(PT_Sys_Para_Opr pt_Sys_Para)
{
	PT_FileStoreOpr FileStore;
	
	FileStore = GetFileOpr("FileStore");

	return FileStore->WriteFile("/opt/syspara",pt_Sys_Para,sizeof(T_Sys_Para_Opr));
	
}



