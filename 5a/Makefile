#Makefile for CS537

CC=gcc
EXFLAGS= -Wall -Werror -lm

all: FSCHECK

debug: EXFLAGS += -g
debug: all

FSCHECK: fscheck.c
	$(CC) -o fscheck $< $(EXFLAGS)

clean: 
	rm *o fscheck
