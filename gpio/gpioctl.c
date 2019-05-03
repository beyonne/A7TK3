#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>

#include <errno.h>
#include <config.h>
#include <gpio_manager.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

//static int fd;

//static int IOwrite(int io,int val)
//{
//	int i;
//
//	if((i=write(fd,&io,val))<0)
//	{
//		DBG_PRINTF("write err=%d\n",i);
//		return i;
//	}
//	return 0;
//}
//
//static int IOread(int io)
//{
//	int i;
//
//	if((i=read(fd,&io,0))<0)
//	{
//		DBG_PRINTF("read err1 =%d\n",i);
//		return i;
//	}
//	DBG_PRINTF("read err2 =%d\n",i);
//	return i;
//}
//
//static int IOctl(int cmd,int GPIO, int *arg)
//{
//	if(cmd)
//	{
//		GPIO = GPIO|0x80;
//	}
//
//	ioctl(fd,GPIO,&arg);
//}
//
//
//static int GPIO_init(void)
//{
//	fd = open(GPIO_DEVICE_NAME, O_RDWR);
//
//	if (fd < 0)
//	{
//		DBG_PRINTF("open device inoutgpio!\n");
//		return -1;
//	}
//	DBG_PRINTF("inoutgpio fd:%d\n",fd);
//}
//
//
//static int GPIO_exti(void)
//{
//	if( close(fd) < 0 )
//	{
//		DBG_PRINTF("close:%s fail\n",GPIO_DEVICE_NAME);
//		return -1;
//	}
//}
//
//static T_IOctlOpr g_tIOctlOpr = {
//	.name    = "IOctl",
//	.Init    = GPIO_init,
//	.Exit    = GPIO_exti,
//	.IOwrite = IOwrite,
//	.IOread  = IOread,
//};
//
//int IODevInit(void)
//{
//	return RegisterIOctlOpr(&g_tIOctlOpr);
//}

//int set_value(int gpio,unsigned char val)
//{
//	unsigned char buf[20];
//	int ret;
//
//
//	int_conver_str(val,buf);
//
//	//strcpy(buf,value);
//
//	ret = write(fd_dev[gpio], buf, strlen(buf));
//
//	if(ret < 0)
//	{
//		DBG_PRINTF("write direction:");
//		//close(fd_export);
//		close(fd_dir[gpio]);
//		close(fd_dev[gpio]);
//		return -1;
//	}
//}

static int fd_dev[196], fd_export, fd_dir[196];

unsigned char value_set_str(unsigned char *buf,unsigned char *value)
{
	strcpy(buf,"/sys/class/gpio/gpio");
	strcat(buf,value);
	strcat(buf,"/value");
	DBG_PRINTF("%s\n",buf);
	return 0;
}

unsigned char dir_set_str(unsigned char *buf,unsigned char *dir)
{
	strcpy(buf,"/sys/class/gpio/gpio");
	strcat(buf,dir);
	strcat(buf,"/direction");
	DBG_PRINTF("%s\n",buf);
	return 0;
}


int int_conver_str(int gpio,unsigned char *strtemp)
{
	unsigned char *p3="%c%c%c";
	unsigned char *p2="%c%c";
	unsigned char *p1="%c";
	//char strtemp[50];

	unsigned char temp1,temp2,temp3;

	//DBG_PRINTF("%d\n",gpio);

	if(gpio>99)
	{
		temp1 = gpio/100;
		temp1 += 0x30;

		temp2 = gpio/10;
		temp2 = temp2%10;
		temp2 += 0x30;

		temp3 = gpio%10;
		temp3 += 0x30;

		sprintf(strtemp,p3,temp1,temp2,temp3);


	}
	else if((gpio<100)&&(gpio>1))
	{
		temp2 = gpio/10;
		temp2 = temp2%10;
		temp2 += 0x30;

		temp3 = gpio%10;
		temp3 += 0x30;

		sprintf(strtemp,p2,temp2,temp3);

	}
	else if(gpio<2)
	{
		temp3 = gpio%10;
		temp3 += 0x30;

		sprintf(strtemp,p1,temp3);

	}

	//DBG_PRINTF("%s\n",strtemp);

}

int open_set_value_file(int gpio)
{
	unsigned char value_buf[50];
	unsigned char gpio_buf[50];

	int_conver_str(gpio,gpio_buf);

	write(fd_export, gpio_buf, strlen(gpio_buf));

	value_set_str(value_buf,gpio_buf);

	fd_dev[gpio] = open(value_buf, O_RDWR);

	if(fd_dev[gpio] < 0)
	{
		DBG_PRINTF("open gpio:");
		return -1;
	}
}

int open_set_dir_file(int gpio)
{
	unsigned char direction_buf[50];
	unsigned char gpio_buf[50];

	int_conver_str(gpio,gpio_buf);

	dir_set_str(direction_buf,gpio_buf);

	fd_dir[gpio] = open(direction_buf, O_RDWR);

	if(fd_dir[gpio] < 0)
	{
		DBG_PRINTF("open direction:");
		return -1;
	}
}

int set_dir(unsigned char *value,int gpio)
{
	unsigned char buf[20];
	int ret;

	strcpy(buf,value);

	ret = write(fd_dir[gpio], buf, strlen(value));

	if(ret < 0)
	{
		DBG_PRINTF("write direction:");
		//close(fd_export);
		close(fd_dir[gpio]);
		close(fd_dev[gpio]);
		return -1;
	}
}

int set_value(int gpio,unsigned char val)
{
	unsigned char buf[20];
	int ret;

	int_conver_str(val,buf);

	//strcpy(buf,value);

	ret = write(fd_dev[gpio], buf, strlen(buf));

	if(ret < 0)
	{
		DBG_PRINTF("write direction:");
		//close(fd_export);
		close(fd_dir[gpio]);
		close(fd_dev[gpio]);
		return -1;
	}
}

int read_value(int gpio)
{
	unsigned char buf[20],ret;

	//int_conver_str(val,buf);


	ret = read(fd_dev[gpio],buf, sizeof(buf));

	if(ret < 0)
	{
		DBG_PRINTF("read gpio:");
		//close(fd_export);
		close(fd_dir);
		close(fd_dev);
		return -1;
	}

	return (buf[0]-0x30);

}


/*
#define	    GPIO_PTT			83
#define	    GPIO_TEST			81
#define	    GPIO_DR1			77
#define	    GPIO_DR2			71

*/
static int GPIO_init(void)
{
	unsigned char value_buf[50], direction_buf[50],buf[20];

	fd_export = open(EXPORT_PATH, O_WRONLY);

	if(fd_export < 0)
	{
		DBG_PRINTF("open export:");
		return -1;
	}

//	open_set_value_file(GPIO_PTT);
//	open_set_dir_file(GPIO_PTT);
//	set_dir(OUT,GPIO_PTT);
//
//	open_set_value_file(GPIO_TEST);
//	open_set_dir_file(GPIO_TEST);
//	set_dir(OUT,GPIO_TEST);
//
//	open_set_value_file(GPIO_DR1);
//	open_set_dir_file(GPIO_DR1);
//	set_dir(OUT,GPIO_DR1);
//
//	open_set_value_file(GPIO_DR2);
//	open_set_dir_file(GPIO_DR2);
//	set_dir(OUT,GPIO_DR2);
//
//	open_set_value_file(GPIO_LGTOIR1);
//	open_set_dir_file(GPIO_LGTOIR1);
//	set_dir(OUT,GPIO_LGTOIR1);
//
//	open_set_value_file(GPIO_LGT_LED1);
//	open_set_dir_file(GPIO_LGT_LED1);
//	set_dir(OUT,GPIO_LGT_LED1);
//
//	open_set_value_file(GPIO_LGTOIR2);
//	open_set_dir_file(GPIO_LGTOIR2);
//	set_dir(OUT,GPIO_LGTOIR2);
//
//	open_set_value_file(GPIO_LGT_LED2);
//	open_set_dir_file(GPIO_LGT_LED2);
//	set_dir(OUT,GPIO_LGT_LED2);
//
//	open_set_value_file(GPIO_QUAD_CH4);
//	open_set_dir_file(GPIO_QUAD_CH4);
//	set_dir(OUT,GPIO_QUAD_CH4);
//
//	open_set_value_file(GPIO_QUAD_CH3);
//	open_set_dir_file(GPIO_QUAD_CH3);
//	set_dir(OUT,GPIO_QUAD_CH3);
//
//	open_set_value_file(GPIO_QUAD_CH2);
//	open_set_dir_file(GPIO_QUAD_CH2);
//	set_dir(OUT,GPIO_QUAD_CH2);
//
//	open_set_value_file(GPIO_QUAD_CH1);
//	open_set_dir_file(GPIO_QUAD_CH1);
//	set_dir(OUT,GPIO_QUAD_CH1);
//
//	open_set_value_file(GPIO_QUAD_MENU);
//	open_set_dir_file(GPIO_QUAD_MENU);
//	set_dir(OUT,GPIO_QUAD_MENU);

	open_set_value_file(RS485_DE_SENSER);
	open_set_dir_file(RS485_DE_SENSER);
	set_dir(OUT,RS485_DE_SENSER);

	//while(1)
	{
		DBG_PRINTF("write direction:\n");

//		set_value(GPIO_PTT,1);
//		set_value(GPIO_TEST,1);
//		set_value(GPIO_DR1,1);
//		set_value(GPIO_DR2,1);
//		set_value(GPIO_LGTOIR1,1);
//		set_value(GPIO_LGT_LED1,1);
//		set_value(GPIO_LGTOIR2,1);
//		set_value(GPIO_LGT_LED2,1);
//		set_value(GPIO_QUAD_CH4,1);
//		set_value(GPIO_QUAD_CH3,1);
//		set_value(GPIO_QUAD_CH2,1);
//		set_value(GPIO_QUAD_CH1,1);
//		set_value(GPIO_QUAD_MENU,1);
//
//		set_value(GPIO_LGTOIR1,0);
//		set_value(GPIO_LGT_LED1,0);
//		set_value(GPIO_LGTOIR2,0);
//		set_value(GPIO_LGT_LED2,0);
//		usleep(1000000);
//		set_value(GPIO_LGTOIR1,1);
//		set_value(GPIO_LGT_LED1,1);
//		set_value(GPIO_LGTOIR2,1);
//		set_value(GPIO_LGT_LED2,1);
		set_value(RS485_DE_SENSER,0);
	}
}



static int IOwrite(int io,int val)
{
	int i;

	if(val>1)
		return -1;

	set_value(io,val);

	return 0;
}

static int IOread(int io)
{

	return read_value(io);
}


static int GPIO_exti(void)
{

}

static T_IOctlOpr g_tIOctlOpr = {
	.name    = "IOctl",
	.Init    = GPIO_init,
	.Exit    = GPIO_exti,
	.IOwrite = IOwrite,
	.IOread  = IOread,
};

int IODevInit(void)
{
	return RegisterIOctlOpr(&g_tIOctlOpr);
}


