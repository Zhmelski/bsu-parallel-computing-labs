#include <iostream>
#include <cmath>
#include <mpi.h>
using namespace std;

int main() {
    int n, myid, numprocs, i;
    double mypi, pi, h, sum, x, t1, t2, PI = 3.141592653589793;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    while (1) {
        if (myid == 0) {
            cout << "Enter the number of intervals: (0 quits) ";
            cin >> n;
            t1 = MPI_Wtime();
        }
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (n == 0) {
            break;
        }
        else {
            h = 1.0 / (double)n;
            sum = 0.0;
            for (i = myid + 1; i <= n; i += numprocs) {
                x = h * ((double)i - 0.5);
                sum += (4.0 / (1.0 + x * x));
            }
            mypi = h * sum;

            MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            if (myid == 0) {
                t2 = MPI_Wtime();
                cout << "pi is approximately " << pi << endl;
                cout << "Error is " << fabs(pi - PI) << endl;
                cout << "time " << t2 - t1 << endl;
            }
        }
    }

    MPI_Finalize();
}