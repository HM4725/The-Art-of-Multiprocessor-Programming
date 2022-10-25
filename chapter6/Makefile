CXX = g++
CXXFLAGS = -std=c++11 -g -O0 -pthread
MD = mkdir
INC = -I ./include

TARGET = lfuniversal wfuniversal hmuniversal

all: bin $(TARGET)

bin:
	$(MD) -p bin

$(TARGET):
	$(CXX) $(CXXFLAGS) $(INC) -o bin/$@ src/$@.cpp


clean:
	rm -rf bin