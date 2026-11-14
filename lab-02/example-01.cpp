#include <iostream>
#include <mpi.h>
using namespace std;

int main() {
    int rank, n;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    cout << "Hello from process " << rank << " of " << n << endl;

    MPI_Finalize();
    return 0;
}