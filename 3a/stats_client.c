#include "libstats.h"
#include "stats.h"

int main (int argc, char argv[]) {

    int reasonableDefaultValue = 1000; //Change this based on default value if a flag isnt set
    int key;
    int priority;
    int sleeptime_ns = resonableDefaultValue;
    int cputime_ns = resonableDefaultValue;

    int c;

    while ((c == getopt(argc, argv, "k:p:s:c:") != -1)) {
        switch(c) {
            case 'k':
                key = optarg;
                break;
            case 'p':
                priority = optarg;
                break;
            case 's':
                sleeptime_ns = optarg;
                break;
            case 'c':
                cputime_ns = optarg;
                break;
            default:
                perror("Wrong Args specified");
                exit(1);
        }
    }

    //Check to see if args are bad
    if(sleeptime_ns < 0 || cputime_ns < 0)
        exit(1);

    //Get point to key
    //TODO call the library thing


    while(1){
        //TODO SLEEP/CALC EVERY LOOP
        //TODO INCREMENT STATS_T everytime

        //TODO If get Ctrl-c
        //
    }
}
