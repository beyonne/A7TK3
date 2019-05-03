#ifndef __CANOPEN_H
#define __CANOPEN_H



#define NMT_PREOPERATION			0x80	//节点进入预处理模式，0x00所有节点，0X01第一节点
#define NMT_START					0x1		//启动节点，0x00所有节点，0x01，第一个节点
#define NMT_STOP					0x2		//停止节点，0x00所有节点，0x01，第一个节点
#define NMT_RESET_COMMUNICATION		0x82	//重置通讯
#define NMT_RESET_NODE				0x81	//重置节点


#define TX_ONE_BYTE			0x2F
#define TX_TWO_BYTE			0x2B
#define TX_THREE_BYTE		0x27
#define TX_FOUR_BYTE		0x23
#define TX_REQUEST_FRAME	0x40

#define RX_ONE_BYTE			0x4F
#define RX_TWO_BYTE			0x4B
#define RX_THREE_BYTE		0x47
#define RX_FOUR_BYTE		0x43
#define FAIL_OPERATION			0x80
#define SUCCESS_OPERATION		0x60

#define READ_PTZ_ANGLE			0x0500
#define READ_ARM_ANGLE			0x2002
#define READ_ANGLE				0x07
#define READ_MOTION_SPEED			0x6960



#define SDO_READ_ID		0x580
#define SDO_WRITE_ID	0x600

#define PDO2_READ_ID	0x280
#define PDO2_WRITE_ID	0x300

#define NMT_ID			0x00
#define HEARTBEAT_ID		0x700


int SDO_write_frame(int fd,int node_id,short index ,unsigned char subindex,unsigned char *send_data,int len);
int NMT_frame(int fd ,int node_id ,unsigned char NMT_cmd);
int SDO_request_frame(int fd,int node_id,short index ,unsigned char subindex);
int read_can_data(int fd,unsigned char *can_data,int *can_id);

int can0_SDO_write_frame(int node_id,short index,unsigned char subindex,unsigned char * send_data,int len);
int CAN0_NMT_frame_manager(int node_id ,unsigned char NMT_cmd);
int can0_PDO2_write_frame(int node_id,short index,unsigned char subindex,unsigned char * send_data,int len);
int can0_SDO_write_frame(int node_id,short index,unsigned char subindex,unsigned char * send_data,int len);








#endif 
