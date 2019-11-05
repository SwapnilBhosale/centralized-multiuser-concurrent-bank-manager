 
FLAGS = -std=c++11 -lpthread
LDFLAGS = -lpthread

BERKELEY_TARGET = Berkeley
SOURCES = $(shell echo *.cpp) 

LOG_LIBRARY_HEADERS = include/

BERKELEY_SRC = $(shell echo *.cpp)

OBJECTS = $(BERKELEY_SRC:.cpp=.o)

compile: ${BERKELEY_TARGET} 

clean:
	rm -f $(SERVER_TARGET)

%.o : %.cpp
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) -c $< -o $@

${BERKELEY_TARGET}: $(BERKELEY_SRC)
	mpic++ -I$(LOG_LIBRARY_HEADERS) $(BERKELEY_SRC) -o $(BERKELEY_TARGET)
	