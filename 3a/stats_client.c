#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include "libstats.h"
#include "stats.h"
#include<math.h>
#include<time.h>
#include<sys/resource.h>
#include<stdlib.h>

int key;

void INThandler(int sig) {
    if (stats_unlink(key) < 0) {
        perror("Error Unlinking");
        exit(1);
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    // Change this based on default value if a flag isnt set
    int reasonableDefaultValue = 1000;
    int priority;
    int sleeptime_ns = reasonableDefaultValue;
    int cputime_ns = reasonableDefaultValue;
    int c;

    stats_t *stat;

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

    // Get point to key
    stat = stat_init(key);
    if (stat == NULL) {
      printf("stat_init Failure\n");
      exit(1);
    }

    // Initalize all of the stat_t fields
    stat->pid = getpid();
    stat->counter = 0;
    stat->priority = priority;
    stat->cpu_secs = cputime_ns;
    // TOD set argv[0] in stats_t, is it supposed to be in char arg[16]???
    // stat->arg = argv[0]

    // Register Interrupt Handler
    signal(SIGINT, INThandler);

    // time Structs
    struct timespec start, end, cpuStart, cpuEnd;
    int returnPriority, endTime;
    // TOD is this the right one? could also be PRIO_PGRP or PRIO_USER
    int which = PRIO_PROCESS;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpuStart);

    // set priority
    returnPriority = setpriority(which, getpid(), priority);
    if (returnPriority < 0) {
        perror("Error Setting priority");
        exit(1);
    }

    while (1) {
        // Sleep for specific amount of time
        sleep(sleeptime_ns);

        // Do Calc for nanosecond time
        clock_gettime(CLOCK_MONOTONIC, &start);
        endTime = 0;
        while (endTime - start.tv_nsec < cputime_ns) {
            clock_gettime(CLOCK_MONOTONIC, &end);
            endTime = end.tv_nsec;
        }
        returnPriority = getpriority(which, getpid());
        if (returnPriority < 0) {
            perror("Error getting priority");
            exit(1);
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpuEnd);
        // Should go to two decimal places, takes nanoseconds divides by 1e7,
        // rounds that, then divides by the 100
        stat->cpu_secs = round((cpuEnd.tv_nsec-cpuStart.tv_nsec)/10000000)/100;
        stat->priority = returnPriority;

        stat->counter = stat->counter + 1;
    }
}
