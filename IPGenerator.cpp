#include "IPGenerator.h"
#include <boost/asio.hpp>

using namespace boost::asio;

IPGenerator::IPGenerator()
    : rng(std::random_device{}()), dist(0, 0xFFFFFFFF) {}

std::string IPGenerator::next() {
    while (true) {
        uint32_t i = dist(rng);
        uint8_t f = (i >> 24) & 0xFF;

        if (f == 10 || f == 127 || f == 192 ||
            f == 172 || f >= 224)
            continue;

        return ip::address_v4(i).to_string();
    }
}