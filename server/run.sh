#!/bin/bash
rm -rf *.o
rm -f Server
g++ -g -lpthread -std=c++11 -c BankServer.cpp ServerSock.cpp ObserverPattern.cpp ServerGUI.cpp `pkg-config gtkmm-3.0 --cflags --libs`
g++ -g -o Server BankServer.o ServerSock.o ObserverPattern.o ServerGUI.o -lpthread  `pkg-config gtkmm-3.0 --cflags --libs`
echo "Running program now."
./Server
