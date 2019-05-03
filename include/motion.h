#ifndef __MOTION_H
#define __MOTION_H


extern unsigned char right_on_off;  // right motor on off indicator
extern unsigned char left_on_off ;  // left motor on off indicator
extern unsigned char flipper_on_off;  // flipper motor on off indicator


#define RLMA_OFF 0
#define RLMA_ON 0xff

#define low_speed 140
#define high_speed 200
#define pwm_step 3
#define flp_pwm_step 2



#define CMD_READ	1
#define CMD_WRITE	2
	
#define SETSPEED 			0x4500
#define SETWRITE 			0x0700
#define BRIDGE 				0x0100
#define READ_SPEED			0x1102
#define READ_ANGLE			0x1A00

#define READ_ERROR			0x0200


#define USERSET_READ_NONE	0
#define USERSET_READ_SPEED	1
#define USERSET_READ_ANGLE	2


#define RL_K 		2

#define ZW_K		1

//#define SPEED_K_VAL			10000
//#define SPEED_K_VAL			5000
#define SPEED_K_VAL			12000
#define SPEED_K_VAL_F		8000

#define MIN_motion_scalar	0
#define MAX_motion_scalar	125
#define offset  		0x10
#define zerospeed		0x80

//can�豸id�ŷ���
#define TRACK_L 		1
#define TRACK_R 		2
#define FLIPPER 		3

#define J1_ID			9
#define J2_ID			4
#define J3_ID			5
#define J4_ID			8
#define J5_ID			6
#define J6_ID			7

#define GRI_CAM_ID			10
#define PTZ_SSG_ID			11		
#define GRI_SSG_ID			12
#define PTZ_ID				0x20
#define ANGLE_J1_ID			0x21
#define ANGLE_J2_ID			0x22
#define ANGLE_J3_ID			0x23
#define ANGLE_J4_ID			0x24

#define WATER_CANNON_ID				0x33


//can�豸�������
#define GRI_CAM_CMD				0x000B
#define PTZ_READ_CMD			0X0005
#define ANGLE_READ_CMD			0X0220
#define ANGLE_SET_ZERO_CMD		0X0120
#define SSG_CMD					0X0009


void motion_get_speed(int id);
void motion_get_angle(int id);
int disable_bridge(int id);
void Platform_Stop(void);



// define joystick information
const static unsigned char LUT1[] = 
{25,25,26,26,27,27,28,28,30,30,32,33,34,35,37,39,41,43,45,47,49,
50,52,53,53,53,54,54,54,55,55,55,56,56,56,57,57,57,58,58,58,
59,59,60,60,61,61,61,62,62,62,63,63,63,64,64,64,65,66,66,68,
68,70,70,71,72,72,73,74,74,75,76,76,77,78,78,79,80,80,81,82,
82,83,84,84,85,86,86,88,88,90,90,92,92,94,94,96,96,98,98,100,
100,102,102,104,106,107,108,109,110,110,111,112,113,114,115,116,117,118,119,120,
125,130,135,140,145,150,160,170,0x80,0x80
};

const static unsigned char LUT2[] = 
{9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,19,20,
20,21,21,22,23,23,24,25,25,26,27,27,28,29,29,30,31,31,32,33,
33,34,34,35,36,36,37,38,38,39,40,40,40,42,42,42,44,44,44,46,
50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,68,70,72,
74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112,
100,102,102,104,106,107,108,109,110,110,111,112,113,114,115,116,117,118,119,120,
114,118,118,118,120,122,124,126,0x80,0x80
};


extern void get_js_input(unsigned char *pcoredata);

#endif

