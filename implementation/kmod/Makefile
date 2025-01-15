
TARGET_KERNEL_VERSION ?= `uname -r`
KERNELDIR ?= /lib/modules/$(TARGET_KERNEL_VERSION)/build
CWD := $(shell pwd)
BUILD_DIR ?= $(CWD)
SRC ?= $(CWD)
# This version information written to the kernel log when the module
# is loaded.
KMOD_VERSION ?= "0.1" #$(shell git describe --tags 2> /dev/null || echo "unknown")
EXTRA_CFLAGS := -Werror -I$(SUPERNOVA_INCLUDES) -I$(BUILD_DIR)
.PHONY: all
all: kbuild
.PHONY: kbuild
kbuild:
	$(MAKE) -C $(KERNELDIR) M=$(BUILD_DIR) src=$(SRC) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules
.PHONY: install
install:
	$(MAKE) -C $(KERNELDIR) M=$(BUILD_DIR) src=$(SRC) modules_install
.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(BUILD_DIR) src=$(SRC) clean