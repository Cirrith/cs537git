

int main(int argc, char argv[]) {
  int c;
  int opterr = 0;
  int memNum = 0;
  
  // Parse Command Inputs

  if(argc > 3) {
    printf("Usage: %s -k <key>", argv[0]);
    exit(1);
  }

  while ((c = getopt (argc, argv, "k:")) != -1) {
    switch(c) {
      case 'k':
        memNum = atoi(optarg);
        break;
      default:
        exit(1);
    }
  }

  if(memNum == 0) {
    printf("Invalid MemNumber");
  }
  
  // Setup Shared Memory

  


  // Every 1 second go through memory and print out
  while(1) {


  }
}
