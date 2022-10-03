CXX = g++
CXXFLAGS = -std=c++11 -g -O0 -pthread
MD = mkdir

all: bin \
	lock1 lock2 peterson lockfree sloppy \
	filter

bin:
	$(MD) -p bin

lock1:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

lock2:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

peterson:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

lockfree:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

sloppy:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

filter:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp


clean:
	rm -rf bin