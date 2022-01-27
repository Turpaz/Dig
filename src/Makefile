CPP :=g++
TARGET :=../bin/dig.exe
LDFLAGS :=
SOURCES := $(shell find . -name "*.cpp")
SOURCES += $(shell find . -name "*.hpp")

TEST_CODE :=../test/test.dg
TEST_TARGET :=../test/test.exe

all:
	$(CPP) $(LDFLAGS) $(CFLAGS) -o $(TARGET) $(SOURCES)

test:
	$(TARGET) $(TEST_CODE) -o $(TEST_TARGET)
#	$(TEST_TARGET)

clean:
	del $(TARGET)