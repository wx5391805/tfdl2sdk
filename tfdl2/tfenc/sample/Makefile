# Cross build support
CROSS_COMPILE?=aarch64-linux-gnu-
CXX:=$(CROSS_COMPILE)g++
CC:=$(CROSS_COMPILE)gcc
AS:=$(CROSS_COMPILE)as
AR:=$(CROSS_COMPILE)ar
NM:=$(CROSS_COMPILE)nm
LD:=$(CROSS_COMPILE)ld
OBJDUMP:=$(CROSS_COMPILE)objdump
OBJCOPY:=$(CROSS_COMPILE)objcopy
RANLIB:=$(CROSS_COMPILE)ranlib
STRIP:=$(CROSS_COMPILE)strip
SIZE:=$(CROSS_COMPILE)size

CFLAGS = -I../ -lpthread -ltfenc -L../

SRC := test.c 


test: $(SRC)
	$(CC) -o $@ $(CFLAGS) $^

all: test

.PHONY:all
