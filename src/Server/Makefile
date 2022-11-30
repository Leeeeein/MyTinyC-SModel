CC = /usr/bin/gcc-9
TARGET = TinyServer
ROOTPATH = /home/liyinzhe/Workspace/MyTinyC-SModel/
LIB = -lpthread

OBJS += $(patsubst %.c, %.o, $(wildcard *.c))
OBJS += $(patsubst %.c, %.o, $(wildcard /home/liyinzhe/Workspace/MyTinyC-SModel/components/source/sysapi/*.c))
OBJS += $(patsubst %.c, %.o, $(wildcard /home/liyinzhe/Workspace/MyTinyC-SModel/components/source/datastructs/*.c))
OBJS += $(patsubst %.c, %.o, $(wildcard /home/liyinzhe/Workspace/MyTinyC-SModel/components/source/utils/*.c))

CFLAGS = -O0 -g3 -Wall
INCLUDE = -I${ROOTPATH}/components/header/datastructs -I${ROOTPATH}/components/header/sysapi -I${ROOTPATH}/components/header/utils
$(TARGET):$(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIB)

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@ $(INCLUDE)

.PHONY:clean
clean:
	rm *.o TinyServer
