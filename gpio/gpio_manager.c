#include <config.h>
#include <gpio_manager.h>
#include <string.h>
#include <unistd.h>


static PT_IOctlOpr g_ptIOctlOprHead;

/**********************************************************************
 * 函数名称： RegisterIOctlOpr
 * 功能描述： 注册"输入模块"
 * 输入参数： ptIOctlOpr - 输入模块的结构体指针
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2015/02/08	     V1.0	  何雄武	      创建
 ***********************************************************************/
int RegisterIOctlOpr(PT_IOctlOpr ptIOctlOpr)
{
	PT_IOctlOpr ptTmp;

	if (!g_ptIOctlOprHead)
	{
		g_ptIOctlOprHead   = ptIOctlOpr;
		ptIOctlOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptIOctlOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptIOctlOpr;
		ptIOctlOpr->ptNext = NULL;
	}

	return 0;
}


/**********************************************************************
 * 函数名称： ShowIOctlOpr
 * 功能描述： 显示本程序能支持的"输入模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2015/02/08	     V1.0	  何雄武	      创建
 ***********************************************************************/
void ShowIOctlOpr(void)
{
	int i = 0;
	PT_IOctlOpr ptTmp = g_ptIOctlOprHead;

	while (ptTmp)
	{
		DBG_PRINTF("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}


PT_IOctlOpr GetIOctlOpr(char *pcName)
{
	PT_IOctlOpr ptTmp = g_ptIOctlOprHead;
	
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



/**********************************************************************
 * 函数名称： AllInputDevicesInit
 * 功能描述： 调用所有"输入模块"的设备相关的初始化函数
 *            并创建用于读取输入数据的子线程
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2015/02/08	     V1.0	  何雄武	      创建
 ***********************************************************************/
int IODevicesInit(void)
{
	PT_IOctlOpr ptTmp = g_ptIOctlOprHead;
	int iError = -1;

	while (ptTmp)
	{
		if (0 == ptTmp->Init())
		{
			iError = 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return iError;
}

int IOCwrite(int io ,int val)
{
	PT_IOctlOpr ptTmp = g_ptIOctlOprHead;
	int iError = -1;

	if (0 == ptTmp->IOwrite(io,val))
	{
		iError = 0;
	}
	
	return iError;
}

int IOCread(int io)
{
	PT_IOctlOpr ptTmp = g_ptIOctlOprHead;
	int iError = -1;

	iError = ptTmp->IOread(io);
	
	return iError;
}

int GPIOctlwrite(int GPIO,int val)
{
	PT_IOctlOpr ptTmp = g_ptIOctlOprHead;
	int iError = -1;
	int valtmp[1];

	valtmp[0] = val;

	iError = ptTmp->IOctl(0,GPIO,valtmp);
	
	return iError;
}

int GPIOctlread(int GPIO)
{
	PT_IOctlOpr ptTmp = g_ptIOctlOprHead;
	int iError = -1;
	int valtmp[1];

	iError = ptTmp->IOctl(1,GPIO,valtmp);
	
	return valtmp[0];
}

/**********************************************************************
 * 函数名称： InputInit
 * 功能描述： 调用各个输入模块的初始化函数,就是注册各个输入模块
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2015/02/08	     V1.0	  何雄武	      创建
 ***********************************************************************/
int IOinit(void)
{
	int iError = 0;

	iError |= IODevInit();
//	iError |= KeyDevInit();
//	iError |= BeepDevInit();
	return iError;
}
