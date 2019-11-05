/*
 * clock_berkeley.h
 *
 *  Created on: Oct 26, 2019
 *      Author: Swapnil Bhosale
 */

#include <iostream>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include "./include/spdlog/spdlog.h"
#include "./include/spdlog/sinks/stdout_color_sinks.h"
#include <vector>
#include <pthread.h>
#include <time.h>

#ifndef CLOCK_BERKELEY_H_
#define CLOCK_BERKELEY_H_
using namespace std;

typedef struct slaveTimes {
	int processId;
	int slaveTime;
	int diff;
	int offset;
}slaveData;

int main(int argc, char *argv[])
{
	vector<spdlog::sink_ptr> sinks;
	sinks.push_back(make_shared<spdlog::sinks::stdout_color_sink_mt>());
	auto combined_logger = std::make_shared<spdlog::logger>("Server", begin(sinks), end(sinks));
	//set log level
	combined_logger -> set_level(spdlog::level::info);

	//set logging pattern
	combined_logger -> set_pattern("[%Y-%m-%d %H:%M:%S.%e] [Process - %P] [%l] %v");
	spdlog::register_logger(combined_logger);

	int rank, size, master;
	int avgTime, slaveTime, adjMin, master_time, slave_time;

	MPI_Status status;

	MPI_Init(&argc, &argv); // Initializing MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // to know the rank of each process
	MPI_Comm_size(MPI_COMM_WORLD, &size); // to know the total number of processes

	master = 0;

	//initialize clie
	slaveData *data[size - 1];

	for (int i=0;i<size;i++){
		data[i] =(slaveData *) malloc(sizeof(slaveData));
		data[i] -> processId = i;
	}

	if(rank == master) // master process
	{
		combined_logger -> info("Master is: {}",master);

		//get random value for cordinator clock
		master_time = rand() % 100;
		data[master] -> slaveTime = master_time;
		data[master] -> diff = 0;
		combined_logger->info("master process is sending time {}",master_time);

		//send master current time to all salves
		for(int i=1; i<size; i++)
			MPI_Send(&master_time, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

		//receive slave time from each slaves
		for(int i=1; i<size; i++){
			slaveTime = 0;
			MPI_Recv(&slaveTime,  1,  MPI_INT,  i, 0, MPI_COMM_WORLD, &status);

			//save slave time and diff with respect to master time
			data[status.MPI_SOURCE] -> slaveTime = slaveTime;
			data[status.MPI_SOURCE] -> diff = slaveTime - master_time;
			combined_logger->info("Cordinator {} has received time {} from client {} with diff {}",master,slaveTime, status.MPI_SOURCE, data[status.MPI_SOURCE] -> diff);
		}

		avgTime = 0;
		int sumOfTime = 0;
		for(int i=1;i<size;i++){
			sumOfTime += data[i] -> diff;
		}

		//calculate average time over all received slave time
		avgTime = sumOfTime / size; // average value

		combined_logger->info("Time differential average is {}",avgTime);

		data[master] -> offset = avgTime - data[master] ->  diff;// adjust master time value
		data[master] -> slaveTime = master_time + data[master] -> offset;

		combined_logger -> info ("master {} set his time to {}",master, data[master] -> slaveTime);
		for(int i=1; i<size; i++){
			adjMin = avgTime - data[i] -> diff; // adjust time to be adjusted for each process
			combined_logger->info("master process is sending the clock adjustment value of {} to process {}",adjMin,i);
			MPI_Send(&adjMin, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // send time to be adjusted to respective processes
		}
	}
	else //slaves
	{
		//receive message from master
		MPI_Recv(&master_time, 1, MPI_INT, master, 0, MPI_COMM_WORLD, &status); // receive time to be adjusted from master
		combined_logger->info("Slave {} has received time {}",rank,master_time);

		//init random value as a time for each slaves
		srand(time(NULL) + rank);
		slave_time = rand() % 100;

		combined_logger->info("Slave {} is sending time {} to master",rank,slave_time);
		MPI_Send(&slave_time, 1, MPI_INT, master, 0, MPI_COMM_WORLD); // send slave time to master
		MPI_Recv(&adjMin, 1, MPI_INT, master, 0, MPI_COMM_WORLD, &status); // receive time to be adjusted from master

		combined_logger->info("Slave {} has received the clock adjustment value of {}",rank,adjMin);
		slave_time += adjMin;
		combined_logger->info("Slave {} has adjusted clock to {}",rank,slave_time);
	}

	MPI_Finalize();
	return 1;
}

#endif /* CLOCK_BERKELEY_H_ */
