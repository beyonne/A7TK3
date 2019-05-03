#ifndef _NET_MANAGER_H_
#define _NET_MANAGER_H_

#include <sys/socket.h>  // socket(), bind(), listen(), accept(), shutdown()
#include <netdb.h>       // gethostbyname()
#include <netinet/tcp.h> // TCP_NODELAY
#include <netinet/in.h>  // struct sockaddr_in
#include <arpa/inet.h>   // htons(), htonl(), ntohl()
#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif


#define NET_NAME_MAX_LENTH		20
#define IP_CHAR_MAX_LENTH		20
#define DIP_CHAR_MAX_LENTH		20


#define SA  struct sockaddr
#define SAI struct sockaddr_in
#define SAU struct sockaddr_un

#define SNL_LISTEN_CALLBACK(CB) void (CB)(unsigned short port, int error, int fd, void *data)

#define SNL_READ_CALLBACK(CB) void (CB)(int fd, int error, void *buf, int len, void *data)

#define SNL_ERR_OK       0   ///< no error
#define SNL_ERR_OPEN    -1   ///< could'nt open socket
#define SNL_ERR_LISTEN  -2   ///< error while listening to socket
#define SNL_ERR_CONNECT -3   ///< connecting to socket failed
#define SNL_ERR_BIND    -4   ///< couldn't bind to socket
#define SNL_ERR_ACCEPT  -5   ///< error while accepting connection
#define SNL_ERR_READ    -6   ///< error while reading from socket
#define SNL_ERR_WRITE   -7   ///< failed to write to socket
#define SNL_ERR_CLOSED  -8   ///< peer closed connection
#define SNL_ERR_BUFFER  -9   ///< out of memory (buffer allocation error)
#define SNL_ERR_ADDRESS -10  ///< could not get address from hostname

typedef struct ListenParam {
	unsigned short port;          // port to listen on
	char *name;                   // snl name for UNIX DOMAIN SOCKET
	void *data;                   // pointer to user specific data
	SNL_LISTEN_CALLBACK(*cb);     // function to call after a connecting
} ListenParam;

typedef struct ReceiveParam {
	int  fd;                      // filedescriptor to read from
	void *data;                   // pointer to user specific data
	SNL_READ_CALLBACK(*cb);       // function to call after reading
} ReceiveParam;


typedef struct TCPWROpr {	
	char *name;
	//int fd;
	int (*Write)(int fd, void *buf, unsigned long int len);
	int (*Read)(int fd, SNL_READ_CALLBACK(*cb), void *data);
	int (*Listen)(const char *name, unsigned short port, SNL_LISTEN_CALLBACK(*cb), void *data);
	int (*Connect)(const char *name, unsigned short port);
	int (*UdpServerInit)(int Port,SAI *sin);
	int (*UDPRead)(int fd, SNL_READ_CALLBACK(*cb), void *data);
	int (*UdpClientInit)(char *DIP,int DPort,SAI **sin);
	int (*UdpClientSend)(int fd,char *data,int len,SAI *server);
	int (*SnlUdpSend)(char *DIP,int DPort,char *data,int len);
	struct TCPWROpr *ptNext;
}T_TCPWROpr, *PT_TCPWROpr;

typedef struct NetDeviceOpr {	
	int fd;
	int Port;
	int DPort;
	char *DIP;
	char *name;
	char *IP;
	struct sockaddr_in *sin;
	struct NetDeviceOpr *ptNext;
}T_NetDeviceOpr, *PT_NetDeviceOpr;

typedef struct ClientOpr{	
	int fd;
	unsigned char IP[4];
	int Port;
	unsigned char cmd;
	int answer;
	struct ClientOpr *ptNext;
}T_ClientOpr, *PT_ClientOpr;


int NetRegister(void);
int AddNetDevice(char *name,char *IP,int Port,char *DIP,int DPort);
int SendData(int fd,unsigned char *data,int len);
int UdpSendWay(char *DIP,int port,char *data,int len);


int InitUDPClient(char *devname);
int InitUDPServer(char *devname);
int InitTcpServer(char *devname);
int InitTcpClient(char *devname);
int TcpInit(void);
int RegisterNetOpr(PT_TCPWROpr ptTcpOpr);

extern T_ClientOpr Devclient[MenNum+1];
extern T_ClientOpr Devudp[MenNum];


#ifndef NULL
#define NULL ((void *)0) ///< NULL is defined in a somehow save manner
#endif

#ifndef FALSE
#define FALSE (0)        ///< FALSE is equal to 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)    ///< TRUE  is not equal to FALSE
#endif

#ifdef __cplusplus
}
#endif

#endif /* _H_ */
