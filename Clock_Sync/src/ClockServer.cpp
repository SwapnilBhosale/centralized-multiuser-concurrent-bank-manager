/*
 * ClockServer.cpp
 *
 *  Created on: Nov 4, 2019
 *      Author: Swapnil Bhosale
 */

#include "ClockerServer.h"

using namespace std;


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
	this -> time.tv_sec = this -> time.tv_sec + timeDrift;
	this -> cooPort = 0;
}

void ClockServer::enterServerLoop() {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	while (1)
	{
		int client_socket = accept(
				multicastSock,
				(struct sockaddr *)&client_addr,
				&client_addr_len);
		//std::cout<<"after accepting the client connection, thread id:"<<pthread_self()<<std::endl;
		if (client_socket < 0) {
			_logger -> error("Error: accept()");
		}
	}
}

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




	if (isCordinator){
		sendCordinatorClock();
		sendCordinatorPortForP2P();
		calcualteTheAverageTime();
	}else{
		timeval t = receiveServerTime();
		getCooPort();
		calculateAndSendDrift(t);
		receiveUpdatedClockFromCoo();
	}

	_logger -> info("server listening on port {}", port);
}

void ClockServer::getCooPort(){
	char message[256];
	socklen_t addr = sizeof(rcv_addr);
	recvfrom(multicastSock, message, sizeof(message), 0, (struct sockaddr *) &rcv_addr, &addr);
	_logger -> info("Coordinator port received: {}",atoi(message));
	this -> cooPort = atoi(message);
}

void ClockServer::sendCordinatorPortForP2P(){
	struct sockaddr_in coo_port;
	socklen_t sa_len = sizeof(coo_port);
	int master_port = (int)ntohs(coo_port.sin_port);
	char buffer[256];
	sprintf(buffer, "%d", master_port);
	_logger -> info("In Coo port is {}",buffer);
	getsockname(pointToPointSock,(struct sockaddr *)&coo_port,&sa_len);
	sendto(multicastSock, buffer, sizeof(buffer), 0, (struct sockaddr *) &rcv_addr, sizeof(rcv_addr));
}

void ClockServer::calculateAndSendDrift(timeval t){
	timeval driftTime;
	driftTime.tv_sec = time.tv_sec - t.tv_sec;
	driftTime.tv_usec = time.tv_usec - t.tv_usec;
	p2p_addr.sin_port = htons(cooPort);

	string epoch(to_string(driftTime.tv_sec));
	epoch.append(" ");
	epoch.append(to_string(driftTime.tv_usec));
	sendto(pointToPointSock, epoch.c_str(), epoch.length(), 0, (struct sockaddr *) &p2p_addr, sizeof(p2p_addr));
	_logger -> info("send diff");
	printDate(driftTime);
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	if(setsockopt(pointToPointSock,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout))<0)
	{
		_logger -> error("Error in timeout!");
	}

}

void ClockServer::calcualteTheAverageTime(){
	int average = 0;
	char buffer[256];
	int sum = 0;
	int counter = 0;
	socklen_t addr1 = sizeof(p2p_addr);

	_logger -> info ("received from clients {}",counter);
	while((recvfrom(pointToPointSock, buffer, sizeof(buffer), 0, (struct sockaddr *) &p2p_addr, &addr1)) > 0)
	{
		sum = sum + atoi(buffer);
		counter++;
	}
	_logger -> info ("received from clients {}",counter);
	average = sum/(counter + 1);
	_logger -> info("Average: {}", average);
	_logger -> info("Total number of processes: {}", (counter + 1));
	time.tv_sec = time.tv_sec + average;
	printDate(time);
	sendCordinatorClock();
}

void ClockServer::receiveUpdatedClockFromCoo(){
	_logger -> info("Waiting to receive clock from coo");
	char buffer[256];
	socklen_t addr = sizeof(rcv_addr);
	recvfrom(multicastSock, buffer, sizeof(buffer), 0, (struct sockaddr *) &rcv_addr, &addr);
	timeval newTime;
	std::string arr[2];
	splitString(arr, buffer);
	newTime.tv_sec = time.tv_sec + std::atol(arr[0].c_str());
	newTime.tv_usec = time.tv_usec + std::atol(arr[1].c_str());

	printDate(newTime);

}

timeval ClockServer::receiveServerTime(){
	char message[256];
	socklen_t addr = sizeof(rcv_addr);
	int messageLength = recvfrom(multicastSock, message, sizeof(message), 0, (struct sockaddr *) &rcv_addr, &addr);
	timeval newTime;
	std::string arr[2];
	splitString(arr, message);
	newTime.tv_sec = std::atol(arr[0].c_str());
	newTime.tv_usec = std::atol(arr[1].c_str());
	printDate(newTime);
	return newTime;

}

void ClockServer::printDate(timeval time){
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[64], buf[64];

	nowtime = time.tv_sec;
	nowtm = localtime(&nowtime);
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
	snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, time.tv_usec);
	_logger ->info("Clock value is {}",buf);
}

void ClockServer::sendCordinatorClock(){
	string epoch(to_string(time.tv_sec));
	epoch.append(" ");
	epoch.append(to_string(time.tv_usec));
	sendto(multicastSock, epoch.c_str(), epoch.length(), 0, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
}




static void usage(const char *progname)
{
    fprintf(stderr, "Usage: %s [options] \n", progname);
/* 80 column ruler:  ********************************************************************************
 */
    fprintf(stderr, "Options are:\n");
    fprintf(stderr, "    -t TimeDrift     Time drift from current clock epoch, Default +10\n");
    fprintf(stderr, "    -p port  	      	    port to listen, Default is 8080\n");
    fprintf(stderr, "    -f file         	    Address of startup data file for customers, Default is './src/Records.txt'\n");
    exit(EINVAL);
}


int main(int argc, char **argv) {
	extern char *optarg;
	char c;
	long int timeDrift = TIME_DRIFT;
	int p = PORT;
	bool isCordinator = false;
	while ((c = getopt (argc, argv, ":p:t:s:")) != -1) {
		switch(c) {
		case 'p' :
			p = atoi(optarg);
			if(p <= 0)
				usage(argv[0]);
			break;
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

	string logFileName = string("./logs/ClockServer").append("_").append(to_string(pid)).append(".txt");
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFileName,1024 * 1024 * 50, 10, true));
	auto combined_logger = std::make_shared<spdlog::logger>("ClockServer", begin(sinks), end(sinks));
	combined_logger -> set_level(spdlog::level::info);
	combined_logger -> set_pattern("[%Y-%m-%d %H:%M:%S.%e] [Thread - %t] [%l] %v");
	spdlog::register_logger(combined_logger);
	ClockServer clockServer(p, isCordinator, timeDrift);
	clockServer.init();

}


