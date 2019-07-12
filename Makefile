.PHONY: test

named = chrdev
namef = /dev/$(named)

obj-m += $(named).o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

cli:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

rm:
		rmmod -f $(named).ko
		rm -f $(namef)

clean: rm cli

test:
		insmod $(named).ko
		mknod $(named) c 249 0
		echo rofl > $(namef)
		wc -c $(namef)
		echo kekichya > $(namef)
		wc -c $(namef)
		echo end > $(namef)
		cat $(namef)
		wc -c $(namef)