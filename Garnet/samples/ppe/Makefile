PPE_TARGET=libPicturePerfectEngine.a
PPE_OBJS=\
	PicturePerfectEngine.o\
	PicturePerfectOpenCV.o

PP_TARGET=PicturePerfect
PP_OBJS=main.o
PP_LIBS=-lopencv_core -lopencv_highgui -lopencv_imgproc

TARGETS=$(PPE_TARGET) $(PP_TARGET)

VPATH=src
CXXFLAGS=-Wall -O3 -g -I ./include -I ../Common/include -std=c++0x

INSTALL_DIR=../bin

all: $(TARGETS)

$(PPE_TARGET): $(PPE_OBJS)
	$(AR) rcv $(LD_FLAGS) -o $(PPE_TARGET) $(PPE_OBJS)

$(PP_TARGET): $(PP_OBJS)
	g++ $(LD_FLAGS) -o $(PP_TARGET) $(PP_OBJS) $(PPE_TARGET) $(PP_LIBS)

clean:
	rm -f $(TARGETS)
	rm -f $(PPE_OBJS)
	rm -f $(PP_OBJS)
	rm -f $(INSTALL_DIR)/$(PP_TARGET)

distclean: clean

install: all
	cp $(PP_TARGET) $(INSTALL_DIR)


