#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#include <can_manager.h>

static int CAN_init(int *fd,char *name,char filter_flag,int *can_id,int *can_mask,int filter_number,char af_can,char pf_can)
{
	int ret;
	struct sockaddr_can addr;
	struct ifreq ifr;
	int master,i;
	//struct can_filter filter[filter_number];

	srand(time(NULL));
	*fd = socket(pf_can, SOCK_RAW, CAN_RAW);		/*�����׽���*/

	if (*fd < 0) 
	{
        perror("socket PF_CAN failed");
        return -1;
	}

	/*�׽��ְ󶨵�CAN�˿�*/
	strcpy(ifr.ifr_name,name);
	
	ret = ioctl(*fd, SIOCGIFINDEX, &ifr);				
	if (ret < 0) 
	{
        perror("ioctl failed");
        return -1;
	}

	addr.can_family = pf_can;
	addr.can_ifindex = ifr.ifr_ifindex;

	ret = bind(*fd, (struct sockaddr *)&addr, sizeof(addr));	
	if (ret < 0) 
	{
        perror("bind failed");
        return -1;
	}

	/**/
    if (filter_flag) 
    {
		struct can_filter filter[filter_number];
		/**/
		for(i=0;i<filter_number;i++)
		{
	        filter[i].can_id = can_id[i] | CAN_EFF_FLAG;
			filter[i].can_mask = can_mask[i];
	    }

		/**/
		ret = setsockopt(*fd, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));
		if (ret < 0) 
		{
			perror("setsockopt failed");
			return -1;
		}
	}

	return 0;
}




/**********************************************************************
 * �������ƣ� CAN���ݽ����̺߳���
 * ���������� 
 * ��������� name - ����
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2016/12/21	     V1.0	  ������	      ����
 ***********************************************************************/
static void *Thread_can_read(void *param)
{
	int err = CAN_ERR_OK;
	ReceiveParamCan *rp;
	int ret;	
	struct can_frame frdup;	
	//struct timeval tv;	
	fd_set rset;

	rp = (ReceiveParamCan *)param;
	
	// we repeat until the connection has been closed
	while (1) 
	{
		usleep(1000);
		
		FD_ZERO(&rset);		
		FD_SET(rp->fd, &rset);
				
		ret = read(rp->fd, &frdup, sizeof(frdup));	  //can ����֡Ϊ��λ���ж�ȡ����ͬ�ڴ���,�û����ݷ���frdup.data����_zhz
		
		if (ret < sizeof(frdup)) 		
		{			
			printf("read failed");	
			err = CAN_ERR_READ;
			goto close_can;
			//return -1;		
		}		
		
		rp->cb(rp->fd, err, (void *)frdup.data,frdup.can_dlc,frdup.can_id, rp->data);    //CAN_READ_CALLBACK(CanReadCallback) 
		
	}
	
	close_can:

	close(rp->fd);
	
	rp->cb(rp->fd, err, NULL, 0,frdup.can_id, rp->data);
	//printf("5\n");
	pthread_exit(0);   
	return (NULL);
}


static int CAN_read(int fd, int master,CAN_READ_CALLBACK(*cb), void *data)
{
	pthread_t thr_id;
	ReceiveParamCan *rp;

	if (!(rp = (ReceiveParamCan *)malloc(sizeof (ReceiveParamCan)))) {
		return (CAN_ERR_BUFFER);
	}
	rp->fd   = fd;
	rp->cb   = cb;
	rp->data = data;
	rp->master = master;
	
	if (pthread_create(&thr_id, NULL, &Thread_can_read, (void *)rp)) {
		return (CAN_ERR_READ);
	}
	if (pthread_detach(thr_id)) {
		return (CAN_ERR_READ);
	}
	return (CAN_ERR_OK);
}


static int CAN_close(int fd)  
{  
    if(close(fd)<0)
    {
		return -1;
    }
    return 0;
}  

//data_len<=8;
static int CAN_senddata(int fd,int can_id,unsigned char *tx_buf,int data_len)
{	
	struct can_frame frdup;
	int ret;

	if(data_len>8)
	return -1;

	frdup.can_id = can_id & CAN_EFF_MASK;
	frdup.can_dlc = data_len;
	
	memcpy(frdup.data,tx_buf,data_len);
		
	ret = write(fd, &frdup, sizeof(frdup));    //���ݷ�װ��frdup.data������з���_zhz
	
	if (ret < 0) 
	{
		DBG_PRINTF("canid:%X\n",can_id);
		myerr("write failed");
		return -1;
	}

	return 0;
}


static T_CanOpr g_tCanOpr = {
	.name 						= "can_dev",
	.CANClose					= CAN_close,
	.CANInit					= CAN_init,
	.CANRead					= CAN_read,
	.CANSendData				= CAN_senddata,
};
 
int CanRegister(void)
{
	 return RegisterCanOpr(&g_tCanOpr);
}



