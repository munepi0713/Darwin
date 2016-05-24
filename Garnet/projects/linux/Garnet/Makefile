
TOP_DIR=$(shell while [ ! -f TOP ]; do cd ..; done ; pwd)

INC_DIR=$(TOP_DIR)/include
SRC_DIR=$(TOP_DIR)/src/Garnet $(TOP_DIR)/External/dSFMT
LIB_DIR=$(TOP_DIR)/lib64
INSTALL_DIR=$(TOP_DIR)/bin64

VPATH=$(SRC_DIR) 

OBJS=main.o\
	dSFMT.o\
	ThreadPool.o\
	Engine.o\
	Affinity.o

TARGET=Garnet

IMPORTS=-L $(LIB_DIR) -lPicturePerfectEngine -lTreeSerializerEngine -lopencv_core -lopencv_highgui -lopencv_imgproc

CXXFLAGS=-Wall -O3 -g -I $(INC_DIR) -fPIC

INSTALL_DIR=$(TOP_DIR)/bin64

all: install

$(TARGET): $(OBJS)
	g++ $(CXXFLAGS) -o $(TARGET) $(OBJS) $(IMPORTS)

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f $(INSTALL_DIR)/$(TARGET)

distclean: clean

install: $(TARGET)
	cp $(TARGET) $(INSTALL_DIR)

