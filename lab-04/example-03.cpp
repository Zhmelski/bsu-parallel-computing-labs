#include <iostream>
#include <mpi.h>
using namespace std;

int main()
{
    int myrank, size, i, N;
    double *A = 0, *B = 0; // входной и выходной массив
    double *a = 0, *b = 0; // части массивов для каждого процесса
    int *sendcounts = 0, *displs = 0; // массив количества элементов в процессе и массив смещений

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (myrank == 0) {
        cout << "Vvedite N";
        cin >> N;
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // выделение памяди для входного и выходного массивов
    if (myrank == 0) {
        A = new double[N];
        B = new double[N];
    }

    // определения количества элементов в каждом процессе
    m = N / size + (N % (size) > (myrank) ? 1 : 0);
    
    // заполнение массива sendcounts
    sendcounts = new int[size];    
    MPI_Gather(&m, 1, MPI_INT, sendcounts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // заполнение массива displs
    displs = new int[size];
    displs[0] = 0;
    for (i = 1; i < size; ++i) {
        displs[i] = displs[i - 1] + sendcounts[i - 1];
    }
    
    a = new double[m];
    b = new double[m];
    if (myrank == 0) {
        f1(A, N);
    }
    
    // заполнение исходной матрицы А
    MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE, a, m, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // рассылка частей матрицы по процессам
    f(a, b, m, myrank + 1); //  основная функция обработки

    MPI_Gatherv(b, m, MPI_DOUBLE, B, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // посылка результата в корневой процесс
    if (myrank == 0) {
        f2(B, N); // вывод результата
    }
    
    MPI_Finalize();
}