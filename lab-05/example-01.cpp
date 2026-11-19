#include <iostream>
#include <vector>
#include <algorithm>
#include <mpi.h>

using namespace std;

// Последовательный алгоритм чет-нечетной перестановки
void sort_chet_nechet(int a[], int n) {
    int upper = n / 2;
    if (n % 2 == 0) {
        upper -= 1;
    }

    for (int i = 0; i < n; i++) {
        if (i % 2 == 1) { // нечетная итерация
            for (int j = 0; j < upper; j++) {
                if (a[2 * j + 1] > a[2 * j + 2]) {
                    swap(a[2 * j + 1], a[2 * j + 2]);
                }
            }
        } else { // четная итерация
            for (int j = 0; j < n / 2; j++) {
                if (a[2 * j] > a[2 * j + 1]) {
                    swap(a[2 * j], a[2 * j + 1]);
                }
            }
        }
    }
}

void print_array(int a[], int n) {
    for (int i = 0; i < n; i++) {
        cout << a[i] << " ";
    }
    cout << endl;
}

// Функция слияния двух отсортированных массивов
void merge_and_keep(int local_a[], int recv_a[], int local_n, bool keep_small) {
    vector<int> merged(2 * local_n);
    merge(local_a, local_a + local_n, recv_a, recv_a + local_n, merged.begin());

    if (keep_small) {
        for (int i = 0; i < local_n; i++) local_a[i] = merged[i];
    } else {
        for (int i = 0; i < local_n; i++) local_a[i] = merged[local_n + i];
    }
}

int main() {
    MPI_Init(NULL, NULL);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Размер массива должен делиться нацело на количество процессов
    const int N = 16; 
    int local_n = N / size;

    vector<int> global_arr;
    if (rank == 0) {
        global_arr = {64, 34, 25, 12, 22, 11, 90, 5, 88, 1, 43, 77, 19, 3, 50, 29};
        cout << "Original array: ";
        for (int x : global_arr) cout << x << " ";
        cout << endl;
    }

    // Память под локальный кусок и буфер для приёма от соседа
    vector<int> local_arr(local_n);
    vector<int> recv_arr(local_n);

    // 1. Рассылаем части массива всем процессам
    MPI_Scatter(global_arr.data(), local_n, MPI_INT, local_arr.data(), local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // 2. Каждый процесс сортирует свой кусок
    sort_chet_nechet(local_arr.data(), local_n);

    // 3. Параллельные чёт-нечётные фазы обмена между процессами
    for (int phase = 0; phase < size; phase++) {
        int partner = -1;

        if (phase % 2 == 0) { // Чётная фаза: пары (0,1), (2,3)...
            if (rank % 2 == 0) partner = rank + 1;
            else partner = rank - 1;
        } else {              // Нечётная фаза: пары (1,2), (3,4)...
            if (rank % 2 == 1) partner = rank + 1;
            else partner = rank - 1;
        }

        if (partner >= 0 && partner < size) {
            MPI_Sendrecv(local_arr.data(), local_n, MPI_INT, partner, 0, recv_arr.data(), local_n, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            bool keep_small = (rank < partner);
            merge_and_keep(local_arr.data(), recv_arr.data(), local_n, keep_small);
        }
    }

    // 4. Собираем отсортированные части обратно в нулевой процесс
    MPI_Gather(local_arr.data(), local_n, MPI_INT, global_arr.data(), local_n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Sorted array: ";
        for (int x : global_arr) cout << x << " ";
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}