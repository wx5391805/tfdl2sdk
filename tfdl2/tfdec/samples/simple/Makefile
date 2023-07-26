TFLIB_NAME := libtfdec.so
TFLIB_PATH := ../../

CROSS_COMPILE ?= /opt/linaro/gcc-linux-7.2.1/bin/aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
ARCH=arm64

CFLAG += -D UNIT -I $(TFLIB_PATH)
LFLAG = -A $(ARCH) -L$(TFLIB_PATH)


TARGET  := simple
OBJECTS := $(TARGET).o
SRC     := $(TARGET).c

$(TARGET) : $(OBJECTS)
	$(CC) $(LFLAG) -o $@ $^ -ltfdec -lpthread

%.o : %.c
	$(CC) $(CFLAG) -c $^ 

all : clean $(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: clean
