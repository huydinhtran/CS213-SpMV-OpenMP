#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 500

int main(int argc, char **argv) {
    omp_set_num_threads(4);//set number of threads here
    int i, j, k;
    double sum;
    double start, end; // used for timing
    double A[N][N], B[N][N], C[N][N];

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = j*1;
            B[i][j] = i*j+2;
            C[i][j] = j-i*2;
        }
    }
    
    double sumTime;
    for(int n = 0; n < 10; n++){ //loop 10 times and get average of the execution time
        start = omp_get_wtime(); //start time measurement
        for (i = 0; i < N; i++) {
            #pragma omp parallel for shared(A,B,C) private(j,k,sum)
                for (j = 0; j < N; j++) {
                    sum = 0;
                    for (k=0; k < N; k++) {
                        sum += A[i][k]*B[k][j];
                    }
                    C[i][j] = sum;
                }
        }

        end = omp_get_wtime(); //end time measurement
        sumTime += end-start;
        printf("Time of computation of iter %d: %f seconds\n", n, end-start);
    }

    printf("Average computation time over 10 runs: %f seconds\n", sumTime/10);
    return(0);
}