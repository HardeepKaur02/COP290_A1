CPP = g++
CFLAGS = -g -Wall

all: target1 target2 target3

target1: baseline.cpp
	$(CPP) baseline.cpp -o baseline $(CFLAGS) -std=c++11 `pkg-config --cflags --libs opencv`

target2: method1.cpp
	$(CPP) method1.cpp -o method1 $(CFLAGS) -std=c++11 `pkg-config --cflags --libs opencv`
	
target3: script1.cpp
	$(CPP) script1.cpp -o script1 $(CFLAGS) -std=c++11
		
clean: 
	$(RM) -r baseline method1 script1
