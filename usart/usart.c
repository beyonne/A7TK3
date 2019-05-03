/************************Copyright(c)*******************************                    
**------------------------------------------FileInfo------------------------------------------------------- 
** File name:                 	usart.c 
** Last modified Date:  		20151029 
** Last Version:              	1.2 
** Descriptions:             
**------------------------------------------------------------------------------------------------------ 
** Created by:               	HXW
** Created date:            	2014-01-31 ，
** Version:                     1.1 
** Descriptions:             	The original version 
**------------------------------------------------------------------------------------------------------ 
*******************************************************************/  
   
   
//串口相关的头文件  
#include <stdio.h>      /*标准输入输出定义*/  
#include <stdlib.h>     /*标准函数库定义*/  
#include <unistd.h>     /*Unix 标准函数定义*/  
#include <sys/types.h>   
#include <sys/stat.h>     
#include <fcntl.h>      /*文件控制定义*/  
#include <termios.h>    /*PPSIX 终端控制定义*/  
#include <errno.h>      /*错误号定义*/  
#include <string.h>  
#include <pthread.h>
#include <string.h>
#include <usart_manager.h>



#include <config.h>
/******************************************************************* 
* 名称：                  UART0_Open 
* 功能：                打开串口并返回串口设备文件描述 
* 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2) 
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/  
static int USARTOpen(int *fd,char* port)  
{  
	*fd = open(port,O_RDWR|O_NOCTTY);  //O_NDELAY
	if (*fd < 0)  
	{  
		
		perror("Can't Open Serial Port");  
		return -1;  
	}  
	//恢复串口为阻塞状态                                 
	if(fcntl(*fd, F_SETFL, 0) < 0)  
	{  
		DBG_PRINTF("fcntl failed!\n");  
		return -1;  
	}       
	else  
	{  
		//DBG_PRINTF("fcntl=%d\n",fcntl(*fd, F_SETFL,0));  
	}  
	//测试是否为终端设备      
	if(0 == isatty(STDIN_FILENO))  
	{  
		DBG_PRINTF("standard input is not a terminal device\n");  
		//return -1;  
	}  
	else  
	{  
		DBG_PRINTF("isatty success!\n");  
	}                
	//DBG_PRINTF("fd->open=%d\n",*fd);  
	return 0;  
}  
/******************************************************************* 
* 名称：                UART0_Close 
* 功能：                关闭串口并返回串口设备文件描述 
* 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2) 
* 出口参数：        void 
*******************************************************************/  
   
static int USARTClose(int fd)  
{  
    if(close(fd)<0)
    {
		return -1;
    }
    return 0;
}  

static int usart_set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)
{
	int  i, speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};  
	int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};  
	struct termios newtio, temptio,oldtio;
	//set new port configurations 
	bzero(&newtio, sizeof(newtio));	
	newtio.c_iflag &= ~IXON;     // no software output flow control
	newtio.c_iflag &= ~IXOFF;    // no software input flow control
	newtio.c_iflag &= ~INPCK;    // no input parity
	newtio.c_iflag &= ~ISTRIP;   // input 8 data bits
	newtio.c_cflag |= CREAD;     // enable receiver
	newtio.c_cflag &= ~PARENB;   // no output parity
	newtio.c_cflag |= CS8;       // output 8 data bits
	newtio.c_cflag &= ~CSTOPB;   // 1 stop bit
	newtio.c_lflag &= ~ICANON;   // use raw input
	newtio.c_lflag &= ~ECHO;     // echo off
	newtio.c_lflag &= ~ECHOE;    // erase treated like regular char
	newtio.c_lflag &= ~ECHOK;    // kill treated like regular char
	newtio.c_lflag &= ~ECHONL;   // don't echo newline chars

	 //设置串口输入波特率和输出波特率  
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
	{  
		if  (speed == name_arr[i])  
		{               
			newtio.c_ispeed = speed_arr[i];    // input baud rate 
			newtio.c_ospeed = speed_arr[i];    // output baud rate
		}  
	}       

	tcflush(fd, TCIFLUSH);		//clear port buffer
	tcsetattr(fd, TCSANOW, &newtio);		//apply new settings

	// check that attributes were set correctly
	tcgetattr(fd,&temptio);
	if ((newtio.c_iflag != temptio.c_iflag) || 
		(newtio.c_cflag != temptio.c_cflag) ||
		(newtio.c_lflag != temptio.c_lflag) || 
		(newtio.c_ispeed != temptio.c_ispeed) ||
		(newtio.c_ospeed != temptio.c_ospeed))
		return (-1);
		
	DBG_PRINTF("SerialOpen%s\n");
	fflush(stdout);	
}
/******************************************************************* 
* 名称：                UART0_Set 
* 功能：                设置串口数据位，停止位和效验位 
* 入口参数：        fd        串口文件描述符 
*                              speed     串口速度 
*                              flow_ctrl   数据流控制 
*                           databits   数据位   取值为 7 或者8 
*                           stopbits   停止位   取值为 1 或者2 
*                           parity     效验类型 取值为N,E,O,,S 
*出口参数：          正确返回为1，错误返回为0 
*******************************************************************/  

static int USARTSet(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  
     
	int   i;  
	int   status;  
	int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};  
	int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};  
           
    struct termios options;  
     
    //tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1. 
     
    
    if  ( tcgetattr( fd,&options)  !=  0)  
       {  
          perror("SetupSerial 1");      
          return -1;   
       }  
    
    //设置串口输入波特率和输出波特率  
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
	{  
		if  (speed == name_arr[i])  
		{               
			cfsetispeed(&options, speed_arr[i]);   
			cfsetospeed(&options, speed_arr[i]);    
		}  
	}       
     
    //修改控制模式，保证程序不会占用串口  
    options.c_cflag |= CLOCAL;  
    //修改控制模式，使得能够从串口中读取输入数据  
    options.c_cflag |= CREAD;  
    
    //设置数据流控制  
    switch(flow_ctrl)  
    {  
        
       case 0 ://不使用流控制  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
       case 1 ://使用硬件流控制  
              options.c_cflag |= CRTSCTS;  
              break;  
       case 2 ://使用软件流控制  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //设置数据位  
    //屏蔽其他标志位  
    options.c_cflag &= ~CSIZE;  
    switch (databits)  
    {    
       case 5    :  
                     options.c_cflag |= CS5;  
                     break;  
       case 6    :  
                     options.c_cflag |= CS6;  
                     break;  
       case 7    :      
                 options.c_cflag |= CS7;  
                 break;  
       case 8:      
                 options.c_cflag |= CS8;  
                 break;    
       default:     
                 DBG_PRINTF(stderr,"Unsupported data size\n");  
                 return -1;   
    }  
    //设置校验位  
    switch (parity)  
    {    
       case N: //无奇偶校验位。  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~INPCK;      
                 break;   

       case O://设置为奇校验      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   

       case E://设置为偶校验    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  

       case S: //设置为空格   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    
                 DBG_PRINTF(stderr,"Unsupported parity\n");      
                 return -1;   
    }   
    // 设置停止位   
    switch (stopbits)  
    {    
       case 1:     
                 options.c_cflag &= ~CSTOPB; break;   
       case 2:     
                 options.c_cflag |= CSTOPB; break;  
       default:     
                       DBG_PRINTF(stderr,"Unsupported stop bits\n");   
                       return -1;  
    }  
  
 	 //修改输出模式，原始数据输出  
  	//options.c_oflag &= ~OPOST;  
  	  
  	//options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//我加的  
	//options.c_lflag &= ~(ISIG | ICANON);  
	
	 options.c_cflag |= CLOCAL | CREAD; 
	 options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
	 options.c_oflag &= ~OPOST; 
	 options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); 

    
    //设置等待时间和最小接收字符  
    options.c_cc[VTIME] = 0; // 读取一个字符等待1*(1/10)s 
    options.c_cc[VMIN] = 1; // 读取字符的最少个数为1 
     
    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读  
    tcflush(fd,TCIFLUSH);  
     
    //激活配置 (将修改后的termios数据设置到串口中）  
    if (tcsetattr(fd,TCSANOW,&options) != 0)    
   {  
       perror("com set error!\n");    
      return -1;   
   }  
   
  
    return 0;   
}  
/******************************************************************* 
* 名称：                UART0_Init() 
* 功能：                串口初始化 
* 入口参数：        fd       :  文件描述符    
*               speed  :  串口速度 
*                              flow_ctrl  数据流控制 
*               databits   数据位   取值为 7 或者8 
*                           stopbits   停止位   取值为 1 或者2 
*                           parity     效验类型 取值为N,E,O,,S 
*                       
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/  
static int USART0Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  
    int err;  
    
    //设置串口数据帧格式  
    if (USARTSet(fd,speed,flow_ctrl,databits,stopbits,parity) < 0 )  
    {                                                           
        return -1;  
    }  
    else  
    {  
               return 0;  
    }  
}  
   
/******************************************************************* 
* 名称：                  UART0_Recv 
* 功能：                接收串口数据 
* 入口参数：        fd                  :文件描述符     
*                              rcv_buf     :接收串口中数据存入rcv_buf缓冲区中 
*                              data_len    :一帧数据的长度 
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/  
static int USARTRecv(int fd, char *rcv_buf,int data_len)  
{  
    int len,fs_sel;  
    fd_set fs_read;  
     
    struct timeval time;  
     
    FD_ZERO(&fs_read);  
    FD_SET(fd,&fs_read);  
     
    time.tv_sec = 10;  
    time.tv_usec = 0;  
     
    //使用select实现串口的多路通信  
    fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);
    
    if(fs_sel)  
	{  
	    len = read(fd,rcv_buf,data_len);  
	  	DBG_PRINTF("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);  
	    return len;  
	}  
    else  
	{  
	  	DBG_PRINTF("Sorry,I am wrong!");  
	    return -1;  
	}       
} 


/**********************************************************************
 * 函数名称：串口数据读取、缓存线程，服务函数_zhz
 * 功能描述： 
 * 输入参数： name - 名字
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2016/12/21	     V1.0	  何雄武	      创建
 ***********************************************************************/
static void *Thread_read(void *param)
{
	int remaining, received, err = COM_ERR_OK, buf_len = 4096;
	unsigned long int data_len=0;
	ReceiveParamCom *rp;
	unsigned char *buf, *ptr;

	rp = (ReceiveParamCom *)param;   //定义一个数据处理结构体指针_zhz
	
	// allocate buffer for received data
	if (!(buf = malloc(buf_len)))   //申请一块内存区域用于存放临时存放串口数据_zhz
	{
		err = COM_ERR_BUFFER;    //申请失败_zhz
		
		goto close_com;   //关闭串口，结束线程_zhz
	}
	
	// we repeat until the connection has been closed
	while (1)  //除非串口出现异常，否则此读取数据线程一直执行_zhz
	{
		usleep(5000);    //串口读取间隔时间_zhz
		received = read(rp->fd, buf, 4096);  //串口底层数据读取_zhz
		if(rp->fd==10)    //此语句为测试用
		printf("---------------------------------len:%d fd:%d\n",received,rp->fd);
		//if (received == 0) 
		{
		//	err = COM_ERR_CLOSED;
		//	goto close_com;
		} 
		if (received < 0)       //读串口失败_zhz
		{
			if (errno == EINTR) continue;  //跳出while()循环_zhz
			err = COM_ERR_READ;
			goto close_com;    //跳转到_zhz
		} 
		
		rp->cb(rp->fd, err, (void *)buf, received, rp->data);   //将串口底层端口read()获得数据放入数据缓存链表(区)_zhz
		
	}
	
	close_com:
	//shutdown(rp->fd, SHUT_RDWR);
	//DBG_PRINTF("err\n");
	close(rp->fd);
	
	if (buf) 
		free(buf);
	
	rp->cb(rp->fd, err, NULL, 0, rp->data);
	//DBG_PRINTF("5\n");
	pthread_exit(0);   
	return (NULL);
}

/**********************************************************************
 * 函数名称： 
 * 功能描述： 
 * 输入参数： name - 名字
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2016/12/21	     V1.0	  何雄武	      创建
 ***********************************************************************/
static int ComRead(int fd, COM_READ_CALLBACK(*cb), void *data)
{
	pthread_t thr_id;
	ReceiveParamCom *rp;

	if (!(rp = (ReceiveParamCom *)malloc(sizeof (ReceiveParamCom)))) {
		return (COM_ERR_BUFFER);
	}
	rp->fd   = fd;
	rp->cb   = cb;
	rp->data = data;
	if (pthread_create(&thr_id, NULL, &Thread_read, (void *)rp)) {
		return (COM_ERR_READ);
	}
	if (pthread_detach(thr_id)) {
		return (COM_ERR_READ);
	}
	return (COM_ERR_OK);
}




static int USARTSendData(int fd, char *tx_buf,int data_len)
{
	int len;

	//DBG_PRINTF("fd:%d\n",fd);

	if(data_len>0)
	{
		len = write(fd,tx_buf,data_len);  

	   	if(len!=data_len)
	   	{
	   		DBG_PRINTF("fd error:%d\n",fd);
			return -1;
	   	}
		//DBG_PRINTF("fd2:%d\n",fd);
	   	return 0;
	}
	else
		DBG_PRINTF("fd1 no data send :%d\n",fd);
}


static T_UsartOpr g_tUsartOpr = {
	.name 						= "usartdev",
	.USARTOpen 					= USARTOpen,
	.USARTClose					= USARTClose,
	.USART0Init					= USART0Init,
	.ComRead					= ComRead,
	.USARTSendData				= USARTSendData,
};
 
int UsartRegister(void)
{
	 return RegisterUsartOpr(&g_tUsartOpr);
}



 
/*********************************************************************                            End Of File                          ** 
*******************************************************************/

