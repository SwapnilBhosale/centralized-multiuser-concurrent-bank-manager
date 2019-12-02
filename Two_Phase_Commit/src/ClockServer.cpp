/*
 * ClockServer.cpp
 *	This class file implements the distributed Berkeley clock sync algorithm
 *  Created on: Nov 4, 2019
 *      Author: Swapnil Bhosale
 */

#include "ClockerServer.h"

using namespace std;


/**
 * This is the constuctore which initializes the class variables
 */
ClockServer::ClockServer(int port, bool isCordinator, long timeDrift){
	this->port = port;
	this -> _logger = spdlog::get("ClockServer");
	this -> multicastSock = 0;
	this -> pointToPointSock = 0;
	this -> timeDrift = timeDrift;
	this -> isCordinator = isCordinator;
	bzero(&srv_addr, sizeof(srv_addr));
	bzero(&rcv_addr, sizeof(rcv_addr));
	bzero(&p2p_addr, sizeof(p2p_addr));
	gettimeofday(&time, NULL);
	this -> cooPort = 0;
}


/**
 * This method initializes the Multicast and point to point socket
 * Bind these sockets to the operating system and set multicast options to the socket
 *
 * @param NA
 * @return NA
 */
void ClockServer::init(){

	multicastSock = socket(AF_INET, SOCK_DGRAM, 0);
	int enable = 1;

	if (setsockopt(multicastSock,
			SOL_SOCKET, SO_REUSEADDR,
			&enable, sizeof(int)) < 0)
		perror("setsockopt(SO_REUSEADDR)");




	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);
	srv_addr.sin_addr.s_addr = inet_addr(HOST);

	rcv_addr.sin_family = AF_INET;
	rcv_addr.sin_port = htons(port);
	rcv_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(multicastSock, (const struct sockaddr *)&rcv_addr, sizeof(rcv_addr)) < 0){
		_logger -> error("Error in bind()");
		exit(1);
	}

	_logger -> info("bound the first port");


	struct ip_mreq mreq;
	if (-1 == inet_pton(AF_INET, HOST/*such as 192.0.2.33*/, &rcv_addr.sin_addr))
	     _logger -> info("cannot set multicast address: %s\n", strerror(errno));
	memcpy(&mreq.imr_multiaddr.s_addr, (void*)&rcv_addr.sin_addr, sizeof(struct in_addr));
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	struct ip_mreq multicast_req;
		multicast_req.imr_multiaddr.s_addr = inet_addr(HOST);
		multicast_req.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(multicastSock,
				IPPROTO_IP, IP_ADD_MEMBERSHIP,
				&multicast_req, sizeof(multicast_req)) < 0)
			perror("setsockopt(IP_ADD_MEMBERSHIP)");

	pointToPointSock = socket(AF_INET, SOCK_DGRAM, 0);
	p2p_addr.sin_family = AF_INET;
	p2p_addr.sin_port = htons(INADDR_ANY);
	p2p_addr.sin_addr.s_addr = INADDR_ANY;

	if (setsockopt(pointToPointSock,
				SOL_SOCKET, SO_REUSEADDR,
				&enable, sizeof(int)) < 0)
			perror("setsockopt(SO_REUSEADDR pointToPointSock)");

	if (bind(pointToPointSock, (const struct sockaddr *)&p2p_addr, sizeof(p2p_addr)) < 0){
		_logger -> error("Error in bind pointToPointSock()");
		exit(1);
	}

	//if process is cordinator then do this
	if (isCordinator){
		sendCordinatorClock();
		sendCordinatorPortForP2P();
		calcualteTheAverageTime();

	}else{		//process is not cordinator
		receiveServerTime();
		getCooPort();
		calculateAndSendDrift();
		receiveUpdatedClockFromCoo();
	}



	_logger -> info("server listening on port {}", port);
}

/**
 * This method helps in initializing the server port value with the port on which coordinator is listening
 *
 * @param NA
 * @return NA
 */
void ClockServer::getCooPort(){
	char message[256];
	socklen_t addr = sizeof(rcv_addr);
	recvfrom(multicastSock, message, sizeof(message), 0, (struct sockaddr *) &rcv_addr, &addr);
	_logger -> info("Coordinator port received: {}",atoi(message));
	this -> cooPort = atoi(message);
}

/**
 * This method multicasts coordinator port number to all other process
 * for point to point communication needed in later stage of algorithm
 *
 * @param NA
 * @return NA
 */
void ClockServer::sendCordinatorPortForP2P(){
	struct sockaddr_in coo_port;
	socklen_t sa_len = sizeof(coo_port);
	getsockname(pointToPointSock,(struct sockaddr *)&coo_port,&sa_len);
	int master_port = (int)ntohs(coo_port.sin_port);
	char buffer[256];
	sprintf(buffer, "%d", master_port);
	_logger -> info("In Coo port is {}",buffer);
	sendto(multicastSock, buffer, strlen(buffer), 0, (struct sockaddr *) &rcv_addr, sizeof(rcv_addr));
}

/**
 * This method calculates the drift using the value received from the server
 * And sends this diff to the coordinator process
 *
 * @param NA
 * @return NA
 */
void ClockServer::calculateAndSendDrift(){
	_logger -> info("Server port is : {} ",cooPort);
	p2p_addr.sin_port = htons(cooPort);
	char buffer[256];
	sprintf(buffer, "%ld", timeDrift);
	int tp = sendto(pointToPointSock, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &p2p_addr, sizeof(p2p_addr));

	_logger -> info("send diff : {} {}",tp, buffer);
	//printDate(driftTime);

}

/**
 * This method is run by coordinator. It calcualted the average from the drift received from all clients.
 * Also muticasts this average to all the clients and syncs own clock
 *
 * @param NA
 * @return NA
 */
void ClockServer::calcualteTheAverageTime(){
	sleep(5);
	int average = 0;
	char buffer[256];
	int sum = 0;
	int counter = 0;
	//p2p_addr.sin_port = htons(INADDR_ANY);
	//p2p_addr.sin_addr.s_addr = INADDR_ANY;

	struct sockaddr_in from;
	socklen_t addr1 = sizeof(p2p_addr);
	_logger -> info ("received from clients {} {}",counter,pointToPointSock);
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(pointToPointSock, &fds);
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	if(setsockopt(pointToPointSock,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout))<0)
	{
		_logger -> error("Error in timeout!");
	}
	int ret;
	while((ret = select(pointToPointSock+1, &fds, NULL, NULL, &timeout)) > 0)
	{
		recvfrom(pointToPointSock, buffer, sizeof(buffer), 0, (struct sockaddr *) &p2p_addr, &addr1);
		_logger -> info("received data: {}",buffer);
		sum = sum + atoi(buffer);
		counter++;
	}
	_logger -> info ("received from clients {}",counter);
	average = sum/(counter + 1);
	_logger -> info("Average: {}", average);
	_logger -> info("Total number of processes: {}", (counter + 1));
	time.tv_sec = time.tv_sec + average;
	printDate(time);
	char buff[256];
	sprintf(buff, "%d", average);
	sendto(multicastSock, buff, sizeof(buff), 0, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
}

/**
 * This method is executed by clients.
 * Receives the average value from the coordinator and sync own clock
 *
 * @param NA
 * @return NA
 */

void ClockServer::receiveUpdatedClockFromCoo(){
	_logger -> info("Waiting to receive clock from coo");
	char buffer[256];
	socklen_t addr = sizeof(rcv_addr);
	recvfrom(multicastSock, buffer, sizeof(buffer), 0, (struct sockaddr *) &rcv_addr, &addr);
	timeval newTime;
	long int newDrift = atol(buffer) - timeDrift;
	newTime.tv_sec = time.tv_sec + newDrift;
	printDate(newTime);

}

/**
 * This method is executed by client and used in receiving the initial clock value
 * from the coordinator
 *
 * @param NA
 * @return NA
 */
void ClockServer::receiveServerTime(){
	char message[256];
	socklen_t addr = sizeof(rcv_addr);
	int messageLength = recvfrom(multicastSock, message, sizeof(message), 0, (struct sockaddr *) &rcv_addr, &addr);
	timeval newTime;
	time.tv_sec = atol(message) + this -> timeDrift;
	printDate(time);
}

void ClockServer::printDate(timeval time){
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[64], buf[64];

	nowtime = time.tv_sec;
	nowtm = localtime(&nowtime);
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
	snprintf(buf, sizeof buf, "%s", tmbuf);
	_logger ->info("Clock value is {}",buf);
}

/**
 * This method is executed by coordinator and used in sending the initial clock value
 * from the coordinator
 *
 * @param NA
 * @return NA
 */
void ClockServer::sendCordinatorClock(){
	string epoch(to_string(time.tv_sec));
	sendto(multicastSock, epoch.c_str(), epoch.length(), 0, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
	printDate(time);
}



/**
 * This method prints the how to use CLI options
 *
 * @param NA
 * @return NA
 */
static void usage(const char *progname)
{
    fprintf(stderr, "Usage: %s [options] \n", progname);
/* 80 column ruler:  ********************************************************************************
 */
    fprintf(stderr, "Options are:\n");
    fprintf(stderr, "    -t TimeDrift     Time drift from current clock epoch, Default +10\n");
    fprintf(stderr, "    -s server  	  Set this option if this process is Server\n");
    exit(EINVAL);
}


/**
 * This is the main method where execution starts
 *
 * @param NA
 * @return int
 */
int main(int argc, char **argv) {
	extern char *optarg;
	char c;
	long int timeDrift = TIME_DRIFT;
	int p = PORT;
	bool isCordinator = false;
	while ((c = getopt (argc, argv, ":t:s:")) != -1) {
		switch(c) {
		case 't' :
			timeDrift = atol(optarg);
			break;
		case 's' :
			isCordinator = true;
			break;
		case ':':
			usage(argv[0]);
			break;
		case '?':
			usage(argv[0]);
			break;

		}
	}
	pid_t pid = getpid();

	//create logger file name
	string logFileName = string("./logs/ClockServer").append("_").append(to_string(pid)).append(".txt");
	std::vector<spdlog::sink_ptr> sinks;

	//create rotating file and console appenders
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFileName,1024 * 1024 * 50, 10, true));
	auto combined_logger = std::make_shared<spdlog::logger>("ClockServer", begin(sinks), end(sinks));

	//set log info and log pattern
	combined_logger -> set_level(spdlog::level::info);
	combined_logger -> set_pattern("[%Y-%m-%d %H:%M:%S.%e] [Thread - %t] [%l] %v");
	spdlog::register_logger(combined_logger);
	ClockServer clockServer(p, isCordinator, timeDrift);
	clockServer.init();

}


