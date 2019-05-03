#ifndef _PARA_H
#define _PARA_H

#define  DeviceNO    0
#define  BOARD_TYPE  0xD0
#define  BOARD_HVER  0x01

#define  SOFTWARE_VER1  0x01
#define  SOFTWARE_VER2  0x00
#define  SOFTWARE_VER3  0x04

typedef struct
{
	int Sn;			//Ψһ����
	int ShortAddr;	//�̵�ַ
	int Endpoint;	//ͨ·��ַ
	int ProfileID;	//Profileid
	int DeviceID;	//�豸ID
	int ZoneType ;	//
	int DevState;	//�豸״̬
	int OnlineState ;//����״̬
	int Val;		//ֵ
	int SnNo;		//SN��
	int BDType1;	//������1
	int BDTypeSn1;	//����������1
	int BDType2;	//������2
	int BDTypeSn2;	//����������2

	unsigned char defence;	//����������־��
	
	int Rs1;		//Ԥ��1
	int Rs2;		
	int Rs3;		
	int Rs4;		
	int Rs5;		
	int Namelen;	//���ֳ���
	unsigned char Name[22];//����

	
}T_Device_Opr,*PT_Device_Opr;


/*ϵͳ��������ṹ*/
typedef struct Sys_Para_Opr
{	//76+23*4=218�ֽ�
	int SetFlag;				//�����Ƿ��趨��

	unsigned char Password[16];
	int Passwordlen;

	int  Contact_State; 			//����λ������״̬��Ĭ��0Ϊ����������1Ϊ�ѻ�������
	
	/*��������*/
	int  DeviceVersion;			//�豸����;
	int  codeVersion;			//����汾��;
	int  ByteMode;				// 3�ֽڹ���ģʽ����4�ֽڹ���ģʽ��
	
	/*�û����ò���*/
	int Contact_Password;		//��λ��ͨѶ��������;

	char  DeviceSn[4];			//ģ���豸��;SN
	int  Device_NameLen;			//�豸���Ƴ���;
	int  GatewaySn;				//����SN��
	char DeviceName[32];	//�豸����
	
	unsigned char IP_Addr[4];			//����IP��ַ��
	unsigned char DIP_Addr[4];			//������IP��ַ;
	unsigned char MAC[6];
	int	DIP_Addr_Flag;			//IP�Ƿ�����
	
	int Port;					//����˿ں�;
	int ParaSetFlag;			//�Ƿ��Զ���ȡIP
	int BfCfflag;				//����������־λ��
	int res1;					//Ԥ��
	int res2;
	int res3;
	int res4;
	int res5;
	int res6;
	int res7;
	int res8;
	int res9;
	int res10;

	
}T_Sys_Para_Opr,*PT_Sys_Para_Opr;   






extern int ParaInit(void);
extern PT_Sys_Para_Opr GetDefultSysParaOpr(void);

#endif 

