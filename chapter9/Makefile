CXX = g++
CXXFLAGS = -std=c++11 -g -pthread -w
MD = mkdir
INC = -I ./src/include

TARGET = FineGrainedSet CoarseGrainedSet \
		 LazySyncSet OptimisticSet LockFreeSet

all: bin $(TARGET)

bin:
	$(MD) -p bin

$(TARGET):
	$(CXX) $(CXXFLAGS) $(INC) -DDATASTRUCTURE=$@ \
		-o bin/$@ src/ds/$@.cpp src/main.cpp

clean:
	rm -rf bin