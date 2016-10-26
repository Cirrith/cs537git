typedef struct {
  // You may add any new fields that you believe are necessary
  int inUse; 
  int pid;        // Do not remove or change
  int counter;    // Do not remove or change
  int priority;   // Do not remove or change
  double cpu_secs; // Do not remove or change
  // You may add any new fields that you believe are necessary
} stats_t;

typedef struct {
  sem_t *sem;
  stats_t stats[numProc];
} scaff;

const int numProc = 16;
