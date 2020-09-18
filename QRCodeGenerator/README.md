A QRCode Generator
---

[![License](https://img.shields.io/badge/License-MIT%20License-blue.svg)](https://opensource.org/licenses/MIT)

---

## How to use

In the ```main.cpp```:

### Input the text

```
// Fix your input text
const char *text = "https://github.com/SaberDa/CPP_Basic_Projects_WareHouse/tree/master/QRCodeGenerator";

// Or input text in command line
// std::cout << "Please input your text: " << std::endl;
// std::string data;
// std::getline(std::cin, data);
// const char *text = data.c_str();
```

### Change the QrCode Style

```
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
QrCode qrcode = QrCode::encodeSegments(segs, QrCode::Ecc::LOW, 5, 5, -1, false);
```

---

## How to run

In the QRCodeGenerator dir:

```
g++ main.cpp generator.cpp
./a.out
```