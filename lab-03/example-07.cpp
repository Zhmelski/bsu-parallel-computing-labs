#include <iostream>
#include <mpi.h>
using namespace std;

int main()
{
    MPI_Datatype vec1;
    double *buf, *lbuf, s;
    int i, j, rank, n, stride;

    MPI_Status status;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    n = 1000;
    stride = 20;
    buf = new double[n * stride];

    /* создаем новый векторный тип с заданным страйдом */
    MPI_Type_vector(n, 1, stride, MPI_DOUBLE, &vec1);
    MPI_Type_commit(&vec1);

    if (rank == 0) {
        lbuf = new double [n * stride];
        for (i = 0; i < n; i++) {
            for (j = 0; j < stride; j++) {
                buf[i * stride + j] = j;
                lbuf[i * stride + j] = 0;
            }
        }
        
        MPI_Send(buf + 3, 1, vec1, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(lbuf + 5, 1, vec1, 1, 0, MPI_COMM_WORLD, &status);
    }

    if (rank == 1) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < stride; j++) {
                buf[i * stride + j] = 0;
            }
        }

        MPI_Recv(buf, 1, vec1, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Send(buf, 1, vec1, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        for (j = 0; j < n * stride; j++) {
            cout << lbuf[j] <<" ";
        }
        cout << endl;
    }

    MPI_Type_free(&vec1);
    MPI_Finalize();
}