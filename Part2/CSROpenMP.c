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
    float value;
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

    //COO arrays
    float  coo_val[NNZ];
    int    coo_row[NNZ];
    int    coo_col[NNZ];
    int    counter = 0;

    while ((read = getline(&line, &len, fptr)) != -1) {
        token = strtok(line, " ");
        for(i = 0; i < 3; i++){
            if(i == 0){
                rowInd = atoi(token);
                coo_row[counter] = rowInd;
            }
            if(i==1){
                colInd = atoi(token);
                coo_col[counter] = colInd;
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
    float csr_val[NNZ];
    int   csr_col[NNZ];
    int   csr_row[NNZ + 1];

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

    float X[NNZ], Y[NNZ];
    
    //Initialize X to 1
    for (i = 0; i < NNZ; ++i) {
        X[i] = 1.0;
    }
    
    start = omp_get_wtime(); //start time measurement
    
     
    for(loop = 0; loop < numIter; loop++){
        #pragma omp parallel for shared(csr_row,csr_col,X,Y) private(i,j)
            for (i = 0; i < NNZ; ++i) {
                
                Y[i] = 0.0;
                for (j = csr_row[i]; j < csr_row[i+1]; ++j){
                    Y[i] += csr_val[j] * X[csr_col[j]];
                }
            }
            memcpy(X, Y, sizeof(X));
    }
    end = omp_get_wtime(); //end time measurement
    printf("Time of computation: %f seconds\n", end-start);

    fclose(fptr);
    if (line)
        free(line);
    return(0);
}