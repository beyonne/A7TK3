#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 

#include <config.h>
#include <store_manager.h>


/*
static int WritePara(Para_Storage_Struct *p, int NumByteToWrite)
{
	FILE *fd;

	DBG_PRINTF("write num byte:%d\n",NumByteToWrite);
	if((fd = fopen("system","wb+")) == NULL)
	{
		DBG_PRINTF("Write open file fail!\n");
		return -1;
	}

	fwrite(p,sizeof(Para_Storage_Struct),1,fd);

	fclose(fd);
	return 0;
}

static int ReadPara(Para_Storage_Struct *p, int NumByteToWrite)
{
	FILE *fd;
	
	if((fd = fopen("system","rb+")) == NULL)
	{
		DBG_PRINTF("Read open system file fail!\n");
		return -1;
	}
	
	fread(p,sizeof(Para_Storage_Struct),1,fd);

	fclose(fd);
	return 0;
}

static int WriteTimePara(Time_Group_Struct *p)
{
	FILE *fd;

	//DBG_PRINTF("write time num byte:%d\n",NumByteToWrite);
	if((fd = fopen("time","wb+")) == NULL)
	{
		DBG_PRINTF("Write open file time fail!\n");
		return -1;
	}

	fwrite(p,sizeof(Time_Group_Struct),7,fd);

	fclose(fd);
	return 0;
}

static int ReadTimePara(Time_Group_Struct *p)
{
	FILE *fd;
	
	if((fd = fopen("time","rb+")) == NULL)
	{
		DBG_PRINTF("Read open time file fail!\n");
		return -1;
	}
	
	fread(p,sizeof(Time_Group_Struct),7,fd);

	fclose(fd);
	return 0;
}

static int WriteHoliPara(Holiday_Group_Struct *p)
{
	FILE *fd;

	//DBG_PRINTF("write time num byte:%d\n",NumByteToWrite);
	if((fd = fopen("holiday","wb+")) == NULL)
	{
		DBG_PRINTF("Write open file holiday fail!\n");
		return -1;
	}

	fwrite(p,sizeof(Holiday_Group_Struct),7,fd);

	fclose(fd);
	return 0;
}

static int ReadHoliPara(Holiday_Group_Struct *p)
{
	FILE *fd;
	
	if((fd = fopen("holiday","rb+")) == NULL)
	{
		DBG_PRINTF("Read open Holiday file fail!\n");
		return -1;
	}
	
	fread(p,sizeof(Holiday_Group_Struct),7,fd);

	fclose(fd);
	return 0;
}
*/
static int WriteFile(char *name,char *p,int Len)
{
	FILE *fd;

	//DBG_PRINTF("write time num byte:%d\n",NumByteToWrite);
	if((fd = fopen(name,"wb+")) == NULL)
	{
		DBG_PRINTF("Write open file fail!\n");
		return -1;
	}

	fwrite(p,Len,1,fd);

	fclose(fd);
	return 0;
}

static int ReadFile(char *name,char *p,int Len)
{
	FILE *fd;
	
	if((fd = fopen(name,"rb+")) == NULL)
	{
		DBG_PRINTF("Read open file fail!\n");
		return -1;
	}
	
	fread(p,Len,1,fd);

	fclose(fd);
	return 0;
}
	
static T_FileStoreOpr g_tFileStoreOpr = {
	.name					= "FileStore",
	.WriteFile  			= WriteFile,
	.ReadFile				= ReadFile,
};

int FileStoreInit(void)
{
	return RegisterFileStoreOpr(&g_tFileStoreOpr);
}

