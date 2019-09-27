#######################
# Makefile
#######################

# source object target
SOURCE := main.cpp 
TARGET := main.out
OBJS := main.o CEM133Collector.o CtcpClient.o

# compile and lib parameter
CC      := g++
#CC      := /home/huangzk/Downloads/CrossCompile_for_RPI/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-g++ #Cross Compile for rpi
LDFLAGS := -L/usr/local/lib
#LDFLAGS := -L/home/huangzk/Downloads/CrossCompile_for_RPI/libmodbus/lib #lib for rpi
LIBS    := -lmodbus -pthread
DEFINES :=
INCLUDE := -I/usr/local/include/modbus
CFLAGS  := 
CXXFLAGS:= 

# link
$(TARGET):$(OBJS)
	@$(CC) -std=c++11 -g $(CFLAGS) $^ $(LDFLAGS) $(LIBS)  -o $@

# compile
main.o:main.cpp CEM133Collector.o
	@$(CC) -std=c++11 -c -g $(CFLAGS) main.cpp -o main.o
CEM133Collector.o:CEM133Collector.h CEM133Collector.cpp
	@$(CC) -std=c++11 -c -g $(CFLAGS) CEM133Collector.cpp -o CEM133Collector.o
CtcpClient.o:CtcpClient.h CtcpClient.cpp
	@$(CC) -std=c++11 -c -g $(CFLAGS) CtcpClient.cpp -o CtcpClient.o

.PHONY:clean
clean:
	@rm main.out $(OBJS) 

