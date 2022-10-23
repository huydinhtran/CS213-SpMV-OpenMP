#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    argc == 4;

    FILE *fptr = fopen(argv[1],"r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    double start, end; // used for timing

    int numIter, numThread;
    numIter = atoi(argv[2]);
    numThread = atoi(argv[3]);
    omp_set_num_threads(numThread);

    printf("Matrix: %s\n",argv[1]); 
    printf("Iteration: %d\n",numIter);
    printf("Thread count: %d\n",numThread);
    
    char *token;
    int rowInd, colInd;
    double value;
    int firstRow = 1;
    int rowNum, colNum, NNZ, i, j, k, loop;

    read = getline(&line, &len, fptr);
    token = strtok(line, " ");
    for(i = 0; i < 3; i++){
        if(i == 0){
            rowNum = atoi(token);
        }
        if(i == 1){
            colNum = atoi(token);
        }
        if(i == 2){
            NNZ = atof(token);
        }
        token = strtok (NULL, " ");
    }
    
    printf("Row: %d, Column: %d, NNZ:%d\n",rowNum,colNum,NNZ);

    //COO arrays
    double  *coo_val = malloc(NNZ * sizeof(double));
    int    *coo_row = malloc(NNZ * sizeof(int));
    int    *coo_col = malloc(NNZ * sizeof(int));
    int    counter = 0;
    
    while ((read = getline(&line, &len, fptr)) != -1) {
        token = strtok(line, " ");
        for(i = 0; i < 3; i++){
            if(i == 0){
                rowInd = atoi(token);
                coo_row[counter] = rowInd-1;
            }
            if(i==1){
                colInd = atoi(token);
                coo_col[counter] = colInd-1;
            }
            if(i==2){
                value = atof(token);
                coo_val[counter] = value;
            } 
            token = strtok (NULL, " ");
        }
        
        counter++;
        
    }

    //CSR arrays
    double *csr_val = malloc(NNZ     * sizeof(double));
    int    *csr_col = malloc(NNZ     * sizeof(int));
    int    *csr_row = malloc((NNZ+1) * sizeof(int));

    for (i = 0; i < NNZ; i++)
    {
        csr_val[i] = 0;
        csr_col[i] = 0;
        csr_row[i] = 0;
    }
    csr_row[NNZ] = 0;

    for (i = 0; i < NNZ; i++)
    {
        csr_val[i] = coo_val[i];
        csr_col[i] = coo_col[i];
        csr_row[coo_row[i] + 1]++;
    }
    for (i = 0; i < NNZ; i++)
    {
        csr_row[i + 1] += csr_row[i];
    }

    //CSC arrays, converted from CSR
    double *csc_val  = malloc(NNZ     * sizeof(double));
    int    *csc_col  = malloc((NNZ+1) * sizeof(int));
    int    *csc_row  = malloc(NNZ     * sizeof(int));
    int    *curr     = malloc(NNZ     * sizeof(int));

    for (i = 0; i < NNZ; i++)
    {
        csc_val[i] = 0;
        csc_col[i] = 0;
        csc_row[i] = 0;
        curr[i]    = 0;
    }
    csr_col[NNZ+1] = 0;

    for(i = 0; i < NNZ+1; i++){
        for(j = csr_row[i]; j < csr_row[i+1]; j++){
            csc_col[csr_col[j] + 1]++;
        }
    }
    for(i = 1; i < NNZ + 1; i++){
        csc_col[i] += csc_col[i-1];
    }
    int loc;
    for(i = 0; i < NNZ; i++){
        for(j = csr_row[i]; j < csr_row[i+1]; j++){
            loc = csc_col[csr_col[j]] + curr[csr_col[j]]++;
            csc_row[loc] = i;
            csc_val[loc] = csr_val[j];
        }
    }

    double *X = malloc(rowNum * sizeof(double));
    double *Y = malloc(rowNum * sizeof(double));
    
    //Initialize X to 1
    for (i = 0; i < rowNum; ++i) {
        X[i] = 1.0;
    }
    
    start = omp_get_wtime(); //start time measurement
    
    for (i = 0; i < rowNum; ++i) {
            Y[i] = 0.0;
    }

    for(loop = 0; loop < numIter; loop++){
        for (i = 0; i < rowNum; ++i) {
            for (j = csc_col[i]; j < csc_col[i+1]; ++j){
                Y[csc_row[j]] += csc_val[j] * X[i];
            }
        }
        memcpy(X, Y, sizeof(double)*rowNum);
        for (i = 0; i < rowNum; ++i) {
            Y[i] = 0.0;
        }
    }
    end = omp_get_wtime(); //end time measurement
    printf("Time of computation: %f seconds\n", end-start);

    FILE *fwrite;
    
    if(strcmp(argv[1], "matrix1.txt") == 0){
        fwrite = fopen ("CSCVec1.txt", "w");
        for(i = 0; i < rowNum; i++){
            fprintf(fwrite, "%.13e\n", X[i]);
        }
    }
    if(strcmp(argv[1], "matrix2.txt") == 0){
        fwrite = fopen ("CSCVec2.txt", "w");
        for(i = 0; i < rowNum; i++){
            fprintf(fwrite, "%.13e\n", X[i]);
        }
    }
    if(strcmp(argv[1], "matrix3.txt") == 0){
        fwrite = fopen ("CSCVec3.txt", "w");
        for(i = 0; i < rowNum; i++){
            fprintf(fwrite, "%.13e\n", X[i]);
        }
    }
    
    return(0);
}