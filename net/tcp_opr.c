#include <errno.h>       // errno, EINTR
#include <stdio.h>       // sprintf()
#include <fcntl.h>       // F_GETFL, F_SETFL, fcntl()
#include <unistd.h>      // close(), read(), write(), getpid()
#include <stdlib.h>      // malloc(), free()
#include <pthread.h>     // pthread_*()
#include <sys/types.h>   // pid_t
#include <sys/socket.h>  // socket(), bind(), listen(), accept(), shutdown()
#include <sys/un.h>      // struct sockaddr_un
#include <sys/uio.h>     // writev()
#include <netdb.h>       // gethostbyname()
#include <netinet/tcp.h> // TCP_NODELAY
#include <netinet/in.h>  // struct sockaddr_in
#include <arpa/inet.h>   // htons(), htonl(), ntohl()

#include <net_manager.h>
#include <config.h>

  

#define SA  struct sockaddr
#define SAI struct sockaddr_in
#define SAU struct sockaddr_un


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
static void *Thread_read(void *param)
{//TCP读数据线程
	int remaining, received, err = SNL_ERR_OK, buf_len = 4096;
	unsigned long int data_len=0;
	ReceiveParam *rp;
	unsigned char *buf, *ptr;

	rp = (ReceiveParam *)param;
	
	// allocate buffer for received data
	if (!(buf = malloc(buf_len))) 
	{
		err = SNL_ERR_BUFFER;
		goto close_socket;
	}
	
	// we repeat until the connection has been closed
	while (TRUE)
	{
		usleep(1000);
		received = read(rp->fd, buf, 4096);
		if (received == 0)
		{
			err = SNL_ERR_CLOSED;
			goto close_socket;
		}
		else if (received < 0)
		{
			if (errno == EINTR)
				continue;
			err = SNL_ERR_READ;
			goto close_socket;
		}
		rp->cb(rp->fd, err, (void *)buf, received, rp->data);
//		void ReadCallback(int fd, int error, void *buf, int len, void *data)
	}
	
close_socket:
	shutdown(rp->fd, SHUT_RDWR);
	DBG_PRINTF("Thread_read err\n");
	close(rp->fd);
	if (buf)
		free(buf);
	rp->cb(rp->fd, err, NULL, 0, rp->data);
	//DBG_PRINTF("5\n");
	pthread_exit(0);   
	return (NULL);
}


static void *Thread_listen(void *param)
{//监听线程
	int err, flags, listen_fd=0, client_fd, ph_len,i;
	unsigned int sa_len;
	void *server, *client;
	ListenParam *lp;

	lp = (ListenParam *)param;
	sa_len = lp->port ? sizeof (SAI) : sizeof (SAU);
	server = malloc(sa_len);
	memset(server, 0, sa_len);
	client = malloc(sa_len);
	memset(client, 0, sa_len);

	if(listen_fd!=0)
		listen_fd = 0;
		
	if ((listen_fd = socket(lp->port ? AF_INET : AF_UNIX, SOCK_STREAM, 0)) < 0) 
	{
		err = SNL_ERR_OPEN;
		goto close_socket;
	}
	

	if ((flags = fcntl(listen_fd, F_GETFL, 0)) >= 0) 
	{
		flags |= SO_REUSEADDR;
		fcntl(listen_fd, F_SETFL, flags);
	}
	
	if (lp->port)
	{
		((SAI *)server)->sin_family = AF_INET;
		((SAI *)server)->sin_port = htons((int)lp->port);
		((SAI *)server)->sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else 
	{
		DBG_PRINTF("lp->name:%s",lp->name);
		((SAU *)server)->sun_family = AF_UNIX;
		ph_len = sizeof (((SAU *)server)->sun_path)-1;
		strncpy(((SAU *)server)->sun_path, lp->name, ph_len);
		unlink(lp->name);
	}
	DBG_PRINTF("listen_fd:%d,%d\n",listen_fd,server);
	
	if (bind(listen_fd, (SA *)server, sa_len)) {
		err = SNL_ERR_BIND;
		goto close_socket;
	}
	
	if (listen(listen_fd, 3)) { // backlog = 3 XXX config via snl_init() ?
		err = SNL_ERR_LISTEN;
		goto close_socket;
	}
	
	while (TRUE) 
	{
		usleep(1000);
		err = SNL_ERR_OK;
		//DBG_PRINTF("123\n");
		if ((client_fd = accept(listen_fd,(SA *)client,&sa_len)) < 0) 
		{
			if (errno == EINTR)
				continue;
			err = SNL_ERR_ACCEPT;
		}
		//�ڴ˴� ���ӵĿͻ��˿���ע�ᵽ�豸�б���Ҫʵ��
		//for(i=0;i<6;i++)
		//DBG_PRINTF("%2X ",(unsigned char)(((SA *)client)->sa_data[i]));
	    //DBG_PRINTF("\nclient_fd:%d\n",client_fd);
		//DBG_PRINTF("c:%s\n",(((SA *)client)->sa_data));
		if(err==SNL_ERR_OK)
		{
			for(i=0;i<MenNum;i++)
			{
				if(Devclient[i].fd==0)
				{
					Devclient[i].fd = client_fd;
					Devclient[i].Port = ((SA *)client)->sa_data[1];
					Devclient[i].Port |= ((SA *)client)->sa_data[0]<<8;
					Devclient[i].IP[0] = ((SA *)client)->sa_data[2];
					Devclient[i].IP[1] = ((SA *)client)->sa_data[3];
					Devclient[i].IP[2] = ((SA *)client)->sa_data[4];
					Devclient[i].IP[3] = ((SA *)client)->sa_data[5];
					Devclient[i].answer = 0;
					Devclient[i].cmd = 0;
					break;
				}
			}
		}
		lp->cb(lp->port, err, client_fd, lp->data);
	}
close_socket:
	if (server)    free(server);
	if (client)    free(client);
	lp->cb(lp->port, err, 0, lp->data);
	pthread_exit(0);   
	return (NULL);
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
static void *Thread_UDPread(void *param)
{
	int remaining, received, err = SNL_ERR_OK, buf_len = 4096;
	unsigned int addrlen, data_len,i;
	ReceiveParam *rp;
	unsigned char *buf, *ptr;
	SA *client; 

	client = malloc(sizeof(SA));
	
	rp = (ReceiveParam *)param;
	addrlen = sizeof(SAI);
	
	// allocate buffer for received data
	if (!(buf = malloc(buf_len))) 
	{
		err = SNL_ERR_BUFFER;
		goto close_socket;
	}

	// we repeat until the connection has been closed
	while (TRUE) 
	{
		usleep(1000);
		
		received = recvfrom(rp->fd,buf,4096,0,(SA*)client,&addrlen); 
		if (received == 0) 
		{
			err = SNL_ERR_CLOSED;
			goto close_socket;
		} 
		else if (received < 0) 
		{
			if (errno == EINTR) continue;
			err = SNL_ERR_READ;
			goto close_socket;
		} 
		else 
		{
			for(i=0;i<MenNum;i++)
			{
				if(Devudp[i].fd==0||Devudp[i].fd == rp->fd)
				{
					Devudp[i].fd = rp->fd;
					Devudp[i].Port = ((SA *)client)->sa_data[1];
					Devudp[i].Port |= ((SA *)client)->sa_data[0]<<8;
					Devudp[i].IP[0] = ((SA *)client)->sa_data[2];
					Devudp[i].IP[1] = ((SA *)client)->sa_data[3];
					Devudp[i].IP[2] = ((SA *)client)->sa_data[4];
					Devudp[i].IP[3] = ((SA *)client)->sa_data[5];
					Devudp[i].answer = 0;
					Devudp[i].cmd = 0;

					DBG_PRINTF(".Port:%d\n",Devudp[i].Port);
					break;
				}
			}
		
			rp->cb(rp->fd, err, (void *)buf, received,((SA *)client)->sa_data);
		}
		
	}
	
	close_socket:
	shutdown(rp->fd, SHUT_RDWR);
	
	close(rp->fd);
	
	
	if (buf) 
		free(buf);

	if(client)
		free(client);
	
	rp->cb(rp->fd, err, NULL, 0, rp->data);
	
	
	return (NULL);
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
static int SnlRead(int fd, SNL_READ_CALLBACK(*cb), void *data)
{
	pthread_t thr_id;
	ReceiveParam *rp;

	if (!(rp = (ReceiveParam *)malloc(sizeof (ReceiveParam)))) {
		return (SNL_ERR_BUFFER);
	}
	rp->fd   = fd;
	rp->cb   = cb;
	rp->data = data;
	if (pthread_create(&thr_id, NULL, &Thread_read, (void *)rp)) {
		return (SNL_ERR_READ);
	}
	if (pthread_detach(thr_id)) {
		return (SNL_ERR_READ);
	}
	return (SNL_ERR_OK);
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
//static int SnlWrite(int fd, void *buf, unsigned long int len)
//{/*
//	struct iovec vec[2];
//
//	vec[1].iov_base = buf;
//	vec[1].iov_len  = len;
//	// convert packet length to net byte order
//	len = htonl(len);
//	vec[0].iov_base = &len;
//	vec[0].iov_len  = sizeof (len);
//
//	write_data:
//	if (writev(fd, &vec[0], 2) == -1)
//	{
//		if (errno == EINTR)
//		goto write_data;
//		return (SNL_ERR_WRITE);
//	}
//	*/
//	int i;
//
//	unsigned char *buff;
//	buff = buf;
//	//DBG_PRINTF("len:%d\n",len);
//	//for(i=0;i<len;i++)
//	//DBG_PRINTF("%X ",buff[i]);
//
//write_data:
//	if (write(fd,buff,len) == -1)
//	{
//		if (errno == EINTR)
//			goto write_data;//造成死循环
//		return (SNL_ERR_WRITE);
//	}
//	//usleep(2000);
//	return (SNL_ERR_OK);
//}
static int SnlWrite(int fd, void *buf, unsigned long int len)
{/*
	struct iovec vec[2];

	vec[1].iov_base = buf;
	vec[1].iov_len  = len;
	// convert packet length to net byte order
	len = htonl(len);
	vec[0].iov_base = &len;
	vec[0].iov_len  = sizeof (len);
	
	write_data:
	if (writev(fd, &vec[0], 2) == -1) 
	{
		if (errno == EINTR) 
		goto write_data;
		return (SNL_ERR_WRITE);
	}
	*/
//	int i;
	
	unsigned char *buff;
	buff = buf;
	//DBG_PRINTF("len:%d\n",len);
	//for(i=0;i<len;i++)
	//DBG_PRINTF("%X ",buff[i]);
write_data:
	printf("write_data\r\n");
	if(write(fd,buff,len) == -1)
	{
		printf("write=-1:errno=%d\r\n",errno);
//		if (errno == EINTR)
//			goto write_data;//造成死循环
		return (SNL_ERR_WRITE);
	}
	printf("exit SnlWrite\r\n");
	//usleep(2000);
	return (SNL_ERR_OK);
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
static int SnlListen(const char *name, unsigned short port, SNL_LISTEN_CALLBACK(*cb), void *data)
{
	pthread_t thr_id;
	ListenParam *lp;

	if (!(lp = (ListenParam *)malloc(sizeof (ListenParam)))) {
		return (SNL_ERR_BUFFER);
	}
	lp->port = port;
	lp->name = (char *)name;
	lp->cb   = cb;
	lp->data = data;
	
	if (pthread_create(&thr_id, NULL, &Thread_listen, (void *)lp)) {
		return (SNL_ERR_LISTEN);
	}
	
	if (pthread_detach(thr_id)) {
		return (SNL_ERR_LISTEN);
	}
	
	return (SNL_ERR_OK);
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
static int SnlConnect(const char *name, unsigned short port)
{
	int fd, flags, sa_len, ph_len;
	struct hostent *host = NULL;
	char addr[INET_ADDRSTRLEN];
	void *server;

	sa_len = port ? sizeof (SAI) : sizeof (SAU);
	server = malloc(sa_len);
	memset(server, 0, sa_len);
	
	if ((fd = socket(port ? AF_INET : AF_UNIX, SOCK_STREAM, 0)) < 0) 
	{
		return (SNL_ERR_OPEN);
	}
	
	if ((flags = fcntl(fd, F_GETFL, 0)) >= 0) 
	{
		flags |= TCP_NODELAY;
		fcntl(fd, F_SETFL, flags);
	}
	
	if (port) 
	{
		((SAI *)server)->sin_family = AF_INET;
		((SAI *)server)->sin_port   = htons((int)port);
		if (!(host = gethostbyname(name))) 
		{
			return (SNL_ERR_ADDRESS);
		}
		
		if (!inet_ntop(AF_INET, host->h_addr, addr, sizeof (addr))) 
		{
			return (SNL_ERR_CONNECT);
		}
		
		if (!inet_pton(AF_INET, name, &((SAI *)server)->sin_addr)) 
		{
			return (SNL_ERR_CONNECT);
		}
	} 
	else 
	{
		((SAU *)server)->sun_family = AF_UNIX;
		ph_len = sizeof (((SAU *)server)->sun_path)-1;
		strncpy(((SAU *)server)->sun_path, name, ph_len);
	}
	
	if (connect(fd, (SA *)server, sa_len)) 
	{
		close(fd);
		return (SNL_ERR_CONNECT);
	}
	DBG_PRINTF("name:%s,Port:%d\n",name,port);
	return (fd);
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
static int SnlUDPRead(int fd, SNL_READ_CALLBACK(*cb), void *data)
{
	pthread_t thr_id;
	ReceiveParam *rp;

	if (!(rp = (ReceiveParam *)malloc(sizeof (ReceiveParam)))) {
		return (SNL_ERR_BUFFER);
	}
	rp->fd   = fd;
	rp->cb   = cb;
	rp->data = data;
	if (pthread_create(&thr_id, NULL, &Thread_UDPread, (void *)rp)) {
		return (SNL_ERR_READ);
	}
	if (pthread_detach(thr_id)) {
		return (SNL_ERR_READ);
	}
	return (SNL_ERR_OK);
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
static int SnlUdpServerInit(int Port,SAI *sin)
{
	int fd, flags, sa_len, ph_len;
	void *server;
	
	sa_len = Port ? sizeof (SAI) : sizeof (SAU);
	sin = server = malloc(sa_len);
	memset(server, 0, sa_len);

    bzero(server,sizeof(SAI));  
    ((SAI *)server)->sin_family=AF_INET;  
    ((SAI *)server)->sin_addr.s_addr=htonl(INADDR_ANY);  
    ((SAI *)server)->sin_port=htons(Port);  

    fd = socket(AF_INET,SOCK_DGRAM,0);  
    
    if(fd<0)
    return -1;
    
    bind(fd,server,sizeof(SAI));  
    
    return fd;
}

static int SnlUdpClientInit(char *DIP,int DPort,SAI **sin)
{
  	int fd, flags, sa_len, ph_len;
	void *server;
	
	sa_len = DPort ? sizeof (SAI) : sizeof (SAU);
	*sin = server = malloc(sa_len);
	memset(server, 0, sa_len);

    bzero((SAI *)server,sizeof(SAI));  
    ((SAI *)server)->sin_family=AF_INET;  
    ((SAI *)server)->sin_addr.s_addr=inet_addr(DIP);//���ﲻһ��  
    ((SAI *)server)->sin_port=htons(DPort);  
	
	fd=socket(AF_INET,SOCK_DGRAM,0); 

	if(fd<0)
		return -1;
	/*
	if(sendto(fd,"abc",3,0,(SA *)server,sizeof(SAI))<0)
    {
		DBG_PRINTF("send udp err!\n"); 
    }*/
    
    return fd;
    
}

static int SnlUdpClientSend(int fd,char *data,int len,SAI *server)
{
	if(sendto(fd,data,len,0,(SA *)server,sizeof(SAI))<0)
    {
		DBG_PRINTF("send udp err!\n"); 
    }
    return 0;
}

static int SnlUdpSend(char *DIP,int DPort,char *data,int len)
{
	int fd, flags, sa_len, ph_len;
	void *server;
	DBG_PRINTF("DIP:%s\n",DIP);
	sa_len = DPort ? sizeof (SAI) : sizeof (SAU);
	server = malloc(sa_len);
	memset(server, 0, sa_len);

    bzero((SAI *)server,sizeof(SAI));  
    ((SAI *)server)->sin_family=AF_INET;  
    ((SAI *)server)->sin_addr.s_addr=inet_addr(DIP);//���ﲻһ��  
    ((SAI *)server)->sin_port=htons(DPort);  
	
	fd=socket(AF_INET,SOCK_DGRAM,0); 

	if(fd<0)
		return -1;
	
	if(sendto(fd,data,len,0,(SA *)server,sizeof(SAI))<0)
    {
		DBG_PRINTF("send udp err!\n"); 
    }
    
    DBG_PRINTF("fd:%d\n",fd);
    shutdown(fd, SHUT_RDWR);
    close(fd);
    return fd;
}

#undef SA
#undef SAI
#undef SAU

T_TCPWROpr g_tTCPWrOpr = {
	.name 			= "netdevice",
	.Write          = SnlWrite,
	.Read    		= SnlRead,
	.Listen  		= SnlListen,
	.Connect		= SnlConnect,
	.UdpServerInit	= SnlUdpServerInit,
	.UDPRead		= SnlUDPRead,
	.UdpClientInit	= SnlUdpClientInit,
	.UdpClientSend	= SnlUdpClientSend,
	.SnlUdpSend     = SnlUdpSend,
};


int TcpInit(void)
{
	return RegisterNetOpr(&g_tTCPWrOpr);
}


