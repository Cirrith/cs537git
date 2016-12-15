#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"


int
main(int argc, char* argv[]) {

  int fd;
  struct stat statBuf;
  char *filepath;

  if (argc != 2) {
    exit();
  }

  filepath = argv[1];

  if ((fd = open(filepath, O_RDONLY)) < 0) {
    printf(2, "cannot open file");
    exit();
  }

  if ( fstat(fd, &statBuf) < 0) {
    printf(2, "Cannot get stats of file");
    exit();
  }

  printf(1, "Type: %d, Size: %d, Checksum: %d\n", statBuf.type, statBuf.size, statBuf.checksum);

  exit();
  

}
