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

  printf(1, "pid: %d", data.pid[0]);
  exit();
}
