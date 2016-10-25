#ifndef libstat_h
#define libstat_h
  /*
    Attch to existing shared memory segment using key
      if sucessful
        return pointer to shared memory segment in process
      else
        return NULL
  */
  extern stat_t* stat_init(key_t key);

  /*

  */
  extern int stat_unlink(key_t key);

#endif
