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

int QrSegment::getTotalBits(const vector<QrSegment> &segs, int version) {
    int result = 0;
    for (const QrSegment &seg : segs) {
        int ccbits = seg.mode.numCharCountBits(version);
        // The segment's length doesn't fit the field's bit width
        if (seg.numChars >= (1L << ccbits)) return -1;
        // The sum will overflow an int type
        if (4 + ccbits > INT_MAX - result) return -1;
        result += 4 + ccbits;
        // The sum will overflow an int type
        if (seg.data.size() > static_cast<unsigned int>(INT_MAX - result)) return -1;
        result += static_cast<int>(seg.data.size());
    }
    return result;
}

bool QrSegment::isAlphanumeric(const char *text) {
    for (; *text != '\0'; text++) {
        if (std::strchr(ALPHANUMERIC_CHARSET, *text) == nullptr) return false;
    }
    return true;
}

bool QrSegment::isNumberic(const char *text) {
    for (; *text != '\0'; text++) {
        char c = *text;
        if (c < '0' || c > '9') return false;
    }
    return true;
}

QrSegment::Mode QrSegment::getMode() const {
    return mode;
}

int QrSegment::getNumChars() const {
    return numChars;
}

const std::vector<bool> &QrSegment::getData() const {
    return data;
}

const char *QrSegment::ALPHANUMERIC_CHARSET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

int QrCode::getFormatBits(Ecc ecl) {
    switch (ecl) {
        case Ecc::LOW : 
            return 1;
        case Ecc::MEDIUM:
            return 0;
        case Ecc::QUARTILE:
            return 3;
        case Ecc::HIGH:
            return 2;
        default:
            throw std::logic_error("Assertion error");
    }
}

QrCode QrCode::encodeText(const char *text, Ecc ecl) {
    vector<QrSegment> segs = QrSegment::makeSegments(text);
    return encodeSegments(segs, ecl);
}

QrCode QrCode::encodeBinary(const vector<uint8_t> &data, Ecc ecl) {
    vector<QrSegment> segs{QrSegment::makeBytes(data)};
    return encodeSegments(segs, ecl);
}

QrCode QrCode::encodeSegments(const vector<QrSegment> &segs, Ecc ecl,
                int minVersion, int maxVersion, int mask, bool boostEcl) {
    if (!(minVersion >= MIN_VERSION && maxVersion <= MAX_VERSION && maxVersion >= minVersion) || mask < -1 || mask > 7) {
        throw std::invalid_argument("Invalid value");
    }

    // Find the minimal version number to use
    int version, dataUseBits;
    for (version = minVersion; ; version++) {
        // number of data bits available
        int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;
        dataUseBits = QrSegment::getTotalBits(segs, version);
        if (dataUseBits != -1 && dataUseBits <= dataCapacityBits) break;
        if (version >= maxVersion) {
            std::ostringstream sb;
            if (dataUseBits == -1) sb << "Segment too long";
            else {
                sb << "Data length = " << dataUseBits << " bits, ";
                sb << "Max capacity = " << dataCapacityBits << " bits";
            }
            throw data_too_long(sb.str());
        }
    }
    if (dataUseBits == -1) throw std::logic_error("Assertion error");

    // Increase the error correction level while the data still fits in the current version number
    // From low to high
    for (Ecc newEcl : vector<Ecc>{Ecc::MEDIUM, Ecc::QUARTILE, Ecc::HIGH}) {
        if (boostEcl && dataUseBits <= getNumDataCodewords(version, newEcl) * 8) {
            ecl = newEcl;
        }
    }

    // Concatenate all segments to create the data bit string
    BitBuffer bb;
    for (const QrSegment &seg : segs) {
        bb.appendBits(static_cast<uint32_t>(seg.getMode().getModeBits()), 4);
        bb.appendBits(static_cast<uint32_t>(seg.getNumChars()), seg.getMode().numCharCountBits(version));
        bb.insert(bb.end(), seg.getData().begin(), seg.getData().end());
    }
    if (bb.size() != static_cast<unsigned int>(dataUseBits)) {
        throw std::logic_error("Assertion error");
    }

    // Add terminator and pad up to a byte if applicable
    size_t dataCapacityBits = static_cast<size_t>(getNumDataCodewords(version, ecl)) * 8;
    if (bb.size() > dataCapacityBits) throw std::logic_error("Assertion error");
    bb.appendBits(0, std::min(4, static_cast<int>(dataCapacityBits - bb.size())));
    bb.appendBits(0, (8 - static_cast<int>(bb.size() % 8)) % 8);
    if (bb.size() % 8 != 0) throw std::logic_error("Assertion error");

    // Pad with alternation bytes until data capacity is reached
    for (uint8_t padByte = 0xEC; bb.size() < dataCapacityBits; padByte ^= 0xEC ^ 0x11) {
        bb.appendBits(padByte, 8);
    }

    // Pack bits into bytes in big endian
    vector<uint8_t> dataCodewords(bb.size() / 8);
    for (size_t i = 0; i < bb.size(); i++) {
        dataCodewords[i >> 3] |= (bb.at(i) ? 1 : 0) << (7 - (i & 7));
    }

    // Create the QR code object
    return QrCode(version, ecl, dataCodewords, mask);
}

QrCode::QrCode(int ver, Ecc ecl, const vector<uint8_t> &dataCodewords, int msk) :
        version(ver), errorCorrectionLevel(ecl) {
    if (ver < MIN_VERSION || ver > MAX_VERSION) throw std::domain_error("Version value out of range");
    if (msk < -1 || msk > 7) throw std::domain_error("Mask value out of range");
    size = ver * 4 + 17;
    size_t sz = static_cast<size_t>(size);
    // Initially with all white
    modules = vector<vector<bool>>(sz, vector<bool>(sz));
    isFunction = vector<vector<bool>>(sz, vector<bool>(sz));

    // Compute Ecc, draw modules
    drawFunctionPatterns();
    const vector<uint8_t> allCodewords = addEccAndInterleave(dataCodewords);
    drawCodewords(allCodewords);

    // Do masking
    if (msk == -1) {
        // Automatically choose best mask
        long minPenalty = LONG_MAX;
        for (int i = 0; i < 8; i++) {
            applyMask(i);
            drawFormatBits(i);
            long penalty = getPenaltyScore();
            if (penalty < minPenalty) {
                msk = i;
                minPenalty = penalty;
            }
            applyMask(i);
        }
    }
    if (msk < 0 || msk > 7) throw std::logic_error("Assertion error");
    this->mask = msk;
    // Apply the final choice of mask
    applyMask(msk);
    // Overwrite old format bits
    drawFormatBits(msk);

    isFunction.clear();
    isFunction.shrink_to_fit();
}

}