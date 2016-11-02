all:
	gcc -c -fpic libstats.c -Wall -Werror
	gcc -shared -o libstats.so libstats.o
	gcc -o stats_client stats_client.c -Wall -Werror -lstats -L. -pthread -lm
	gcc -o stats_server stats_server.c -Wall -Werror -lstats -L. -pthread -lm

clean: 
	rm *o $(EXECUTABLE)