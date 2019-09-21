/*
 * Server.h
 *
 *  Created on: Sep 16, 2019
 *      Author: lilbase
 */

#ifndef SERVER_H_
#define SERVER_H_
#include <iostream>
#include <unordered_map>
#include <list>
#include <pthread.h>
#include <fstream>
#include <vector>

#include "Customer.h"
#include "ObserverPattern.h"
#include "ServerSock.h"
#include "Transaction.h"
#include "util.h"
#include "ServerSock.h"
#include "constants.h"



class BankServer: public Observer{
public:
	BankServer();
	virtual ~BankServer();
	void init();

	void static print_stats(int signal_number);
	void create_thread(int index, ServerSock *serverSock);
	void do_action(char * data);
	void notify(char * data){
		do_action(data);
	}
	static void *intrest_service_invoke_helper(void *context){
			return ((BankServer *)context)->handle_intrest_service();
	}

	void  * handle_intrest_service();

	pthread_t get_intrest_Service_thread(){
		return intrest_service__thread;
	}

private:
	ServerSock *serverSock;
	pthread_t threads[THREADS_COUNT];
	pthread_t intrest_service__thread;
	std::unordered_map<int, Customer> customer_map;
	std::unordered_map< int, std::vector< Transaction > > transaction_map;
	void withdrawal(std::string tstamp, std::string account_id, std::string amount);
	void deposit(std::string tstamp, std::string account_id, std::string amount);
	void initialize_static_data();
	void create_intrest_service();
	void update_customer_map(Customer c);
};

#endif /* SERVER_H_ */
