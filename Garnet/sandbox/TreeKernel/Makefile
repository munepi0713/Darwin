TARGET=TreeKernel
OBJS=$(TARGET).o Tree.o
INCLUDE=-I ../../include
CXXFLAGS=-Wall -O3 -g $(INCLUDE) -std=c++0x
VPATH=../../src/Garnet

all: $(TARGET)
	

$(TARGET): $(OBJS)
	g++ -Wall -O3 -g -o $(TARGET) $(OBJS)

