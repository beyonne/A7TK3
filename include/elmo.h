#ifndef _ELMO_H
#define _ELMO_H	 

#define RUN_MODE				0x6060		//ģʽѡ��
#define CONTROL_WORD			0x6040		//������
#define TARGET_VELOCITY			0x60FF		//Ŀ���ٶ�

#define GET_TARGET_VELOCITY			0x6069		//Ŀ��ʵ���ٶ�
#define GET_LIMIT_STA 				0x60FD		//Ŀ��ʵ���ٶ�



#define MODE_VELOCITY		0X03		//�ٶ�ģʽ
#define MODE_POSITION		0X01		//λ��ģʽ
#define RUN_ENABLE			0X0F		//ʹ�ܣ�
#define RUN_DISENABLE			0X00		//��ʹ�ܣ�



//���ÿ���ģʽ��
int mode_operation(int id,unsigned char mode);

//���ÿ�����
int ctrl_word(int id,unsigned char status);

//ʹ�ܺͷ�ʹ������
int run_enable(int id,unsigned char status);

//�����ٶȣ����ݵ�λ��ǰ����λ�ں�
int set_speed(int id,int speed);








#endif 
