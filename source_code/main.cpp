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

// Fungsi untuk membaca data dari berkas CSV sampel e-commerce (Fitur 4.1-A)
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

// Fungsi input manual data tunggal (Fitur 4.1-A)
void inputManualData() {
    Transaction t;
    std::cout << "Masukkan Invoice No: "; std::cin >> t.invoiceNo;
    std::cout << "Masukkan Stock Code: "; std::cin >> t.stockCode;
    std::cin.ignore();
    std::cout << "Masukkan Description: "; std::getline(std::cin, t.description);
    std::cout << "Masukkan Quantity (Integer): "; std::cin >> t.quantity;
    std::cout << "Masukkan Invoice Date: "; std::cin >> t.invoiceDate;
    std::cout << "Masukkan Unit Price: "; std::cin >> t.unitPrice;
    std::cout << "Masukkan Customer ID (Integer): "; std::cin >> t.customerID;
    std::cout << "Masukkan Country: "; std::cin >> t.country;
    
    globalTransactions.push_back(t);
    std::cout << "[Sukses] Data manual berhasil ditambahkan ke memori.\n";
}

// Fungsi Analisis Eksperimental Akurat Sesuai Rubrik Dosen (Fitur 4.1-D)
void executeBenchmark() {
    int sizes[] = {100, 500, 1000, 5000, 10000}; 
    int numSizes = 5;
    std::stringstream report;
    
    report << "=========================================================================\n";
    report << "            TABEL PERBANDINGAN EKSPERIMEN PERFORMA SORTING LENGKAP       \n";
    report << "=========================================================================\n";
    report << std::left << std::setw(8) << "Ukuran" 
           << std::setw(18) << "Kondisi Data" 
           << std::setw(15) << "Algoritma" 
           << std::setw(15) << "Waktu (ms)" 
           << std::setw(15) << "Perbandingan" 
           << std::setw(12) << "Pertukaran" << "\n";
    report << "=========================================================================\n";

    for (int s = 0; s < numSizes; s++) {
        int n = sizes[s];
        std::string kondisiTeks[] = {"Acak", "Terurut Naik", "Terurut Turun", "Banyak Duplikat"};
        
        for (int k = 0; k < 4; k++) {
            int* arrQS = new int[n];
            int* arrBS = new int[n];
            
            for (int i = 0; i < n; i++) {
                int val = 0;
                if (k == 0) { 
                    val = rand() % 20000;
                } else if (k == 1) { 
                    val = i;
                } else if (k == 2) { 
                    val = n - i;
                } else if (k == 3) { 
                    val = (i % 5 == 0) ? 999 : rand() % 10;
                }
                arrQS[i] = val;
                arrBS[i] = val;
            }
            
            SortResult rQS = quickSort(arrQS, n, false);
            SortResult rBS = bubbleSort(arrBS, n);
            
            report << std::left << std::setw(8) << n 
                   << std::setw(18) << kondisiTeks[k] 
                   << std::setw(15) << "Quick Sort" 
                   << std::setw(15) << rQS.time_ms 
                   << std::setw(15) << rQS.comparisons 
                   << std::setw(12) << rQS.swaps << "\n";
                   
            report << std::left << std::setw(8) << "" 
                   << std::setw(18) << "" 
                   << std::setw(15) << "Bubble Sort" 
                   << std::setw(15) << rBS.time_ms 
                   << std::setw(15) << rBS.comparisons 
                   << std::setw(12) << rBS.swaps << "\n";
            report << "-------------------------------------------------------------------------\n";
            
            delete[] arrQS;
            delete[] arrBS;
        }
        report << "=========================================================================\n";
    }
    
    lastBenchmarkReport = report.str();
    std::cout << lastBenchmarkReport;
    std::cout << "\n[Sukses] Pengujian 4 kondisi data selesai. Pilih Menu 5 untuk ekspor hasil.\n";
}

int main() {
    srand(time(0));
    int choice;
    
    do {
        std::cout << "\n============================================\n";
        std::cout << "   MENU UTAMA TUGAS PROYEK STRUKTUR DATA    \n";
        std::cout << "============================================\n";
        std::cout << "1. Input / Baca Data Transaksi E-Commerce\n";
        std::cout << "2. Jalankan Sorting (Quick Sort vs Bubble)\n";
        std::cout << "3. Jalankan Searching (Exponential Search)\n";
        std::cout << "4. Perbandingan & Benchmark Otomatis\n";
        std::cout << "5. Ekspor Hasil Benchmark Ke File\n";
        std::cout << "0. Keluar Aplikasi\n";
        std::cout << "============================================\n";
        std::cout << "Pilih menu (0-5): ";
        std::cin >> choice;
        
        switch (choice){
            case 1: {
                int subChoice;
                std::cout << " 1. Input Data Manual\n 2. Generate Acak\n 3. Baca dari File CSV\nPilihan: ";
                std::cin >> subChoice;
                if (subChoice == 1) inputManualData();
                else if (subChoice == 2) {
                    int num; std::cout << "Jumlah elemen: "; std::cin >> num;
                    generateRandomData(num);
                } else if (subChoice == 3) {
                    std::string fName; std::cout << "Masukkan nama file (misal online_retail.csv): "; std::cin >> fName;
                    readFromCSV(fName);
                }
                break;
            }
            case 2: {
                if (globalTransactions.empty()) {
                    std::cout << "[Peringatan] Data kosong. Silakan isi data di menu 1 terlebih dahulu.\n";
                    break;
                }
                int sortType;
                std::cout << "Pilih field data untuk di-sorting:\n 1. Quantity (Integer)\n 2. Stock Code (String)\nPilihan: ";
                std::cin >> sortType;
                
                int n = globalTransactions.size();
                std::cout << "\n--- PROSES SORTING MENGGUNAKAN QUICK SORT ---\n";
                if (sortType == 1) {
                    int* arr = new int[n];
                    for(int i=0; i<n; i++) arr[i] = globalTransactions[i].quantity;
                    std::cout << "Array sebelum sorting (10 data pertama):\n";
                    for(int i=0; i<std::min(n, 10); i++) std::cout << arr[i] << " ";
                    std::cout << "\n";
                    
                    SortResult res = quickSort(arr, n, true);
                    
                    std::cout << "\nArray sesudah sorting (10 data pertama):\n";
                    for(int i=0; i<std::min(n, 10); i++) std::cout << arr[i] << " ";
                    std::cout << "\n\n[Statistik Performa Quick Sort]:\nWaktu: " << res.time_ms << " ms\nPerbandingan: " << res.comparisons << "\nPertukaran: " << res.swaps << "\n";
                    for(int i=0; i<n; i++) globalTransactions[i].quantity = arr[i];
                    delete[] arr;
                } else {
                    std::string* arr = new std::string[n];
                    for(int i=0; i<n; i++) arr[i] = globalTransactions[i].stockCode;
                    SortResult res = quickSort(arr, n, true);
                    std::cout << "[Sukses] Berhasil mengurutkan Stock Code berdasarkan Alphabet.\n";
                    for(int i=0; i<n; i++) globalTransactions[i].stockCode = arr[i];
                    delete[] arr;
                }
                break;
            }
            case 3: {
                if (globalTransactions.empty()) {
                    std::cout << "[Peringatan] Mengambil data kosong. Isi data terlebih dahulu.\n";
                    break;
                }
                int searchType;
                std::cout << "Cari berdasarkan:\n 1. Quantity (Integer)\n 2. Stock Code (String)\nPilihan: ";
                std::cin >> searchType;
                
                int n = globalTransactions.size();
                if (searchType == 1) {
                    int target; std::cout << "Masukkan nilai Quantity yang dicari: "; std::cin >> target;
                    int* arr = new int[n];
                    for(int i=0; i<n; i++) arr[i] = globalTransactions[i].quantity;
                    
                    SearchResult sRes = exponentialSearch(arr, n, target);
                    if (sRes.index != -1) {
                        std::cout << "\n[Ditemukan] Element " << target << " ada pada indeks ke-" << sRes.index << " (Hasil urutan aktif)\n";
                    } else {
                        std::cout << "\n[Gagal] Data tidak ditemukan dalam array.\n";
                    }
                    std::cout << "Jumlah Langkah Pengecekan: " << sRes.steps << "\nWaktu Pencarian: " << sRes.time_ms << " ms\n";
                    delete[] arr;
                } else {
                    // Pencarian berdasarkan Stock Code (string)
                    std::string targetStr; std::cout << "Masukkan Stock Code yang dicari: "; std::cin >> targetStr;
                    std::string* arrs = new std::string[n];
                    for (int i = 0; i < n; i++) arrs[i] = globalTransactions[i].stockCode;
                    SearchResult sRes2 = exponentialSearch(arrs, n, targetStr);
                    if (sRes2.index != -1) {
                        std::cout << "\n[Ditemukan] Stock Code '" << targetStr << "' ada pada indeks ke-" << sRes2.index << "\n";
                    } else {
                        std::cout << "\n[Gagal] Stock Code tidak ditemukan.\n";
                    }
                    std::cout << "Jumlah Langkah Pengecekan: " << sRes2.steps << "\nWaktu Pencarian: " << sRes2.time_ms << " ms\n";
                    delete[] arrs;
                }
                break;
            }
            case 4: {
                executeBenchmark();
                break;
            }
            case 5: {
                std::string outfn; std::cout << "Masukkan nama file output (misal hasil.txt): "; std::cin >> outfn;
                std::ofstream ofs(outfn);
                if (!ofs.is_open()) {
                    std::cout << "[Error] Gagal membuka file untuk menulis: " << outfn << "\n";
                } else {
                    ofs << lastBenchmarkReport;
                    ofs.close();
                    std::cout << "[Sukses] Hasil benchmark disimpan ke " << outfn << "\n";
                }
                break;
            }
            case 0: {
                std::cout << "Keluar aplikasi. Terima kasih.\n";
                break;
            }
            default: {
                std::cout << "Pilihan tidak valid. Silakan pilih 0-5.\n";
                break;
            }
        }
    } while (choice != 0);
    return 0;
}
