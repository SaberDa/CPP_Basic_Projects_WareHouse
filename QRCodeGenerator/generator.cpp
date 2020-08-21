#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "generator.hpp"

using std::int8_t;
using std::uint8_t;
using std::size_t;
using std::vector;

namespace qrcodegen {

QrSegment::Mode::Mode(int mode, int cc0, int cc1, int cc2) : modeBits(mode) {
    numBitCharCount[0] = cc0;
    numBitCharCount[1] = cc1;
    numBitCharCount[2] = cc2;
}

int QrSegment::Mode::getModeBits() const {
    return modeBits;
}

int QrSegment::Mode::numCharCountBits(int ver) const {
    return numBitCharCount[(ver + 7) / 17];
}

const QrSegment::Mode QrSegment::Mode::NUMERIC      (0x1, 10, 12, 14);
const QrSegment::Mode QrSegment::Mode::ALPHANUMERIC (0x2, 9, 11, 13);
const QrSegment::Mode QrSegment::Mode::BYTE         (0x4, 8, 16, 16);
const QrSegment::Mode QrSegment::Mode::KANJI        (0x8, 8, 10, 12);
const QrSegment::Mode QrSegment::Mode::ECI          (0x7, 0, 0 , 0);

QrSegment QrSegment::makeBytes(const vector<uint8_t> &data) {
    if (data.size() > static_cast<unsigned int>(INT_MAX)) 
        throw std::length_error("Data too long");
    BitBuffer bb;
    for (uint8_t b : data) {
        bb.appendBits(b, 8);
    }
    return QrSegment(Mode::BYTE, static_cast<int>(data.size()), std::move(bb));
}


}