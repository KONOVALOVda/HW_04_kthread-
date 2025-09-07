KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

obj-m := reader_writer_kern.o

all: modules app_us

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

app_us:
	gcc -pthread -O2 reader_writer_us.c -o reader_writer_us

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f reader_writer_us perf_app_us.log flg_kern.html

load:
	sudo insmod reader_writer_kern.ko

unload:
	sudo rmmod reader_writer_kern

format:
	clang-format -i *.c

check:
	@for f in *.c; do \
		./scripts/checkpatch.pl --no-tree --file $$f || true; \
	done
