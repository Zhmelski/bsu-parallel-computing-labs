#include <iostream>
#include <mpi.h>
using namespace std;

int main() {
    int rank, k = 77, i, *buf = 0, size;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        buf = new int[size];
        for (i = 0; i < size; ++i) {
            buf[i] = size - i;
        }
    }
    MPI_Scatter(buf, 1, MPI_INT, &k, 1, MPI_INT, 0, MPI_COMM_WORLD);
    cout << "Process " << rank << " k = " << k << endl;

    MPI_Finalize();
}