#include <iostream>
#include <chrono>
#include <string>

// Struktur untuk menyimpan hasil analisis sorting
struct SortResult {
    double time_ms;
    long long comparisons;
    long long swaps;
};

// Fungsi pembantu untuk menampilkan array (untuk fitur trace data kecil)
template <typename T>
void printArray(T arr[], int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << " | ";
    }
    std::cout << "\n";
}

// Implementasi Bubble Sort sebagai Pembanding
template <typename T>
SortResult bubbleSort(T arr[], int n) {
    SortResult res = {0.0, 0, 0};
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            res.comparisons++;
            if (arr[j] > arr[j + 1]) {
                T temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                res.swaps++;
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    res.time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    return res;
}

// Fungsi pembantu Rekursif untuk Quick Sort dengan perhitungan performa
template <typename T>
void quickSortHelper(T arr[], int low, int high, long long &comps, long long &swaps, bool verbose, int total_size) {
    if (low < high) {
        int mid = low + (high - low) / 2;
        T pivot = arr[mid]; // Memilih elemen tengah sebagai pivot
        int i = low;
        int j = high;
        
        while (i <= j) {
            while (arr[i] < pivot) {
                comps++;
                i++;
            }
            comps++; // Untuk perbandingan terakhir yang gagal
            
            while (arr[j] > pivot) {
                comps++;
                j--;
            }
            comps++; // Untuk perbandingan terakhir yang gagal
            
            if (i <= j) {
                T temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
                swaps++;
                i++;
                j--;
                
                // Menampilkan langkah per-iterasi jika ukuran data kecil (n <= 20)
                if (verbose && total_size <= 20) {
                    std::cout << "  [Trace Quick Sort] Melakukan pertukaran elemen. Array saat ini:\n   ";
                    printArray(arr, total_size);
                }
            }
        }
        
        quickSortHelper(arr, low, j, comps, swaps, verbose, total_size);
        quickSortHelper(arr, i, high, comps, swaps, verbose, total_size);
    }
}

