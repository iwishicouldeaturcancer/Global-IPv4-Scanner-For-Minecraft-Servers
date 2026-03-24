#include "Scanner.h"
#include <thread>
#include <algorithm>

Scanner::Scanner(io_context& c, UIManager& u, LogManager& l, int conn)
    : ioc(c), ui(u), log(l), max_conn(conn) {}


// =====================================================
// json parselama eyliyoz
// =====================================================
std::string Scanner::parseMinecraftData(const std::string& json) {

    //biraz boktan ama bunu yapmadan sunucunun kesinliğinden emin olmak daha zor
    if (json.find("\"version\"") == std::string::npos ||
        json.find("\"players\"") == std::string::npos) {
        return "";
    }

    std::string version = "Bilinmiyor";
    std::string players = "?/?";

    // -------- sürüm çekioz --------
    size_t v_pos = json.find("\"version\"");
    if (v_pos != std::string::npos) {
        size_t name_pos = json.find("\"name\"", v_pos);

        if (name_pos != std::string::npos) {
            size_t start = json.find("\"", name_pos + 6);

            if (start != std::string::npos) {
                size_t end = json.find("\"", start + 1);

                if (end != std::string::npos) {
                    version = json.substr(start + 1, end - start - 1);
                }
            }
        }
    }

    // -------- player count çekiom --------
    size_t p_pos = json.find("\"players\""); //komik yöntem ama çalışıyor lol
    if (p_pos != std::string::npos) {

        size_t online_pos = json.find("\"online\"", p_pos);
        size_t max_pos    = json.find("\"max\"", p_pos);

        std::string online = "?";
        std::string max    = "?";

        // online oyuncu sayısı
        if (online_pos != std::string::npos) {
            size_t colon = json.find(":", online_pos);
            size_t comma = json.find_first_of(",}", colon);

            if (colon != std::string::npos && comma != std::string::npos) {
                online = json.substr(colon + 1, comma - colon - 1);

                // filtering eyliyoz
                online.erase(remove_if(online.begin(), online.end(), ::isspace), online.end());
            }
        }

        // maks (en fazla) oyuncu sayısı
        if (max_pos != std::string::npos) {
            size_t colon = json.find(":", max_pos);
            size_t comma = json.find_first_of(",}", colon);

            if (colon != std::string::npos && comma != std::string::npos) {
                max = json.substr(colon + 1, comma - colon - 1);

                // boşlukları temizliom
                max.erase(remove_if(max.begin(), max.end(), ::isspace), max.end());
            }
        }

        players = online + "/" + max; //aklıma daha iyi bir yol gelmedi jsondan böyle gelse yapmazdım zaten yarram ne boş yapion
    }

    return "Versiyon: " + version + " | Oyuncular: " + players;
}

// bismillah
void Scanner::start() {

    for (int i = 0; i < max_conn; ++i) {
        spawn();
    }

    // istatistik threadi (gemmini den aldım)
    std::thread([this]() {
        uint64_t last_s = 0;

        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            uint64_t current_s = scanned.load();

            ui.updateStatusBar(
                current_s,
                found.load(),
                (double)(current_s - last_s)
            );

            log.logStats(current_s, found.load());

            last_s = current_s;
        }
    }).detach();
}


// yeni bağlantı oluştur
void Scanner::spawn() {

    auto sock = std::make_shared<tcp::socket>(ioc);
    std::string ip = ipGen.next();

    tcp::endpoint ep(ip::make_address(ip), 25565);

    // timeout kontroling
    auto timer = std::make_shared<steady_timer>(ioc);
    timer->expires_after(std::chrono::milliseconds(1000));

    timer->async_wait([sock](const boost::system::error_code& ec) {
        if (!ec) {
            try {
                sock->close();
            } catch (...) {}
        }
    });

    // aç kapıyı gardiyan
    sock->async_connect(ep,
        [this, sock, timer, ip](const boost::system::error_code& ec) {

        timer->cancel();

        // limite denk gelirsek LÜTFEN DAHA SONRA YENİDEN DENEYİN
        if (ec == boost::asio::error::no_descriptors) {
            auto retry = std::make_shared<steady_timer>(ioc);
            retry->expires_after(std::chrono::milliseconds(100));

            retry->async_wait([this](const boost::system::error_code&) {
                spawn();
            });

            return;
        }

        scanned++;

        if (!ec) {
            query(sock, ip);
        } else {
            spawn();
        }
    });
}


// minecraft sorgusu gönder
void Scanner::query(std::shared_ptr<tcp::socket> sock, const std::string& ip) {

    // minecraft ping paketi
    auto packet = std::make_shared<std::vector<uint8_t>>(
        std::vector<uint8_t>{
            0x0F, 0x00, 0x2F, 0x09,
            'l','o','c','a','l','h','o','s','t',
            0x63, 0xDD, 0x01, 0x01, 0x00
    });

    async_write(*sock, buffer(*packet),
        [this, sock, ip, packet](const boost::system::error_code& ec, size_t) {

        if (!ec) {
            auto res = std::make_shared<std::vector<uint8_t>>(2048);

            sock->async_read_some(buffer(*res),
                [this, sock, ip, res](const boost::system::error_code& ec, size_t len) {

                if (!ec && len > 5) {

                    std::string raw(res->begin(), res->begin() + len);

                    //jsonın başını buluyom
                    size_t j = raw.find('{');

                    if (j != std::string::npos) {

                        std::string jsonStr = raw.substr(j);

                        std::string parsedInfo = parseMinecraftData(jsonStr);

                        // parselediğimiz veri şayet boş değilse gerçek olduğuna karar veriyoruz
                        if (!parsedInfo.empty()) {
                            found++; // bu bazen 2 tane arttırıyor gibime geliyor ama bu imkansız ilaçlarımı almam gerekiyor

                            ui.logFound(ip, parsedInfo);
                            log.logHit(ip, parsedInfo);
                        }
                    }
                }

                // BAŞTAN GO BRRRRRRRRRRRRRRRR
                spawn();
            });

        } else {
            spawn();
        }
    });
}