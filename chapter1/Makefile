CXX = g++
CXXFLAGS = -std=c++11 -g -O0 -pthread
MD = mkdir

all: bin \
	sequential figure1 figure2 swarm atomic \
	mutex0 mutex1 mutex2 producer_consumer

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

mutex0:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

mutex1:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

mutex2:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp

producer_consumer:
	$(CXX) $(CXXFLAGS) -o bin/$@ src/$@.cpp


clean:
	rm -rf bin