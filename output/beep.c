#include <config.h>
#include <gpio_manager.h>
#include <string.h>
#include <unistd.h>

#define BEEP_GPIO  1
#define BEEP_BIT 	21

#define BEEP1_GPIO  1
#define BEEP1_BIT 	21

#define BEEP2_GPIO  1
#define BEEP2_BIT 	21

#define BEEP3_GPIO  1
#define BEEP3_BIT 	21

#define BEEP4_GPIO  1
#define BEEP4_BIT 	21


void Beep_ON(void)
{
	IOCwrite(GetIO(BEEP_GPIO,BEEP_BIT),0);
}

void Beep_OFF(void)
{
	IOCwrite(GetIO(BEEP_GPIO,BEEP_BIT),1);
}

int BeepOut_ON(int bit)
{
	switch(bit)
	{
		case 1:
		{
			IOCwrite(GetIO(BEEP1_GPIO,BEEP1_BIT),0);
			break;
		}
		case 2:
		{
			IOCwrite(GetIO(BEEP2_GPIO,BEEP2_BIT),0);
			break;
		}
		case 3:
		{
			IOCwrite(GetIO(BEEP3_GPIO,BEEP3_BIT),0);
			break;
		}
		case 4:
		{
			IOCwrite(GetIO(BEEP4_GPIO,BEEP4_BIT),0);
			break;
		}
		default:
		{
			return -1;
			break;
		}
	}
	return 0;
}

int BeepOut_OFF(int bit)
{
	switch(bit)
	{
		case 1:
		{
			IOCwrite(GetIO(BEEP1_GPIO,BEEP1_BIT),1);
			break;
		}
		case 2:
		{
			IOCwrite(GetIO(BEEP2_GPIO,BEEP2_BIT),1);
			break;
		}
		case 3:
		{
			IOCwrite(GetIO(BEEP3_GPIO,BEEP3_BIT),1);
			break;
		}
		case 4:
		{
			IOCwrite(GetIO(BEEP4_GPIO,BEEP4_BIT),1);
			break;
		}
		default:
		{
			return -1;
			break;
		}
	}
	return 0 ;
}

