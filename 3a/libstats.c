#include<unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include"libstats.h"
#include<stdio.h>

scaff* getMem(key_t);

stats_t* stats_init(key_t key) {
  scaff *shm;
  stats_t *stat;
  shm = getMem(key);

  if (shm == NULL) {
    return NULL;
  }
  
  sem_wait(shm->sem);
  
  for (stat = shm->stats; stat < &shm->stats[numProc]; stat++) {
    if (stat->inUse == 0) {
      stat->inUse = 1;
      sem_post(shm->sem);
      return stat;
    }
  }
  sem_post(shm->sem);

  return NULL;
}

int stats_unlink(key_t key) {
  // If can remove return 0
  // Else return -1
  scaff *shm;
  stats_t *stat;
  int pid;
  
  shm = getMem(key);

  if (shm == NULL) {
    return -1;
  }

  pid = getpid();
  
  sem_wait(shm->sem);  // Don't think this is critical section
  for (stat = shm->stats; stat < &shm->stats[numProc]; stat++) {
    if (stat->pid == pid) {
      stat->inUse = 0;
      shmdt(shm);  // Remove shm from Address Space
      sem_post(shm->sem);
      return 0;
    }
  }
  sem_post(shm->sem);
  return -1;
}

// Given a key return a pointer to the memory
scaff* getMem(key_t key) {
  int pgSize = getpagesize();
  int shmid;
  scaff *shm;

  if ((shmid = shmget(key, pgSize, 0666)) == -1) {  // Get shared memory id
    return NULL;
  }

  // Get pointer to memory segment
  if ((shm = shmat(shmid, NULL, 0)) == (scaff *) -1) {
    return NULL;
  }
  return shm;
}
