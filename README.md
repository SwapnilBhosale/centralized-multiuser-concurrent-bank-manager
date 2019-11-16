# CLOCK, MULTICAST and COMMIT

## how to compile the binaries:

1) run "make" to generate all binaries
2) or you can generate individual binaries by running "make Berkeley", "make Causal", "make NonCausal", "make DistributedME"
3) run "make clean" to delete all the binaries and object files


## how to run the programs:

I have provided Python script which allows you to run multiple instances of the same program without manual intervention.
You just need to pass command line argument to Python script specifying which executable you want to run

1) run "“./helper.py berkeley" to execute Berkeley binary
2) run "./helper.py causal" to execute Causal binary
3) run "./helper.py noncausal" to execute NonCausal binary
4) run "./helper.py bonus" to execute Distributed Mutual Exclusion algorithm binary.


