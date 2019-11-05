/*
 * ClockServer.cpp
 *
 *  Created on: Nov 4, 2019
 *      Author: Swapnil Bhosale
 */

#include "ClockerServer.h"


static void usage(const char *progname)
{
    fprintf(stderr, "Usage: %s [options] \n", progname);
/* 80 column ruler:  ********************************************************************************
 */
    fprintf(stderr, "Options are:\n");
    fprintf(stderr, "    -t thread pool size     Thread pool count, Default is 100\n");
    fprintf(stderr, "    -p port  	      	    Server port to listen, Default is 8080\n");
    fprintf(stderr, "    -f file         	    Address of startup data file for customers, Default is './src/Records.txt'\n");
    exit(EINVAL);
}


int main(int argc, char **argv) {
	extern char *optarg;
	char c;
	long int timeDrift = TIME_DRIFT;
	int p = PORT;
	while ((c = getopt (argc, argv, ":p:t:")) != -1) {
		switch(c) {
		case 'p' :
			p = atoi(optarg);
			if(p <= 0)
				usage(argv[0]);
			break;
		case 't' :
			timeDrift = atol(optarg);
			break;
		case ':':
			usage(argv[0]);
			break;
		case '?':
			usage(argv[0]);
			break;

		}
	}
	char *logFileName = "ClockServer".
	vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("./logs/server.txt",1024 * 1024 * 50, 10, true));
	auto combined_logger = std::make_shared<spdlog::logger>(string("ClockServer"), begin(sinks), end(sinks));
}


