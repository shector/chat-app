CXX = clang++
CXXFLAGS = -Wall -g


default: all

all: util.o server_example.o client_example.o 
	$(CXX) $(CXXFLAGS) -o server -pthread server_example.o util.o
	$(CXX) $(CXXFLAGS) -o client client_example.o util.o
	$(RM) *.o 

# once the binary is built i don't need .o clogging up my directory.
server:	server_example.o util.o
	$(CXX) $(CXXFLAGS) -o server server_example.o util.o
	$(RM) *.o 

client: client_example.o util.o
	$(CXX) $(CXXFLAGS) -o client client_example.o util.o
	$(RM) *.o 

server_example.o:	server_example.cc
	$(CXX) $(CXXFLAGS) -c server_example.cc -pthread

client_example.o: client_example.cc
	$(CXX) $(CXXFLAGS) -c client_example.cc

util.o:	util.cc util.h
	$(CXX) $(CXXFLAGS) -c util.cc

clean:
	$(RM) server client *.o 