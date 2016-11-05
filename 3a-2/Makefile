#Makefile for CS537

CC=gcc
EXFLAGS= -Wall -Werror -lstats -L. -pthread -lm
LIBFLAGS= -shared

all: LIBRARY SERVER CLIENT

debug: EXFLAGS += -g
debug: LIBFLAGS += -g
debug: all

SERVER: stats_server.c
	$(CC) -o stats_server $< $(EXFLAGS)

CLIENT: stats_client.c
	$(CC) -o stats_client $< $(EXFLAGS)

LIBRARY: libstats.c
	$(CC) -c $< -Wall -Werror -fpic -o libstats.o
	$(CC) $(LIBFLAGS) -o libstats.so libstats.o

clean: 
	rm *o stats_server stats_client