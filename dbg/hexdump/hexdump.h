// https://elinux.org/Debugging_by_printing
// https://github.com/Yveaux/Arduino_HexDump
// https://elixir.bootlin.com/linux/latest/source/lib/hexdump.c#L243

#ifndef HEXDUMP_H_
#define HEXDUMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

void HexDump(uint8_t *buff, size_t len, size_t base);

#ifdef __cplusplus
}
#endif

#endif

/* [] END OF FILE */
