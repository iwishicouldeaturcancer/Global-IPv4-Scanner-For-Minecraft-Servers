#include "LogManager.h"
#include <iomanip>
#include <sstream>

LogManager::LogManager() {
    hitFile.open("found_servers.log", std::ios::app);
    diagFile.open("network_diagnostics.log", std::ios::app);
    statsFile.open("session_stats.log", std::ios::app);

    startTime = std::chrono::steady_clock::now();
    writeHeader(statsFile, "OTURUM BASLATILDI");
}

LogManager::~LogManager() {
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count();

    writeHeader(statsFile, "OTURUM KAPATILDI");
    statsFile << "Toplam Sure: " << duration << " saniye\n";
}

std::string LogManager::getTS() {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "[%H:%M:%S] ");
    return ss.str();
}

void LogManager::logHit(const std::string& ip, const std::string& info) {
    std::lock_guard<std::mutex> lock(mtx);
    hitFile << getTS() << "HIT: " << ip << " | " << info << "\n" << std::flush;
}

void LogManager::logDiag(const std::string& ip, const std::string& err) {
    std::lock_guard<std::mutex> lock(mtx);
    diagFile << getTS() << "ADDR: " << ip << " | ERR: " << err << "\n";
}

void LogManager::logStats(uint64_t s, uint64_t f) {
    std::lock_guard<std::mutex> lock(mtx);
    statsFile << getTS()
              << "Durum: " << s << " taranan, "
              << f << " bulunan.\n" << std::flush;
}

void LogManager::writeHeader(std::ofstream& f, const std::string& title) {
    f << "\n==== " << title << " ====\n";
}