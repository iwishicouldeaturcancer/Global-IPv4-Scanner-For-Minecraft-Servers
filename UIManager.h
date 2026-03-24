#pragma once
#include <mutex>
#include <string>

class UIManager {
    std::mutex mtx;

public:
    void logFound(const std::string& ip, const std::string& info);
    void updateStatusBar(uint64_t scanned, uint64_t found, double speed);
};