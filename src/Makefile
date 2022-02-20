CPP :=g++
GDB :=gdb
TARGET :=../bin/dig.exe
LDFLAGS :=
SOURCES := $(shell find . -name "*.cpp")
SOURCES += $(shell find . -name "*.hpp")

TEST_CODE :=../test/test.dg
TEST_TARGET :=../test/test.exe
BASIC_CODE=../test/basic.dg
BASIC_TARGET=../test/basic.asm

all:
	$(CPP) $(LDFLAGS) $(CFLAGS) -o $(TARGET) $(SOURCES)

test:
	$(TARGET) $(TEST_CODE) -o $(TEST_TARGET)
#	$(TEST_TARGET)
basic:
	$(TARGET) $(BASIC_CODE) -o $(BASIC_TARGET)
#	$(BASIC_TARGET)

g:
	$(CPP) $(LDFLAGS) $(CFLAGS) -g -o $(TARGET) $(SOURCES)
gdb:
	$(GDB) $(TARGET)

clean:
	del $(TARGET)