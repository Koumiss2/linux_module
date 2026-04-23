ifneq ($(KERNELRELEASE),)

ccflags-y += -I$(M)/inc

obj-m := ping_mod.o
ping_mod-y := src/main.o src/netif.o src/proc_ip.o

else

SDK_PATH = $(HOME)/openwrt-sdk-23.05.0-x86-64_gcc-12.3.0_musl.Linux-x86_64
export STAGING_DIR = $(SDK_PATH)/staging_dir
KERNEL_DIR = $(shell find $(SDK_PATH)/build_dir -name "linux-5.15*" -type d | head -n 1)
TOOLCHAIN_BIN = $(shell find $(STAGING_DIR) -name "bin" -type d | grep toolchain | head -n 1)
CROSS_COMPILE = $(TOOLCHAIN_BIN)/x86_64-openwrt-linux-musl-

PWD := $(shell pwd)
BUILD_DIR := $(PWD)/bin/tmp
KO := ping_mod.ko

all:
	mkdir -p bin
	rm -rf $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/src $(BUILD_DIR)/inc
	cp Makefile $(BUILD_DIR)/
	cp src/*.c $(BUILD_DIR)/src/
	cp inc/*.h $(BUILD_DIR)/inc/
	$(MAKE) -C $(KERNEL_DIR) M=$(BUILD_DIR) ARCH=x86_64 CROSS_COMPILE=$(CROSS_COMPILE) modules
	cp $(BUILD_DIR)/$(KO) bin/

clean:
	rm -rf bin/

endif
