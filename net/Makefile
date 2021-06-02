# Kevin Kellar's Makefile
# 2021

CC= gcc
CXX= g++
FLAGS= -g -Wall -O2 -Werror 
CFLAGS= $(FLAGS) -std=gnu99 
CXXFLAGS= $(FLAGS) -std=gnu++11 
LIBS = 
SERVOBJS= log.o incoming.o networks.o gethostbyname6.o handle.o compat.o packet.o message.o data_structure.o 
PROBLEMHDRS= log.h compat.h packet.h handle.h message.h

BINARY_TARGETS = server

all: $(BINARY_TARGETS) 

%.o: %.cpp %.h $(PROBLEMHDRS)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIBS)

server: log.h main_server.cpp $(SERVOBJS) $(PROBLEMHDRS)
	$(CXX) $(CXXFLAGS) -o $@ main_server.cpp $(SERVOBJS) $(LIBS)

%.o: %.c %.h $(PROBLEMHDRS)
	$(CC) -c $(CFLAGS) $< -o $@ $(LIBS)

%.cpp: copy/%.cpp 
	cp $< $@

%.c: copy/%.c
	cp $< $@

%.h: copy/%.h
	cp $< $@

cleano:
	rm -f *.o *.a 

clean: cleano
	rm -f $(BINARY_TARGETS) 


# Kevin Kellar's Makefile
# 2021