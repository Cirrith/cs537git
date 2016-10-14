#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

int
main(int argc, char *argv[])
{
  if(argc > 2){
    printf(2, "Usage: mkdir\n");
    exit();
  }

  struct pstat data;

  getpinfo(&data);

  int i;
  for (i = 0; i < NPROC; i++) {
    printf(1, "Use: %d, PID: %d, Priority: %d, State: %d, Tick0: %d, Tick1: %d, Tick2: %d, Tick3: %d\n", data.inuse[i], data.pid[i], data.priority[i], data.state[i], data.ticks[i][0], data.ticks[i][1], data.ticks[i][2], data.ticks[i][3]);
  }
  exit();
}
