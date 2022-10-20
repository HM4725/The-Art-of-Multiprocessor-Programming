CXX = g++
CXXFLAGS = -std=c++11 -g -O0 -pthread
MD = mkdir
INC = -I src/include

TARGET = tas ttas backoff \
				 anderson anderson_pad clh mcs \
				 atomic sequential \
				 sloppy_thread_local sloppy_array_global sloppy_array_local

all: bin $(TARGET)

bin:
	$(MD) -p bin

$(TARGET):
	$(CXX) $(CXXFLAGS) $(INC) -o bin/$@ src/$@.cpp


clean:
	rm -rf bin