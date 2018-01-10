obj-m +=kernel_code.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc user_code.c -o user_code.out 
run:
	./user_code.out
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers *.out
