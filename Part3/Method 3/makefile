CPP = g++
CFLAGS = -g -Wall

all: target1 target2 target3

target1: baseline.cpp
	$(CPP) baseline.cpp -o baseline $(CFLAGS) -std=c++11 -pthread `pkg-config --cflags --libs opencv`

target2: method3.cpp
	$(CPP) method3.cpp -o method3 $(CFLAGS) -std=c++11 -pthread `pkg-config --cflags --libs opencv`
	
target3: script3.cpp
	$(CPP) script3.cpp -o script3 $(CFLAGS) -std=c++11
		
clean: 
	$(RM) -r baseline method3 script3
