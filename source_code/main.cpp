#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm> // Penting untuk fungsi std::min

// Struktur untuk menyimpan hasil analisis sorting
struct SortResult {
    double time_ms;
    long long comparisons;
    long long swaps;
};

// Struktur untuk menyimpan hasil pencarian
struct SearchResult {
    int index;
    int steps;
    double time_ms;
};

// Deklarasi fungsi eksternal agar bisa terhubung dengan sorting.cpp dan searching.cpp
template <typename T> extern SortResult quickSort(T arr[], int n, bool verbose);
template <typename T> extern SortResult bubbleSort(T arr[], int n);
template <typename T> extern SearchResult exponentialSearch(T arr[], int n, T target);

// Struktur data Representasi baris transaksi e-commerce
struct Transaction {
    std::string invoiceNo;
    std::string stockCode;
    std::string description;
    int quantity;
    std::string invoiceDate;
    double unitPrice;
    int customerID;
    std::string country;
};

