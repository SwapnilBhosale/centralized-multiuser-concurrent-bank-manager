/*
 * ServerSock.h
 *
 *  Created on: Sep 16, 2019
 *      Author: Swapnil Bhosale
 */

#ifndef SERVER_SERVERSOCK_H_
#define SERVER_SERVERSOCK_H_

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <time.h>
#include <locale.h>
#include "../../include/spdlog/spdlog.h"
#include "../../include/spdlog/sinks/stdout_color_sinks.h"
#include "../../include/spdlog/sinks/rotating_file_sink.h"
#include "../utils/constants.h"
#include "../utils/ObserverPattern.h"
#include "../utils/util.h"


class ServerSock {
public:
	ServerSock();
	virtual ~ServerSock();
	void init();
	void * enter_server_loop();
	static void *thread_pool_loop_helper(void *context){
		return ((ServerSock *)context)->enter_server_loop();
	}
private:
	void handle_client(int client_socket);
	unsigned int port;
	int sockfd;
	pthread_mutex_t mlock;
	ObserverPattern *obj;
	std::shared_ptr<spdlog::logger> _logger;
};

#endif /* SERVER_SERVERSOCK_H_ */
