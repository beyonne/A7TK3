
#include <stdlib.h>
#include <motion.h>
#include <config.h>
#include <usart_manager.h>
#include <timer.h>


int Prepare_Motion_Command(unsigned char Driver_Adress,\
unsigned char Msg_Sequence,\
char wr, unsigned short cmd,\
unsigned char *data,\
int word_lenth);

void Motion_Init(void)
{
	enable_driver(FLIPPER);
	usleep(50000);
	disable_bridge(FLIPPER);
	usleep(50000);
	enable_bridge(FLIPPER);
	usleep(50000);
	
	enable_driver(TRACK_L);
	usleep(50000);
	disable_bridge(TRACK_L);
	usleep(50000);
	enable_bridge(TRACK_L);
	usleep(50000);
	
	enable_driver(TRACK_R);
	usleep(50000);
	disable_bridge(TRACK_R);
	usleep(50000);
	enable_bridge(TRACK_R);
}

//For driver motion 
int enable_driver(int id)
{
	unsigned char data[16];
	data[0] = 0x0f;
	data[1] = 0x00;
	
	if(Prepare_Motion_Command(id,USERSET_READ_NONE,CMD_WRITE,SETWRITE,data,1))//Prepare the speed command
	{
	}
	return 0;
}

int enable_bridge(int id)
{
	unsigned char data[16];
	
	data[0] = 0x00;
	data[1] = 0x00;
	if(Prepare_Motion_Command(id,USERSET_READ_NONE,CMD_WRITE,BRIDGE,data,1))//Prepare the speed command
	{
		
	}
	return 0;
}

int set_motor(int id,char motor)
{
	unsigned char data[16];
	
	data[0] = motor;
	data[1] = 0x00;
	if(Prepare_Motion_Command(id,USERSET_READ_NONE,CMD_WRITE,BRIDGE,data,1))//Prepare the speed command
	{
		
	}
	return 0;
}


int disable_bridge(int id)
{	
	unsigned char data[16];
		
	data[0] = 0x01;
	data[1] = 0x00;

	if(Prepare_Motion_Command(id,USERSET_READ_NONE,CMD_WRITE,BRIDGE,data,1))//Prepare the speed command
	{
		
	}
	return 0;
}

int Prepare_Motion_Command(unsigned char Driver_Adress, unsigned char Msg_Sequence,char wr, unsigned short cmd, unsigned char *data,int word_lenth)
{
	short crc16;
	unsigned char MSG_To_Driver[32];
	PT_UsartDeviceOpr ptMotion;

	ptMotion = GetUsartOpr(COM_MOTION);

	if(Driver_Adress>63)
		return(0);

	if(Msg_Sequence>15)
		Msg_Sequence = 15;

	MSG_To_Driver[0] = 0XA5;
	MSG_To_Driver[1] = Driver_Adress;
	MSG_To_Driver[2] = (Driver_Adress<<4)+(Msg_Sequence<<2)+(wr&0x03);	// 0
	MSG_To_Driver[3] = cmd>>8;
	
	MSG_To_Driver[4] = cmd;
	MSG_To_Driver[5] = word_lenth;
	crc16  = Table_CRC16(MSG_To_Driver,6);
	MSG_To_Driver[6] = crc16>>8;
	MSG_To_Driver[7] = crc16;

	if(wr!=1)
	{
		memcpy(&MSG_To_Driver[8],data,word_lenth*2);
		crc16 = Table_CRC16(&MSG_To_Driver[8],word_lenth*2);
		MSG_To_Driver[8+(word_lenth*2)] = crc16>>8;
		MSG_To_Driver[9+(word_lenth*2)] = crc16;
	}
	//DBG_PRINTF("---\n");
	UsartSendDatafd(ptMotion->fd,MSG_To_Driver,8+(word_lenth*2+(!!word_lenth*2))*(wr-1));
	return(1);	
}

void flipper_on()
{
	enable_bridge(FLIPPER);
}

void flipper_off()
{
	//cmd_move_flipper(0);
	disable_bridge(FLIPPER);
}

void motion_get_speed(int id)
{
	unsigned char data[4];
	Prepare_Motion_Command(id,USERSET_READ_SPEED,CMD_READ,READ_SPEED,data,2);
}

void motion_get_angle(int id)
{
	unsigned char data[4];
	Prepare_Motion_Command(id,USERSET_READ_ANGLE,CMD_READ,READ_ANGLE,data,1);
}

int motor_set_speed(int id, int speed,int speed_k)
{
	unsigned char speeddata[16];
	speed = speed*speed_k;
	speeddata[0]=speed;
	speeddata[1]=speed>>8;
	speeddata[2]=speed>>16;
	speeddata[3]=speed>>24;

	//DBG_PRINTF("speed0:%X\n",speeddata[0]);
	//DBG_PRINTF("speed1:%X\n",speeddata[1]);
	//DBG_PRINTF("speed2:%X\n",speeddata[2]);
	//DBG_PRINTF("speed3:%X\n",speeddata[3]);
	if(Prepare_Motion_Command(id,USERSET_READ_NONE,CMD_WRITE,SETSPEED,speeddata,2))//Prepare the speed command
	{
	}
	return 0;
}

/*
int Set_speed_flipper(int speed)
{
	int velocity_flipper_int;
	
	velocity_flipper_int = speed*FLIPPER_LOCAL_SCALAR;
	
	motor_set_speed(FLIPPER,velocity_flipper_int);
	
	return (1);
}

int Set_L_speed_Tyre(int speed)	//�����ٶ��趨
{
	int velocity_flipper_int;

	velocity_flipper_int = speed*AAI_TRACTION_LOCAL_SCALAR;
	
	motor_set_speed(TRACK_L,velocity_flipper_int);
	
	return (1);
}

int Set_speed(int id,int speed)	//�����ٶ��趨
{
	int velocity_flipper_int;

	velocity_flipper_int = speed*AAI_TRACTION_LOCAL_SCALAR;
	
	motor_set_speed(TRACK_L,velocity_flipper_int);
	
	return (1);
}
*/

void Platform_Stop(void)
{
	motor_set_speed(TRACK_R,0,0);
	motor_set_speed(TRACK_L,0,0);
	motor_set_speed(FLIPPER,0,0);
}


//设定轮子和翻转臂的速度等参数
void get_js_input(unsigned char *pcoredata)
{
	unsigned char Parking, Reset,E_Stop,Platform_Mode,Flipper_lock;
	unsigned char max_speed=2;
	unsigned char js_x, js_y, js_z;
	static int R_cnt;
	static int L_cnt;
	static int stop_flag;
	
	//int right, left, flipper;
	//int R,R1,L,L1,turn;
	//int h,display;
	//int flipper1;
	unsigned int R_val=0,L_val=0,F_val=0,B_val=0,motion_scalar,Fli_R=0,Fli_L=0;
	int Speed_R=0,Speed_L=0,Fli_Speed=0;
	float K_val,allspeed;

	PT_TimeStopList_Opr	time_stop;
	time_stop = get_def_time();
	
	js_y = pcoredata[1];//前进后退，大于127前进;小于127后退
	js_x = pcoredata[2];//转向，大于127顺时针;小于127逆时针
	js_z = pcoredata[3];//翻转臂，大于127顺时针;小于127逆时针
	motion_scalar = pcoredata[4];//旋钮速度,最大255

//	DBG_PRINTF("y:%d,x:%d,z:%d\n",js_y,js_x,js_z);
//	DBG_PRINTF("d:%X\n",pcoredata[5]);
	//digital part
	Parking			= (pcoredata[5]&128)/128;
	Reset			= (pcoredata[5]&64)/64;
	E_Stop			= (pcoredata[5]&32)/32;
	Platform_Mode	= (pcoredata[5]&0x18)/24;
	Flipper_lock    = (pcoredata[5]&4)/4;	

	if(Flipper_lock	== 1)
		js_z = 0x80;

	if(Parking|E_Stop)// ignore the joystick inout for driving
	{
		js_x = 0x80;
		js_y = 0x80;
	}

	if(js_x>(zerospeed+offset)) //左转     绝对值两个轮子都为正数时，左转
	{
		L_val = js_x - zerospeed;
		L_val = L_val/RL_K;

		if(abs(js_y - zerospeed)>offset)	//有前进或者后退数据时；
		{
			//Speed_L -= L_val*(RL_K/2);
		}
		else
		{
//			if(L_val >=35)
//			{
//				L_val = 35;
//			}
			if(L_val >=50)//修改20181206
			{
				L_val = 50;
			}

			Speed_L += (L_val/ZW_K);
			Speed_R += (L_val/ZW_K);
			
//			DBG_PRINTF("Speed_L:%d,Speed_R:%d\n",Speed_L,Speed_R);
			Speed_R=-Speed_R;
		}
	}

	if(js_x<(zerospeed-offset)) //右转	 绝对值两个轮子都为负数数时，右转
	{
		R_val = zerospeed - js_x;
		R_val = R_val/RL_K;
		
		if(abs(js_y - zerospeed)>offset)	//有前进或者后退数据时；
		{
			//Speed_R -= R_val*(RL_K/2);
		}
		else
		{
//			if(R_val >=35)
//			{
//				R_val = 35;
//			}
			if(R_val >=50)//修改20181206
			{
				R_val = 50;
			}
			
			Speed_L += (R_val/ZW_K);
			Speed_R += (R_val/ZW_K);
//			DBG_PRINTF("Speed_L:%d,Speed_R:%d\n",Speed_L,Speed_R);
			Speed_L=-Speed_L;
			//Speed_R=-Speed_R;
		}
	}

	if(js_y<(zerospeed-offset)) //前进   绝对值右轮子为正，左轮子为负数时，前进；
	{
		B_val = zerospeed - js_y;

		Speed_L += B_val;
		Speed_R += B_val;

		Speed_L -= L_val;
		Speed_R -= R_val;

		Speed_L = abs(Speed_L);
		Speed_R = abs(Speed_R);

		Speed_L = -Speed_L;
		Speed_R = -Speed_R;
	}

	if(js_y>(zerospeed+offset)) //后退		绝对值左轮子为正，右轮子为负数时，后退；
	{
		F_val = js_y - zerospeed;

		Speed_L += F_val;
		Speed_R += F_val;

		Speed_L -= L_val;
		Speed_R -= R_val;
		
		Speed_L = abs(Speed_L);
		Speed_R = abs(Speed_R);
	}

	if(js_z>(zerospeed+offset)) //翻转臂
	{
		Fli_Speed = js_z - zerospeed;
	}

	if(js_z<(zerospeed-offset)) //翻转臂
	{
		Fli_Speed = js_z - zerospeed;
	}

	//DBG_PRINTF("Speed_L:%d,Speed_R:%d\n",Speed_L,Speed_R);
	/*速度调节旋钮速度设置*/
	printf("R:%d,L:%d,%d\n",Speed_R,Speed_L,motion_scalar);
	
	if(motion_scalar<MIN_motion_scalar)
		motion_scalar = MIN_motion_scalar;

	if(motion_scalar>MAX_motion_scalar)
		motion_scalar = MAX_motion_scalar;

	Speed_R = Speed_R*motion_scalar;
	Speed_L = Speed_L*motion_scalar;
	Fli_Speed = Fli_Speed*motion_scalar;

	//DBG_PRINTF("Speed_R1:%d\n",Speed_R);
	//DBG_PRINTF("Speed_L1:%d\n",Speed_L);
	
	Speed_R = Speed_R/MAX_motion_scalar;
	Speed_L = Speed_L/MAX_motion_scalar;
	Fli_Speed = Fli_Speed/MAX_motion_scalar;

	//DBG_PRINTF("R:%d,L:%d\n",Speed_R,Speed_L);
//	DBG_PRINTF("R:%d L:%d F:%d\n",(-Speed_R),Speed_L,Fli_Speed);
	//if(R_cnt<=20)	//0速度时,发送20次足够;空闲出总线;让翻转臂角度读取响应速度加快;
	motor_set_speed(TRACK_R,Speed_R,SPEED_K_VAL);
	usleep(3000);
	//if(L_cnt<=20)
	motor_set_speed(TRACK_L,-Speed_L,SPEED_K_VAL);
//		motor_set_speed(TRACK_R,-Speed_R,SPEED_K_VAL);
////		usleep(20000);
//		usleep(3000);
//		//if(L_cnt<=20)
//		motor_set_speed(TRACK_L,Speed_L,SPEED_K_VAL);
	usleep(3000);
	motor_set_speed(FLIPPER,Fli_Speed,SPEED_K_VAL);
	//usleep(5000);

	time_stop->motion_R_rx_timeout.TimeEn = 1;
	time_stop->motion_R_rx_timeout.Times = MOTION_L_RX_TIMEOUT;

	time_stop->motion_L_rx_timeout.TimeEn = 1;
	time_stop->motion_L_rx_timeout.Times = MOTION_L_RX_TIMEOUT;
//	printf("exit get_js_input\r\n");

#if 0

	if(Flipper_lock	==1)
		js_z = 0x80;	

	if ((js_y>=0x90) || (js_y<=0x70))
	{
		if (js_y>=0x90) 	
			js_y=0x80+LUT2[(js_y-0x80-1)];
			
		if (js_y<=0x70) 	
			js_y=0x80-LUT2[(0x80-js_y-1)];
	}
	if ((js_x>=0x90) || (js_x<=0x70))
	{
		if (js_x>=0x90) 	
			js_x=0x80+LUT2[(js_x-0x80-1)];
			
		if (js_x<=0x70) 	
			js_x=0x80-LUT2[(0x80-js_x-1)];
	}

	if(Parking)				// ignore the joystick inout for driving
	{
		js_x = 0x80;
		js_y = 0x80;
	}


	/*********************************************轮子运动***********************************************/

	if (js_y>0x85)
	{		
		if ((right<(max_speed*(js_y-0x7f))) && (right<=max_speed*123)) right=right+pwm_step; 
		else if ((right>(max_speed*(js_y-0x7f))) && (right>=pwm_step)) right=right-pwm_step;
	}
	else if (js_y<0x7a)
	{
		if ((right<max_speed*(-(0x7f-js_y))) && (right<=-pwm_step)) right=right+pwm_step;
		else if ((right>max_speed*(-(0x7f-js_y))) && (right>=-(max_speed*123))) right=right-pwm_step;
	}


	
	if ((max_speed==0) || ((js_y>=0x7a) && (js_y<=0x85)) )//js is in zero position
	{
		if (right>10*pwm_step)		 right=right-10*pwm_step;
		else if (right>=3*pwm_step)  right=right-3*pwm_step;
		else if (right>=pwm_step)	 right=right-pwm_step;
		else if(right>=0)			   right = 0;// On Jan 26, 2010, to make sure it will stop
		
		if (right<-10*pwm_step) 	 right=right+10*pwm_step;
		else if (right<=-3*pwm_step) right=right+3*pwm_step;
		else if (right<=-pwm_step)	 right=right+pwm_step;
		else if(right<=0)			   right = 0;// On Jan 26, 2010,to make sure it will stop
		
		// LeoMa 20140416 delelte redundant
		/*			
		if (left>10*pwm_step)		 left=left-10*pwm_step;
		else if (left>=3*pwm_step)	 left=left-3*pwm_step;
		else if (left>=pwm_step)	 left=left-pwm_step;
		else if(left>=0)			   left = 0;// On Jan 26, 2010, to make sure it will stop
				
		if (left<-10*pwm_step)		 left=left+10*pwm_step;
		else if (left<=-3*pwm_step)  left=left+3*pwm_step;
		else if (left<=-pwm_step)	 left=left+pwm_step;	
		else if(left<=0)			   left = 0;// On Jan 26, 2010,to make sure it will stop		
		*/	   
	}  //finish calculating right,left
	
//	DBG_PRINTF("\nBefore brake: %d,%d", right,left);fflush(stdout);
	h = abs(right);
	if ((h<=pwm_step) && (js_y>0x7a) && (js_y<0x85) && (js_x>0x7a) && (js_x<0x85))
	{
		if(right_on_off==RLMA_ON)	
			turn_off_motor(1);
	}
	else if (right_on_off == RLMA_OFF)	
		turn_on_motor(1);
	
	h = abs(left);
	if ((h<=pwm_step) && (js_y>0x7a) && (js_y<0x85) && (js_x>0x7a) && (js_x<0x85))
	{
		if(left_on_off==RLMA_ON)	
			turn_off_motor(2);
	}
	else if (left_on_off == RLMA_OFF)	
		turn_on_motor(2);
	
	if ((js_x>=0x7a) && (js_x<=0x85)) //go stright no right, no left							  
	{									// left wheel should be the same as right wheel.
		if ((left>right)&&abs(left-right)>pwm_step) left=left-pwm_step;
		if ((left<right)&&abs(left-right)>pwm_step) left=left+pwm_step;
		R=right;
		L=right;
		
		if(display ==1)
		{
			DBG_PRINTF("\nVeqeq    Strait  right=left=%d",R);fflush(stdout);
		}
//		if((right_on_off==RLMA_ON) || (left_on_off==RLMA_ON))
			cmd_move_w_vel(-R,L);
	}
	else //Js is not in middle. Rotate the Robot!
	{  // rotate left or right
		R=right;
		L=right;
		if(display ==1)
		{		
			DBG_PRINTF("\right %d,left %d ", right, left);fflush(stdout);
		}
		if(max_speed == 0)
		{			
			cmd_move_w_vel(-R,L); //never come			
		}
		
		if ( (js_x>0x90) && (max_speed>0) ) //turn right
		{
			turn=(js_x-0x80)/(3-max_speed);
			if (max_speed==1) turn=turn+8;
			
			h=L+turn;
			if (max_speed==1) { if (h>low_speed) h=low_speed; }
			else if (max_speed==2) {if (h>high_speed) h=high_speed;}
			L=h;
			
			h=R-turn;
			if (max_speed==1) { if (h<-low_speed) h=-low_speed; }
			else if (max_speed==2) { if (h<-high_speed) h=-high_speed; }
			R=h;
			
			h=R;R=L;L=h;	//swap R, L

			L1=abs(L);
			R1=abs(R);
			if ((L1>R1) && (js_y>0x7a) && (js_y<0x85)) R=-L;
			else if ((L1<R1) && (js_y>0x7a) && (js_y<0x85)) L=-R;		
			
			if (left_on_off == RLMA_OFF)	turn_on_motor(2);
			if (right_on_off == RLMA_OFF)	turn_on_motor(1);
			
			if(display ==1)
			{			
				DBG_PRINTF("RRTurn %d,%d ", -R, L);fflush(stdout);
			}
			cmd_move_w_vel(-R,L); 
			//DBG_PRINTF("\n jsX=%d, right %d,left %d \n", js_x, R, L);fflush(stdout);
		} // turn right

		if ( (js_x<0x70) && (max_speed>0) ) //turn left
		{
			turn=(0x80-js_x)/(3-max_speed);
			if (max_speed==1) turn=turn+8;

			h=L+turn;
			if (max_speed==1) { if (h>low_speed) h=low_speed; }
			else if (max_speed==2) { if (h>high_speed) h=high_speed; }
			L=h;
			
			h=R-turn;
			if (max_speed==1) { if (h<-low_speed) h=-low_speed; }
			else if (max_speed==2) { if (h<-high_speed) h=-high_speed; }
			R=h;

			L1=abs(L);			  
			R1=abs(R);
			if ((L1>R1) && (js_y>0x7a) && (js_y<0x85)) R=-L;
			else
				if ((L1<R1) && (js_y>0x7a) && (js_y<0x85)) L=-R;
			
			if (left_on_off == RLMA_OFF)	turn_on_motor(2);
			if (right_on_off == RLMA_OFF)	turn_on_motor(1);
			if(display ==1)
			{	DBG_PRINTF("LLTurn %d,%d ", -R, L); fflush(stdout);}
			cmd_move_w_vel(-R,L);
			//DBG_PRINTF("\n jsX=%d, right %d,left %d \n", js_x, R, L);fflush(stdout);
		} //turn left
	} // rotate left or right

	/********************************************翻转臂****************************************/
	flipper1 = (js_z - 0x80);
	if ( ((js_z>=0x70) && (js_z<=0x90)) ) //js is in zero position
	{
		cmd_move_flipper(0);
		if(flipper_on_off == RLMA_ON)
		{
			//flipper_off();
			flipper_on_off = RLMA_OFF;
			;
		}
		//cmd_move_flipper(0);
		//DBG_PRINTF("\n   flipper= %d ",0);fflush(stdout);	
	}
	else 
	{
 		if(flipper_on_off == RLMA_OFF)
		{
			flipper_on();
			flipper_on_off = RLMA_ON;
		}
		cmd_move_flipper(flipper1);
		//DBG_PRINTF("\n   flipper= %d ",flipper1);fflush(stdout);			
	}
#endif
}

