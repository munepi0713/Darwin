
TARGET=bgltest.exe
OBJS=main.obj Tree.obj TraceLevel.obj

INCLUDE=$(INCLUDE);../../include
GARNETSRCDIR=../../src/Garnet

CPPFLAGS=/EHsc
CXXFLAGS=/EHsc

VPATH=../../src/Garnet

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) /EHsc /Fe$(TARGET) $(OBJS)

Tree.obj: $(GARNETSRCDIR)/Tree.cpp
	$(CXX) $(CXXFLAGS) /c $?

TraceLevel.obj: $(GARNETSRCDIR)/TraceLevel.cpp
	$(CXX) $(CXXFLAGS) /c $?

clean:
	del $(TARGET)
	del $(OBJS)
