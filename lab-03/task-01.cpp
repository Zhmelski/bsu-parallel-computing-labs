#include <iostream>
#include <mpi.h>
using namespace std;

int main()
{
    MPI_Datatype vec;
    int rank, root = 0;
    int size;
    
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(comm, &size);
    int rows = size, cols = size;
    MPI_Comm_rank(comm, &rank);
    MPI_Type_vector(rows-rank, 1, cols, MPI_INT, &vec);
    MPI_Type_commit(&vec);
    
    int* matrixA = new int[rows*cols];
    int* arrB = nullptr;
    int* recvcounts = nullptr;
    int* displs = nullptr;
    int k = 0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrixA[i*cols+j] = rank;
        }
    }

    if (rank == root) {
        recvcounts = new int[size];
        displs = new int[size];

        displs[0] = 0;
        recvcounts[0] = rows;

        for (int i = 1; i < size; i++) {
            recvcounts[i] = rows-i;
            displs[i] = displs[i-1]+recvcounts[i-1];
        }

        arrB = new int[size*rows];
        for (int i = 0; i < size*rows; i++) {
            arrB[i] = 0;
        }
    }
    k = 0;
    MPI_Gatherv(matrixA + k, 1, vec, arrB, recvcounts, displs, MPI_INT, root, comm);

    if (rank == root) {
        for (int i = 0; i < size*rows; i++) {
            cout << arrB[i] << " ";
        }
        cout << endl;
    }

    delete[] matrixA;
    if (rank == root) {
        delete[] recvcounts;
        delete[] displs;
        delete[] arrB;
    }

    MPI_Type_free(&vec);
    MPI_Finalize();
    return 0;
}