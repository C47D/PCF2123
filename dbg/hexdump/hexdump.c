#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "hexdump.h"

#include "DBG.h"

/**
 * Print a single hexadecimal value to the provided stream.
 * The value will be prefixed by zeroes, when required, to force the
 * number of output digits to the 2 times the size of the valueT.
 * @tparam valueT  Type of value to be printed, e.g. size_t or uint8_t.
 * @param  stream  Output stream (e.g. Serial) to print to.
 * @param  val     Value to print as hexadecimal.
 */
static void PrintHex(const uint8_t val)
{
    const uint8_t digits = sizeof(uint8_t) << 1;
    uint8_t i = 0;
    
    while (i < digits) {
        // Each nibble stores 1 digit
        uint8_t v = (val >> ((digits - i - 1) << 2)) & 0x0F;
        DBG_print("%X", v);
        ++i;
    }
}

static void PrintDir(const uint16_t val)
{
    const uint8_t digits = sizeof(uint16_t) << 1;
    uint8_t i = 0;
    
    while (i < digits) {
        // Each nibble stores 1 digit
        uint8_t v = (val >> ((digits - i - 1) << 2)) & 0x0F;
        DBG_print("%X", v);
        ++i;
    }
}

/**
 * Dump a block of data as hexadecimal and ASCII to the provided stream.
 * @tparam addrT        Address type. This type determines the amount of digits displayed for
 *                      the address, e.g. size_t to leave it up to the architecture or uint8_t
 *                      to use only a single byte.
 * @tparam bytesPerRow  Number of bytes to print per row, e.g. 16 or 8.
 * @param  stream       Output stream (e.g. Serial) to print to.
 * @param  buff         Ptr to block of data to print.
 * @param  len          Length of block to print, in [bytes].
 * @param  base         Optional address offset to use in priting.
*/
void HexDump(uint8_t *buff, size_t len, size_t base)
{
    const uint8_t bytesPerRow = 16;
    const size_t rows = (len + bytesPerRow - 1) / bytesPerRow;
    uint8_t *p = buff;

    for (size_t r = 0; r < rows; ++r) {
        // imprime las direcciones
        PrintDir(base + p - buff);
        DBG_print(": ");

        char *pc = (char *) p;
        const size_t cols = len < bytesPerRow ? len : bytesPerRow;
        
        // impresion de bytes
        for (size_t c = 0; c < bytesPerRow; ++c) {
            if (c < cols) {
                PrintHex(*p++);
            } else {
                DBG_print("  ");
            }
            
            DBG_print(" ");
        }
        
        DBG_print(" |");
        
        // impresion de ascii
        for (size_t i = 0; i < cols; ++i) {
            char c = *pc++;
            if (isgraph(c)) {
                DBG_print(&c);
            } else {
                DBG_print(".");
            }
        }
        
        DBG_print("|\r\n");
        len -= bytesPerRow;
    }
}

/* [] END OF FILE */
