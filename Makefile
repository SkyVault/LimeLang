CC=g++
CCFLAGS=-std=c++17 -Wall -g -O0 -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter
LDFLAGS=-lrt -lpthread -lm
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)
	TARGET=a.out

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cc %.h
		$(CC) $(CCFLAGS) -c $<

%.o: %.cc 
		$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)
