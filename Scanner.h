#pragma once
#include <boost/asio.hpp> //dünyadaki en gzel socket libi
#include <atomic>
#include "UIManager.h"
#include "LogManager.h"
#include "IPGenerator.h"

using namespace boost::asio;
using ip::tcp;

class Scanner {
    io_context& ioc;
    UIManager& ui;
    LogManager& log;

    IPGenerator ipGen;

    std::atomic<uint64_t> scanned{0};
    std::atomic<uint64_t> found{0};

    int max_conn;

    std::string parseMinecraftData(const std::string& json);

public:
    Scanner(io_context& c, UIManager& u, LogManager& l, int conn);
    void start();

private:
    void spawn();
    void query(std::shared_ptr<tcp::socket> sock, const std::string& ip);
};