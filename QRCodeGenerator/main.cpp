#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "generator.hpp"

using qrcodegen::QrCode;
using qrcodegen::QrSegment;

static const std::vector<QrCode::Ecc> ECC_LEVELS {
    QrCode::Ecc::LOW,
    QrCode::Ecc::MEDIUM,
    QrCode::Ecc::QUARTILE,
    QrCode::Ecc::HIGH,  
};

int main() {
 
    // Fix your input text
    const char *text = "https://github.com/SaberDa/CPP_Basic_Projects_WareHouse/tree/master/QRCodeGenerator";

    // Or input text in command line
    // std::cout << "Please input your text: " << std::endl;
    // std::string data;
    // std::getline(std::cin, data);
    // const char *text = data.c_str();

    std::vector<QrSegment> segs = QrSegment::makeSegments(text);

    /*
     * Format: encodeSegments(segs, QrCode::Ecc::ECC_LEVELS, minVersion, maxVersion, mask, boostEcl)
     * 
     * minVersion >= 1
     * maxVersion <= 40
     * 
     * mask: -1 for auto, [0, 7] for manual
     * 
     * boostEcl: if or not to increase ECC level
    */ 
    QrCode qrcode = QrCode::encodeSegments(segs, QrCode::Ecc::LOW, 3, 40, -1, false);

    for (int y = 0; y < qrcode.getSize(); y++) {
        for (int x = 0; x < qrcode.getSize(); x++) {
            std::cout << (qrcode.getModule(x, y) ? "\033[33;47m  \033[0m" : "  ");
        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}