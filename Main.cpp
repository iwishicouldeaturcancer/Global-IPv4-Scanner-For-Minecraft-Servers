#include <iostream>
#include <thread>
#include <vector>
#include <boost/asio.hpp>

#include "Scanner.h"
#include "UIManager.h"
#include "LogManager.h"

//terminal için bazı shits
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

using namespace boost::asio;

int main() {
    std::cout << BOLD << MAGENTA
              << "--- mc random ip tarayıcı 31 ---\n"
              << RESET;

    std::cout << CYAN
              << "Tarama başlıyor 31. Hedef: Global Minecraft ipv4 havuzu\n\n"
              << RESET;

    io_context ioc;
    UIManager ui;
    LogManager log;

    Scanner scanner(ioc, ui, log, 1500);
    scanner.start();

    std::vector<std::thread> pool;

    for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        pool.emplace_back([&ioc]() {
            ioc.run();
        });
    }

    for (auto& t : pool)
        t.join();

    return 0;
}