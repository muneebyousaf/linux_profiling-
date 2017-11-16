EXTRA_CFLAGS:= -I/usr/include/x86_64-linux-gnu -I/usr/include

obj-m:=threads.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
#ERNELDIR ?= /lib/modules/$(shell uname -r)/build

#WD := $(shell pwd)

default:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules 


