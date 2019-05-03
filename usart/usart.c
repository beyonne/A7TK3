/************************Copyright(c)*******************************                    
**------------------------------------------FileInfo------------------------------------------------------- 
** File name:                 	usart.c 
** Last modified Date:  		20151029 
** Last Version:              	1.2 
** Descriptions:             
**------------------------------------------------------------------------------------------------------ 
** Created by:               	HXW
** Created date:            	2014-01-31 ��
** Version:                     1.1 
** Descriptions:             	The original version 
**------------------------------------------------------------------------------------------------------ 
*******************************************************************/  
   
   
//������ص�ͷ�ļ�  
#include <stdio.h>      /*��׼�����������*/  
#include <stdlib.h>     /*��׼�����ⶨ��*/  
#include <unistd.h>     /*Unix ��׼��������*/  
#include <sys/types.h>   
#include <sys/stat.h>     
#include <fcntl.h>      /*�ļ����ƶ���*/  
#include <termios.h>    /*PPSIX �ն˿��ƶ���*/  
#include <errno.h>      /*����Ŷ���*/  
#include <string.h>  
#include <pthread.h>
#include <string.h>
#include <usart_manager.h>



#include <config.h>
/******************************************************************* 
* ���ƣ�                  UART0_Open 
* ���ܣ�                �򿪴��ڲ����ش����豸�ļ����� 
* ��ڲ�����        fd    :�ļ�������     port :���ں�(ttyS0,ttyS1,ttyS2) 
* ���ڲ�����        ��ȷ����Ϊ1�����󷵻�Ϊ0 
*******************************************************************/  
static int USARTOpen(int *fd,char* port)  
{  
	*fd = open(port,O_RDWR|O_NOCTTY);  //O_NDELAY
	if (*fd < 0)  
	{  
		
		perror("Can't Open Serial Port");  
		return -1;  
	}  
	//�ָ�����Ϊ����״̬                                 
	if(fcntl(*fd, F_SETFL, 0) < 0)  
	{  
		DBG_PRINTF("fcntl failed!\n");  
		return -1;  
	}       
	else  
	{  
		//DBG_PRINTF("fcntl=%d\n",fcntl(*fd, F_SETFL,0));  
	}  
	//�����Ƿ�Ϊ�ն��豸      
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
* ���ƣ�                UART0_Close 
* ���ܣ�                �رմ��ڲ����ش����豸�ļ����� 
* ��ڲ�����        fd    :�ļ�������     port :���ں�(ttyS0,ttyS1,ttyS2) 
* ���ڲ�����        void 
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

	 //���ô������벨���ʺ����������  
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
* ���ƣ�                UART0_Set 
* ���ܣ�                ���ô�������λ��ֹͣλ��Ч��λ 
* ��ڲ�����        fd        �����ļ������� 
*                              speed     �����ٶ� 
*                              flow_ctrl   ���������� 
*                           databits   ����λ   ȡֵΪ 7 ����8 
*                           stopbits   ֹͣλ   ȡֵΪ 1 ����2 
*                           parity     Ч������ ȡֵΪN,E,O,,S 
*���ڲ�����          ��ȷ����Ϊ1�����󷵻�Ϊ0 
*******************************************************************/  

static int USARTSet(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  
     
	int   i;  
	int   status;  
	int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};  
	int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};  
           
    struct termios options;  
     
    //tcgetattr(fd,&options)�õ���fdָ��������ز������������Ǳ�����options,�ú��������Բ��������Ƿ���ȷ���ô����Ƿ���õȡ������óɹ�����������ֵΪ0��������ʧ�ܣ���������ֵΪ1. 
     
    
    if  ( tcgetattr( fd,&options)  !=  0)  
       {  
          perror("SetupSerial 1");      
          return -1;   
       }  
    
    //���ô������벨���ʺ����������  
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
	{  
		if  (speed == name_arr[i])  
		{               
			cfsetispeed(&options, speed_arr[i]);   
			cfsetospeed(&options, speed_arr[i]);    
		}  
	}       
     
    //�޸Ŀ���ģʽ����֤���򲻻�ռ�ô���  
    options.c_cflag |= CLOCAL;  
    //�޸Ŀ���ģʽ��ʹ���ܹ��Ӵ����ж�ȡ��������  
    options.c_cflag |= CREAD;  
    
    //��������������  
    switch(flow_ctrl)  
    {  
        
       case 0 ://��ʹ��������  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
       case 1 ://ʹ��Ӳ��������  
              options.c_cflag |= CRTSCTS;  
              break;  
       case 2 ://ʹ�����������  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //��������λ  
    //����������־λ  
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
    //����У��λ  
    switch (parity)  
    {    
       case N: //����żУ��λ��  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~INPCK;      
                 break;   

       case O://����Ϊ��У��      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   

       case E://����ΪżУ��    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  

       case S: //����Ϊ�ո�   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    
                 DBG_PRINTF(stderr,"Unsupported parity\n");      
                 return -1;   
    }   
    // ����ֹͣλ   
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
  
 	 //�޸����ģʽ��ԭʼ�������  
  	//options.c_oflag &= ~OPOST;  
  	  
  	//options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//�Ҽӵ�  
	//options.c_lflag &= ~(ISIG | ICANON);  
	
	 options.c_cflag |= CLOCAL | CREAD; 
	 options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
	 options.c_oflag &= ~OPOST; 
	 options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); 

    
    //���õȴ�ʱ�����С�����ַ�  
    options.c_cc[VTIME] = 0; // ��ȡһ���ַ��ȴ�1*(1/10)s 
    options.c_cc[VMIN] = 1; // ��ȡ�ַ������ٸ���Ϊ1 
     
    //�����������������������ݣ����ǲ��ٶ�ȡ ˢ���յ������ݵ��ǲ���  
    tcflush(fd,TCIFLUSH);  
     
    //�������� (���޸ĺ��termios�������õ������У�  
    if (tcsetattr(fd,TCSANOW,&options) != 0)    
   {  
       perror("com set error!\n");    
      return -1;   
   }  
   
  
    return 0;   
}  
/******************************************************************* 
* ���ƣ�                UART0_Init() 
* ���ܣ�                ���ڳ�ʼ�� 
* ��ڲ�����        fd       :  �ļ�������    
*               speed  :  �����ٶ� 
*                              flow_ctrl  ���������� 
*               databits   ����λ   ȡֵΪ 7 ����8 
*                           stopbits   ֹͣλ   ȡֵΪ 1 ����2 
*                           parity     Ч������ ȡֵΪN,E,O,,S 
*                       
* ���ڲ�����        ��ȷ����Ϊ1�����󷵻�Ϊ0 
*******************************************************************/  
static int USART0Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  
    int err;  
    
    //���ô�������֡��ʽ  
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
* ���ƣ�                  UART0_Recv 
* ���ܣ�                ���մ������� 
* ��ڲ�����        fd                  :�ļ�������     
*                              rcv_buf     :���մ��������ݴ���rcv_buf�������� 
*                              data_len    :һ֡���ݵĳ��� 
* ���ڲ�����        ��ȷ����Ϊ1�����󷵻�Ϊ0 
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
     
    //ʹ��selectʵ�ִ��ڵĶ�·ͨ��  
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
 * �������ƣ��������ݶ�ȡ�������̣߳�������_zhz
 * ���������� 
 * ��������� name - ����
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2016/12/21	     V1.0	  ������	      ����
 ***********************************************************************/
static void *Thread_read(void *param)
{
	int remaining, received, err = COM_ERR_OK, buf_len = 4096;
	unsigned long int data_len=0;
	ReceiveParamCom *rp;
	unsigned char *buf, *ptr;

	rp = (ReceiveParamCom *)param;   //����һ�����ݴ���ṹ��ָ��_zhz
	
	// allocate buffer for received data
	if (!(buf = malloc(buf_len)))   //����һ���ڴ��������ڴ����ʱ��Ŵ�������_zhz
	{
		err = COM_ERR_BUFFER;    //����ʧ��_zhz
		
		goto close_com;   //�رմ��ڣ������߳�_zhz
	}
	
	// we repeat until the connection has been closed
	while (1)  //���Ǵ��ڳ����쳣������˶�ȡ�����߳�һֱִ��_zhz
	{
		usleep(5000);    //���ڶ�ȡ���ʱ��_zhz
		received = read(rp->fd, buf, 4096);  //���ڵײ����ݶ�ȡ_zhz
		if(rp->fd==10)    //�����Ϊ������
		printf("---------------------------------len:%d fd:%d\n",received,rp->fd);
		//if (received == 0) 
		{
		//	err = COM_ERR_CLOSED;
		//	goto close_com;
		} 
		if (received < 0)       //������ʧ��_zhz
		{
			if (errno == EINTR) continue;  //����while()ѭ��_zhz
			err = COM_ERR_READ;
			goto close_com;    //��ת��_zhz
		} 
		
		rp->cb(rp->fd, err, (void *)buf, received, rp->data);   //�����ڵײ�˿�read()������ݷ������ݻ�������(��)_zhz
		
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
 * �������ƣ� 
 * ���������� 
 * ��������� name - ����
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2016/12/21	     V1.0	  ������	      ����
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

