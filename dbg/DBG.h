#ifndef DBG_H_
#define DBG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define DEBUG_ENABLE 			1
#define BOOL_PARAM(param)   (param ? '1' : '0')

void DBG_init(void *handle);
void DBG_clear_screen(void);
void DBG_println(const char *fmt, ...);
void DBG_print(const char *fmt, ...);
void DBG_hexdump(uint8_t *buff, size_t len, size_t base);

#ifdef __cplusplus
}
#endif

#endif /* DBG_H_ */
