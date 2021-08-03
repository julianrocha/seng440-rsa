/*
Library for Arbitrary-Precision Arithmetic
Heavily inspired from: https://github.com/kokke/tiny-bignum-c


Adapted to handle 96 bit bignums for a 32-bit ARM Processor
with interfacing functions optimized for RSA encryption/decryption

big_num is 96 bits = 12 bytes = 3 ints
big_num range: [0, 2^96-1] = [0, 7.9e28]
big_num max value:  79,228,162,514,264,337,593,543,950,335 (29 digits)

Least signinficant bit in the first index, big endian:
Index 		: | 					2					| 						1					|						0					|
Binary Value: |	0000 0000 0000 0000 0000 0000 0000 0000	|	0000 0000 0000 0000 0000 0000 0000 0000 | 	0000 0000 0000 0000 0000 0000 0000 0000 |
*/

#include <stdint.h> // to specify specific dtype sizes for unsigned ints

#define WORD_BYTES 4 // ARM is a 32-bit machine
#define BN_BYTES 12*2 // TODO: this is too large, should be 96 bit numbers (12 bytes)
#define BN_ARRAY_LEN (BN_BYTES / WORD_BYTES)

#define BN_DTYPE uint32_t	  // dtype of each array element, ARM is a 32-bit machine
#define BN_DTYPE_TMP uint64_t // twice the BN_DTYPE size to handle operations like multiplication
#define BN_HEX_STR_LEN (2 * WORD_BYTES * BN_ARRAY_LEN + 1)
#define SPRINTF_FORMAT_STR "%.08x"
#define SSCANF_FORMAT_STR "%8x"
#define MAX_VAL ((BN_DTYPE_TMP)0xFFFFFFFF) // 32-bit max value in a 64 bit type

typedef struct bignum
{
	BN_DTYPE arr[BN_ARRAY_LEN];
} bignum;

/* bignum arithmetic operations below */

/* Init/Printing: */
void bignum_init(bignum *n);
void bignum_from_int(bignum *n, BN_DTYPE_TMP i);
unsigned int bignum_to_int(bignum *n);
void bignum_from_string(bignum *n, char *str);
void bignum_to_string(bignum *n, char *str);
void print_bignum(bignum *n);
void bignum_assign(bignum *dst, bignum *src); /* Copy src into dst -- dst := src */

/* Arithmetic operations: */
void bignum_add(bignum *a, bignum *b, bignum *c);				/* c = a + b */
void bignum_sub(bignum *a, bignum *b, bignum *c);				/* c = a - b */
void bignum_mul(bignum *a, bignum *b, bignum *c);				/* c = a * b */
void bignum_div(bignum *a, bignum *b, bignum *c);				/* c = a / b */
void bignum_mod(bignum *a, bignum *b, bignum *c);				/* c = a % b */
void bignum_divmod(bignum *a, bignum *b, bignum *c, bignum *d); /* c = a/b, d = a%b */

/* Bitwise operators: */
void bignum_rshift(bignum *a, bignum *b, unsigned int nbits); /* b = a >> nbits */
void bignum_or(bignum *a, bignum *b, bignum *c);			  /* c = a | b */

/* Other:*/
/* Tokens returned by bignum_cmp() for value comparison */
enum
{
	SMALLER = -1,
	EQUAL = 0,
	LARGER = 1
};
int bignum_cmp(bignum *a, bignum *b); /* Compare: returns LARGER, EQUAL or SMALLER */
int bignum_is_zero(bignum *n);		  /* For comparison with zero */