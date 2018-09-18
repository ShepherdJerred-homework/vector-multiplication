#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

const int MAIN_PID = 0;

void getInput(int pid, int* m, int* n, double** vector, double** matrix) {
    if (pid == MAIN_PID) {
        scanf("%i", m);
    }

    if (pid == MAIN_PID) {
        scanf("%i", n);
    }

    // TODO broadcast

    if (pid == MAIN_PID) {
        *vector = calloc(*n, sizeof(double));

        int i;
        for (i = 0; i < *n; i++) {
            scanf("%lf", vector[i]);
        }
    }

    // TODO broadcast

    if (pid == MAIN_PID) {
        int numberOfNumbersInMatrix = *n * *m;
        *matrix = calloc(numberOfNumbersInMatrix, sizeof(double));

        int i;
        for (i = 0; i < numberOfNumbersInMatrix; i++) {
            // TODO pointer arithmetic
        }
    }

    // TODO broadcast
}

void calculateColumnsPerProcess(int numberOfProcesses, int n, int* numberOfColumns) {
    *numberOfColumns = numberOfProcesses / n;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    int numberOfProcesses;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

    int m;
    int n;
    double* vector;
    double* matrix;

    int numberOfColumns;

    getInput(pid, &m, &n, &vector, &matrix);
    calculateColumnsPerProcess(numberOfProcesses, n, &numberOfColumns);

    MPI_Finalize();
}
