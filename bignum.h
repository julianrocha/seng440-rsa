/*
Inspired from: https://github.com/kokke/tiny-bignum-c

Adapted to handle 96 bit bignums for a 32-bit ARM Processor
*/

#include <stdint.h> // to specify specific dtype sizes for unsigned ints

#define WORD_BYTES 4 // ARM is a 32-bit machine
#define BN_BYTES 12	 // We want to work with 96 bit numbers
#define BN_ARRAY_LEN (BN_BYTES / WORD_BYTES)

#define BN_DTYPE uint32_t	  // dtype of each array element, ARM is a 32-bit machine
#define BN_DTYPE_TMP uint64_t // twice the BN_DTYPE size to handle operations like multiplication
#define BN_HEX_STR_LEN (2 * WORD_BYTES * BN_ARRAY_LEN + 1)
#define SPRINTF_FORMAT_STR "%.08x"
#define SSCANF_FORMAT_STR "%8x"

typedef struct bignum
{
	BN_DTYPE arr[BN_ARRAY_LEN];
} bignum;

/* bignum arithmetic operations below */

/* Init/Printing: */
void bignum_init(bignum *n);
void bignum_from_int(bignum *n, unsigned int i);
unsigned int bignum_to_int(bignum *n);
void bignum_from_string(bignum *n, char *str);
void bignum_to_string(bignum *n, char *str);
void print_bignum(bignum *n);