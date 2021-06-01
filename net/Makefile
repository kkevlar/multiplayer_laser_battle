# Kevin Kellar's Makefile
# 2021

CC= gcc
CXX= g++
FLAGS= -g -Wall -O2 -Werror 
CFLAGS= $(FLAGS) -std=gnu99 
CXXFLAGS= $(FLAGS) -std=gnu++11 
LIBS = 

BINARY_TARGETS = simple_ll_netlib.a server

all: $(BINARY_TARGETS) 

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIBS)

simple_ll_netlib.a: simple_ll_netlib_client.o fromserver_client.o networks.o gethostbyname6.o handle.o packet.o message.o compat.o fdselect.o
	ar rcs $@ $^ 

server: main_server.cpp incoming.o networks.o gethostbyname6.o handle.o packet.o message.o data_structure.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o *.a 

clean: cleano
	rm -f $(BINARY_TARGETS) 


# Kevin Kellar's Makefile
# 2021