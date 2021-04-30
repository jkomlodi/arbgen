#include "strutils.h"

bool asciihex_to_byte(const uint8_t *ascii, size_t ascii_size, uint8_t *num, size_t *num_size)
{
	size_t i;
	bool retval = true;
	size_t output_size = 0;

	// the ASCII input must be divisible by 2, otherwise our input doesn't make sense
	if (ascii_size % 2) {
		retval = false;
	}
	else {
		// Initialize the output memory and size variable.  It will always be half of the input size on proper input
		memset(num, 0, ascii_size/2);

		for (i=0; i<ascii_size; ++i) {
			if (ascii[i] >= '0' && ascii[i] <= '9') {
				num[output_size] |= ascii[i] - '0';
			}
			else if (ascii[i] >= 'A' && ascii[i] <= 'F') {
				num[output_size] |= (ascii[i] - 'A') + 0x0A;
			}
			else if (ascii[i] >= 'a' && ascii[i] <= 'f') {
				num[output_size] |= (ascii[i] - 'a') + 0x0A;
			}
			else {
				retval = false;
			}

			// If we're at the first ASCII character in an ASCII byte, shift our raw byte over 4 to handle the next nybble
			if (!(i % 2)) {
				num[output_size] <<= 4;
			}
			// Otherwise, we finished parsing this ASCII byte, adjust our output offset
			else {
				++output_size;
			}
		}
	}

	if (num_size) {
		*num_size = output_size;
	}

	return retval;
}
