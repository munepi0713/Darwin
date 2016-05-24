
TOP_DIR=$(shell while [ ! -f TOP ]; do cd ..; done ; pwd)

INC_DIR=$(TOP_DIR)/include
SRC_DIR=$(TOP_DIR)/src/TreeSerializerEngine
INSTALL_DIR=$(TOP_DIR)/lib64

VPATH=$(SRC_DIR)

OBJS=\
	InstructionSet.o\
	TreeSerializerImpl.o\
	TreeSerializer.o

TARGET=libTreeSerializerEngine.so

CXXFLAGS=-Wall -O3 -g -I $(INC_DIR) -fPIC

all: install

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) --shared -o $(TARGET) $(OBJS)

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f $(INSTALL_DIR)/$(TARGET)

distclean: clean

install: $(TARGET)
	mkdir -p $(INSTALL_DIR)
	cp $(TARGET) $(INSTALL_DIR)

