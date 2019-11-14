 
FLAGS = -std=c++11 -lpthread
LDFLAGS = -lpthread

BERKELEY_TARGET = Berkeley
CAUSAL_TARGET = Berkeley

LOG_LIBRARY_HEADERS = include/

CAUSAL_SRC = $(shell echo src/CausalOrdering.cpp) $(shell echo src/util/*.cpp)
BERKELEY_SRC = $(shell echo src/ClockServer.cpp) $(shell echo src/util/*.cpp)


compile: ${BERKELEY_TARGET} ${CAUSAL_TARGET}

clean:
	rm -f src/*.o src/util/*.o $(BERKELEY_TARGET) $(CAUSAL_TARGET)

%.o : %.cpp
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) -c $< -o $@

${BERKELEY_TARGET}: $(BERKELEY_SRC)
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) $(BERKELEY_SRC) -o $(BERKELEY_TARGET) ${LDFLAGS}
	
${CAUSAL_TARGET}: $(CAUSAL_SRC)
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) $(CAUSAL_SRC) -o $(CAUSAL_TARGET) ${LDFLAGS}