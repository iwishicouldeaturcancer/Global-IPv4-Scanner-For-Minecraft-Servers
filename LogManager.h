#pragma once
#include <fstream>
#include <mutex>
#include <chrono>
#include <string>

class LogManager {
    std::mutex mtx;
    std::ofstream hitFile;
    std::ofstream diagFile;
    std::ofstream statsFile;
    std::chrono::steady_clock::time_point startTime;

public:
    LogManager();
    ~LogManager();

    std::string getTS();

    void logHit(const std::string& ip, const std::string& info);
    void logDiag(const std::string& ip, const std::string& err);
    void logStats(uint64_t s, uint64_t f);

private:
    void writeHeader(std::ofstream& f, const std::string& title);
};