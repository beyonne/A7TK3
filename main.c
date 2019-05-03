#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include </opt/install/include/sqlite3.h>
#include <errno.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <pthread.h>
#include <time.h>  
#include <sys/time.h>  
#include <signal.h>  

#include <input_manager.h>
#include <gpio_manager.h>
#include <net_manager.h>
#include <store_manager.h>
#include <timer.h>
#include <communication.h>
#include <rtctime.h>
#include <para.h>
#include <usart_manager.h>
#include <can_manager.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <timer.h>


long getlocalhostip()
{
	int  MAXINTERFACES=16;
	unsigned long ip[3];
	int fd,num, intrface, retn = 0;
	struct ifreq buf[MAXINTERFACES]; ///if.h
	struct ifconf ifc; ///if.h
	ip[0] = -1;
	ip[1] = -1;
	ip[2] = -1;
	
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) //socket.h
	{
		ifc.ifc_len = sizeof buf;
		ifc.ifc_buf = (caddr_t) buf;
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) //ioctl.h
		{
			num=intrface = ifc.ifc_len / sizeof (struct ifreq); 
			//DBG_PRINTF("intrface:%d \n",intrface);
			while (intrface-- > 0)
			{
				
				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
				{
					ip[intrface]=inet_addr( inet_ntoa( ((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr) );//types
					DBG_PRINTF("ip:%x\n",ip[intrface]);

					
					//break;
				}

			}
			return ip[2];
			
		}
		close (fd);
	}
	return  ip[0];
}

static int *get_pid(char *key)
{
    char buff[80];
    int *pid = NULL;
    int i = 0;
    char *ret;
    pid = (int *)malloc(8*sizeof(int));
    if (NULL == pid)
    {
        DBG_PRINTF("pid malloc error!\n");
    }
 
    memset(pid,0,8*sizeof(int));
    sprintf(buff,"pidof %s",key);
    FILE *pidreader = popen(buff,"r");
    memset(buff,0,80);
 
    ret = fgets(buff,79,pidreader);
    pclose(pidreader);
 
    ret = strtok(buff," ");
    while (ret != NULL)
    {
        pid[i++]=atoi(ret);
        ret =strtok(NULL," ");
    }
    return pid;
 
}
 
int setip(unsigned char *ip)
{
	char sqltemp[50];
	char tempdata[50];
    struct ifreq temp;
	unsigned char *sql = "%d.%d.%d.%d";

    struct sockaddr_in *addr;
    DBG_PRINTF("set:%s\n",ip);
    int fd = 0;

    int ret = -1;
   
    
	sprintf(sqltemp,sql,ip[0],ip[1],ip[2],ip[3]);
	
	DBG_PRINTF("Set ip:%s\n",sqltemp);

    strcpy(temp.ifr_name, "eth0");

    if((fd=socket(AF_INET, SOCK_STREAM, 0))<0)

    {

        return -1;

    }

    addr = (struct sockaddr_in *)&(temp.ifr_addr);

    addr->sin_family = AF_INET;

    addr->sin_addr.s_addr = inet_addr(sqltemp);

    ret = ioctl(fd, SIOCSIFADDR, &temp);

    close(fd);

    sprintf(tempdata,"route add default gw %d.%d.%d.%d",ip[0],ip[1],ip[2],1);
	DBG_PRINTF("run set:%s\n",tempdata);
    system(tempdata);

    if(ret < 0)

        return -1;

    return 0;

} 
 
/*this function  is usesd to signal the process with "kill -10" */
void stop_process()
{

    int *pid=get_pid("zll");
    int i = 0;
    if (pid[i])
    {
        DBG_PRINTF("signal to pid=%d\n",pid[i++]);
        //kill(pid[i++],10);
    }
    else
    {/*
		if(vfork()==0)
	    {
	        system("/opt/runzll");
	        exit(0);
	        return 0;
	        DBG_PRINTF("-------------wait for run zll!-------------\n");
	    }
	    else
	    {

			DBG_PRINTF("wait for run zll!\n");
			sleep(3);
			
		}*/
    }
    free(pid);


}

/*
#define COM_NAME1			"/dev/ttymxc0"					//预留
#define COM_NAME2			"/dev/ttymxc1"					//RF_OCU
#define COM_NAME3			"/dev/ttymxc2"					//光纤_OCU
#define COM_NAME4			"/dev/ttymxc3"					//轮子和反转臂
#define COM_NAME5			"/dev/ttymxc4"					//PTZ
#define COM_NAME6			"/dev/ttymxc5"					//ARM臂
#define COM_NAME7			"/dev/ttymxc6"					//Head底板
#define COM_NAME8			"/dev/ttymxc7"					//WIFI_OCU
*/


static T_TimeStop_Opr g_tRxOcuTimeout =
{
	.name 			= "RxOcu",
	.Times 			= RX_OCU_TIMEOUT,
	.TimeEn			= 0,
};


int main(int argc, char **argv)
{	
	int iError;
	unsigned char SendData[10];
	static T_TCP_Rx_Opr tRxOcu,tPtz,tARMBoard,tHeadBoard,tTack;
	unsigned char data[8] = {0};	
	unsigned long ip;
	PT_Sys_Para_Opr Para;

	unsigned char sqltemp[20];		//本机IP
	unsigned char sqltemp1[20];		//目的IP
	unsigned char *sql = "%d.%d.%d.%d";
	PT_CanDeviceOpr ptDevTmp;
	int can_id[2];
	int can_mask[2];

	DBG_PRINTF("--------------Start!--------------\n");
	usleep(10000);

	/* 注册调试通道 */
	DebugInit();
	/* 初始化调试通道 */
	InitDebugChanel();

	iError=StoreInit();								//实例化储存操作
	open_sqlite3("/opt/dev");						//打开数据库
	ParaInit();										//读取配置文件参数
	Para = GetDefultSysParaOpr();					//获得配置参数
	ip = getlocalhostip();							//获取IP

	//Para->IP_Addr[0] = ip;
	//Para->IP_Addr[1] = ip>>8;
	//Para->IP_Addr[2] = ip>>16;
	//Para->IP_Addr[3] = ip>>24;
	if(ip==0x8802a8c0)
	{
		DBG_PRINTF("set ip");
		setip(Para->IP_Addr);
	}
	else
	{
		Para->IP_Addr[0] = ip;
		Para->IP_Addr[1] = ip>>8;
		Para->IP_Addr[2] = ip>>16;
		Para->IP_Addr[3] = ip>>24;
		SetSysPara(Para);
		sprintf(sqltemp,sql,Para->IP_Addr[0],Para->IP_Addr[1],Para->IP_Addr[2],Para->IP_Addr[3]);
		DBG_PRINTF("get dhcp ip:%s \n",sqltemp);
		//setip(Para->IP_Addr);
	}

	sprintf(sqltemp,sql,Para->IP_Addr[0],Para->IP_Addr[1],Para->IP_Addr[2],Para->IP_Addr[3]);
	sprintf(sqltemp1,sql,Para->DIP_Addr[0],Para->DIP_Addr[1],Para->DIP_Addr[2],Para->DIP_Addr[3]);
	DBG_PRINTF("IP:%s\n",sqltemp);
	SetSysPara(Para);
	/* 初始化调试模块: 可以通过"标准输出"也可以通过"网络"打印调试信息
	 * 因为下面马上就要用到DBG_PRINTF函数, 所以先初始化调试模块
	 */

	PcTxTimeMalloc();
	time_create(0);
	//set_timer1(0,TIME100MS,0,TIME1MS);
	/* 注册GPIO设备 */
	IOinit();
    /* 调用所有GPIO设备的初始化函数 */
	IODevicesInit();
	
    /* 注册输入设备 */
	//InputInit();
    /* 调用所有输入设备的初始化函数 */
	//AllInputDevicesInit();

	/* 分配接收缓存 */
	AllocMem(MenNum);
	//CAN操作注册
	Can_conctrl_Register();

	can_id[0] = 0x200;	//过滤器
	can_id[1] = 0x20F;
	can_mask[0] = 0xfff;
	can_mask[1] = 0xfff;

	AddCanDevice("can0",0,can_id,can_mask,2,29,500000,1);
	//AddCanDevice("can1",0,can_id,can_mask,2,29,500000,1);
	
	CANDevInit();
	ptDevTmp=GetCanOpr("can0");

	//注册串口设备
	Usart_Register();
	//增加串口设备
	AddUsartDevice(COM_RF,COM_RF,115200,NONE,D_BIT8,S_BIT1,N);
	AddUsartDevice(COM_FIBER,COM_FIBER,115200,NONE,D_BIT8,S_BIT1,N);
	AddUsartDevice(COM_MOTION,COM_MOTION,115200,NONE,D_BIT8,S_BIT1,N);
	AddUsartDevice(COM_PTZ,COM_PTZ,9600,NONE,D_BIT8,S_BIT1,N);
	AddUsartDevice(COM_ARM,COM_ARM,115200,NONE,D_BIT8,S_BIT1,N);
	AddUsartDevice(COM_HEAD,COM_HEAD,115200,NONE,D_BIT8,S_BIT1,N);
	AddUsartDevice(COM_WIFI,COM_WIFI,115200,NONE,D_BIT8,S_BIT1,N);

	/*初始化所有串口设备*/
	UsartDevInit();

	/* 注册网络操作设备 */
	NetRegister();
	//iError=AddNetDevice(CONNECT_TO_SERVER,sqltemp,15000,sqltemp1,8580);
	//DBG_PRINTF("CONNECT_TO_SERVER:%d\n",iError);
	iError=AddNetDevice(PHONE_SERVER,sqltemp,15003,sqltemp1,15003);
	DBG_PRINTF("PHONE_SERVER:%d\n",iError);
	//iError=AddNetDevice("udp_zll",sqltemp,9090,sqltemp,9090);
	//DBG_PRINTF("udp_send:%d\n",iError);
	usleep(100000);
	//iError=AddNetDevice(UDP_SERVER,sqltemp,15001,sqltemp1,15001);
	//DBG_PRINTF("UDP_SERVER:%d\n",iError);
	Motion_Init();		//轮子和翻转臂初始化
	//iError=InitTcpClient(CONNECT_TO_SERVER);
	//if(iError==0)
	//{
	//	DBG_PRINTF("connect server ok!\n");
	//}
	//else
	//DBG_PRINTF("-------------connetct server fail!-------------\n");
	//DBG_PRINTF("Server IP:%s\n",sqltemp1);
	iError=InitTcpServer(PHONE_SERVER);
	DBG_PRINTF("-------------tcp_server:%d---------------\n",iError);

	//iError=InitUDPServer(UDP_SERVER);
	//DBG_PRINTF("udp_rx:%d\n",iError);
	RtcTimeInit();

	tRxOcu.Rx_Data = malloc(512);
	tARMBoard.Rx_Data = malloc(512);
	tHeadBoard.Rx_Data = malloc(512);
	tTack.Rx_Data = malloc(512);
	tPtz.Rx_Data = malloc(512);
	
	data[1]=0x40;
	data[2]=0x06;
	data[3]=0x55;
	can_tx_data(ptDevTmp->fd,0x633,data,8);
	AMC_Inintial_PARAMETER();
	//RL_OFF(1);
    /* 运行主 */
	//Page("main")->Run(NULL);
	//Application("main")->Run
	//SendData[0] = 0x88;
	DBG_PRINTF("---------------------------App:V%X.%X.%X.%X-------------------------\n",((Para->codeVersion&0xFF000000)>>24),((Para->codeVersion&0xFF0000)>>16),((Para->codeVersion&0xFF00)>>8),(Para->codeVersion&0xFF));
	DBG_PRINTF("---------------------------Hardware:V%d.%d-------------------------\n",((Para->DeviceVersion&0xFF00)>>8),(Para->DeviceVersion&0xFF));
	DBG_PRINTF("\r\n.\r\n..\r\n...\r\n");
	DBG_PRINTF("Start app...\r\n");
	DBG_PRINTF("******TK3 A7 Control******\r\n");
	DBG_PRINTF("DeviceNo.      : %d\r\n",DeviceNO);
	DBG_PRINTF("Board type     : 0x%02X\r\n",BOARD_TYPE);
	DBG_PRINTF("Hard Version   : V%d\r\n",BOARD_HVER);
	DBG_PRINTF("App Version    : V%d.%d.%d\r\n",SOFTWARE_VER1,SOFTWARE_VER2,SOFTWARE_VER3);
	DBG_PRINTF("Build date     : %s\r\n",__DATE__);
	DBG_PRINTF("Build time     : %s\r\n",__TIME__);
//	GasSensorData.Cmd = 0;
//	GasSensorData.Len = 0;
//	GasSensorData.Value = 0;
//	GasSensorData.Arr = 0;
//	GasSensorData.Cmd = 0;
//	GasSensorData.Len = 0;
//	GasSensorData.Value = 0;
	//RegGawyToServer();
	//savdCommonIRCommand();
	//KeyPro();						//处理按键事件；
	//Hand();							//开启定时处理连接服务器；
	//GetzlludpData();
	while(1)
	{
		//DBG_PRINTF("---------------------------RUN:V1.0.0.16-------------------------\n");
		//RX_PC_Info_Process(TcpRx);		//接收手机数据发送到无线zigbee;
		//RxZigbeeProcess(zigbeeRx);		//接收无线的数据发送到手机或者服务器；
		//Shake_hands();					//执行定时任务，比如心跳；
		//BeepWorning();					//处理蜂鸣器报警事件；
//		printf("run\r\n");
		Rx_OCU_Prs(&tRxOcu);
//		Rx_ARM_Prs(&tARMBoard);
		motion_send();
		Rx_Head_Prs(&tHeadBoard);
		Rx_Motion_Prs(&tTack);
//		canopen_data_pro();
		canopen_data_pro(&tHeadBoard,&tARMBoard,&tTack);
		usleep(1000);
		Rx_Ptz_Prs(&tPtz);
		Tacker_To_Ocu(&tHeadBoard,&tARMBoard);  //车体数据返回OCU
		//timingCheck();
//		printf("while1\n");
	}
	return 0;
}
