#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

const int MAIN_PID = 0;

struct Params {
    int m;
    int n;
};

// https://stackoverflow.com/questions/10788180/sending-columns-of-a-matrix-using-mpi-scatter
// https://www.mpich.org/static/docs/v3.2/www3/MPI_Type_vector.html
void createVectorType(int m, int n, MPI_Datatype *newType) {
    MPI_Type_vector(m, 1, n, MPI_DOUBLE, newType);
    MPI_Type_create_resized(*newType, 0, sizeof(double), newType);
    MPI_Type_commit(newType);
}

// https://stackoverflow.com/questions/20040663/how-to-create-new-type-in-mpi
// https://stackoverflow.com/questions/9864510/struct-serialization-in-c-and-transfer-over-mpi
void createParamsType(MPI_Datatype *newType) {
    int count = 2;
    int blocklens[] = {1, 1};
    MPI_Aint indices[2];

    indices[0] = (MPI_Aint) offsetof(
    struct Params, m);
    indices[1] = (MPI_Aint) offsetof(
    struct Params, n);

    MPI_Datatype mpiTypes[] = {MPI_INT, MPI_INT};

    MPI_Type_create_struct(count, blocklens, indices, mpiTypes, newType);
    MPI_Type_commit(newType);
}

void calculateNumberOfNumbersInMatrix(int m, int n, int *numberOfNumbersInMatrix) {
    *numberOfNumbersInMatrix = m * n;
}

void getInput(int *m, int *n, double **vector, double **matrix) {
    scanf("%i", m);
    scanf("%i", n);

    *vector = calloc((size_t) * m, sizeof(double));

    int i;
    for (i = 0; i < *m; i++) {
        scanf("%lf", (*vector + i));
    }

    int numberOfNumbersInMatrix;
    calculateNumberOfNumbersInMatrix(*m, *n, &numberOfNumbersInMatrix);

    *matrix = calloc((size_t) numberOfNumbersInMatrix, sizeof(double));

    for (i = 0; i < numberOfNumbersInMatrix; i++) {
        scanf("%lf", (*matrix + i));
    }
}

void calculateColumnsPerProcess(int numberOfProcesses, int n, int *numberOfColumns) {
    *numberOfColumns = n / numberOfProcesses;
}

void calculateNumberOfNumbersPerProcess(int m, int numberOfColumnsPerProcess, int *numberOfNumbersPerProcess) {
    *numberOfNumbersPerProcess = m * numberOfColumnsPerProcess;
}

void broadcastParams(int *m, int *n) {
    MPI_Datatype paramsType;
    createParamsType(&paramsType);
    struct Params params;
    params.m = *m;
    params.n = *n;
    MPI_Bcast(&params, 1, paramsType, MAIN_PID, MPI_COMM_WORLD);
    *m = params.m;
    *n = params.n;
}

void broadcastVector(int pid, int m, double **vector) {
    if (pid != MAIN_PID) {
        *vector = calloc((size_t) m, sizeof(double));
    }
    MPI_Bcast(*vector, m, MPI_DOUBLE, MAIN_PID, MPI_COMM_WORLD);
}

void
broadcastMatrix(int m, int n, int numberOfNumbersPerProcess, int numberOfColumnsPerProcess, double *matrix,
                double **myMatrix) {
    MPI_Datatype vectorType;
    createVectorType(m, n, &vectorType);
    *myMatrix = calloc((size_t) numberOfNumbersPerProcess, sizeof(double));
    MPI_Scatter(matrix, numberOfColumnsPerProcess, vectorType, *myMatrix, numberOfNumbersPerProcess, MPI_DOUBLE,
                MAIN_PID, MPI_COMM_WORLD);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    int numberOfProcesses;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

    int m;
    int n;
    double *vector;
    double *matrix;

    double *myMatrix;

    int numberOfColumnsPerProcess;
    int numberOfNumbersPerProcess;

    if (pid == MAIN_PID) {
        getInput(&m, &n, &vector, &matrix);
    }

    broadcastParams(&m, &n);

    calculateColumnsPerProcess(numberOfProcesses, n, &numberOfColumnsPerProcess);
    calculateNumberOfNumbersPerProcess(m, numberOfColumnsPerProcess, &numberOfNumbersPerProcess);

    broadcastVector(pid, m, &vector);
    broadcastMatrix(m, n, numberOfNumbersPerProcess, numberOfColumnsPerProcess, matrix, &myMatrix);

    MPI_Finalize();
}
