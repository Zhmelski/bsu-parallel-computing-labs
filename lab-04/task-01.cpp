#include <iostream>
#include <mpi.h>
using namespace std;

int main() {
    int rank, size;
    int n = 8;
    int q = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double* A = new double[n*n];
    double* S = new double[n*n];    // S = A + A^2
    double* B = nullptr;            // B = S^2

    if (rank == q) {
        B = new double[n*n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i*n+j] = 1;
            }
        }
    }

    // Распределение строк между процессами
    int* sendcounts = new int[size];
    int* displs = new int[size];

    int local_rows = n/size + ((n%size > rank) ? 1 : 0);

    displs[0] = 0;
    for (int i = 0; i < size; i++) {
        int rows_i = n/size + ((n%size > i) ? 1 : 0);
        sendcounts[i] = rows_i*n;
        if (i > 0) {
            displs[i] = displs[i-1] + sendcounts[i-1];
        }        
    }

    int start_row = displs[rank]/n;

    double* local_S = new double[local_rows*n];
    double* local_B = new double[local_rows*n];

    // Передача исходной матрицы A от корневого процесса q всем остальным
    MPI_Bcast(A, n*n, MPI_DOUBLE, q, MPI_COMM_WORLD);

    // Вычисление своей полосы матрицы S = A+A^2
    for (int i_local = 0; i_local < local_rows; i_local++) {
        int global_i = start_row + i_local;
        for (int j = 0; j < n; j++) {
            double sum = 0.0;

            for (int k = 0; k < n; k++) {
                sum += A[global_i*n+k]*A[k*n+j];
            }

            local_S[i_local*n+j] = A[global_i*n+j] + sum;   // S = A + A^2
        }
    }

    // Сборка полной матрицы S во всех процессах
    MPI_Allgatherv(local_S, local_rows*n, MPI_DOUBLE, S, sendcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

    // Вычисление своей полосы матрицы B = S^2
    for (int i_local = 0; i_local < local_rows; i_local++) {
        int global_i = start_row + i_local;
        for (int j = 0; j < n; j++) {
            double sum = 0.0;

            for (int k = 0; k < n; k++) {
                sum += S[global_i*n+k]*S[k*n+j];
            }

            local_B[i_local*n+j] = sum;   // B = S^2
        }
    }

    // Сборка итоговой матрицы B в корневом процессе q
    MPI_Gatherv(local_B, local_rows*n, MPI_DOUBLE, B, sendcounts, displs, MPI_DOUBLE, q, MPI_COMM_WORLD);

    if (rank == q) {
        cout << "B:" << endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << B[i * n + j] << " ";
            }
            cout << endl;
        }
        delete[] B;
    }

    delete[] A;
    delete[] S;
    delete[] local_S;
    delete[] local_B;
    delete[] sendcounts;
    delete[] displs;

    MPI_Finalize();
    return 0;
}