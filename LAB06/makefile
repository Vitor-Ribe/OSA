CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

all: btree

btree: main.o btree.o
	$(CXX) $(CXXFLAGS) -o btree main.o btree.o

main.o: main.cpp btree.h
	$(CXX) $(CXXFLAGS) -c main.cpp

btree.o: btree.cpp btree.h
	$(CXX) $(CXXFLAGS) -c btree.cpp

run: btree
	./btree

clean:
	rm -f *.o btree
