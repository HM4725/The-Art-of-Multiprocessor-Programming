CXX = g++
CXXFLAGS = -std=c++11 -lpthread
MD = mkdir

all: bin \
	sequential figure1 figure2 swarm atomic

bin:
	$(MD) -p bin

sequential:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

figure1:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

figure2:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

swarm:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp 

atomic:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp