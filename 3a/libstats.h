#ifndef libstats_h
#define libstats_h

  #include"stats.h"
  /*
    Attch to existing shared memory segment using key
      if sucessful
        return pointer to shared memory segment in process
      else
        return NULL
  */
  extern stats_t* stats_init(key_t key);

  /*

  */
  extern int stats_unlink(key_t key);

  extern int semInit(key_t key);

  extern int semDel(key_t key);
  
  extern sem_t *sem;
  
#endif
