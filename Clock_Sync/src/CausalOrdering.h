/*
 * CausualOrdering.cpp
 *
 *  Created on: Nov 12, 2019
 *      Author: lilbase
 */

#ifndef CAUSUALORDERING_CPP_
#define CAUSUALORDERING_CPP_


#include <stdio.h>
#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/time.h>
#include "Process.h"

#include "../include/spdlog/spdlog.h"
#include "../include/spdlog/sinks/stdout_color_sinks.h"
#include "../include/spdlog/sinks/rotating_file_sink.h"
#include "util/utils.cpp"
using namespace std;

class CausalOrdering {
public:
	CausalOrdering(int port, int id, char *msg);
	void init();
	void createSendAndRecvThread();
	pthread_t getSenderThread(){
		return senderThread;
	}

	pthread_t getRecvThread(){
		return recvThread;
	}

	static void *senderServiceHelper(void *context){
		return ((CausalOrdering *)context)->handleSenderService();
	}


	static void *recvServiceHelper(void *context){
		return ((CausalOrdering *)context)->handleRecvService();
	}
private:
	std::shared_ptr<spdlog::logger> _logger;
	struct sockaddr_in srvAddr;
	struct sockaddr_in recvAddr;
	struct sockaddr_in p2pAddr;
	pthread_t senderThread;
	pthread_t recvThread;
	int multicastSock;
	int pointToPointSock;
	int port;
	int id;
	char *msg;
	void * handleSenderService();
	void * handleRecvService();
	Processes *process;
};


#endif /* CAUSUALORDERING_CPP_ */
