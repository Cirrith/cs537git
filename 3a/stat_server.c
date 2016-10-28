#include"stats.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<semaphore.h>
#include<string.h>

static int keepRunning = 1;

void INThandler(int sig) {
  keepRunning = 0;
}

int main(int argc, char argv[]) {
  int c;
  int opterr = 0;
  int key = 0;
  int servIt = 1;  
  // Parse Command Inputs

  if(argc > 3) {
    printf("Usage: %s -k <key>", argv[0]);
    exit(1);
  }

  while ((c = getopt (argc, argv, "k:")) != -1) {
    switch(c) {
      case 'k':
        key = atoi(optarg);
        break;
      default:
        exit(1);
    }
  }

  if (key == 0) {
    printf("Invalid MemNumber");
  }

// Setup Interrupt Handler
  signal(SIGINT, INThandler)
  
// Setup Shared Memory

  int pgSize = getpagesize();
  int shmid;
  scaff *shm;

  if ((shmid = shmget(key, pgSize, IPC_CREAT)) == -1) {  // Create shared memory segment
    perror("shmget");
    exit(1);
  }

  if ((shm = shmat(shmid, NULL, 0)) == (scaff *) -1) {  // Get pointer to memory segment
    perror("shmat");
    exit(1);
  }

// Setup Shared Memory Data (Semaphore, Table)

  // Init Data Inside Shared Memory

  void *memset(shm, 0, sizeof(scaff));
  
  // Init Semaphore at top of shared memory
  if ((shm->sem = sem_open("mysem", O_CREAT, 0644, 1)) == SEM_FAILED) {
    perror("sem_open");
    exit(1);
  }

// Every 1 second go through memory and print out
  stats_t stat;
  while (keepRunning) {
    for (stat = shm->stats; stat < &shm->stats[numProc]; stat++) {
      if (stat->inUse) {
        printf("%d %d %s %d %f %d\n", servIt, stat->pid, stat->arg, stat->counter, stat->cpu_secs, stat->priority);
      }
    }
    servIt++;
  }

// Mark Shared Memory Segment for Deletion
  shmdt(shm); 
  shmctl(shmid, IPC_RMID, 0);

// Remove Semaphore
  if(sem_unlink("mysem")) {
    perror("sem_unlink");
    exit(1);
  }

}

















