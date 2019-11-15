 
FLAGS = -std=c++11 -lpthread
LDFLAGS = -lpthread

BERKELEY_TARGET = Berkeley
CAUSAL_TARGET = Causal
NON_CAUSAL_TARGET = NonCausal
DIST_ME_TARGET = NonCausal

LOG_LIBRARY_HEADERS = include/

CAUSAL_SRC = $(shell echo src/CausalOrdering.cpp) $(shell echo src/util/*.cpp) $(shell echo src/Process.cpp)
NON_CAUSAL_SRC = $(shell echo src/NonCausalOrdering.cpp) $(shell echo src/util/*.cpp) $(shell echo src/Process.cpp)
BERKELEY_SRC = $(shell echo src/ClockServer.cpp) $(shell echo src/util/*.cpp)
DIST_ME_SRC = $(shell echo src/MutualExclusion.cpp) $(shell echo src/util/*.cpp)


compile: ${BERKELEY_TARGET} ${CAUSAL_TARGET} ${NON_CAUSAL_TARGET} ${DIST_ME_TARGET}

clean:
	rm -f src/*.o src/util/*.o $(BERKELEY_TARGET) $(CAUSAL_TARGET) ${NON_CAUSAL_TARGET} ${DIST_ME_TARGET}

%.o : %.cpp
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) -c $< -o $@

${BERKELEY_TARGET}: $(BERKELEY_SRC)
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) $(BERKELEY_SRC) -o $(BERKELEY_TARGET) ${LDFLAGS}
	
${CAUSAL_TARGET}: $(CAUSAL_SRC)
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) $(CAUSAL_SRC) -o $(CAUSAL_TARGET) ${LDFLAGS}
	
${NON_CAUSAL_TARGET}: ${NON_CAUSAL_SRC}
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) $(NON_CAUSAL_SRC) -o $(NON_CAUSAL_TARGET) ${LDFLAGS}
	
${DIST_ME_TARGET} : ${DIST_ME_SRC}
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) $(DIST_ME_SRC) -o $(DIST_ME_TARGET) ${LDFLAGS}