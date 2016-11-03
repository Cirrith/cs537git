#include<unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include"libstats.h"
#include<stdio.h>
#include<signal.h>
#include<fcntl.h>

scaff* getMem(key_t);
sem_t *sem;

char semKey[] = "bambrough";

stats_t* stats_init(key_t key) {
  scaff *shm;
  stats_t *stat;
  shm = getMem(key);

  if (shm == NULL) {
    return NULL;
  }
  
  if((sem = sem_open(semKey, O_RDWR)) == SEM_FAILED) {
	return NULL;
  }
  
  printf("%p\n", sem);
  sem_wait(sem);
  printf("Got Here\n");
  for (stat = shm->stats; stat < &shm->stats[numProc]; stat++) {
    if (stat->inUse == 0) {
      stat->inUse = 1;
      sem_post(sem);
      return stat;
    }
  }
  sem_post(sem);

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
  
  sem_wait(sem);  // Don't think this is critical section
  
  printf("Got Here\n");
  for (stat = shm->stats; stat < &shm->stats[numProc]; stat++) {
    if (stat->pid == pid) {
      stat->inUse = 0;
      shmdt(shm);  // Remove shm from Address Space
      sem_post(sem);
      return 0;
    }
  }
  sem_post(sem);
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

int semInit(void){
  // Init Semaphore at top of shared memory
  if ((sem = sem_open("bambrough3", O_CREAT, 0666, 1)) == SEM_FAILED) {
    return -1;
  }
  return 0;
}

int semDel(void) 
{  
    // Remove Semaphore
    if (sem_unlink("bambrough3")) {
        return -1;
    }
    return 0;
}
