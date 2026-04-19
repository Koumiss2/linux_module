ifneq ($(KERNELRELEASE),)

ccflags-y := -I$(src)/inc

obj-m := ping_mod.o
ping_mod-y := src/main.o src/icmp.o src/proc_ip.o

else

SDK_PATH = $(HOME)/openwrt-sdk-23.05.0-x86-64_gcc-12.3.0_musl.Linux-x86_64
KERNEL_DIR = $(shell find $(SDK_PATH)/build_dir -name "linux-5.15*" -type d | head -n 1)
TOOLCHAIN_BIN = $(shell find $(SDK_PATH)/staging_dir -name "bin" -type d | grep toolchain | head -n 1)
CROSS_COMPILE = $(TOOLCHAIN_BIN)/x86_64-openwrt-linux-

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=x86_64 CROSS_COMPILE=$(CROSS_COMPILE) modules
	mkdir -p bin
	cp ping_mod.ko bin/

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=x86_64 CROSS_COMPILE=$(CROSS_COMPILE) clean
	rm -rf bin/

endif