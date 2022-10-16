CXX = g++
CXXFLAGS = -std=c++11 -g -O0 -pthread
MD = mkdir
INC = -I src/include

TARGET = lock1 lock2 peterson lockfree sloppy \
		filter bakery

all: bin $(TARGET)

bin:
	$(MD) -p bin

$(TARGET):
	$(CXX) $(CXXFLAGS) $(INC) -o bin/$@ src/$@.cpp


clean:
	rm -rf bin