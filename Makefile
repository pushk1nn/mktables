obj-m += net_helper.o

all:
	make -C /lib/modules/$(KERN)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KERN)/build M=$(PWD) clean
