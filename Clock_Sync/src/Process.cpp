/*
 * Process.cpp
 *
 *  Created on: Nov 13, 2019
 *      Author: lilbase
 */


#include "Process.h"
#include "util/utils.cpp"


/**
 * This is the constructor initializes the class instance variables

 */
Processes::Processes(int id, char *msg, int soc, sockaddr_in addr1, sockaddr_in addr2, bool isCausal){
	this-> sendCnt = 0;
	this -> recvCnt = 0;
	this -> id = id;
	this -> msg = msg;
	this -> multicastSocket = soc;
	this -> sendAddr = addr1;
	this -> recvAddr = addr2;
	this -> isCasual = isCausal;
	if (isCausal)
		this -> _logger = spdlog::get(CAUSAL);
	else
		this -> _logger = spdlog::get(NON_CAUSAL);
}


/**
 * This method actually multicasts the message to all the process in the group
 * Then it sleep for few seconds after sending message
 */
void Processes::sendMessage(){
	sleep(id + SLEEP_CONSTANT);
	sendCnt += 1;
	this ->vectorClok[id] = sendCnt;
	char buff[256];
	sprintf(buff,"%d:%d:%d:%d:%d:%s", id, vectorClok[0], vectorClok[1], vectorClok[2], vectorClok[3], msg);
	_logger -> info("Sending message: {}",buff);
	sendto(multicastSocket, buff, sizeof(buff), 0, (struct sockaddr *) &sendAddr, sizeof(sendAddr));
	sleep(id + SLEEP_CONSTANT);
}

/**
 * This method is responsible for receiving the multicast messages from
 * other processes. It also verifies the rules of causality

 */
void Processes::receiveMessage(){
	char buff[256];
	socklen_t len = sizeof(recvAddr);
	while((recvfrom(multicastSocket, buff, sizeof(buff), 0, (struct sockaddr *) &recvAddr, &len)) >= 0){
		recvCnt += 1;
		string str(buff);
		int recvdClock[] = {0, 0, 0, 0};
		int pId = atoi(strtok(buff, ":"));

		recvdClock[0] = atoi(strtok(NULL, ":"));
		recvdClock[1] = atoi(strtok(NULL, ":"));
		recvdClock[2] = atoi(strtok(NULL, ":"));
		recvdClock[3] = atoi(strtok(NULL, ":"));
		char * msg = strtok(NULL, ":");
		bool flag = true;
		if (pId != id){
			_logger -> info("Message received: {} from process: {}",str, pId);
			if (isCasual) {
				for(int i=0;i<4;i++){
					if(i == pId){
						if (recvdClock[i] - vectorClok[i] != 1){
							flag = false;
							break;
						}
					}else {
						if(recvdClock[i] > vectorClok[i]){
							flag = false;
							break;
						}

					}
				}
				if(!flag){
					v.push_back(str);
					_logger -> info ("message buffered");
				}else{
					vectorClok[pId] = recvdClock[pId];
					_logger -> info("Causal msg delivered successfully!");
				}
			}else{
				_logger -> info("NonCausal msg delivered successfully!");
			}
		}
	}
}
