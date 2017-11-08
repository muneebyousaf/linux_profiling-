obj-m:=threads.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
#ERNELDIR ?= /lib/modules/$(shell uname -r)/build

#WD := $(shell pwd)

default:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules 


