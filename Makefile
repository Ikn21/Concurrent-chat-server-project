CPPFLAGS := -Wall -Wextra -std=c++17 -g -pthread

all: server client

server: server.o Monitor.o
	g++ $(CPPFLAGS) -o server server.o Monitor.o

server.o: server.cpp Monitor.hpp
	g++ $(CPPFLAGS) -c server.cpp

Monitor.o: Monitor.cpp Monitor.hpp
	g++ $(CPPFLAGS) -c Monitor.cpp

client: client.cpp
	g++ $(CPPFLAGS) -o client client.cpp

clean:
	rm -f server client