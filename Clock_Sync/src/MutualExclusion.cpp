/*
 * MutualExclusion.cpp
 *
 *  Created on: Nov 14, 2019
 *      Author: lilbase
 */

#include "MutualExclusion.h"

using namespace std;


MutualExclusion::MutualExclusion(int id, int n){
	this->port = PORT;
	this -> _logger = spdlog::get(DISTRIBUTED_ME);
	this -> multicastSock = 0;
	this -> pointToPointSock = 0;
	bzero(&srv_addr, sizeof(srv_addr));
	bzero(&rcv_addr, sizeof(rcv_addr));
	bzero(&p2p_addr, sizeof(p2p_addr));
	//gettimeofday(&time, NULL);
	this -> ownPort = 0;
	this -> id = id;
	this -> senderThread = NULL;
	this -> recvThread = NULL;
	this -> isDoneUpdatingFile = false;
	this -> numOfProcesses = n;
	this -> hasSentRequest = false;
	this -> sentTimestamp = "";
	this -> isUpdatingFile = false;
}

void MutualExclusion::init(){

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

	this -> ownPort = getOwnPort();
	/*if (isCordinator){
		sendCordinatorPortForP2P();
		calcualteTheAverageTime();

	}else{
		receiveServerTime();
		getCooPort();
		calculateAndSendDrift();
		receiveUpdatedClockFromCoo();
	}*/
	_logger -> info("Process {} is listening on own port {}",id, ownPort);
}

int MutualExclusion::getOwnPort(){
	struct sockaddr_in coo_port;
	socklen_t sa_len = sizeof(coo_port);
	getsockname(pointToPointSock,(struct sockaddr *)&coo_port,&sa_len);
	int port = (int)ntohs(coo_port.sin_port);
	return port;
}

void MutualExclusion::sendCordinatorPortForP2P(){
	struct sockaddr_in coo_port;
	socklen_t sa_len = sizeof(coo_port);
	getsockname(pointToPointSock,(struct sockaddr *)&coo_port,&sa_len);
	int master_port = (int)ntohs(coo_port.sin_port);
	char buffer[256];
	sprintf(buffer, "%d", master_port);
	_logger -> info("In Coo port is {}",buffer);
	sendto(multicastSock, buffer, strlen(buffer), 0, (struct sockaddr *) &rcv_addr, sizeof(rcv_addr));
}

/*void MutualExclusion::calculateAndSendDrift(){
	_logger -> info("Server port is : {} ",cooPort);
	p2p_addr.sin_port = htons(cooPort);
	char buffer[256];
	sprintf(buffer, "%ld", timeDrift);
	int tp = sendto(pointToPointSock, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &p2p_addr, sizeof(p2p_addr));

	_logger -> info("send diff : {} {}",tp, buffer);
	//printDate(driftTime);

}*/

void MutualExclusion::calcualteTheAverageTime(){
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

/*void MutualExclusion::receiveUpdatedClockFromCoo(){
	_logger -> info("Waiting to receive clock from coo");
	char buffer[256];
	socklen_t addr = sizeof(rcv_addr);
	recvfrom(multicastSock, buffer, sizeof(buffer), 0, (struct sockaddr *) &rcv_addr, &addr);
	timeval newTime;
	long int newDrift = atol(buffer) - timeDrift;
	newTime.tv_sec = time.tv_sec + newDrift;
	printDate(newTime);

}

void MutualExclusion::receiveServerTime(){
	char message[256];
	socklen_t addr = sizeof(rcv_addr);
	int messageLength = recvfrom(multicastSock, message, sizeof(message), 0, (struct sockaddr *) &rcv_addr, &addr);
	timeval newTime;
	time.tv_sec = atol(message) + this -> timeDrift;
	printDate(time);
}*/

void MutualExclusion::printDate(timeval time){
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[64], buf[64];

	nowtime = time.tv_sec;
	nowtm = localtime(&nowtime);
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
	snprintf(buf, sizeof buf, "%s", tmbuf);
	_logger ->info("Clock value is {}",buf);
}

void MutualExclusion::createSendAndRecvThread() {
	int res;
	res = pthread_create(&senderThread, NULL, &MutualExclusion::senderServiceHelper, this);
	if(res<0) {
		_logger -> error("Error in creating sender thread!");
		exit(1);
	}

	res = pthread_create(&recvThread,NULL, &MutualExclusion::recvServiceHelper, this);
	if(res<0)
	{
		_logger -> error("Error in creating receiver thread!");
		exit(1);
	}
}

void *MutualExclusion::handleSenderService(){
	gettimeofday(&time, NULL);
	sentTimestamp = to_string(time.tv_sec);
	char buff[256];
	sprintf(buff,"%d:%d:%d:%s", 0, id, ownPort, sentTimestamp);
	sendto(multicastSock, sentTimestamp.c_str(), sentTimestamp.length(), 0, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
	hasSentRequest = true;
}

void *MutualExclusion::handleRecvService(){
	int replyCount = 0;
	while (true) {
		if(!isDoneUpdatingFile && replyCount == numOfProcesses - 1) {
			FILE *f = fopen("./counter.txt","r+");
			char buffer[256];
			fgets(buffer,sizeof(buffer),f);
			int val = atoi(buffer);
			_logger -> info("Value in the file is: {}",val);
			val += 1;
			fprintf(f, "%d\n",val);
			_logger -> info("Updated value wrote to file is: ",val);
			fclose(f);
			isDoneUpdatingFile = true;
			for(string &m : v) {
				char buff[255];
				strcpy(buff, m.c_str());
				int typeOfMsg = atoi(strtok(NULL, ":"));
				int pId = atoi(strtok(NULL, ":"));
				int senderPort = atoi(strtok(NULL, ":"));
				p2p_addr.sin_port = htons(senderPort);
				char buffer[256];
				sprintf(buffer, "%s", OK);
				sendto(pointToPointSock, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &p2p_addr, sizeof(p2p_addr));
			}
		}
		char buff[256];
		socklen_t addr = sizeof(rcv_addr);
		recvfrom(multicastSock, buff, sizeof(buff), 0, (struct sockaddr *) &rcv_addr, &addr);
		string str(buff);
		int typeOfMsg = atoi(strtok(buff, ":"));
		int pId = atoi(strtok(buff, ":"));
		int senderPort = atoi(strtok(NULL, ":"));
		long int epoch = atol(strtok(NULL, ":"));
		if(pId != id){
			if(typeOfMsg == 0 ) {
				if(isUpdatingFile){
					v.push_back(str);
				}else if(isDoneUpdatingFile || (hasSentRequest && atol(sentTimestamp.c_str()) > epoch)){
					p2p_addr.sin_port = htons(senderPort);
					char buffer[256];
					sprintf(buffer, "%s", OK);
					sendto(pointToPointSock, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &p2p_addr, sizeof(p2p_addr));
				}else{
					v.push_back(str);
				}
			}else{
				replyCount ++;
			}
		}
	}
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
	int  n = 0;
	int id = 0;
	while ((c = getopt (argc, argv, ":p:t:n:")) != -1) {
		switch(c) {
		case 'i' :
			id = atoi(optarg);
			break;
		case 't' :
			timeDrift = atol(optarg);
			break;
		case 'n' :
			n = atoi(optarg);
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

	string logFileName = string("./logs/").append(DISTRIBUTED_ME).append("_").append(to_string(pid)).append(".txt");
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFileName,1024 * 1024 * 50, 10, true));
	auto combined_logger = std::make_shared<spdlog::logger>(DISTRIBUTED_ME, begin(sinks), end(sinks));
	combined_logger -> set_level(spdlog::level::info);
	combined_logger -> set_pattern("[%Y-%m-%d %H:%M:%S.%e] [Thread - %t] [%l] %v");
	spdlog::register_logger(combined_logger);
	MutualExclusion mutualExclusion(id, n);
	mutualExclusion.init();
	mutualExclusion.createSendAndRecvThread();
	(void) pthread_join(mutualExclusion.getSenderThread(), NULL);
	(void) pthread_join(mutualExclusion.getRecvThread(), NULL);
}


