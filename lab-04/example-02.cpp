#include <iostream>
#include <mpi.h>
using namespace std;

int main() {
    int myrank, size, i, m;
    double *A = 0, *B = 0; // входной и выходной массив
    int N;                 // размер массивовов
    double *a = 0, *b = 0; // части массивов для каждого процесса

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (myrank == 0) {
        cout << "Vvedite N";
        cin >> N;
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // выделение памяди
    if (myrank == 0) {
        A = new double[N]; // для входного
        B = new double[N]; // и выходного массивов
    }

    // определения количества элементов в каждом процессе
    m = N / size;
    a = new double[m];
    b = new double[m];
    if (myrank == 0) {
        f1(A, N); // заполнение исходной матрицы А
    }

    MPI_Scatter(A, m, MPI_DOUBLE, a, m, MPI_DOUBLE, 0, MPI_COMM_WORLD); // рассылка частей матрицы по процессам

    f(a, b, m, myrank + 1); // основная функция обработки

    MPI_Gather(b, m, MPI_DOUBLE, B, m, MPI_DOUBLE, 0, MPI_COMM_WORLD); // сбор результатов в корневой процесс

    if (myrank == 0) {
        f2(B, N); // вывод результата
    }

    MPI_Finalize();
}