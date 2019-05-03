#include <store_manager.h>
#include <config.h>

static PT_SqlStoreOpr g_ptSqlStoreOprHead;
static PT_FileStoreOpr g_ptFileStoreOprHead;


int RegisterSqlStoreOpr(PT_SqlStoreOpr ptSqlStoreOpr)
{
	PT_SqlStoreOpr ptTmp;

	if (!g_ptSqlStoreOprHead)
	{
		g_ptSqlStoreOprHead   = ptSqlStoreOpr;
		ptSqlStoreOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptSqlStoreOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptSqlStoreOpr;
		ptSqlStoreOpr->ptNext = NULL;
	}

	return 0;
}


PT_SqlStoreOpr GetsqlOpr(char *pcName)
{
	PT_SqlStoreOpr ptTmp = g_ptSqlStoreOprHead;
	
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

PT_SqlStoreOpr GetDefultSqlOpr(void)
{
	return g_ptSqlStoreOprHead;
}

int RegisterFileStoreOpr(PT_FileStoreOpr ptFileStoreOpr)
{
	PT_FileStoreOpr ptTmp;

	if (!g_ptFileStoreOprHead)
	{
		g_ptFileStoreOprHead   = ptFileStoreOpr;
		ptFileStoreOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptFileStoreOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptFileStoreOpr;
		ptFileStoreOpr->ptNext = NULL;
	}

	return 0;
}


PT_FileStoreOpr GetFileOpr(char *pcName)
{
	PT_SqlStoreOpr ptTmp = g_ptFileStoreOprHead;
	
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

PT_FileStoreOpr GetDefultFileOpr(void)
{
	return g_ptFileStoreOprHead;
}


int open_sqlite3(char *dbname)
{/*
	PT_SqlStoreOpr Tmp;
	
	Tmp = GetsqlOpr("sqlite3");
	
	if(Tmp->sql_open(dbname,&db)<0)
	{
		DBG_PRINTF("open sqlite fail!\n");
		return -1;
	}

	//Tmp->sql_create_Dev_table(db,DEV_INFO);
	CreateGroupTable(GROUP_INFO);
	CreateGroupTable(SCENE_INFO);
	CreateTable(DEV_INFO);
	CreateLdTable(SENSORLD_INFO);
	
	//Tmp->sql_create_index(db,"dev_info","Sn_index","Sn");
	
	return 0;*/
}


int DorpTable(char *name)
{/*
	PT_SqlStoreOpr Sqlctl;

	Sqlctl = GetDefultSqlOpr();
	
	Sqlctl->sql_delete_table(db,name);		//Dorp table 
	Sqlctl->sql_db_clean(db);						//clean zone
	return 0;*/
}

int CreateTable(char *name)
{
	/*PT_SqlStoreOpr Sqlctl;

	Sqlctl = GetDefultSqlOpr();
	
	Sqlctl->sql_create_Dev_table(db,name); 	//create table
	return 0;*/
}

int CreateGroupTable(char *name)
{
	/*PT_SqlStoreOpr Sqlctl;

	Sqlctl = GetDefultSqlOpr();
	
	Sqlctl->sql_create_group_table(db,name); 	//create table
	
	return 0;*/
}

int CreateLdTable(char *name)
{
	/*PT_SqlStoreOpr Sqlctl;

	Sqlctl = GetDefultSqlOpr();
	
	Sqlctl->sql_create_Ld_table(db,name); 	//create table
	
	return 0;*/
}


int DeleteDev(int ShortAddr,int Endpoint)
{
	/*PT_SqlStoreOpr Sqlctl;

	Sqlctl = GetDefultSqlOpr();
	
	Sqlctl->sql_delete_Dev(db,DEV_INFO,ShortAddr,Endpoint);
	*/
}




int StoreInit(void)
{
	int iError = 0;
	//iError = SqlStoreInit();
	iError |= FileStoreInit();

	return iError;
}



