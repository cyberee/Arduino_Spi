This program is designed to interface with MX25L1006E (May work with others)
The schematic currently show  the MX25L1006E connected to an Arduino Nano

Current Commands:

RDID:  Read ID
RDSR:  Read Status Register
REMS:  Read Electronic Manufacturer ID
RES:  Read Electronic Signature
READ,*:  Read Memory followed by number of 4K Sectors

TODO:

Add compile time if def for cpu types for working with 16 Bit or 32 Bit CPU’s (issue with high order addresses)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define DEBUG_CAPTURE_SIZE 7168
#define CAPTURE_SIZE 7168
#elif defined(__AVR_ATmega328P__)
#define DEBUG_CAPTURE_SIZE 1024
#define CAPTURE_SIZE 1024
#else
#define DEBUG_CAPTURE_SIZE 532
#define CAPTURE_SIZE 532
#endif

Add help function
Add write functions 
Add Read SFDP Serial Flash Discoverable Parameters
