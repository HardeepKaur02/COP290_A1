CPP = g++
CFLAGS = -g -Wall

all: task2.cpp
	$(CPP) task2.cpp -o cam $(CFLAGS) -std=c++11 `pkg-config --cflags --libs opencv`
clean: 
	$(RM) -r cam
