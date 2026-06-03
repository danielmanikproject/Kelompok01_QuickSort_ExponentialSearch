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

// Variabel Global untuk menampung data di memori program
std::vector<Transaction> globalTransactions;
std::string lastBenchmarkReport = "";

// Fungsi untuk membuat data integer acak
void generateRandomData(int n) {
    globalTransactions.clear();
    for (int i = 0; i < n; i++) {
        Transaction t;
        t.invoiceNo = std::to_string(100000 + rand() % 900000);
        t.stockCode = "ITEM" + std::to_string(100 + rand() % 900);
        t.description = "RANDOM GENERATED PRODUCT " + std::to_string(i);
        t.quantity = rand() % 100 + 1;
        t.invoiceDate = "12/1/2010 08:00";
        t.unitPrice = (rand() % 500 + 1) / 10.0;
        t.customerID = 10000 + rand() % 90000;
        t.country = "United Kingdom";
        globalTransactions.push_back(t);
    }
    std::cout << "[Sukses] Berhasil menghasilkan " << n << " data transaksi acak.\n";
}

