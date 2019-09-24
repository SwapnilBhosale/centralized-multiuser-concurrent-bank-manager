#!/bin/bash
rm -rf *.o client
g++ -std=c++11 -c client.cpp -lpthread
g++ -o client client.o -lpthread

./client

