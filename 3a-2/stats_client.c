#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<math.h>
#include<time.h>
#include<sys/resource.h>
#include<stdlib.h>
#include<string.h>  // strncpy()
#include<semaphore.h>  // sem_t
#include"libstats.h"

#define secNano 1000000000

sem_t *sem;
stats_t *stat;
int key;

void INThandler(int sig) {
    stat->inUse = 0;
    if (stats_unlink(key) < 0) {
      perror("unlink");
      exit(1);
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    // Change this based on default value if a flag isnt set
    int priority = 1;
    int sleeptime_ns = secNano/2;
    int cputime_ns = secNano/2;
    int c;
    struct sigaction sa;

    while ((c = getopt(argc, argv, "k:p:s:c:")) != -1) {
        switch (c) {
            case 'k':
                key = atoi(optarg);
                break;
            case 'p':
                priority = atoi(optarg);
                break;
            case 's':
                sleeptime_ns = atoi(optarg);
                break;
            case 'c':
                cputime_ns = atoi(optarg);
                break;
            default:
                perror("Wrong Args specified");
                exit(1);
        }
    }

// Check to see if args are bad
    if (sleeptime_ns < 0 || cputime_ns < 0) {
        exit(1);
    }

    int pid = getpid();

// Get point to key
    stat = stats_init(key);
    if (stat == NULL) {
      printf("stat_init Failure\n");
      exit(1);
    }

// Register Interrupt Handler
    sa.sa_handler = INThandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) != 0) {
      perror("sigaction");
      exit(1);
    }

// Setup Priority and Clock
    struct timespec cpuStart, cpuCurr, procStart;
    int currPriority;
    int prior = PRIO_PROCESS;
    int clock = CLOCK_PROCESS_CPUTIME_ID;

    if (setpriority(prior, getpid(), priority) < 0) {
      perror("setpriority");
      exit(1);
    }

    if ((currPriority = getpriority(prior, pid)) < 0) {
        perror("getpriority");
        exit(1);
    }
    clock_gettime(clock, &procStart);

// Initalize all of the stat_t fields
    stat->pid = pid;
    stat->counter = 0;
    stat->priority = currPriority;
    stat->cpu_secs = procStart.tv_nsec/secNano;  // Cutoff on server end
    strncpy(stat->arg, argv[0], 15);
    stat->arg[15] = '\0';

// Struct for sleeping
    struct timespec sleepSpec;  // sleepRem;
    sleepSpec.tv_sec = sleeptime_ns / secNano;
    sleepSpec.tv_nsec = sleeptime_ns % secNano;
    // int notDoneSleeping = 0;
    while (1) {
    // Sleep for specific amount of time
      nanosleep(&sleepSpec, 0);

      clock_gettime(clock, &cpuStart);
      cpuCurr = cpuStart;

      while (((cpuCurr.tv_sec - cpuStart.tv_sec)*secNano +
             cpuCurr.tv_nsec - cpuStart.tv_nsec) < cputime_ns) {
        clock_gettime(clock, &cpuCurr);
            // printf("Diff: %ld\n", cpuCurr. - cpuStart.tv_nsec);
      }

    // Get Current Priority
      if ((currPriority = getpriority(prior, pid)) < 0) {
        perror("getpriority");
          exit(1);
      }

    // Get current cpu time
        clock_gettime(clock, &cpuCurr);

    // Update shared memory
        stat->cpu_secs += (double) ((cpuCurr.tv_sec - cpuStart.tv_sec)*secNano +
                                    cpuCurr.tv_nsec - cpuStart.tv_nsec)/secNano;
        stat->priority = currPriority;
        stat->counter++;
    }
}
