#ifndef stats_h
#define stats_h
    #include<semaphore.h>
    
    #define numProc 16
    
    typedef struct {
    // You may add any new fields that you believe are necessary
    int inUse; 
    int pid;        // Do not remove or change
    int counter;    // Do not remove or change
    int priority;   // Do not remove or change
    double cpu_secs; // Do not remove or change
    char arg[16]; 
    // You may add any new fields that you believe are necessary
    } stats_t;

    typedef struct {
    sem_t *sem;
    stats_t stats[numProc];
    } scaff;
    
#endif