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

