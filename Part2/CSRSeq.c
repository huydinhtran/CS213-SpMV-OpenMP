#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    omp_set_num_threads(8);
    argc == 4;
    FILE *fptr = fopen(argv[1],"r");
    int numIter, numThread;
    numIter = atoi(argv[2]);
    numThread = atoi(argv[3]);
    printf("%d and %d\n",numIter,numThread);
    double start, end; // used for timing



    // fclose(fptr); 
    return(0);
}