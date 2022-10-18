CXX = g++
CXXFLAGS = -std=c++11 -g -O0 -pthread
MD = mkdir
INC = -I src/include

TARGET = flag queue \
			multi_assign \
			flag_atomic

all: bin $(TARGET)

bin:
	$(MD) -p bin

$(TARGET):
	$(CXX) $(CXXFLAGS) $(INC) -o bin/$@ src/$@.cpp


clean:
	rm -rf bin