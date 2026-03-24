#include "UIManager.h"
#include <iostream>
#include <iomanip>

// linux terminalinde çalışıyorlar ama wingay de denemedim zaten bu amk kodu siksen winde çalışmaz
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BLUE    "\033[34m"
#define BG_GRAY "\033[47;30m"

void UIManager::logFound(const std::string& ip, const std::string& info) {
    std::lock_guard<std::mutex> lock(mtx);

    std::cout << "\r\033[2K"
              << BOLD << GREEN << "[+] " << RESET
              << BOLD << std::left << std::setw(16) << ip << RESET
              << YELLOW << " -> " << info << RESET << "\n";
}

void UIManager::updateStatusBar(uint64_t scanned, uint64_t found, double speed) {
    std::lock_guard<std::mutex> lock(mtx);

    std::cout << "\r\033[2K"
              << BG_GRAY << BOLD << " MC-SCAN v1.2 " << RESET
              << BLUE << " Taranan: " << BOLD << scanned << RESET
              << RED  << " Hits: "    << BOLD << found   << RESET
              << CYAN << " Hız: "     << std::fixed << std::setprecision(1)
              << speed << " ip/s" << RESET
              << std::flush;
}