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

