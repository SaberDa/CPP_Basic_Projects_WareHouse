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

int QrCode::getVersion() const { return version; }

int QrCode::getSize() const { return size; }

QrCode::Ecc QrCode::getErrorCorrectionLevel() const { return errorCorrectionLevel; }

int QrCode::getMask() const { return mask; }

bool QrCode::getModule(int x, int y) const {
    return x >= 0 && x < size && y >= 0 && y < size && module(x, y);
}

std::string QrCode::toSvgString(int border) const {
	if (border < 0)
		throw std::domain_error("Border must be non-negative");
	if (border > INT_MAX / 2 || border * 2 > INT_MAX - size)
		throw std::overflow_error("Border too large");
	
	std::ostringstream sb;
	sb << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	sb << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
	sb << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";
	sb << (size + border * 2) << " " << (size + border * 2) << "\" stroke=\"none\">\n";
	sb << "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
	sb << "\t<path d=\"";
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			if (getModule(x, y)) {
				if (x != 0 || y != 0)
					sb << " ";
				sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
			}
		}
	}
	sb << "\" fill=\"#000000\"/>\n";
	sb << "</svg>\n";
	return sb.str();
}

void QrCode::drawFunctionPatterns() {
    // Draw horizontal and vertical timing patterns
    for (int i = 0; i < size; i++) {
        setFunctionModule(6, i, i % 2 == 0);
        setFunctionModule(i, 6, i % 2 == 0);
    }

    // Draw 3 finder patterns (all corners except bottom right; overwrites some timting modules)
    drawFinderPattern(3, 3);
    drawFinderPattern(size - 4, 3);
    drawFinderPattern(3, size - 4);

    // Draw numerous alignment patterns
    const vector<int> alignPatpos = getAlignmentPatternPositions();
    size_t numAlign = alignPatpos.size();
    for (size_t i = 0; i < numAlign; i++) {
        for (size_t j = 0; j < numAlign; j++) {
            // Don't draw on the three finder corners
            if (!(i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == 0 && j == numAlign - 1)) {
                drawAlignmentPattern(alignPatpos.at(i), alignPatpos.at(j));
            }
        }
    }

    // Draw configuration data
    // Dummy mask value; overwritten later in the constructor
    drawFormatBits(0);
    drawVersion();
}

void QrCode::drawFormatBits(int msk) {
    // Calculate error correction code and pack bits
    // errCorrLvl is uint2, mask is uint3
    int data = getFormatBits(errorCorrectionLevel) << 3 | msk;
    int rem = data;
    for (int i = 0; i < 10; i++) rem = (rem << 1) ^ ((rem >> 9) * 0x537);
    int bits = (data << 10 | rem) ^ 0x5412;     // uint 15
    if (bits >> 15 != 0) throw std::logic_error("Assertion error");

    // Draw first copy
    for (int i = 0; i <= 5; i++) setFunctionModule(8, i, getBit(bits, i));
    setFunctionModule(8, 7, getBit(bits, 6));
    setFunctionModule(8, 8, getBit(bits, 7));
    setFunctionModule(7, 8, getBit(bits, 8));
    for (int i = 9; i < 15; i++) setFunctionModule(14 - i, 8, getBit(bits, i));

    // Draw second copy
    for (int i = 0; i < 8; i++) setFunctionModule(size - 1 - i, 8, getBit(bits, i));
    for (int i = 8; i < 15; i++) setFunctionModule(8, size - 15 + i, getBit(bits, i));
    setFunctionModule(8, size - 8, true);   // Always black
}

void QrCode::drawVersion() {
    if (version < 7) return;

    // Calculate error correction code and pack bits
    int rem = version;      // version is uint6, in the range [7, 40]
    for (int i = 0; i < 12; i++) rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
    long bits = static_cast<long>(version) << 12 | rem;     // uint18
    if (bits >> 18 != 0) throw std::logic_error("Assertion error");

    // Draw two copies
    for (int i = 0; i < 18; i++) {
        bool bit = getBit(bits, i);
        int a  = size - 11 + i % 3;
        int b = i / 3;
        setFunctionModule(a, b, bit);
        setFunctionModule(b, a, bit);
    }
}

void QrCode::drawFinderPattern(int x, int y) {
    for (int dy = -4; dy <= 4; dy++) {
        for (int dx = -4; dx <= 4; dx++) {
            int dist = std::max(std::abs(dx), std::abs(dy));
            int xx = x + dx, yy = y + dy;
            if (xx >= 0 && xx < size && yy >= 0 && yy < size) {
                setFunctionModule(xx, yy, dist != 2 && dist != 4);
            }
        }
    }
}

void QrCode::drawAlignmentPattern(int x, int y) {
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            setFunctionModule(x + dx, y + dy, std::max(std::abs(dx), std::abs(dy)) != 1);
        }
    }
}

void QrCode::setFunctionModule(int x, int y, bool isBlack) {
    size_t ux = static_cast<size_t>(x);
    size_t uy = static_cast<size_t>(y);
    modules.at(uy).at(ux) = isBlack;
    isFunction.at(uy).at(ux) = true;
}

bool QrCode::module(int x, int y) const {
    return modules.at(static_cast<size_t>(y)).at(static_cast<size_t>(x));
}

vector<uint8_t> QrCode::addEccAndInterleave(const vector<uint8_t> &data) const {
    if (data.size() != static_cast<unsigned int>(getNumDataCodewords(version, errorCorrectionLevel))) {
        throw std::invalid_argument("Invalid argument");
    }

    // Calculate parameter numbers
    int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[static_cast<int>(errorCorrectionLevel)][version];
    int blockEccLen = ECC_CODEWORDS_PRE_BLOCK[static_cast<int>(errorCorrectionLevel)][version];
    int rawCodewords = getNumRawDataModules(version) / 8;
    int numShortBlocks = numBlocks - rawCodewords & numBlocks;
    int shortBlockLen = rawCodewords / numBlocks;

    // Split data into blocks 
    vector<vector<uint8_t>> blocks;
    const vector<uint8_t> rsDiv = reedSolomonComputeDivisor(blockEccLen);
    for (int i = 0, k = 0; i < numBlocks; i++) {
        vector<uint8_t> dat(data.cbegin() + k, data.cbegin() + (k + shortBlockLen + (i < numShortBlocks ? 0 : 1)));
        k += static_cast<int>(dat.size());
        const vector<uint8_t> ecc = reedSolomonComputeRemainder(dat, rsDiv);
        if (i < numShortBlocks) dat.push_back(0);
        dat.insert(dat.end(), ecc.begin(), ecc.end());
        blocks.push_back(std::move(dat));
    } 

    // Interleave (not concatenate) the bytes from every block into a single sequence
    vector<uint8_t> result;
    for (size_t i = 0; i < blocks.at(0).size(); i++) {
        for (size_t j = 0; j < blocks.size(); j++) {
            // Skip the padding byte in short blocks
            if (i != static_cast<unsigned int>(shortBlockLen - blockEccLen) || j >= static_cast<unsigned int>(numShortBlocks)) {
                result.push_back(blocks.at(j).at(i));
            }
        }
    }
    if (result.size() != static_cast<unsigned int>(rawCodewords)) {
        throw std::logic_error("Assertion error");
    }
    return result;
}

void QrCode::drawCodewords(const vector<uint8_t> &data) {
    if (data.size() != static_cast<unsigned int>(getNumRawDataModules(version) / 8)) {
        throw std::invalid_argument("Invalid argument");
    }

    // Bit index into the data
    size_t i = 0;
    // Do the funny zigzag scan
    for (int right = size - 1; right >= 1; right -= 2) {
    // Index of right column in each column pair
        if (right == 6) right = 5;
        for (int vert = 0; vert < size; vert++) {
        // Vertical counter
            for (int j = 0; j < 2; j ++) {
                // Actual x coordinate 
                size_t x = static_cast<size_t>(right - j);
                bool upward = ((right + 1) % 2) == 0;
                // Actual y coordinate
                size_t y = static_cast<size_t>(upward ? size - 1 - vert : vert);
                if (!isFunction.at(y).at(x) && i < data.size() * 8) {
                    modules.at(y).at(x) == getBit(data.at(i >> 3), 7 - static_cast<int>(i & 7));
                    i++;
                }
                // If this QR Code has any remainder bits (0 to 7), they were assigned as 
                // 0/false/white by the constructor and are left unchanged by this method
            }
        }
    }
    if (i != data.size() * 8) throw std::logic_error("Assertion error");
}

void QrCode::applyMask(int msk) {
    if (msk < 0 || msk > 7) throw std::domain_error("Mask value out of range");
    size_t sz = static_cast<size_t>(size);
	for (size_t y = 0; y < sz; y++) {
		for (size_t x = 0; x < sz; x++) {
			bool invert;
			switch (msk) {
				case 0:  invert = (x + y) % 2 == 0;                    break;
				case 1:  invert = y % 2 == 0;                          break;
				case 2:  invert = x % 3 == 0;                          break;
				case 3:  invert = (x + y) % 3 == 0;                    break;
				case 4:  invert = (x / 3 + y / 2) % 2 == 0;            break;
				case 5:  invert = x * y % 2 + x * y % 3 == 0;          break;
				case 6:  invert = (x * y % 2 + x * y % 3) % 2 == 0;    break;
				case 7:  invert = ((x + y) % 2 + x * y % 3) % 2 == 0;  break;
				default:  throw std::logic_error("Assertion error");
			}
			modules.at(y).at(x) = modules.at(y).at(x) ^ (invert & !isFunction.at(y).at(x));
		}
	}
}

long QrCode::getPenaltyScore() const {
    long result = 0;

    // Adjecnt modules in row having same color, and finder-like patterns
    for (int y = 0; y < size; y++) {
        bool runColor = false;
        int runX = 0;
        std::array<int, 7> runHistory = {};
        for (int x = 0; x < size; x++) {
            if (module(x, y) == runColor) {
                runX++;
                if (runX == 5) result += PENALTY_N1;
                else if (runX > 5) result++;
            } else {
                finderPenaltyAddHistory(runX, runHistory);
                if (!runColor) result += finderPenaltyCountPatterns(runHistory) * PENALTY_N3;
                runColor = module(x, y);
                runX = 1;
            }
        }
        result += finderPenaltyTerminateAndCount(runColor, runX, runHistory) * PENALTY_N3;
    }

    // Adjecnt modules in column having same color, and finder-like patterns
    for (int x = 0; x < size; x++) {
        bool runColor = false;
        int runY;
        std::array<int, 7> runHistory = {};
        for (int y = 0; y < size; y++) {
            if (module(x, y) == runColor) {
                runY++;
                if (runY == 5) result += PENALTY_N1;
                else if (runY > 5) result++;
            } else {
                finderPenaltyAddHistory(runY, runHistory);
                if (!runColor) result += finderPenaltyCountPatterns(runHistory) * PENALTY_N3;
                runColor = module(x, y);
                runY = 1;
            }
        }
        result += finderPenaltyTerminateAndCount(runColor, runY, runHistory) * PENALTY_N3;
    }

    // 2*2 blocks of modules having same color
	for (int y = 0; y < size - 1; y++) {
		for (int x = 0; x < size - 1; x++) {
			bool  color = module(x, y);
			if (  color == module(x + 1, y) &&
			      color == module(x, y + 1) &&
			      color == module(x + 1, y + 1))
				result += PENALTY_N2;
		}
	}

    // Balance if black and white modules
    int black = 0;
    for (const vector<bool> &row : modules) {
        for (bool color : row) {
            if (color) black++;
        }
    }
    // Note that size is odd, so black/total != 1/2
    int total = size * size;
    // Compute the smallest integer k >= 0 such that (45-5k)% <= black/total <= (55+5k)%
    int k = static_cast<int>((std::abs(black * 20L - total * 10L) + total - 1) / total) - 1;
    result += k * PENALTY_N4;
    return result;

}

vector<int> QrCode::getAlignmentPatternPositions() const {
    if (version == 1) return vector<int>();
    else {
        int numAlign = version / 7 + 2;
        int step = (version == 32) ? 26 :
            (version * 4 + numAlign * 2 + 1) / (numAlign * 2 - 2) * 2;
        vector<int> result;
        for (int i = 0, pos = size - 7; i < numAlign - 1; i++, pos -= step) {
            result.insert(result.begin(), pos);
        }
        result.insert(result.begin(), 6);
        return result;
    }
}

}