#include <iostream>
#include <mpi.h>
using namespace std;

int main() {
    int rank, value;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    value = rank;
    if (rank == 0) {
        value = 10;
    }
    MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);
    cout << "Process " << rank << " got " << value << endl;

    MPI_Finalize();
}