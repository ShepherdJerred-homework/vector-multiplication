int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int pid = getPid();
    int numberOfProcesses = getNumberOfProcesses();

    run(pid, numberOfProcesses);

    MPI_Finalize();
}
