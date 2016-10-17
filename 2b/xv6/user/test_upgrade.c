#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

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

  for (i = 0; i < 6; i++) {
    int c_pid = fork();
    // Child
    if (c_pid == 0) {
      workload(80000000);
      exit();
    }   
  }

  exit();
}
