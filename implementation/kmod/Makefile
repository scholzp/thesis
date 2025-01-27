
TARGET_KERNEL_VERSION ?= `uname -r`
KERNELDIR ?= /lib/modules/$(TARGET_KERNEL_VERSION)/build
CWD := $(shell pwd)
BUILD_DIR ?= $(CWD)
SRC ?= $(CWD)
# This version information written to the kernel log when the module
# is loaded.
KMOD_VERSION ?= "0.1" #$(shell git describe --tags 2> /dev/null || echo "unknown")
EXTRA_CFLAGS := -Werror -I$(BUILD_DIR) -DSTART_ADDRESS=$(START_ADDRESS) -DLOW_MEM_SIZE=$(LOW_MEM_SIZE)
.PHONY: all
all: kbuild
.PHONY: kbuild
kbuild: startup
	$(MAKE) -C $(KERNELDIR) M=$(BUILD_DIR) src=$(SRC) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules
.PHONY: install
install:
	$(MAKE) -C $(KERNELDIR) M=$(BUILD_DIR) src=$(SRC) modules_install
.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(BUILD_DIR) src=$(SRC) clean
.PHONY: startup
#generates bytecode from startup.S for insertion
startup:
	as -q -o ${SRC}/startup.o -mx86-used-note=no -c ${SRC}/startup.S
	ld -w -Ttext $(START_ADDRESS) --oformat binary -o ${SRC}/startup.bin ${SRC}/startup.o
	xxd -n STARTUP_CODE -i ${SRC}/startup.bin startup_code.h