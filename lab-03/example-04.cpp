#include <iostream>
#include <mpi.h>
using namespace std;

int main() {
    int summ = 0, myid, i, root = 0, *rbuff = 0;
    int gsize, msize, sendarray[10];

    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(comm, &gsize);
    MPI_Comm_rank(comm, &myid);

    msize = gsize * 10;
    if (myid == root) {
        rbuff = new int[msize];
    }
    for (i = 0; i < 10; ++i) {
        sendarray[i] = myid;
    }
    MPI_Gather(sendarray, 10, MPI_INT, rbuff, 10, MPI_INT, root, comm);

    if (myid == root) {
        for (i = 0; i < msize; ++i) {
            summ += rbuff[i];
        }
        cout << summ << endl;
    }
    MPI_Finalize();
}