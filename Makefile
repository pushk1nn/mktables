obj-m += mktables.o

all:
	make -C /lib/modules/$(KERN)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KERN)/build M=$(PWD) clean
