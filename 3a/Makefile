#Makefile for CS537

CC=gcc
CFLAGS=-c -Wall -Werror
LDFLAGS= -lstats -L. -pthread
SOURCES=stat_server.c
LIB=libstats.c
DEPS=libstats.h stats.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(SOURCES) $(DEPS) LIBRARY $(EXECUTABLE) CLIENT

debug: CFLAGS += -g
debug: all

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

CLIENT: stats_client.c
  $(CC) -Wall -Werror $<

LIBRARY: $(LIB)
	$(CC) $(CFLAGS) $(LIB) -fpic -o libstats.o
	$(CC) -shared -o libstats.so libstats.o
.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm *o $(EXECUTABLE)
