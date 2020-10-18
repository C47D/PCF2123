#include <string.h>

#include "DBG.h"

#include "printf.h"
#include "hexdump.h"

#include "usart.h"

static UART_HandleTypeDef *DBG_UART_PORT = NULL;

void _putchar(char character)
{
    HAL_UART_Transmit(DBG_UART_PORT, (uint8_t *) &character, 1, 250);
}

void DBG_init(void *handle)
{
    DBG_UART_PORT = (UART_HandleTypeDef *) handle;
}

void DBG_clear_screen(void)
{
	uint8_t clear_page = 0x0C;
	HAL_UART_Transmit(DBG_UART_PORT, &clear_page, 1, 250);
}

void DBG_println(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	HAL_UART_Transmit(DBG_UART_PORT, (uint8_t *) "\r\n", strlen("\r\n"), 250);
}

void DBG_print(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
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
void DBG_hexdump(uint8_t *buff, size_t len, size_t base)
{
	char *header = "\r\n      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n";
	char *header_sep = "      -----------------------------------------------\r\n";

        DBG_print(header);
        DBG_print(header_sep);
	HexDump(buff, len, base);
        DBG_print("\r\n");
}

