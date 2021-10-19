# UMBC CMSC 621  Advanced Operating System 


## This project conntaines three miniprojects which I implemented as a part of Coursework

1. Centralized Multi User Concurrent Bank Account Manager
2. Two Phase Commit protocol 
3. Clock Synchronization 


### 1. Centralized Multi User Concurrent Bank Account Manager

Just run make file to compile and create executables.
1. It uses SPDLog concurrent logging library (https://github.com/gabime/spdlog)
2. Uses Thread-Pool implementation for concurrency
3. Uses Apache AB tool to test concurrency and server KPI such as Requests Per Minutes etc.

We benchmark the bank server by sending 50000 transactional total with client varying from 30 to 300.
The total_no_req/sec and total_time are calculated and plotted.

When you kill the Server it also prints the stats about request/sec and other KPI's


#### Client CLI options
```
Usage: ./Client [options]
Options are:
-n requests     Number of requests to perform, Default is 20000
-c concurrency  Number of concurrent requests to make, Default is 30
-h host  	    Host to connect to, Default is 127.0.0.1
-p port  	    Port to connect to, Default is 8080
-f file         Address Transaction file address, Default is ./src/Transactions.txt
```


#### Server CLI options
```
Usage: ./Server [options]
Options are:
-t thread pool size     Thread pool count, Default is 100;
-p port                 Server port to listen, Default is 8080
-f file                 Address of startup data file for customers, Default is ./src/Records.txt
```

#### how to compile and run
```
    1. make
    2. Run Server
        -   ./Server -t 200 -p 9999 -f Records.txt
    3. Run Client
        -   ./Client -n 20000 -c 50 -h 127.0.0.1 -p 9999 -f Transactions.txt
```
### 2. Clock Synchronization 

### how to compile the binaries:

1) run "make" to generate all binaries
2) or you can generate individual binaries by running "make Berkeley", "make Causal", "make NonCausal", "make DistributedME"
3) run "make clean" to delete all the binaries and object files


## 3. Two Phase Commit protocol 

### how to run the programs:

I have provided Python script which allows you to run multiple instances of the same program without manual intervention.
You just need to pass command line argument to Python script specifying which executable you want to run

1) run "“./helper.py berkeley" to execute Berkeley binary
2) run "./helper.py causal" to execute Causal binary
3) run "./helper.py noncausal" to execute NonCausal binary
4) run "./helper.py bonus" to execute Distributed Mutual Exclusion algorithm binary.


