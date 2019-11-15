/*
 * MutualExclusion.h
 *
 *  Created on: Nov 14, 2019
 *      Author: lilbase
 */

#ifndef MUTUALEXCLUSION_H_
#define MUTUALEXCLUSION_H_

#include <stdio.h>
#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

#include "../include/spdlog/spdlog.h"
#include "../include/spdlog/sinks/stdout_color_sinks.h"
#include "../include/spdlog/sinks/rotating_file_sink.h"
#include "util/utils.cpp"
using namespace std;

class MutualExclusion {
public:
	MutualExclusion(int id, int n);
	void init();
	pthread_t getSenderThread(){
		return senderThread;
	}

	pthread_t getRecvThread(){
		return recvThread;
	}

	static void *senderServiceHelper(void *context){
		return ((MutualExclusion *)context)->handleSenderService();
	}


	static void *recvServiceHelper(void *context){
		return ((MutualExclusion *)context)->handleRecvService();
	}
	void createSendAndRecvThread();
private:
	int port;
	int multicastSock;
	int pointToPointSock;
	int id;
	std::shared_ptr<spdlog::logger> _logger;
	struct sockaddr_in srv_addr;
	struct sockaddr_in rcv_addr;
	struct sockaddr_in p2p_addr;
	void receiveServerTime();
	void printDate(timeval time);
	struct timeval time;
	void calculateAndSendDrift();
	void sendCordinatorPortForP2P();
	int getOwnPort();
	int ownPort;
	void calcualteTheAverageTime();
	void receiveUpdatedClockFromCoo();
	vector<string> v;
	pthread_t senderThread;
	pthread_t recvThread;
	void * handleSenderService();
	void * handleRecvService();
	bool isDoneUpdatingFile;
	int numOfProcesses;
	bool hasSentRequest;
	long int sentTimestamp;
	bool isUpdatingFile;
};



#endif /* MUTUALEXCLUSION_H_ */
