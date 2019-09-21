#!/bin/bash
rm -rf *.o
rm -f Server
g++ -g -lpthread -std=c++11 -c BankServer.cpp ServerSock.cpp ObserverPattern.cpp
g++ -g -o Server BankServer.o ServerSock.o ObserverPattern.o -lpthread
echo "Running program now."
./Server
