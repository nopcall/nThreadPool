CC = gcc
CXXFLAGS = -std=c++11 -lstdc++ -Wall -lpthread -g

ALL: test

test: test.cc ThreadPool.cc ThreadPool.H
	$(CC) $(CXXFLAGS) -o test test.cc ThreadPool.cc ThreadPool.H
