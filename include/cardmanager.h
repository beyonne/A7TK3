#ifndef	__CARDMANAGER_H
#define	__CARDMANAGER_H



/*ˢ��ʧ��ԭ��*/
#define SUCCESSED					0x01		//�Ϸ���
#define WQUANXIANERROR				0x02		//��Ȩ��;
#define GUASHICARD					0x03		//��ʧ��;
#define ZHUXIAOCARD					0x04		//δע��/ע����;
#define GUOQICARD					0x05 		//���ڿ�;
#define PASSWORDERROR				0x06		//�������;
#define JQUANXIANERROR				0x07		//��������Ȩ��;
#define TQUANXIANERROR				0x08		//ʱ������Ȩ��;
#define DOORCLOSE					0x09		//��Ϊ���ս�ֹ����״̬;
#define FANQIANHUI					0x0a		//��Ǳ��;
#define DUOKAOPEN					0x0b		//�࿨����;
#define DUOMENHUSUO					0x0c		//���Ż���;
#define SHOUKAOPEN					0x0d		//�׿����ţ�
#define INVALID						0x0e		//���˿���


/*Ȩ����֤λ����*/
#define AUTHGUASHI	(1<<6)					//�Ƿ�Ϊ��ʧ��;// 01000000 0x40;
#define MODEAUTH	(1<<1)					//ģ���Ƿ���ڱ�־λ��
#define WEEK 		(1<<(Rtc->Week - 1))		//û������0�����Լ�ȥ1��
#define MONTH 		(1<<(Rtc->Days - 1))	//û��0�գ����Լ�ȥ1��
#define SFTC		(1<<0)					//�Ƿ��˿���
#define DOORCO		(1<<(ID-1))				//�Ž�ֹ�����ߴ򿪱�־λ��
#define SFSC		(1<<2)					//�Ƿ��׿����ţ�
#define SFDC		(1<<3)					//�Ƿ�࿨���ţ�
#define AUTHBIT		(1<<(ID-1))				//�Ž�ֹ�����ߴ򿪱�־λ��


int DorpTable(char *name);

int CreateTable(char *name);
			
int CreateTableInx(char *name,char *namex,char *inxname);
int DeleteCard(char *data);
int Charge_Authority(int Serial_Number,unsigned char Key[],unsigned char ID);




#endif
