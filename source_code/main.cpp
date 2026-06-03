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

// Fungsi untuk membaca data dari berkas CSV sampel e-commerce 
// SUDAH DIPERBAIKI DENGAN TRY-CATCH AGAR TIDAK CRASH STOI
void readFromCSV(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[Error] Gagal membuka file " << filename << ". Pastikan file berada di folder yang sama.\n";
        return;
    }
    
    globalTransactions.clear();
    std::string line;
    std::getline(file, line); // Melewati baris header CSV
    
    auto trim = [](std::string &s) {
        // trim both ends
        size_t start = 0;
        while (start < s.size() && std::isspace((unsigned char)s[start])) start++;
        size_t end = s.size();
        while (end > start && std::isspace((unsigned char)s[end-1])) end--;
        s = s.substr(start, end - start);
    };

    auto removeSurroundingQuotes = [](std::string &s) {
        if (s.size() >= 2 && ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\''))) {
            s = s.substr(1, s.size() - 2);
        }
    };

    auto parseCSVLine = [&](const std::string &ln) {
        std::vector<std::string> fields;
        std::string cur;
        bool inQuotes = false;
        for (size_t i = 0; i < ln.size(); ++i) {
            char c = ln[i];
            if (c == '"') {
                inQuotes = !inQuotes;
                cur += c; // keep quotes for now so we can strip later
            } else if (c == ',' && !inQuotes) {
                fields.push_back(cur);
                cur.clear();
            } else {
                cur += c;
            }
        }
        fields.push_back(cur);
        return fields;
    };

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        try {
            std::vector<std::string> cols = parseCSVLine(line);
            if (cols.size() < 8) {
                // jika kolom kurang, coba lanjutkan namun laporkan
                std::cerr << "[Warning] Baris CSV tidak lengkap (kolom < 8), melewati: " << line << "\n";
                continue;
            }

            // ambil 8 kolom pertama, bersihkan spasi dan kutip
            std::string inv = cols[0]; trim(inv); removeSurroundingQuotes(inv);
            std::string stock = cols[1]; trim(stock); removeSurroundingQuotes(stock);
            std::string desc = cols[2]; trim(desc); removeSurroundingQuotes(desc);
            std::string qtyStr = cols[3]; trim(qtyStr); removeSurroundingQuotes(qtyStr);
            std::string date = cols[4]; trim(date); removeSurroundingQuotes(date);
            std::string priceStr = cols[5]; trim(priceStr); removeSurroundingQuotes(priceStr);
            std::string custStr = cols[6]; trim(custStr); removeSurroundingQuotes(custStr);
            std::string country = cols[7]; trim(country); removeSurroundingQuotes(country);

            Transaction t;
            t.invoiceNo = inv;
            t.stockCode = stock;
            t.description = desc;
            t.invoiceDate = date;
            t.country = country;

            // Bersihkan karakter non-digit dari angka sederhana (misal tanda kutip, $)
            auto sanitizeNumberString = [&](std::string s) {
                std::string out;
                for (char ch: s) if (ch == '.' || ch == '-' || (ch >= '0' && ch <= '9')) out.push_back(ch);
                return out;
            };

            try {
                std::string q = sanitizeNumberString(qtyStr);
                t.quantity = q.empty() ? 0 : std::stoi(q);
            } catch (...) {
                t.quantity = 0;
            }

            try {
                std::string p = sanitizeNumberString(priceStr);
                t.unitPrice = p.empty() ? 0.0 : std::stod(p);
            } catch (...) {
                t.unitPrice = 0.0;
            }

            try {
                std::string c = sanitizeNumberString(custStr);
                t.customerID = c.empty() ? 0 : std::stoi(c);
            } catch (...) {
                t.customerID = 0;
            }

            globalTransactions.push_back(t);
        } catch (const std::exception &ex) {
            std::cerr << "[Error] Exception saat memproses baris CSV: " << ex.what() << "\nBaris: " << line << "\n";
            // lanjutkan ke baris berikutnya tanpa menghentikan program
            continue;
        } catch (...) {
            std::cerr << "[Error] Unknown exception saat memproses baris CSV. Baris: " << line << "\n";
            continue;
        }
    }
    file.close();
    std::cout << "[Sukses] Berhasil membaca " << globalTransactions.size() << " data dari file " << filename << ".\n";
}

