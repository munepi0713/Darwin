
TOP_DIR=$(shell while [ ! -f TOP ]; do cd ..; done ; pwd)

INC_DIR=$(TOP_DIR)/include
SRC_DIR=$(TOP_DIR)/src/PicturePerfectEngine
INSTALL_DIR=$(TOP_DIR)/lib64

VPATH=$(SRC_DIR) $(TOP_DIR)/External/maxflow-v3.01

OBJS=main.o\
	PicturePerfectEngine.o\
	PicturePerfectOpenCV.o\
	graph.o\
	maxflow.o

TARGET=libPicturePerfectEngine.so

CXXFLAGS=-Wall -O3 -g -I $(INC_DIR) -fPIC

all: install

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) --shared -o $(TARGET) $(OBJS)

install: $(TARGET)
	mkdir -p $(INSTALL_DIR)
	cp $(TARGET) $(INSTALL_DIR)/

.PHONY: clean
clean:
	rm -rf $(TARGET)
	rm -rf $(OBJS)
	rm -rf $(INSTALL_DIR)/$(TARGET)

