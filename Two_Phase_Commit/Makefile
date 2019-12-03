 
FLAGS = -std=c++11 -lpthread
LDFLAGS = -lpthread

BACKEND_TARGET = Backend
FRONTEND_TARGET = Frontend
CLIENT_TARGET = Client

LOG_LIBRARY_HEADERS = src/include/

BACKEND_SRC = $(shell echo src/back_end_server/*.cpp) $(shell echo src/utils/*.cpp)
FRONTEND_SRC = $(shell echo src/fron_end_server/*.cpp) $(shell echo src/utils/*.cpp)
CLIENT_SRC = $(shell echo src/client/*.cpp) $(shell echo src/utils/*.cpp)


compile: ${BACKEND_TARGET} ${FRONTEND_TARGET} ${CLIENT_TARGET}

clean:
	rm -f src/back_end_server/*.o src/front_end_server/*.o src/client/*.o src/utils/*.o $(BACKEND_TARGET) $(FRONTEND_TARGET) ${CLIENT_TARGET}

%.o : %.cpp
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) -c $< -o $@

${BACKEND_TARGET}: $(BACKEND_SRC)
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) $(BACKEND_SRC) -o $(BACKEND_TARGET) ${LDFLAGS}
	
${FRONTEND_TARGET}: $(FRONTEND_SRC)
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) $(FRONTEND_SRC) -o $(FRONTEND_TARGET) ${LDFLAGS}
	
${CLIENT_TARGET}: ${CLIENT_SRC}
	g++ $(FLAGS) -I$(LOG_LIBRARY_HEADERS) $(CLIENT_SRC) -o $(CLIENT_TARGET) ${LDFLAGS}