#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<semaphore.h>
#include<string.h>
#include<signal.h>
#include<fcntl.h>
#include"stats.h"
#include"libstats.h"

static int keepRunning = 1;

void INThandler(int sig) {
  keepRunning = 0;
}

int main(int argc, char *argv[]) {
  int c;
  char *arg;
  int key = 0;
  int servIt = 1;
  // Parse Command Inputs

  if (argc != 3) {
    printf("Usage: %s -k <key>\n", argv[0]);
    exit(1);
  }

  while ((c = getopt(argc, argv, "k:")) != -1) {
    switch (c) {
      case 'k':
        arg = optarg;
        break;
      default:
        exit(1);
    }
  }

  if ((key = atoi(arg)) ==  0) {
    printf("Invalid MemNumber");
  }

// Setup Interrupt Handler
  signal(SIGINT, INThandler);

// Setup Shared Memory
  int pgSize = getpagesize();
  int shmid;
  scaff *shm;

  // Create shared memory segment
  if ((shmid = shmget(key, pgSize, IPC_EXCL | IPC_CREAT | 0644)) < 0) {
    perror("shmget");
    exit(0);
  }

  // Get pointer to memory segment
  if ((shm = shmat(shmid, NULL, 0)) < (scaff *) 0) {
    perror("shmat");
    exit(0);
  }

// Setup Shared Memory Data (Semaphore, Table)

  // Init Data Inside Shared Memory
  memset(shm, 0, sizeof(scaff));

  if (semInit() < 0) {
    perror("sem_open");
    exit(1);
  }

// Every 1 second go through memory and print out
  stats_t *stat;
  while (keepRunning) {
    sleep(1);
    // printf("Checking ...\n");
    for (stat = shm->stats; stat < &shm->stats[numProc]; stat++) {
      if (stat->inUse) {
        printf("%d %d %s %d %.2f %d\n", servIt, stat->pid, stat->arg,
          stat->counter, stat->cpu_secs, stat->priority);
      }
    }
    servIt++;
    printf("\n");
  }

// Mark Shared Memory Segment for Deletion
  shmctl(shmid, IPC_RMID, 0);

  if (semDel() < 0) {
    perror("sem_unlink");
    exit(1);
  }

  return 0;
}
