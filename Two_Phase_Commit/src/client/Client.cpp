/*
 * client.cpp
 *
 *  Created on: Sep 21, 2019
 *      Author: lilbase
 */

#include "Client.h"
using namespace std;

void Client::startClientService() {
	//_logger -> info("started transaction {}", //++Client::count);
	char *buf;
	struct sockaddr_in serv_addr;
	if ((frontEndSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("\n Socket creation error \n");
	}
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_port = htons(this->port);
	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(PF_INET, this->host.c_str(), &serv_addr.sin_addr) <= 0) {
		perror("\nInvalid address/ Address not supported \n");
	}

	if (connect(frontEndSock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))
			< 0) {
		perror("\nConnection Failed \n");
	}

	//we have connected to front end server
	_logger->info("OK");
	string command;
	char inputCommand[255];
	char serverRes[255];
	while (1) {
		cout << endl;
		cout << "Enter the command: " << endl;
		getline(cin, command);

		strcpy(inputCommand, command.c_str());
		int send_tran = send(frontEndSock, inputCommand, sizeof(inputCommand),
				0);
		//Recieve transaction status

		int read_count = read(frontEndSock, serverRes, sizeof(serverRes));

		if (read_count > 0 and strcmp(serverRes, "OK") == 0) {
			cout << serverRes << endl;
			cout << "Connection closed by foreign host" << endl;
			break;
		} else {

			cout << serverRes << endl;
			bzero(serverRes, 256);
			bzero(inputCommand, 256);
		}
	}

	close(frontEndSock);
}

static void usage(const char *progname) {
	fprintf(stderr, "Usage: %s [options] \n", progname);
	fprintf(stderr, "Options are:\n");
	fprintf(stderr,
			"    -h host  	    Host to connect to, Default is 127.0.0.1\n");
	fprintf(stderr, "    -p port  	    Port to connect to, Default is 8080\n");
	exit(EINVAL);
}

int main(int argc, char **argv) {
	extern char *optarg;
	char c;
	int p = FRONT_END_PORT;
	std::string host(HOST);

	while ((c = getopt(argc, argv, ":h:p:")) != -1) {
		switch (c) {
		case 'h':
			host = optarg;
			if (host.length() == 0)
				usage(argv[0]);
			break;
		case 'p':
			p = atoi(optarg);
			if (p <= 0)
				usage(argv[0]);
			break;
		case ':':
			usage(argv[0]);
			break;
		case '?':
			usage(argv[1]);
			break;
		}
	}
	vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(
			std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
					"./logs/client.txt", 1024 * 1024 * 5, 10, true));
	auto combined_logger = std::make_shared<spdlog::logger>("Client",
			begin(sinks), end(sinks));
	combined_logger->set_level(spdlog::level::info);
	combined_logger->set_pattern(
			"[%Y-%m-%d %H:%M:%S.%e] [Thread - %t] [%l] %v");
	spdlog::register_logger(combined_logger);
	Client client(host, p);
	client.startClientService();
}

