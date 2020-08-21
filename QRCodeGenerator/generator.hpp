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
        /* ---- Constants ---- */ 
        public: static const Mode NUMERIC;
        public: static const Mode ALPHANUMERIC;
        public: static const Mode BYTE;
        public: static const Mode KANJI;
        public: static const Mode ECI;

        /* ---- Fields ---- */ 

        // The mode indicator bits, which is a uint4 value (range 0 to 15)
        private: int modeBits;

        // Number of character count bits for three different version ranges
        private: int numBitCharCount[3];

        /* Constructor */
        private: Mode(int mode, int cc0, int cc1, int cc2);

        /* ---- Methods ---- */

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

    /* ---- Static factory functions (mid level) ---- */

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


    /* ---- Public static helper functions ---- */

    /*
     * Tests whether the given string can be encoded as a segment in 
     * alphanumeric mode. 
     * A string is encodable if each character is in the following set:
     * 0 to 9, A to Z (uppercase only), space, dollar, percent, asterisk,
     * plus, hyphen, period, slash and colon
    */
    public: static bool isAlphanumeric(const char* text);

    /*
     * Tests whether the given string can be encoded as a segment in numeric
     * mode. 
     * A string is encodable if each character is in the range 0 to 9
    */
    public: static bool isNumberic(const char* text);


    /* ---- Instance fields ---- */

    /*
     * The mode indicator of this segment. Accessed through getMode().
    */
    private: Mode mode;

    /*
     * The length of this segment's unencoded data. Measured in characters 
     * for numeric/alphanumeric/kanji mode, bytes for byte mode, 
     * and 0 for ECI mode. Always zero or positive. Not the same as the data's
     * bit length.
     * Accessed through getNumChars().
    */
    private: int numChars;

    /*
     * The data bits of this segment. Accessed through getData().
    */
    private: std::vector<bool> data;


    /* ---- Constructors (low level) ---- */

    /*
     * Create a new QR Code segment with the given attributes and data.
     * The character count (numCh) must agree with the mode and the bit
     * buffer length, but the constraint isn't checked. The given bit buffer
     * is copied and stroed.
    */
    public: QrSegment(Mode md, int numCh, const std::vector<bool> &dt);

    /*
     * Create a new QR Code segment with the given attributes and data.
     * The character count (numCh) must agree with the mode and the bit
     * buffer length, but the constraint isn't checked. The given bit buffer
     * is copied and stroed.
    */
    public: QrSegment(Mode md, int numCh, std::vector<bool> &dt);

    
    /* ---- Method ---- */

    /*
     * Returns the mode field of this segment.
    */
    public: Mode getMode() const;

    /*
     * Returns the character const field of this segment.
    */
    public: int getNumChars() const;

    /*
     * Returns the data bits of this segment.
    */
    public: const std::vector<bool> &getData() const;

    /*
     * (Package Private)
     * Calculates the number of bits needed to encode the given 
     * segments at the given version. Returns a non-negative number
     * if successful. Otherwise returns -1 if a segment has too many 
     * characters to fit its length field, or the total bits exceeds
     * INT_MAX.
    */
    public: static int getTotalBits(const std::vector<QrSegment> &segs, int version);


    /* ---- Private constant ---- */

    /*
     * The set of all legal characters in alphanumeric mode, 
     * where each character value maps to the index in the 
     * string. 
    */
    private: static const char* ALPHANUMERIC_CHARSET;
};


/*
 * A QR Code symbol, which is a type of two-dimension barcode.
 * Invented by Dense Wave and describe in the ISO/IEC 18004 standard.
 * Instances of this class represents an immutable square grid of 
 * black and white cells. The class provide static factory functions 
 * to create a QR Code from text or binary data. The class covers the 
 * QR Code Model 2 specification, supporting all versions (sizes)
 * from 1 to 40, all 4 error correction levels, and 4 character 
 * encoding nodes.
 * 
 * Ways to create a QR Code objects:
 * - High level: Take the payload data and call QrCode::encodeText() or QrCode::encodeBinary().
 * - Mid level: Custom-make the list of segments and call QrCode::encodeSegment().
 * - Low level: Custom-make the array if data codeword bytes(including 
 *              segment headers and final padding, excluding error correction
 *              codewords), supply the appropriate version number, and call the 
 *              QrCode() constructor.
 * (Note that all ways require supplying the desired error correction level).
*/
class QrCode final {


    /* ---- Public helper enumeration ---- */

    /*
     * The error correction level in a QR Code symbol.
    */
    public: enum class Ecc {
        LOW = 0,        // The QR Code can tolerate about  7% erroneous codewords
        MEDIUM,         // The QR Code can tolerate about 15% erroneous codewords
        QUARTILE,       // The QR Code can tolerate about 25% erroneous codewords
        HIGH,           // The QR Code can tolerate about 30% erroneous codewords
    };

    /*
     * Returns a value in the range 0 to 3 (unsigned 2-bit integer)  
    */
    private: static int getFormatBits(Ecc ecl);


    /* ---- Static factory functions (high level) ---- */

    /*
     * Returns a QR Code representing the given Unicode text string at the given error correction level.
     * As a conservation upper bound, this function is guaranteed to succeed for strings 
     * that have 2953 or fewer UTF-8 code units (not Unicode code points) if the 
     * low error correctino level is used. The smallest possible QR Code version is automatically 
     * chosen for the output. The ECC level of the result may be higher than the ECL argument if
     * it can be done without increasing the version.
    */
    public: static QrCode encodeText(const char* text, Ecc ecl);

    /*
     * Returns a QR Code representing the given binary data at the given error correction level.
     * This function always encodes using the binary segment mode, not ant text mode. The maximum
     * number of bytes allowed is 2953. The smallest possible QR Code version is automatically
     * chosen for the output. The ECC level of the result may be higher than the ecl argument 
     * if it can be done without increasing the version.
    */
    public: static QrCode encodeBinary(const std::vector<std::uint8_t> &data, Ecc ecl);


    /* ---- Static factory functions (mid level) ---- */

	/* 
	 * Returns a QR Code representing the given segments with the given encoding parameters.
	 * The smallest possible QR Code version within the given range is automatically
	 * chosen for the output. Iff boostEcl is true, then the ECC level of the result
	 * may be higher than the ecl argument if it can be done without increasing the
	 * version. The mask number is either between 0 to 7 (inclusive) to force that
	 * mask, or -1 to automatically choose an appropriate mask (which may be slow).
	 * This function allows the user to create a custom sequence of segments that switches
	 * between modes (such as alphanumeric and byte) to encode text in less space.
	 * This is a mid-level API; the high-level API is encodeText() and encodeBinary().
	*/
    public: static QrCode encodeSegments(const std::vector<QrSegment> &segs, Ecc ecl, 
                                         int minVersion = 1, int maxVersion = 40, int mask = -1,
                                         bool boostEcl = true);         // All optional parameters
    

    /* ---- Instance fields ---- */

    // Immutable scalar parameters

    /*
     * The version number of this QR Code, which is between 1 to 40 (inclusive).
     * This determines the size of this barcode.
    */
    private: int version;

    /*
     * The width and height of this QR Code, measured in modules, 
     * between 21 and 177 (inclusive). This is equal to version * 4 + 17
    */
    private: int size;

    /*
     * The error correction level used in this QR Code
    */
    private: Ecc errorCorrectionLevel;

    /*
     * The index of the mask pattern used in this QR Code, which is
     * between 0 and 7 (inclusive). Even if a QR Code is created with 
     * automatic masking requested (mask = -1), the resulting object
     * still has a mask value between 0 and 7.
    */
    private: int mask;

    // Private grids of modules/pixels, with dimensions of size * size

    /*
     * The modules of this QR Code (false = white, true = black)
     * Immutable after constructor finishes. Accessed through getModule()
    */
    private: std::vector<std::vector<bool>> modules;

    /*
     * Indicates function modules that are not subjected to masking. 
     * Discarded when constructor finished
    */
    private: std::vector<std::vector<bool>> isFunction;


    /* ---- Constructure (low level) ---- */

    /*
     * Creates a new QR Code with the given version number, error
     * correction level, data codeword bytes, and mask number.
     * This is a low-level API that most users shoule not use 
     * directly. 
     * A mid-level API is the encodeSegment() function.
    */
    public: QrCode(int ver, Ecc ecl, const std::vector<std::uint8_t> &dataCodeWords, int msk);


    /* ---- Public instance methods ---- */

    /*
     * Returns this QR Code's version, in the range [1, 40]
    */
    public: int getVersion() const;

    /*
     * Returns this QR Code's size, int the range [21, 177]
    */
    public: int getSize() const;

    /*
     * Returns this QR Code's error correction level
    */
    public: Ecc getErrotCorrectionLevel() const;

    /*
     * Returns this QR Code's mask, in the range [0, 7]
    */
    public: int getMask() const;

    /*
     * Returns the color of the module (pixel) at the given coordinates, which
     * is false for white or true for black. The top left conner has coordinates
     * (x = 0, y = 0). If the given coordinates are out of the bound, then false
     * (white) is returned.
    */
    public: bool getModule(int x, int y) const;

    /*
     * Returns a string of SVG code for an image depicting this QR Code, with
     * the given number of border modules. The string always uses Unix newlines(\n),
     * regardless of the platform.
    */
    public: std::string toSvgString(int border) const;


    /* ---- Private helper methods for constructor: Drawing function modules ---- */

    /*
     * Reads this object's version field, and draws and marks all function modules
    */
    private: void drawFunctionPatterns();

    /*
     * Draws two copies of the format bits (with its own error correction code)
     * based on the given mask and this object's error correction level field.
    */
    private: void drawFormatBits(int msk);

    /*
     * Draws two copies of the version bits (with its own error correction code)
     * based on this object's version field, if 7 <= version <= 40 
    */
    private: void drawVersion();

    /*
     * Draws a 9 * 9 finder pattern including the border separator, 
     * with the center module at (x, y). Modules can be out of bounds.
    */
    private: void drawFinderPattern(int x, int y);

    /*
     * Draws a 5 * 5 alignment pattern, with the center module at (x, y).
     * All modules must be in bounds.
    */
    private: void drawAlignmentPattern(int x, int y);

    /*
     * Sets the color of a module and marks it as a function module.
     * Only used by the constructor. Coordinates must by in bound.
    */
    private: void setFunctionModule(int x, int y, bool isBlack);

    /*
     * Returns the color of the module at the given coordinates, 
     * which must be in range.
    */
    private: bool module(int x, int y) const;




    /*
     * 
    */

};
} // namespace qrcodeGen