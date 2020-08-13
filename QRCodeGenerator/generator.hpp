#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <stdexcept>

using namespace std;

namespace qrcodeGen {

class QrSegment final {
/*
 * Public helper enumeration
 * 
 * Describes how a segment's data bits are interpreted.
 * Immutable
*/
    public: class Mode final {
        /* Constants */ 
        public: static const Mode NUMERIC;
        public: static const Mode ALPHANUMERIC;
        public: static const Mode BYTE;
        public: static const Mode KANJI;
        public: static const Mode ECI;

        /* Fields */ 

        // The mode indicator bits, which is a uint4 value (range 0 to 15)
        private: int modeBits;

        // Number of character count bits for three different version ranges
        private: int numBitCharCount[3];

        /* Constructor */
        private: Mode(int mode, int cc0, int cc1, int cc2);

        /* Methods */

        /*
         * (Package private)
         * Returns the mode indicator bits, which is an unsigned 4-bits value
        */
        public: int getModeBits() const;

        /*
         * (Package private)
         * Returns the bit width of the character count field for a segment in
         * this mode in a QR Code at the given version number.
         * 
         * The result is in the range [0, 16]
        */
        public: int numCharCountBits(int ver) const;
    };


};

} // namespace qrcodeGen