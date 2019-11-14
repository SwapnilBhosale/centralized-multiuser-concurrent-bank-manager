/*
 * Process.cpp
 *
 *  Created on: Nov 13, 2019
 *      Author: lilbase
 */


#include "Process.h"

Process::Process(int id, char *msg, int soc, struct sockaddr_in addr1, struct sockaddr_in addr2){
	this-> sendCnt = 0;
	this -> recvCnt = 0;
	this -> id = id;
	this -> msg = msg;
	this -> multicastSocket = soc;
	this -> sendAddr = addr1;
	this -> recvAddr = addr2;

}

void Process::sendMessage(){
	sleep(5);
	sendCnt += 1;
	this ->vectorClok[id] = sendCnt;
	char buff[256];
	sprintf(buff,"%d:%d:%d:%d:%d:%s", id, vectorClok[0], vectorClok[1], vectorClok[2], vectorClok[3], msg);
	_logger -> info("Sending message: ",buff);
	sendto(multicastSocket, buff, sizeof(buff), 0, (struct sockaddr *) &sendAddr, sizeof(sendAddr));
	sleep(5);
}

void Process::receiveMessage(){
	char buff[256];
	while(recvfrom(multicastSocket, buff, sizeof(buff), 0, (struct sockaddr *) &recvAddr, &(socklen_t)sizeof(recvAddr)) >= 0){
		recvCnt += 1;
		_logger -> info("Message received: ",buff,", count: ",recvCnt);
		string str(buff);
		int recvdClock = {0, 0, 0, 0};
		int pId = atoi(strtok(buff, ":"));
		recvdClock[0] = atoi(strtok(NULL, ":"));
		recvdClock[1] = atoi(strtok(NULL, ":"));
		recvdClock[2] = atoi(strtok(NULL, ":"));
		recvdClock[2] = atoi(strtok(NULL, ":"));
		char * msg = strtok(NULL, ":");
		bool flag = true;
		for(int i=0;i<4;i++){
			if((recvdClock[i] != vectorClok[i]) or (recvdClock[id] - vectorClok[id] == 1)){
				flag =false;
			}
		}
		if(!flag){
			v.push_back(str);
			_logger -> info ("message buffered");
		}else{
			_logger -> info("Causal msg received successfully!");
		}
	}
}

