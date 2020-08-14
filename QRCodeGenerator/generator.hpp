#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <stdexcept>



namespace qrcodeGen {

/*
 * A segment of character/binary/control data in a QR Code symbol.
 * Instances of this class are immutable.
 * 
 * The mid-level way to create a segment is to take the payload data
 * and call a static factory function such as QrSegment::makeNumeric().
 * 
 * The low-level way to create a segment is to custom-make the bit buffer
 * and call the QrSegment() constructor with appropriate values.
 * 
 * This segment class imposes no length restrictions, but QR Code have restrictions.
 * Even in the most favorable conditions, a QR Code can only hold 7089 characters of data.
 * Any segment longer than this is meaningless for the purpose of generation QR Code.
*/

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

    /* ---- Static factory functions (mid level) */

    /*
     * Returns a segment representing the given binary data encoded in 
     * byte mode. All input byte vectors are acceptable. Any text string
     * can be converted to UTF-8 bytes and encoded as a byte mode segment.
    */
    public: static QrSegment makeBytes(const std::vector<std::uint8_t> &data);

    /*
     * Returns a segment representing the given string of decimal digits 
     * encoded in numeric mode
    */
    public: static QrSegment makeNumeric(const char* digits);

    /*
     * Returns a segment representing the given text string encoded in 
     * alphanumeric mode. The characters allowed are : 0 to 9, 
     * A to Z (uppercase only), space, dollar, percent, asterisk,
     * plus, hyphen, period, slash and colon
    */
    public: static QrSegment makeAlphanumeric(const char* text);

    /*
     * Returns a list of zero or more segments to represent the given 
     * text string. The result may use various segment modes and switch
     * modes to optimize the length of the bit stream
    */
    public: static std::vector<QrSegment> makeSegments(const char *text);

    /*
     * Returns a segment representing an Extended Channel Interpretation
     * (ECI) designator with the given assignment value
    */
    public: static QrSegment makeEci(long assignVal);

};

} // namespace qrcodeGen