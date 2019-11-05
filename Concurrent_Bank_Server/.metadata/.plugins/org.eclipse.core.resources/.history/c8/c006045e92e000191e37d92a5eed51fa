/*
 * client.h
 *
 *  Created on: Sep 21, 2019
 *      Author: lilbase
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
#include "../../include/spdlog/spdlog.h"
#include "../../include/spdlog/sinks/stdout_color_sinks.h"
#include "../../include/spdlog/sinks/rotating_file_sink.h"
#include "../utils/constants.h"
#include "../utils/Transaction.h"
#include "../utils/util.h"

static int count = 0;
class Client {
private:
	pthread_t client_service_thread;
	int client_socket;
	std::shared_ptr<spdlog::logger> _logger;
public:
	Client(){
		client_socket = 0;
		_logger = spdlog::get("Server");
		client_service_thread = 0;
	}
	static void *client_service_invoke_helper(void *context){
		return ((Client *)context)->handle_client_service();
	}
	void create_client_Service();
	void  * handle_client_service();

	pthread_t getClientServiceThread() const
	{
		return client_service_thread;
	}

	void setClientServiceThread(pthread_t clientServiceThread)
	{
		client_service_thread = clientServiceThread;
	}

	std::string do_transaction(Transaction t);
};

#endif /* CLIENT_CLIENT_H_ */
