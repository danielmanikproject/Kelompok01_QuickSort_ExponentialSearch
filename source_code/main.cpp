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

