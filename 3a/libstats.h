#ifndef libstats_h
#define libstats_h
  /*
    Attch to existing shared memory segment using key
      if sucessful
        return pointer to shared memory segment in process
      else
        return NULL
  */
  extern stats_t* stat_init(key_t key);

  /*

  */
  extern int stats_unlink(key_t key);

#endif
