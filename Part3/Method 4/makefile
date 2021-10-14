CPP = g++
CFLAGS = -g -Wall

all: target1 target2 target3

target1: baseline.cpp
	$(CPP) baseline.cpp -o baseline $(CFLAGS) -std=c++11 -pthread `pkg-config --cflags --libs opencv`

target2: method4.cpp
	$(CPP) method4.cpp -o method4 $(CFLAGS) -std=c++11 -pthread `pkg-config --cflags --libs opencv`
	
target3: script4.cpp
	$(CPP) script4.cpp -o script4 $(CFLAGS) -std=c++11
		
clean: 
	$(RM) -r baseline method4 script4
