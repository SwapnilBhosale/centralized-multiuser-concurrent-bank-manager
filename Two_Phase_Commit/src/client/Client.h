/*
 * client.h
 *
 *  Created on: Sep 21, 2019
 *      Author: Swapnil Bhosale
 */

#ifndef CLIENT_CLIENT_H_
#define CLIENT_CLIENT_H_
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "../include/spdlog/spdlog.h"
#include "../include/spdlog/sinks/stdout_color_sinks.h"
#include "../include/spdlog/sinks/rotating_file_sink.h"
#include "../utils/constants.h"
#include <sys/time.h>
#include <getopt.h>
#include <stdlib.h>


class Client {
private:

	std::string host;
	int port;
	int frontEndSock;
	std::shared_ptr<spdlog::logger> _logger;
public:

	Client(std::string host, int port){
		frontEndSock = 0;
		_logger = spdlog::get("Client");
		this -> port = port;
	}

	void startClientService();
};

#endif /* CLIENT_CLIENT_H_ */
