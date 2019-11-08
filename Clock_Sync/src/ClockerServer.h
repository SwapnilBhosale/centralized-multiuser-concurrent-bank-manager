/*
 * ClockerServer.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Swapnil Bhosale
 */

#ifndef CLOCKERSERVER_H_
#define CLOCKERSERVER_H_

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
#include <sys/time.h>

#include "../include/spdlog/spdlog.h"
#include "../include/spdlog/sinks/stdout_color_sinks.h"
#include "../include/spdlog/sinks/rotating_file_sink.h"
#include "util/utils.cpp"
using namespace std;

class ClockServer {
public:
	ClockServer(int port, bool isCordinator, long timeDrift);
	void init();
private:
	int port;
	bool isCordinator;
	int multicastSock;
	int pointToPointSock;
	long int timeDrift;
	std::shared_ptr<spdlog::logger> _logger;
	void sendCordinatorClock();
	struct sockaddr_in srv_addr;
	struct sockaddr_in rcv_addr;
	struct sockaddr_in p2p_addr;
	void receiveServerTime();
	void printDate(timeval time);
	struct timeval time;
	void calculateAndSendDrift();
	void sendCordinatorPortForP2P();
	void getCooPort();
	int cooPort;
	void calcualteTheAverageTime();
	void receiveUpdatedClockFromCoo();
};


#endif /* CLOCKERSERVER_H_ */
