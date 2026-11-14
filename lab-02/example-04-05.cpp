#include <iostream>
#include <cstring>
#include <mpi.h>
using namespace std;

int main() {
    char message[20];
    int myrank, n, i;

    MPI_Status status;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    if (myrank == 0) {
        strcpy_s(message, 20, "Hello");
        for (i = 1; i < n; i++)
            MPI_Send(message, strlen(message), MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
    else {
        MPI_Recv(message, 20, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        cout << "message recive process " << myrank << endl;
    }

    MPI_Finalize();
}