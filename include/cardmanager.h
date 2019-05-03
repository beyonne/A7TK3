#ifndef	__CARDMANAGER_H
#define	__CARDMANAGER_H



/*刷卡失败原因*/
#define SUCCESSED					0x01		//合法卡
#define WQUANXIANERROR				0x02		//无权限;
#define GUASHICARD					0x03		//挂失卡;
#define ZHUXIAOCARD					0x04		//未注册/注销卡;
#define GUOQICARD					0x05 		//过期卡;
#define PASSWORDERROR				0x06		//密码错误;
#define JQUANXIANERROR				0x07		//假日组无权限;
#define TQUANXIANERROR				0x08		//时间组无权限;
#define DOORCLOSE					0x09		//门为常闭禁止开门状态;
#define FANQIANHUI					0x0a		//反潜回;
#define DUOKAOPEN					0x0b		//多卡开门;
#define DUOMENHUSUO					0x0c		//多门互锁;
#define SHOUKAOPEN					0x0d		//首卡开门；
#define INVALID						0x0e		//已退卡；


/*权限验证位定义*/
#define AUTHGUASHI	(1<<6)					//是否为挂失卡;// 01000000 0x40;
#define MODEAUTH	(1<<1)					//模版是否过期标志位；
#define WEEK 		(1<<(Rtc->Week - 1))		//没有星期0，所以减去1；
#define MONTH 		(1<<(Rtc->Days - 1))	//没有0日，所以减去1；
#define SFTC		(1<<0)					//是否退卡；
#define DOORCO		(1<<(ID-1))				//门禁止开或者打开标志位；
#define SFSC		(1<<2)					//是否首卡开门；
#define SFDC		(1<<3)					//是否多卡开门；
#define AUTHBIT		(1<<(ID-1))				//门禁止开或者打开标志位；


int DorpTable(char *name);

int CreateTable(char *name);
			
int CreateTableInx(char *name,char *namex,char *inxname);
int DeleteCard(char *data);
int Charge_Authority(int Serial_Number,unsigned char Key[],unsigned char ID);




#endif
