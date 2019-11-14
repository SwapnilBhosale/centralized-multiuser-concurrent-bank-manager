/*
 * Process.h
 *
 *  Created on: Nov 13, 2019
 *      Author: lilbase
 */

#ifndef PROCESS_H_
#define PROCESS_H_
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include <string>
#include<string.h>
#include<vector>
#include <sys/socket.h>
#include <unordered_map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <cctype>
#include "../include/spdlog/spdlog.h"
#include "../include/spdlog/sinks/stdout_color_sinks.h"
#include "../include/spdlog/sinks/rotating_file_sink.h"

using namespace std;
class Process{

public:
	Process(int id, char *msg, int multicastSocket, struct sockaddr_in addr1, struct sockaddr_in addr2);
	void sendMessage();
	void receiveMessage();
private:
	int id;
	int multicastSocket;
	char * msg;
	int vectorClok[] = {0, 0, 0, 0};
	int sendCnt;
	int recvCnt;
	std::shared_ptr<spdlog::logger> _logger;
	struct sockaddr_in sendAddr;
	struct sockaddr_in recvAddr;
	char *buff[100];
	vector<string> v;
};


#endif /* PROCESS_H_ */
