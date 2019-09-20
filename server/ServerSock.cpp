/*
 * ServerSock.cpp
 *
 *  Created on: Sep 16, 2019
 *      Author: lilbase
 */

#include "ServerSock.h"
#include <exception>

ServerSock::ServerSock() {
	sockfd = 0;
	port = DEFAULT_SERVER_PORT;
	mlock = PTHREAD_MUTEX_INITIALIZER;

}

ServerSock::ServerSock(unsigned int p) {
	port = p;
	sockfd = 0;
	mlock = PTHREAD_MUTEX_INITIALIZER;
}

ServerSock::~ServerSock() {
}

void ServerSock::init() {
	struct sockaddr_in srv_addr;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd <= 0)
		perror( "error : socket()");

	int enable = 1;
	if (setsockopt(sockfd,
			SOL_SOCKET, SO_REUSEADDR,
			&enable, sizeof(int)) < 0)
		perror("setsockopt(SO_REUSEADDR)");

	//bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = PF_INET;
	srv_addr.sin_port = htons(port);
	srv_addr.sin_addr.s_addr = INADDR_ANY;

	/* We bind to a port and turn this socket into a listening
	 * socket.
	 * */
	if (bind(sockfd, (const struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0)
		perror("Error:bind()");

	if (listen(sockfd, 10) < 0){
		perror("error: listen()");
		exit(1);
	}
	std::cout<<"server listening on port "<<this->port <<std::endl;
}

void * ServerSock::enter_server_loop() {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	while (1)
	{
		pthread_mutex_lock(&mlock);
		int client_socket = accept(
				sockfd,
				(struct sockaddr *)&client_addr,
				&client_addr_len);
		//std::cout<<"after accepting the client connection, thread id:"<<pthread_self()<<std::endl;
		if (client_socket < 0) {
			perror("accept error");
			pthread_mutex_unlock(&mlock);
			return NULL;
		}

		pthread_mutex_unlock(&mlock);

		handle_client(client_socket);
	}
}

int get_line(int sock, char *buf, int size) {
	ssize_t n;

	//while ((i < size - 1) && (c != '\n'))
	//{

	int i = 0;
	char c;
	while(i<size && c != '\n'){
		n = recv(sock, &c, 1, 0);
		if(n > 0){
			buf[i] = c;
			i++;
		}
	}
	buf[i]= '\0';
	return (i);
}

void ServerSock::handle_client(int client_socket) {
	char *line_buffer = (char *)malloc(sizeof(1024));
	/*
	 * Setup a timeout on recv() on the client socket
	 * */
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	unsigned long len = get_line(client_socket, line_buffer, sizeof(line_buffer));
	if(len > 0){
		std::cout<<"received data of bytes "<<len<<std::endl;
		std::cout<<"received Data : "<<line_buffer<<std::endl;
	}
	free(line_buffer);
	close(client_socket);
	return;
}

