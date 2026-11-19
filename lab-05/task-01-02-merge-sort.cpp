#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <mpi.h>

using namespace std;

// Функция для последовательного слияния двух отсортированных массивов
void sequential_merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (i = left, k = 0; i <= right; i++, k++) arr[i] = temp[k];
}

int main() {
    int rank, size;
    
    MPI_Init(NULL, NULL);    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 100000;
    // Делаем размер кратным количеству процессов для равномерного распределения
    if (n % size != 0) n = (n / size + 1) * size;
    int local_n = n / size;

    vector<int> global_array;
    if (rank == 0) {
        global_array.resize(n);
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 1000000);
        for (int i = 0; i < n; i++) {
            global_array[i] = dis(gen);
        }
        cout << "Array generated. Size: " << n << endl;
    }

    vector<int> local_array(local_n);

    // 1. Распределяем данные по процессам
    MPI_Scatter(global_array.data(), local_n, MPI_INT, local_array.data(), local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // 2. Локальная сортировка (Merge Sort)
    stable_sort(local_array.begin(), local_array.end());

    // 3. Сбор данных обратно на главный процесс
    MPI_Gather(local_array.data(), local_n, MPI_INT, global_array.data(), local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // 4. Главный процесс сливает отсортированные куски
    if (rank == 0) {
        // Последовательное слияние блоков
        for (int step = 1; step < size; step *= 2) {
            for (int i = 0; i + step < size; i += 2 * step) {
                int left = i * local_n;
                int mid = (i + step) * local_n - 1;
                int right = min((i + 2 * step) * local_n - 1, n - 1);
                sequential_merge(global_array, left, mid, right);
            }
        }
    }

    if (rank == 0) {
        cout << "Parallel Merge Sort has been implemented." << endl;
        // Проверка первых 10 элементов
        cout << "First 10 elements: ";
        for (int i = 0; i < 10; i++) cout << global_array[i] << " ";
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}