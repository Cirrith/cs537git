#include<unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include"libstats.h"
#include<stdio.h>
#include<signal.h>
#include<fcntl.h>

scaff* getMem(key_t);

char name[] = "bambrough";

stats_t* stats_init(key_t key) {
  scaff *shm;
  stats_t *stat;

  if ((shm = getMem(key)) == NULL) {
    return NULL;
  }

  for (stat = shm->stats; stat < &shm->stats[numProc]; stat++) {
    if (stat->inUse == 0) {
      stat->inUse = 1;
      return stat;
    }
  }
  return NULL;
}

int stats_unlink(key_t key) {
  scaff *shm;
  stats_t *stat;
  int pid = getpid();

  if ((shm = getMem(key)) == NULL) {
    return -1;
  }

  for (stat = shm->stats; stat < &shm->stats[numProc]; stat++) {
    if (stat->pid == pid) {
      stat->inUse = 0;
      shmdt(shm);  // Remove shm from Address Space
      return 0;
    }
  }
  return -1;
}

// Given a key return a pointer to the memory
scaff* getMem(key_t key) {
  int pgSize = getpagesize();
  int shmid;
  scaff *shm;

  if ((shmid = shmget(key, pgSize, 0666)) == -1) {  // Get shared memory id
    printf("shmget fail\n");
    return NULL;
  }

// Get pointer to memory segment
  if ((shm = shmat(shmid, NULL, 0)) == (scaff *) -1) {
    printf("shmat fail\n");
    return NULL;
  }
  return shm;
}

int semInit(key_t key) {
  return 0;
}

int semDel(key_t key) {
  return 0;
}
