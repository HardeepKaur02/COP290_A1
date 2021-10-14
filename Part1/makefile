CPP = g++
CFLAGS = -g -Wall

all: main.cpp
	$(CPP) main.cpp -o cam $(CFLAGS) -std=c++11 `pkg-config --cflags --libs opencv`
clean: 
	$(RM) -r cam
