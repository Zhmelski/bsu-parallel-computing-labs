#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <mpi.h>

using namespace std;

int main() {
    int rank, size;
    
    MPI_Init(NULL, NULL);    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 100000; 

    vector<int> global_array;
    vector<int> pivots(size - 1);
    vector<int> sendcounts(size, 0);
    vector<int> displs(size, 0);
    int local_n = 0;

    double start_time = MPI_Wtime();

    if (rank == 0) {
        global_array.resize(n);
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 1000000);
        for (int i = 0; i < n; i++) {
            global_array[i] = dis(gen);
        }
        cout << "Array generated. Size: " << n << endl;
        
        // 1. Выбор опорных элементов
        int sample_size = size * 100;
        vector<int> sample(sample_size);
        for (int i = 0; i < sample_size; i++) {
            sample[i] = global_array[gen() % n];
        }
        sort(sample.begin(), sample.end());
        for (int i = 1; i < size; i++) {
            pivots[i - 1] = sample[i * sample_size / size];
        }
    }

    // Рассылаем опорные элементы всем процессам
    MPI_Bcast(pivots.data(), size - 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> scatter_buffer;
    if (rank == 0) {
        vector<vector<int>> buckets(size);
        
        // 2. Раскладываем элементы по корзинам
        for (int i = 0; i < n; i++) {
            // Ищем индекс корзины для текущего элемента
            int bucket_idx = lower_bound(pivots.begin(), pivots.end(), global_array[i]) - pivots.begin();
            buckets[bucket_idx].push_back(global_array[i]);
        }

        // 3. Вычисляем размеры и смещения для MPI_Scatterv
        int offset = 0;
        scatter_buffer.resize(n);
        for (int i = 0; i < size; i++) {
            sendcounts[i] = buckets[i].size();
            displs[i] = offset;
            
            // Копируем данные из корзин в единый плоский буфер
            copy(buckets[i].begin(), buckets[i].end(), scatter_buffer.begin() + offset);
            offset += sendcounts[i];
        }
    }

    // Рассылаем информацию о размерах корзин всем процессам
    MPI_Bcast(sendcounts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
    local_n = sendcounts[rank];

    vector<int> local_array(local_n);

    // 4. Распределение данных
    MPI_Scatterv(rank == 0 ? scatter_buffer.data() : nullptr, sendcounts.data(), displs.data(), MPI_INT, local_array.data(), local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // 5. Локальная сортировка
    if (local_n > 0) {
        sort(local_array.begin(), local_array.end());
    }

    // 6. Сбор данных обратно
    MPI_Gatherv(local_array.data(), local_n, MPI_INT, rank == 0 ? scatter_buffer.data() : nullptr, sendcounts.data(), displs.data(), MPI_INT, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        global_array = scatter_buffer; 
        
        cout << "Parallel Quick Sort has been implemented." << endl;
        cout << "Time: " << end_time - start_time << " seconds." << endl;
        cout << "The first 10 elements: ";
        for (int i = 0; i < 10; i++) cout << global_array[i] << " ";
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}