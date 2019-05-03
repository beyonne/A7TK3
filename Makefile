CROSS_COMPILE = arm-linux-gnueabihf-
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump


export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

#CFLAGS := -Wall -Werror -O2 -g
CFLAGS := -Wall -O2 -g
CFLAGS += -I $(shell pwd)/include

LDFLAGS := -I/opt/install/include -L/opt/install/lib -lsqlite3 -lrt -lpthread 

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR

TARGET := mainapp


obj-y += main.o
obj-y += usart/
#obj-y += input/
#obj-y += output/
#obj-y += weigen/
#obj-y += gpio/
obj-y += debug/
obj-y += net/
obj-y += store/
obj-y += timer/
obj-y += communication/
obj-y += can/
obj-y += alloc/


all : 
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC) $(LDFLAGS) -o $(TARGET) built-in.o


clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	
