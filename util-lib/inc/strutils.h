#ifndef UTIL_LIB_INC_STRUTILS_H_
#define UTIL_LIB_INC_STRUTILS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

bool asciihex_to_byte(const uint8_t *ascii, size_t ascii_size, uint8_t *num, size_t *num_size);

#endif /* UTIL_LIB_INC_STRUTILS_H_ */
