#Makefile for CS537

CC=gcc
CFLAGS=-c -Wall -Werror
LDFLAGS= -lstats -L
SOURCES=stat_server.c
LIB=libstats.c
DEPS=libstats.h stats.h
OBJECTS=$(SOURCES:.c=.o)
LIBRARY=$(LIB:.c=.so)
EXECUTABLE=main

all: $(SOURCES) $(DEPS) $(LIB) $(EXECUTABLE) 
    
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.c.so:
  $(CC) $(CFLAGS) -fpic $< -o $@
  $(CC) -shared  $

clean: 
	rm *o $(EXECUTABLE)
