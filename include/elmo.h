#ifndef _ELMO_H
#define _ELMO_H	 

#define RUN_MODE				0x6060		//模式选择
#define CONTROL_WORD			0x6040		//控制字
#define TARGET_VELOCITY			0x60FF		//目标速度

#define GET_TARGET_VELOCITY			0x6069		//目标实际速度
#define GET_LIMIT_STA 				0x60FD		//目标实际速度



#define MODE_VELOCITY		0X03		//速度模式
#define MODE_POSITION		0X01		//位置模式
#define RUN_ENABLE			0X0F		//使能；
#define RUN_DISENABLE			0X00		//关使能；



//设置控制模式；
int mode_operation(int id,unsigned char mode);

//设置控制字
int ctrl_word(int id,unsigned char status);

//使能和非使能驱动
int run_enable(int id,unsigned char status);

//设置速度，数据低位在前，高位在后
int set_speed(int id,int speed);








#endif 
