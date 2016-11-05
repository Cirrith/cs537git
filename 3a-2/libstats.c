#include<unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include"libstats.h"
#include<stdio.h>
#include<signal.h>
#include<fcntl.h>

scaff* getMem(key_t);

char name[] = "bambrough";
scaff *shm;
sem_t *sem;
char semKey[128];

stats_t* stats_init(key_t key) {
  stats_t *stat;

  sprintf(semKey, "%s%d", name, (int)key);
  if ((sem = sem_open(semKey, O_RDWR)) == SEM_FAILED) {
    return NULL;
  }

  if ((shm = getMem(key)) == NULL) {
    return NULL;
  }

  if (sem_wait(sem) < 0) {
    shmdt(shm);
    return NULL;
  }

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
  stats_t *stat;
  int pid = getpid();

  if (sem_wait(sem) < 0) {
    shmdt(shm);
    return -1;
  }

  for (stat = shm->stats; stat < &shm->stats[numProc]; stat++) {
    if (stat->pid == pid) {
      // stat->inUse = 0;
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

// Get shared memory id
  if ((shmid = shmget(key, pgSize, S_IWUSR | S_IRUSR)) == -1) {
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
  sprintf(semKey, "%s%d", name, (int)key);
  if ((sem = sem_open(semKey, O_CREAT, 0644, 1)) == SEM_FAILED) {
    return -1;
  }
  return 0;
}

int semDel(key_t key) {
  sem_post(sem);
  if (sem_close(sem) < 0) {
      return -1;
  }
  if (sem_unlink(semKey) < 0) {
      return -1;
  }
  return 0;
}
