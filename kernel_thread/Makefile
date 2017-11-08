#obj-m:=threads.o

#all:
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
#ERNELDIR ?= /lib/modules/$(shell uname -r)/build

#WD := $(shell pwd)

#default:
 #  $(MAKE) -C $(KERNELDIR) M=$(PWD) modules 

EXTRA_CFLAGS:=-I/usr/local/include -I/usr/include -I/usr/include/x86_64-linux-gnu

obj-m:=mymodule.o
mymodule-objs:=threads.o papi.o libpapi.a

all:
   # make -C $(KERNEL_PATH) M=$(PWD) modules

	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
    make -C $(KERNEL_PATH) M=$(PWD) clean



