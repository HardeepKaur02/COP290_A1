CPP = g++
CFLAGS = -g -Wall

all: target1 target2 target3

target1: baseline.cpp
	$(CPP) baseline.cpp -o baseline $(CFLAGS) -std=c++11 `pkg-config --cflags --libs opencv`

target2: method2.cpp
	$(CPP) method2.cpp -o method2 $(CFLAGS) -std=c++11 `pkg-config --cflags --libs opencv`
	
target3: script2.cpp
	$(CPP) script2.cpp -o script2 $(CFLAGS) -std=c++11
		
clean: 
	$(RM) -r baseline method2 script2
