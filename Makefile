obj-m += xorg_ebpf.o

all:
	make -C /lib/modules/$(KERN)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KERN)/build M=$(PWD) clean
