#include <iostream>
#include <chrono>
#include <string>
#include <algorithm>

// Struktur untuk menyimpan hasil pencarian
struct SearchResult {
    int index;
    int steps;
    double time_ms;
};

// Fungsi internal Binary Search yang digunakan oleh Exponential Search
template <typename T>
SearchResult binarySearchInternal(T arr[], int low, int high, T target) {
    SearchResult res = {-1, 0, 0.0};
    auto start = std::chrono::high_resolution_clock::now();
    
    while (low <= high) {
        res.steps++;
        int mid = low + (high - low) / 2;
        
        if (arr[mid] == target) {
            res.index = mid;
            break;
        }
        if (arr[mid] < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    res.time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    return res;
}

// Implementasi Exponential Search Mandiri
template <typename T>
SearchResult exponentialSearch(T arr[], int n, T target) {
    SearchResult res = {-1, 0, 0.0};
    auto start = std::chrono::high_resolution_clock::now();
    
    if (n == 0) {
        res.index = -1;
        return res;
    }
    
    res.steps++;
    if (arr[0] == target) {
        res.index = 0;
        auto end = std::chrono::high_resolution_clock::now();
        res.time_ms = std::chrono::duration<double, std::milli>(end - start).count();
        return res;
    }
    
    // Temukan jangkauan untuk binary search dengan melompat secara eksponensial
    int i = 1;
    while (i < n && arr[i] <= target) {
        res.steps++;
        std::cout << "  [Trace Search] Target kemungkinan berada setelah indeks " << i << "\n";
        i = i * 2;
    }
    
    int low = i / 2;
    int high = std::min(i, n - 1);
    std::cout << "  [Trace Search] Jangkauan target dipersempit ke indeks: [" << low << " sampai " << high << "]\n";
    
    // Lakukan Binary Search di rentang yang ditemukan
    SearchResult bsRes = binarySearchInternal(arr, low, high, target);
    
    auto end = std::chrono::high_resolution_clock::now();
    res.index = bsRes.index;
    res.steps += bsRes.steps;
    res.time_ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    return res;
}

// Instansiasi Eksplisit template
template SearchResult exponentialSearch<int>(int arr[], int n, int target);
template SearchResult exponentialSearch<double>(double arr[], int n, double target);
template SearchResult exponentialSearch<std::string>(std::string arr[], int n, std::string target);
