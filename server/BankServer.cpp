/*
 * Server.cpp
 *
 *  Created on: Sep 16, 2019
 *      Author: lilbase
 */

#include "BankSever.h"
#include "ServerSock.h"

BankServer::BankServer() {
	 //std::cout.flush();
	 serverSock = NULL;

}

BankServer::~BankServer() {
	delete serverSock;
}

void BankServer::init(){
	initialize_static_data();
	serverSock = new ServerSock();
	serverSock -> init();
	for (int i = 0; i < THREADS_COUNT; ++i) {
		create_thread(i, serverSock);
	}
}

void BankServer::initialize_static_data(){
	std::ifstream file;
	file.open("data.txt");
	std::string line;
	if(file.is_open()) {
		while(getline(file, line)){
			std::string arr[3];
			splitString(arr, line);

			CustomerBuilder b;
			Customer c = b.set_account_number(std::stoi(arr[0]))
							.set_name(arr[1])
							.set_balance(std::stol(arr[2]))
							.build();
			customer_map.insert(std::make_pair(c.getAccountNumber(), c));
		}
	}
	file.close();
}



void BankServer::print_stats(int signal_Number) {
    double user, sys;
    struct rusage   myusage;

    if (getrusage(RUSAGE_SELF, &myusage) < 0)
        std::cout<< "Error: getrusage()";

    user = (double) myusage.ru_utime.tv_sec +
                    myusage.ru_utime.tv_usec/1000000.0;
    sys = (double) myusage.ru_stime.tv_sec +
                   myusage.ru_stime.tv_usec/1000000.0;

    printf("\nuser time = %g, sys time = %g\n", user, sys);
    exit(0);
}


void BankServer::create_thread(int index, ServerSock *serverSock) {
	pthread_create(&threads[index], NULL, &ServerSock::loop_helper, serverSock);
}


int main(int argc, char **argv) {
		BankServer server;
	    std::signal(SIGINT, server.print_stats);
	    signal(SIGPIPE, server.print_stats);
	    server.init();
	    for (;;)
	        pause();
}
