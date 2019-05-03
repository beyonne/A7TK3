#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#define FRAME_SIZE 			1024
#define RX_OK	1			//接受一帧数据完成
#define RX_OK_P	0			//处理完一帧数据；
#define Velocity		0


#define JOINT_TIME		15

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//byte array converters
union  {
    int i;
    unsigned char c[4];
} cvtInt32;

union  {
    unsigned int i;
    unsigned char c[4];
} cvtUInt32;

union  {
    short s;
    unsigned char c[2];
} cvtInt16;

union {
    unsigned short s;
    unsigned char c[2];
} cvtUInt16;

union {
    char c;
    unsigned char b;
} cvtInt8;

typedef enum {
            Torrent=0,
            Shoulder,	//��ؽ�
            Elbow,		//��ؽ�
            Pitch,
            Roll,
            Gripper,	//����
            J_6,
            J_7,
            Far_Pitch,
            Far_Roll,
            Far_Gripper,
            J_11,
            J_12,
            JOINT_NUM
}JointId;

typedef enum {
            IncEncoder = 0,
            AbsEncoder,
            Potentiometer,
MEASURE_TYPE
}_MEASURE_TYPE;

typedef enum {
            Disable = 0,
            Enable,
CMD_STATUS
}_CMD_STATUS;

typedef struct {
  _CMD_STATUS       CMD_STATUS;
  _MEASURE_TYPE     MSR_type;
  unsigned char     NET_ID;
  int               POSITION;
  short             CMD_VELOCITY;
  short             CMD_VELOCITY_PRE;
  unsigned char     CMD_SCALE;
  unsigned char     cmd_count;
  unsigned char     read_time_out;
} __GRIPPER_MOTOR;

typedef enum {
   NoData = 1,
   HaveData = 2
}_Cmd_Data;


#define READ_CMD		0x03

typedef struct TCP_Rx_Opr
{
	unsigned char *Rx_Data; 				//接收的数据
	int  Rx_Len;	  						//接收数据长度
	int  Device_Type;  						//接收到的数据属于的设备类型
	int  Device_Sn;   						//设备编号
	int  Rx_OK;   							//接收完成   0x00:未完成;0x01:完成
	int  Rx_Bot;  							//接收数组下标
	int  ACK_TIME_C_EN;  					//接收方应答计时使能  0:禁止;1:使能
	int  ACK_TIME;      					//超时定时时间
}T_TCP_Rx_Opr,*PT_TCP_Rx_Opr;   			//接收结构体
// #pragma pack()//让编译器对这个结构作1字节对齐;

 
/*PC与控制器通讯协议结构*/
typedef struct Comcation_Opr
{
	unsigned char Pro_SOH ;						//起始帧;
	int ID ;								//主控器设备号;
	int Len ;							//长度(命令+数据);
	int Cmd;								//命令;
	unsigned char *DataAddr;				//数据;
	int BCC;							//效验;
}T_Comcation_Opr,PT_Comcation_Opr;   

typedef struct 
{
	int PC_TX_Time_Out;
	int  PC_TX_Out_En;
	int  PC_TX_OutNumber;
	int  PC_TX_SendFlag;
}T_pcTxTime,*PT_pcTxTime;

typedef struct senser_Opr
{
	unsigned char type ; 		//起始帧;
	int val;
}T_senser_Opr,PT_senser_Opr;

typedef struct GasSenserFrame
{
	unsigned char Arr;     //地址
	unsigned char Cmd;     //指令
	unsigned char Len;     //长度
	unsigned short Value;  //值
}T_GasSenserFrame,PT_GasSenserFrame;

#define PC_ACK_TIME  	3						//应答超时时间
#define PC_ACK_OUTRST  	3						//应答超时次数
#define PC_SEND_TIME	10						//发起心跳时间间隔S
#define To_OCU_Core_Sub_Length 30
#define  TO_HEAD_LENGTH  14
#define  FROM_HEAD_LENGTH 23

#define HEAD_BOARD_HAEDDATA		0x48
#define HEAD_BOARD_HAEDDATA1	0x44

//#define ARMRXHAEDDATA		0x51
#define ARMRXHAEDDATA		0x3A
#define ARMRXHAEDDATA1		0xEF
#define AMC_Pro_SOH			0xA5


extern unsigned char rx_led_flag;
extern int temperature ,Vbat;
extern int adccnt;

PT_pcTxTime GetPcTxTime(void);
void Shake_hands(void);
int PcTxTimeMalloc(void);
void RegGawyToServer(void);

void canopen_data_pro(PT_TCP_Rx_Opr HeadRx,PT_TCP_Rx_Opr ARMRx,PT_TCP_Rx_Opr PTZRx);
void AMC_Inintial_PARAMETER(void);


#endif

