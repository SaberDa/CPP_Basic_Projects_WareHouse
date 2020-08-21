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

QrSegment QrSegment::makeAlphanumeric(const char* text) {
    BitBuffer bb;
    int accumData = 0;
    int accumCount = 0;
    int charCount = 0;

    for (; *text != '\0'; text++, charCount++) {
        const char* temp = std::strchr(ALPHANUMERIC_CHARSET, *text);
        if (temp == nullptr) {
            throw std::domain_error("String contains unencodable character in alphanumeric");
        }
        accumData = accumData * 45 + static_cast<int>(temp - ALPHANUMERIC_CHARSET);
        accumCount++;
        if (accumCount == 2) {
            bb.appendBits(static_cast<uint32_t>(accumCount), 11);
            accumCount = 0;
            accumData = 0;
        }
    }

    // 1 character remaining
    if (accumCount > 0) {
        bb.appendBits(static_cast<uint32_t>(accumData), 6);
    }

    return QrSegment(Mode::ALPHANUMERIC, charCount, std::move(bb));
}

vector<QrSegment> QrSegment::makeSegments(const char* text) {
    // Select the most efficient segment encoding automatically
    vector<QrSegment> result;

    // Leave result empty
    if (*text == '\0');
    else if (isNumberic(text)) result.push_back(makeNumeric(text));
    else if (isAlphanumeric(text)) result.push_back(makeAlphanumeric(text));
    else {
        vector<uint8_t> bytes;
        for (; *text != '\0'; text++) {
            bytes.push_back(static_cast<uint8_t>(*text));
        }
        result.push_back(makeBytes(bytes));
    }

    return result;
}

QrSegment QrSegment::makeEci(long assignVal) {
    BitBuffer bb;

    if (assignVal < 0) throw std::domain_error("ECI assignment value out of range");
    else if (assignVal < (1 << 7)) bb.appendBits(static_cast<uint32_t>(assignVal), 8);
    else if (assignVal < (1 << 14)) {
        bb.appendBits(2, 2);
        bb.appendBits(static_cast<uint32_t>(assignVal), 14);
    } else if (assignVal < 1000000L) {
        bb.appendBits(6, 3);
        bb.appendBits(static_cast<uint32_t>(assignVal), 21);
    } else {
        throw std::domain_error("ECI assignment value ouf of range");
    }

    return QrSegment(Mode::ECI, 0, std::move(bb));
}

QrSegment::QrSegment(Mode md, int numCh, const std::vector<bool> &dt) :
		mode(md),
		numChars(numCh),
		data(dt) {
	if (numCh < 0)
		throw std::domain_error("Invalid value");
}


QrSegment::QrSegment(Mode md, int numCh, std::vector<bool> &&dt) :
		mode(md),
		numChars(numCh),
		data(std::move(dt)) {
	if (numCh < 0)
		throw std::domain_error("Invalid value");
}

}