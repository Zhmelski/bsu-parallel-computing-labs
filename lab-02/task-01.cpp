#include <iostream>
#include <mpi.h>
using namespace std;

int main()
{
	int rank, value = 77, n;
	int i = 1, j = 4;
	MPI_Status status;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n);

	if ((i < 0) || (i > n) || (j < 0) || (j > n)) {
		cout << "Incorrect i or/and j";
		MPI_Finalize();
		return 3;
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
			value = 0;
			MPI_Send(&value, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
		}
		else {
			MPI_Recv(&value, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &status);
			value++;

			if (rank != j) {
				MPI_Send(&value, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
			}
		}


		// обратно
		step = -step;
		prev = rank - step;
		next = rank + step;

		if (rank == j) {
			MPI_Send(&value, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
		}
		else {
			MPI_Recv(&value, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &status);
			value++;

			if (rank != i) {
				MPI_Send(&value, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
			}
		}

		cout << "Process " << rank << " got " << value << endl;
	}
	else {

	}

	MPI_Finalize();
	return 0;
}
