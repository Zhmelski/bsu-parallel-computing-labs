#include <iostream>
#include <mpi.h>
using namespace std;

int main() {
    int rank, value = 77, n;

    MPI_Status status;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    if (rank == 0) {
        value = 0;
        MPI_Send(&value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }
    else {
        MPI_Recv(&value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        value++;
        if (rank < n - 1)
            MPI_Send(&value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }
    cout << "Process " << rank << " got " << value << endl;
    
    MPI_Finalize();
}