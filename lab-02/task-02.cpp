#include <iostream>
#include <mpi.h>
using namespace std;

int main()
{
	int rank, value, n;
	MPI_Status status;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j == i) {
                continue;
            }

            int start, end;
            if (i < j) {
                start = i;
                end = j;
            }
            else {
                start = j;
                end = i;
            }

            if (rank >= start && rank <= end) {
                int step, prev, next;

                // вперед
                if (i < j) {
                    step = 1;
                }
                else {
                    step = -1;
                }

                prev = rank - step;
                next = rank + step;
                if (rank == i) {
                    value = i;
                    MPI_Send(&value, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
                }
                else {
                    MPI_Recv(&value, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &status);

                    if (rank != j) {
                        MPI_Send(&value, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
                    }
                    else {
                        cout << "Process " << rank << " got " << value << " from [" << i << "]" << endl;
                    }
                }
            }
        }
    }

	MPI_Finalize();
	return 0;
}