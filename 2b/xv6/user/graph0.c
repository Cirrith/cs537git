#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#include "fcntl.h"

int workload(int n) {
  int i, j = 0;
  for (i = 0; i < n; i++) {
    j += i * j + 1;
  }
  return j;
}


  int
main(int argc, char *argv[])
{
  int i;
  //char msg[512];

  if(argc > 2){
    printf(2, "Usage: graph0\n");
    exit();
  }
  
  for(i = 0; i < 3; i++) {  // Process of 3 high usage processes
    int c_pid = fork();
    // Child
    if (c_pid == 0) {
      workload(40000000);
      exit();
    }
  }

  exit();
}
