#include <iostream>
#include <mpi.h>
using namespace std;

int main() {
    int i, j, sum_j;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &i);

    j = i % 2;
    MPI_Reduce(&j, &sum_j, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (i == 0) {
        cout << sum_j << endl;
    }
    
    MPI_Finalize();
}