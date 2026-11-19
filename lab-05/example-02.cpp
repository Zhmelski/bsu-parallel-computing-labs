#include <iostream>
#include <vector>
#include <algorithm>
#include <mpi.h>

using namespace std;

// Функция слияния и разделения для процессов
void compareSplit(int keep_smaller, vector<int> &local_arr, vector<int> &recv_arr) {
    int size = local_arr.size();
    vector<int> merged(size * 2);
    
    // Слияние двух отсортированных массивов в один общий
    merge(local_arr.begin(), local_arr.end(), recv_arr.begin(), recv_arr.end(), merged.begin());
    
    // В зависимости от роли, процесс забирает себе либо левую, либо правую часть
    if (keep_smaller)
        copy(merged.begin(), merged.begin() + size, local_arr.begin()); // исправлена опечатка lo - cal_arr
    else
        copy(merged.begin() + size, merged.end(), local_arr.begin());
}

// parallel odd-even transposition
void OddEven_Sort(int rank, int num_procs, int LOCAL_SIZE, vector<int> &local_arr, vector<int> &recv_arr) {
    for (int phase = 0; phase < num_procs; ++phase) {
        int partner = -1;
        if (phase % 2 == 0) {   // Четная фаза
            if (rank % 2 == 0)
                partner = rank + 1; // Четный обменивается с правым
            else
                partner = rank - 1; // Нечетный обменивается с левым
        } else {    // Нечетная фаза
            if (rank % 2 == 1)
                partner = rank + 1; // Нечетный обменивается с правым
            else
                partner = rank - 1; // Четный обменивается с левым
        }
        
        // Проверка границ (чтобы крайние процессы не вышли за пределы топологии)
        if (partner >= 0 && partner < num_procs) {
            // Синхронный обмен данными между соседями
            MPI_Sendrecv(local_arr.data(), LOCAL_SIZE, MPI_INT, partner, 0, recv_arr.data(), LOCAL_SIZE, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Сравнение и разделение элементов
            if (rank < partner) {
                compareSplit(1, local_arr, recv_arr);   // Меньший ранг оставляет меньшие элементы                
            } else {
                compareSplit(0, local_arr, recv_arr);   // Больший ранг оставляет большие элементы
            }
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Общий размер массива (должен делиться на num_procs без остатка)
    const int GLOBAL_SIZE = 16;
    
    if (GLOBAL_SIZE % num_procs != 0) {
        if (rank == 0) {
            cout << "Error: size of array (" << GLOBAL_SIZE << ") must be divisible by the number of processes (" << num_procs << ")!" << endl;
        }
        MPI_Finalize();
        return 1;
    }

    int LOCAL_SIZE = GLOBAL_SIZE / num_procs;
    vector<int> global_arr;

    // Нулевой процесс создает исходный массив
    if (rank == 0) {
        global_arr = {42, 17, 8, 99, 23, 56, 4, 71, 15, 84, 31, 60, 12, 90, 3, 27};
        cout << "Original array: ";
        for (int val : global_arr) cout << val << " ";
        cout << endl;
    }

    // Буферы для каждого процесса
    vector<int> local_arr(LOCAL_SIZE);
    vector<int> recv_arr(LOCAL_SIZE);

    // 1. Раздаем куски массива всем процессам
    MPI_Scatter(global_arr.data(), LOCAL_SIZE, MPI_INT, local_arr.data(), LOCAL_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    // 2. Локально сортируем свой кусок перед фазами обмена
    sort(local_arr.begin(), local_arr.end());

    // 3. Вызываем параллельную сортировку из методички
    OddEven_Sort(rank, num_procs, LOCAL_SIZE, local_arr, recv_arr);

    // 4. Собираем отсортированные части обратно в нулевой процесс
    MPI_Gather(local_arr.data(), LOCAL_SIZE, MPI_INT, global_arr.data(), LOCAL_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Sorted array: ";
        for (int val : global_arr) cout << val << " ";
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}
