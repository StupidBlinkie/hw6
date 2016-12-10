# define the commands we will use for compilation
CC = gcc
CXX = g++

# where the socket source code lives
SOCKDIR = ../socketCode


CFLAGS = -g -Wall -Wpedantic -O0 -I$(SOCKDIR)
LDFLAGS =  -L../hw2 -L../jansson/lib -lhw2 -l:libjansson.a
IFLAGS =  -I../hw2 -I../jansson/include
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`
LIBS=../hw2/libhw2.a


# define common dependencies
HEADERS = $(SOCKDIR)/CSE333.h $(SOCKDIR)/CSE333Socket.h 



OBJS = $(SOCKDIR)/CSE333Socket.o $(SOCKDIR)/Verify333.o $(SOCKDIR)/ClientSocket.o


all: hw6-client hw5_server
hw5_server:  hw5_server.o $(OBJS) $(SOCKDIR)/ServerSocket.o
	cd $(SOCKDIR) && make
	$(CXX) $(CFLAGS) $(IFLAGS) $(GTKCFLAGS) $(SOCKDIR)/ServerSocket.o hw5_server.o $(OBJS) $(GTKLDFLAGS) $(LDFLAGS) -o hw5_server

hw6-client:  hw6-client.o $(OBJS) $(LIBS)
	cd $(SOCKDIR) && make
	$(CXX) $(CFLAGS) hw6-client.o $(OBJS) $(IFLAGS) $(LDFLAGS) -o hw6-client

%.o: %.cc $(HEADERS)
	$(CXX) -std=c++11 $(CFLAGS) $(IFLAGS) $(GTKCFLAGS) $(LDFLAGS) -c $<

%.o: %.c $(HEADERS)
	$(CC) -std=c11 $(CFLAGS) $(IFLAGS) $(GTKCFLAGS) $(LDFLAGS) -c -std=c11 $<

clean: FORCE
	/bin/rm -f *.o *~  hw6-client hw5_server

FORCE:
